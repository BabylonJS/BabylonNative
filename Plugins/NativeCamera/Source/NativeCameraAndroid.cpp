#include "NativeCamera.h"
#include <string>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraError.h>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadata.h>
#include <camera/NdkCameraMetadataTags.h>
#include <camera/NdkCameraWindowType.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <AndroidExtensions/Globals.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <arcana/threading/task.h>
#include <arcana/threading/dispatcher.h>

using namespace android;
using namespace android::global;

namespace Babylon::Plugins::Internal
{
    const int PERMISSION_REQUEST_ID{ 8436 };

    struct CameraInterfaceAndroid : public CameraInterface
    {
        CameraInterfaceAndroid();
        virtual ~CameraInterfaceAndroid();
        void UpdateCameraTexture(bgfx::TextureHandle textureHandle) override;

    private:

        std::string getBackFacingCamId();

        ACameraManager* cameraManager{};
        ACameraDevice* cameraDevice{};
        ACameraOutputTarget* textureTarget{};
        ACaptureRequest* request{};
        ANativeWindow* textureWindow{};
        ACameraCaptureSession* textureSession{};
        ACaptureSessionOutput* textureOutput{};
        ACaptureSessionOutput* output{};
        ACaptureSessionOutputContainer* outputs{};
        GLuint cameraTextureId{};
    };


    std::string CameraInterfaceAndroid::getBackFacingCamId()
    {
        ACameraIdList *cameraIds = nullptr;
        ACameraManager_getCameraIdList(cameraManager, &cameraIds);

        std::string backId;

        for (int i = 0; i < cameraIds->numCameras; ++i)
        {
            const char *id = cameraIds->cameraIds[i];

            ACameraMetadata *metadataObj;
            ACameraManager_getCameraCharacteristics(cameraManager, id, &metadataObj);

            ACameraMetadata_const_entry lensInfo = {};
            ACameraMetadata_getConstEntry(metadataObj, ACAMERA_LENS_FACING, &lensInfo);

            auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(
                    lensInfo.data.u8[0]);

            // Found a back-facing camera?
            if (facing == ACAMERA_LENS_FACING_BACK)
            {
                backId = id;
                break;
            }
        }

        ACameraManager_deleteCameraIdList(cameraIds);
        return backId;
    }

    // device callbacks
    static void onDisconnected(void* /*context*/, ACameraDevice* /*device*/)
    {
    }

    static void onError(void* /*context*/, ACameraDevice* /*device*/, int /*error*/)
    {
    }

    static ACameraDevice_stateCallbacks cameraDeviceCallbacks = {
            .context = nullptr,
            .onDisconnected = onDisconnected,
            .onError = onError
    };

    // session callbacks
    static void onSessionActive(void* /*context*/, ACameraCaptureSession* /*session*/)
    {
    }

    static void onSessionReady(void* /*context*/, ACameraCaptureSession* /*session*/)
    {
    }

    static void onSessionClosed(void* /*context*/, ACameraCaptureSession* /*session*/)
    {
    }

    static ACameraCaptureSession_stateCallbacks sessionStateCallbacks {
        .context = nullptr,
        .onClosed = onSessionClosed,
        .onReady = onSessionReady,
        .onActive = onSessionActive
    };

    // capture callbacks
    static void onCaptureFailed(void* /*context*/, ACameraCaptureSession* /*session*/, ACaptureRequest* /*request*/, ACameraCaptureFailure* /*failure*/)
    {
    }

    static void onCaptureSequenceCompleted(void* /*context*/, ACameraCaptureSession* /*session*/, int /*sequenceId*/, int64_t /*frameNumber*/)
    {
    }

    static void onCaptureSequenceAborted(void* /*context*/, ACameraCaptureSession* /*session*/, int /*sequenceId*/)
    {
    }

    static void onCaptureCompleted (void* /*context*/, ACameraCaptureSession* /*session*/, ACaptureRequest* /*request*/, const ACameraMetadata* /*result*/)
    {
    }

