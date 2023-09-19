#include <stdlib.h>
#include <stdio.h>
#include <CBoil.h>

#include "CSON.h"

static uint64_t hash_key(const char* key) {
    // Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
    uint64_t hash = OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= PRIME;
    }
    return hash;
}

static JSONValue* get(JSONObject* object, const char* name) {
    // Given a Capture and name string, perform hash lookup and return matching CaptureKVList
    if (!object || object->numKVs == 0)
        return NULL;

    uint64_t hash = hash_key(name);
    size_t index = (size_t)(hash & (uint64_t)(object->capacity - 1));

    int iter = 0;
    while (iter < object->capacity) {
        if (object->kvs[index].key) {
            if (strcmp(name, object->kvs[index].key) == 0) {
                return object->kvs + index;
            }
        }

        index++;
        if (index >= object->capacity) index = 0;
        iter++;
    }
    return NULL;
}

static JSONValue* insert(JSONObject* parent, JSONValue child) {
    // Given a parent and child JSON, perform hash insertion
    assertf(parent, "Cannot insert into NULL.");

    JSONValue* match = get(parent, child.key);
    assertf(!match, "Cannot insert existing object into parent.")
    if (parent->numKVs >= parent->capacity / 2) {
        if (parent->capacity == 0) parent->capacity = INITIAL_CAPACITY;
        else parent->capacity *= 2;

        parent->kvs = realloc(parent->kvs, parent->capacity*sizeof(JSONValue));
        if (parent->capacity == INITIAL_CAPACITY) for (int i = 0; i < INITIAL_CAPACITY; i++)
            parent->kvs[i] = (JSONValue){NULL, 0, NULL};
        else for (int i = parent->capacity / 2; i < parent->capacity; i++)
            parent->kvs[i] = (JSONValue){NULL, 0, NULL};
    }

    uint64_t hash = hash_key(child.key);
    size_t index = (size_t)(hash & (uint64_t)(parent->capacity - 1));

    while (parent->kvs[index].key) {
        index++;
        if (index >= parent->capacity) index = 0;
    }

    parent->kvs[index] = child;
    parent->numKVs++;
    return parent->kvs + index;
}

static char* strcopy(char* str) {
    char* toRet = malloc(strlen(str));
    strcpy(toRet, str);
    return toRet;
}

static bool* boolcopy(bool val) {
    bool* toRet = malloc(sizeof(bool));
    *toRet = val;
    return toRet;
}

static double* floatcopy(double flt) {
    double* toRet = malloc(sizeof(double));
    *toRet = flt;
    return toRet;
}

static int* intcopy(int num) {
    int* toRet = malloc(sizeof(int));
    *toRet = num;
    return toRet;
}

static JSONObject* getJSONObject(Capture* cap);
static JSONArray* getJSONArray(Capture* cap);

static JSONValue getJSONValue(char* key, Capture* value) {
    #define CBOILIF(cap, search) CaptureKVList* search; if ((search = CBoil.get(cap, STRING(search))))
    #define CBOILELSE(check) else {check}

    CBOILIF(value, body) {
        // Attach JSONObject to AST
        return ((JSONValue){key, OBJECT, getJSONObject(body->captures)});
    } CBOILELSE(CBOILIF(value, array) {
        // Attach JSONArray to AST
        return ((JSONValue){key, ARRAY, getJSONArray(array->captures)});
    } CBOILELSE(CBOILIF(value, string) {
        // Attach JSONValue<char*> to AST
        return ((JSONValue){key, STR, strcopy(string->captures->firstCap->str)});
    } CBOILELSE(CBOILIF(value, boolean) {
        // Attach JSONValue<bool> to AST
        return ((JSONValue){key, BOOL, boolcopy(strcmp(boolean->captures->firstCap->str, "true") == 0)});
    } CBOILELSE(CBOILIF(value, number) {
        // Attach JSONValue<int> or JSONValue<float> to AST
        char* numstr = number->captures->firstCap->str;
        if (strstr(numstr, "."))
            return ((JSONValue){key, FLOAT, floatcopy(atof(numstr))});
        else
            return ((JSONValue){key, INT, intcopy(atoi(numstr))});
    } CBOILELSE(CBOILIF(value, null) {
        // Attach JSONValue<> to AST
        return ((JSONValue){key, NULLTYPE, NULL});
    })))))

    return (JSONValue){NULL, 0, NULL};
}

static JSONArray* getJSONArray(Capture* cap) {
    JSONArray* array = calloc(1, sizeof(JSONArray));
    CaptureKVList* values = CBoil.get(cap, "value");

    if (!values) return array;

    array->size = values->matches;
    array->values = malloc(sizeof(JSONValue)*array->size);
    for (int i = 0; i < array->size; i++)
        array->values[i] = getJSONValue(NULL, values->captures + i);

    return array;
}

