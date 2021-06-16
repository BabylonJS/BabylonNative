#pragma once
#include "gtest/gtest.h"
#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>

namespace Babylon
{

    int test(int argc, char** argv)
    {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    void WaitForCompletion(arcana::task<void, std::exception_ptr> task) {
        bool done{false};
        task.then(arcana::inline_scheduler, arcana::cancellation::none(), [&done](arcana::expected<void, std::exception_ptr> result) {
            done = true;
        });
        while (!done);
    }

    TEST(URLLibTest, EmptyUrl)
    {
        UrlLib::UrlRequest req;
        req.Open(UrlLib::UrlMethod::Get, "");
        WaitForCompletion(req.SendAsync());
        ASSERT_EQ(req.ResponseType(), UrlLib::UrlResponseType::String);
        ASSERT_EQ(req.ResponseString(), "");
        ASSERT_EQ(req.StatusCode(), UrlLib::UrlStatusCode::None);
    }
}