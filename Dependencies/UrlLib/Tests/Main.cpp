#include <gtest/gtest.h>

#if defined(_WIN32)
#include <winrt/base.h>
#endif

int main(int argc, char** argv)
{
#if defined(_WIN32)
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
#endif
    testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();
#if defined(_WIN32)
    winrt::uninit_apartment();
#endif
    return result;
}