static JSONObject* getJSONObject(Capture* cap) {
    JSONObject* json = calloc(1, sizeof(JSONObject));
    CaptureKVList* KVs = CBoil.get(cap, "kv");

    if (!KVs) return json;

    for (int i = 0; i < KVs->matches; i++) {
        // We determine the KV type and insert it into the JSON AST
        Capture* KV = KVs->captures + i;
        char* key = strcopy(CBoil.get(KV, "key")->captures->firstCap->str);
        Capture* value = CBoil.get(KV, "value")->captures;
        insert(json, getJSONValue(key, value));
    }

    return json;
}

static JSONObject* parse(char* src) {
    Capture* cap = PARSE(top, src);
    if (!cap) return NULL;

    JSONObject* json = getJSONObject(cap);

    CBoil.clear(cap);
    return json;
}

static int getInt(JSONObject* object, const char* name) {
    JSONValue* value = get(object, name);
    assertf(value && value->type == INT, "Expected int under \"%s\", but not found.", name);
    return *(int*)(value->value);
}

static double getFloat(JSONObject* object, const char* name) {
    JSONValue* value = get(object, name);
    assertf(value && value->type == FLOAT, "Expected float under \"%s\", but not found.", name);
    return *(double*)(value->value);
}

static bool getBool(JSONObject* object, const char* name) {
    JSONValue* value = get(object, name);
    assertf(value && value->type == BOOL, "Expected bool under \"%s\", but not found.", name);
    return *(bool*)(value->value);
}

static char* getString(JSONObject* object, const char* name) {
    JSONValue* value = get(object, name);
    assertf(value && value->type == STR, "Expected string under \"%s\", but not found.", name);
    return (char*)(value->value);
}

static JSONObject* getObject(JSONObject* object, const char* name) {
    JSONValue* value = get(object, name);
    assertf(value && value->type == OBJECT, "Expected object under \"%s\", but not found.", name);
    return (JSONObject*)(value->value);
}

static JSONArray* getArray(JSONObject* object, const char* name) {
    JSONValue* value = get(object, name);
    assertf(value && value->type == ARRAY, "Expected array under \"%s\", but not found.", name);
    return (JSONArray*)(value->value);
}

static JSONValue* getFromArray(JSONArray* array, int idx) {
    assertf(array->size > idx, "Index %d out of range for size %d.", idx, array->size);
    return array->values + idx;
}

static int getIntFromArray(JSONArray* array, int idx) {
    JSONValue* value = getFromArray(array, idx);
    assertf(value && value->type == INT, "Expected int under array[%d], but not found.", idx);
    return *(int*)(value->value);
}

static double getFloatFromArray(JSONArray* array, int idx) {
    JSONValue* value = getFromArray(array, idx);
    assertf(value && value->type == FLOAT, "Expected float under array[%d], but not found.", idx);
    return *(double*)(value->value);
}

static bool getBoolFromArray(JSONArray* array, int idx) {
    JSONValue* value = getFromArray(array, idx);
    assertf(value && value->type == BOOL, "Expected bool under array[%d], but not found.", idx);
    return *(bool*)(value->value);
}

static char* getStringFromArray(JSONArray* array, int idx) {
    JSONValue* value = getFromArray(array, idx);
    assertf(value && value->type == STR, "Expected string under array[%d], but not found.", idx);
    return (char*)(value->value);
}

static JSONObject* getObjectFromArray(JSONArray* array, int idx) {
    JSONValue* value = getFromArray(array, idx);
    assertf(value && value->type == OBJECT, "Expected object under array[%d], but not found.", idx);
    return (JSONObject*)(value->value);
}

static JSONArray* getArrayFromArray(JSONArray* array, int idx) {
    JSONValue* value = getFromArray(array, idx);
    assertf(value && value->type == ARRAY, "Expected array under array[%d], but not found.", idx);
    return (JSONArray*)(value->value);
}

static void clear(JSONObject* object);

static void clearValue(JSONValue value);

static void clearArray(JSONArray* array) {
    for (int i = 0; i < array->size; i++) clearValue(array->values[i]);
    free(array->values);
    free(array);
}

static void clearValue(JSONValue value) {
    if (value.key) free(value.key);
    switch (value.type) {
        case OBJECT:
            clear(value.value);
            break;
        case ARRAY:
            clearArray(value.value);
            break;
        case NULLTYPE:
            break;
        default:
            free(value.value);
    }
}

static void clear(JSONObject* object) {
    for (int i = 0; i < object->capacity; i++)
        if (object->kvs[i].key) clearValue(object->kvs[i]);
    free(object->kvs);
    free(object);
}

const CSONLib CSON = {
    parse,
    get,
    getInt,
    getFloat,
    getBool,
    getString,
    getObject,
    getArray,
    getFromArray,
    getIntFromArray,
    getFloatFromArray, 
    getBoolFromArray,
    getStringFromArray,
    getObjectFromArray,
    getArrayFromArray,
    clear
};