#include <AndroidExtensions/Permissions.h>
#include <AndroidExtensions/Globals.h>

using namespace android;
using namespace android::global;

namespace android::Permissions
{
    // Permission request ID used to uniquely identify our request in the callback when calling requestPermissions.
    const int PERMISSION_REQUEST_ID{ 8435 };

    arcana::task<void, std::exception_ptr> CheckCameraPermissionAsync()
    {
        auto task{ arcana::task_from_result<std::exception_ptr>() };

        // Check if permissions are already granted.
        if (!GetAppContext().checkSelfPermission(ManifestPermission::CAMERA()))
        {
            // Register for the permission callback request.
            arcana::task_completion_source<void, std::exception_ptr> permissionTcs;
            auto permissionTicket
            {
                AddRequestPermissionsResultCallback(
                [permissionTcs](int32_t requestCode, const std::vector<std::string>& /*permissionList*/, const std::vector<int32_t>& results) mutable
                {
                    // Check if this is our permission request ID.
                    if (requestCode == PERMISSION_REQUEST_ID)
                    {
                        // If the permission is found and granted complete the task.
                        if (results[0] == 0 /* PackageManager.PERMISSION_GRANTED */)
                        {
                            permissionTcs.complete();
                            return;
                        }

                        // Permission was denied.  Complete the task with an error.
                        permissionTcs.complete(arcana::make_unexpected(make_exception_ptr(std::runtime_error{"Camera permission not acquired successfully"})));
                    }
                })
            };

            // Kick off the permission check request, and set the task for our caller to wait on.
            GetCurrentActivity().requestPermissions(ManifestPermission::CAMERA(), PERMISSION_REQUEST_ID);
            task = permissionTcs.as_task().then(arcana::inline_scheduler, arcana::cancellation::none(), [ticket{ std::move(permissionTicket) }](){
                return;
            });
        }

        return task;
    }
}