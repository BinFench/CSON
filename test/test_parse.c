#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include <CSON.h>

int main() {
    TESTS(
        TEST(
            JSONObject* obj = CSON.parse("{}");
            ASSERT(obj, "Failed to match\n");
            CSON.clear(obj);
        ),
        TEST(
            JSONObject* obj = CSON.parse("{"
            "\"int\": 1,"
            "\"float\": 1.2,"
            "\"bool\": true"
            "}");
            ASSERT(obj, "Failed to match\n");
            ASSERT(CSON.getInt(obj, "int") == 1, "Expected \"int\": 1\n");
            ASSERT(CSON.getFloat(obj, "float") == 1.2, "Expected \"float\": 1.2\n");
            ASSERT(CSON.getBool(obj, "bool"), "Expected \"bool\" to be true\n");
            CSON.clear(obj);
        ),
        TEST(
            JSONObject* obj = CSON.parse(
                "{\"widget\": {"
                    "\"debug\": \"on\","
                    "\"window\": {"
                        "\"title\": \"Sample Konfabulator Widget\","
                        "\"name\": \"main_window\","
                        "\"width\": 500,"
                        "\"height\": 500"
                    "},"
                    "\"image\": { "
                        "\"src\": \"Images/Sun.png\","
                        "\"name\": \"sun1\","
                        "\"hOffset\": 250,"
                        "\"vOffset\": 250,"
                        "\"alignment\": \"center\""
                    "},"
                    "\"text\": {"
                        "\"data\": \"Click Here\","
                        "\"size\": 36,"
                        "\"style\": \"bold\","
                        "\"name\": \"text1\","
                        "\"hOffset\": 250,"
                        "\"vOffset\": 100,"
                        "\"alignment\": \"center\","
                        "\"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\""
                    "}"
                "}}");
            ASSERT(obj, "Failed to match\n");
            JSONObject* widget = CSON.getObject(obj, "widget");
            ASSERT(widget, "Expected \"widget\" in JSON\n");
            char* debug = CSON.getString(widget, "debug");
            ASSERT(strcmp(debug, "on") == 0, "Expected debug == on, got '%s'\n", debug);
            JSONObject* window = CSON.getObject(widget, "window");
            ASSERT(window, "Expected \"window\" in \"widget\"");
            ASSERT(CSON.getInt(window, "width") == CSON.getInt(window, "height"), "Expected window height and width to match\n");
            CSON.clear(obj);
        ),
        TEST(
            JSONObject* obj = CSON.parse("{"
            "\"intarr\": [1, 2, 3],"
            "\"floatarr\": [1.1, 2.2, 3.3],"
            "\"boolarr\": [true, false, true],"
            "\"strarr\": [\"first\", \"second\", \"third\"],"
            "\"objarr\": [{}, {}, {}],"
            "\"arrarr\": [[], [], []]"
            "}");
            ASSERT(obj, "Failed to match\n");
            JSONArray* intarr = CSON.getArray(obj, "intarr");
            ASSERT(intarr, "Expected int array in object\n");
            ASSERT(CSON.getIntFromArray(intarr, 2) == 3, "Expected intarr[2] == 3\n");
            JSONArray* floatarr = CSON.getArray(obj, "floatarr");
            ASSERT(floatarr, "Expected float array in object\n");
            ASSERT(CSON.getFloatFromArray(floatarr, 1) == 2.2, "Expected floatarr[1] == 2.2\n");
            JSONArray* boolarr = CSON.getArray(obj, "boolarr");
            ASSERT(boolarr, "Expected bool array in object\n");
            ASSERT(CSON.getBoolFromArray(boolarr, 0), "Expected boolarr[0] == true\n");
            JSONArray* strarr = CSON.getArray(obj, "strarr");
            ASSERT(strarr, "Expected string array in object\n");
            ASSERT(strcmp(CSON.getStringFromArray(strarr, 2), "third") == 0, "Expected strarr[2] == \"third\"\n");
            JSONArray* objarr = CSON.getArray(obj, "objarr");
            ASSERT(objarr, "Expected object array in object\n");
            ASSERT(CSON.getObjectFromArray(objarr, 1), "Expected objarr[1]\n");
            JSONArray* arrarr = CSON.getArray(obj, "arrarr");
            ASSERT(arrarr, "Expected array array in object\n");
            ASSERT(CSON.getArrayFromArray(arrarr, 0), "Expected arrarr[0]\n");
            CSON.clear(obj);
        )
    );
    assertf(testsPassed == totalTests, "Failed tests");
    return 0;
}