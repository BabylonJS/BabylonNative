#pragma once

#include <arcana/threading/task.h>

namespace android::Permissions
{
    arcana::task<void, std::exception_ptr> CheckCameraPermissionAsync();
}