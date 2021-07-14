#pragma once
#include "gtest/gtest.h"
#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>

namespace Babylon
{
    // Waits for an arcana task to complete before continuing
    void WaitForCompletion(arcana::task<void, std::exception_ptr> task) {
        // The boolean flag is a temporary hack that should be replaced by proper synchronization code
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