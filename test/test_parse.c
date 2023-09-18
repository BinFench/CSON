#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include <CSON.h>
#include <CBoil/printutils.h>

int main() {
    TESTS(
        TEST(
            JSONObject* obj = CSON.parse("{}");
            ASSERT(obj, "Failed to match\n");
            CSON.clear(obj);
        ),
        TEST(
            JSONObject* obj = CSON.parse("{"
            "\"int\": 1"
            "}");
            ASSERT(obj, "Failed to match\n");
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
            CSON.clear(obj);
        )
    );
    assertf(testsPassed == totalTests, "Failed tests");
    return 0;
}