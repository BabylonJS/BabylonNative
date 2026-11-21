#include "App.h"
#include <gtest/gtest.h>
#if defined(NODE_API_AVAILABLE_NATIVE_TESTS)
#include "test_main.h"
#endif

int RunTests()
{
    testing::InitGoogleTest();
#if defined(NODE_API_AVAILABLE_NATIVE_TESTS)
    node_api_tests::RegisterNodeApiTests();
#endif
    return RUN_ALL_TESTS();
}
