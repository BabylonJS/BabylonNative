#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();
    std::cerr << result << std::endl;
    return result;
}
