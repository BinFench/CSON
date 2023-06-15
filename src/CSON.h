#include <stdbool.h>
#include "CSON.def"

#ifndef CSONLIB
#define CSONLIB

typedef enum JSONType {
    INT,
    FLOAT,
    BOOL,
    STR,
    OBJECT,
    ARRAY,
    NULLTYPE
} JSONType;

typedef struct JSONValue JSONValue;

typedef struct JSONObject {
    int numKVs;
    int capacity;
    JSONValue* kvs;
} JSONObject;

typedef struct JSONArray {
    int size;
    JSONValue* values;
} JSONArray;

typedef struct JSONValue {
    char* key;
    JSONType type;
    void* value;
} JSONValue;

typedef struct CSONLib {
    JSONObject* (*parse)(char* src);
    JSONValue* (*get)(JSONObject* object, const char* name);
    int (*getInt)(JSONObject* object, const char* name);
    double (*getFloat)(JSONObject* object, const char* name);
    bool (*getBool)(JSONObject* object, const char* name);
    char* (*getString)(JSONObject* object, const char* name);
    JSONObject* (*getObject)(JSONObject* object, const char* name);
    JSONArray* (*getArray)(JSONObject* object, const char* name);
    JSONValue* (*getFromArray)(JSONArray* array, int idx);
    int (*getIntFromArray)(JSONArray* array, int idx);
    double (*getFloatFromArray)(JSONArray* array, int idx);
    bool (*getBoolFromArray)(JSONArray* array, int idx);
    char* (*getStringFromArray)(JSONArray* array, int idx);
    JSONObject* (*getObjectFromArray)(JSONArray* array, int idx);
    JSONArray* (*getArrayFromArray)(JSONArray* array, int idx);
    void (*clear)(JSONObject* object);
} CSONLib;

extern const CSONLib CSON;

#endif