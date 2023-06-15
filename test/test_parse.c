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
        )
    );
    return 0;
}