    static ACameraCaptureSession_captureCallbacks captureCallbacks {
        .context = nullptr,
        .onCaptureStarted = nullptr,
        .onCaptureProgressed = nullptr,
        .onCaptureCompleted = onCaptureCompleted,
        .onCaptureFailed = onCaptureFailed,
        .onCaptureSequenceCompleted = onCaptureSequenceCompleted,
        .onCaptureSequenceAborted = onCaptureSequenceAborted,
        .onCaptureBufferLost = nullptr,
    };

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
    
    CameraInterfaceAndroid::CameraInterfaceAndroid()
    {
        CheckCameraPermissionAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [this]()
        {
            cameraManager = ACameraManager_create();
            auto id = getBackFacingCamId();
            ACameraManager_openCamera(cameraManager, id.c_str(), &cameraDeviceCallbacks, &cameraDevice);

            // Prepare surface

            auto env = GetEnvForCurrentThread();

            glGenTextures(1, &cameraTextureId);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, cameraTextureId);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

            jclass surfaceTextureClass{env->FindClass("android/graphics/SurfaceTexture")};
            auto surfaceTextureConstructor = env->GetMethodID(surfaceTextureClass, "<init>", "(I)V");
            jobject surfaceTexture = env->NewObject(surfaceTextureClass, surfaceTextureConstructor,  cameraTextureId);

            jclass surfaceClass{env->FindClass("android/view/Surface")};
            auto surfaceConstructor = env->GetMethodID(surfaceClass, "<init>", "(Landroid/graphics/SurfaceTexture;)V");
            jobject surface = env->NewObject(surfaceClass, surfaceConstructor,  surfaceTexture);
/*
            ASurfaceTexture* surfaceTexture = ASurfaceTexture_fromSurfaceTexture(env, obj);
            ASurfaceTexture_attachToGLContext(surfaceTexture, cameraTextureId);
            textureWindow = ASurfaceTexture_acquireANativeWindow(surfaceTexture);
*/
            textureWindow = ANativeWindow_fromSurface(env, surface);
            // Prepare request for texture target
            ACameraDevice_createCaptureRequest(cameraDevice, TEMPLATE_PREVIEW, &request);

            // Prepare outputs for session
            ACaptureSessionOutput_create(textureWindow, &textureOutput);
            ACaptureSessionOutputContainer_create(&outputs);
            ACaptureSessionOutputContainer_add(outputs, textureOutput);

            // Prepare target surface
            ANativeWindow_acquire(textureWindow);
            ACameraOutputTarget_create(textureWindow, &textureTarget);
            ACaptureRequest_addTarget(request, textureTarget);

            // Create the session
            ACameraDevice_createCaptureSession(cameraDevice, outputs, &sessionStateCallbacks, &textureSession);

            // Start capturing continuously
            ACameraCaptureSession_setRepeatingRequest(textureSession, &captureCallbacks, 1, &request, nullptr);
        });
    }

    CameraInterfaceAndroid::~CameraInterfaceAndroid()
    {
        // Stop recording to SurfaceTexture and do some cleanup
        ACameraCaptureSession_stopRepeating(textureSession);
        ACameraCaptureSession_close(textureSession);
        ACaptureSessionOutputContainer_free(outputs);
        ACaptureSessionOutput_free(output);

        ACameraDevice_close(cameraDevice);
        ACameraManager_delete(cameraManager);

        // Capture request for SurfaceTexture
        ANativeWindow_release(textureWindow);
        ACaptureRequest_free(request);
    }

    void CameraInterfaceAndroid::UpdateCameraTexture(bgfx::TextureHandle /*textureHandle*/)
    {
        //bgfx::overrideInternal(textureHandle, cameraTextureId);
    }

    CameraInterface* CameraInterface::CreateInterface()
    {
        CameraInterfaceAndroid* cameraInterfaceAndroid = new CameraInterfaceAndroid;
        return cameraInterfaceAndroid;
    }
}