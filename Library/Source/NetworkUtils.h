#pragma once

#include <arcana/threading/task.h>

#include <string>
#include <vector>

namespace Babylon
{
    std::string GetAbsoluteUrl(const std::string& url, const std::string& rootUrl);

    arcana::task<std::string, std::exception_ptr> LoadUrlToStringAsync(const std::string url);
    arcana::task<std::vector<uint8_t>, std::exception_ptr> LoadUrlToBytesAsync(const std::string url);
}