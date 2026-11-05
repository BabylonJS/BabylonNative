#include <jni.h>

#include <Babylon/Graphics/Device.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <AndroidExtensions/Globals.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>

#include <BabylonNative.h>

using namespace facebook;

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_initialize(JNIEnv* env, jclass obj, jobject context, jlong jsiRuntimeRef, jobject jsCallInvokerHolder)
{
    static bool initializedJVM{ false };
    if (!initializedJVM)
    {
        JavaVM* javaVM{};
        if (env->GetJavaVM(&javaVM) != JNI_OK)
        {
            throw std::runtime_error("Failed to get Java VM");
        }

        android::global::Initialize(javaVM, context);

        initializedJVM = true;
    }

    auto jsiRuntime{ reinterpret_cast<jsi::Runtime*>(jsiRuntimeRef) };
    auto jsCallInvoker{ jni::alias_ref<react::CallInvokerHolder::javaobject>{ reinterpret_cast<react::CallInvokerHolder::javaobject>(jsCallInvokerHolder) }->cthis()->getCallInvoker() };
    auto jsDispatcher{ [jsCallInvoker{ std::move(jsCallInvoker) }](std::function<void()> func)
    {
        jsCallInvoker->invokeAsync([func{ std::move(func) }]
        {
            func();
        });
    } };

    BabylonNative::Initialize(*jsiRuntime, std::move(jsDispatcher));
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_setCurrentActivity(JNIEnv* env, jclass obj, jobject activity)
{
    android::global::SetCurrentActivity(activity);
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_pause(JNIEnv* env, jclass obj)
{
    android::global::Pause();
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_resume(JNIEnv* env, jclass obj)
{
    android::global::Resume();
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_updateView(JNIEnv* env, jclass obj, jobject surface)
{
    ANativeWindow* window{ ANativeWindow_fromSurface(env, surface) };
    auto width{ static_cast<size_t>(ANativeWindow_getWidth(window)) };
    auto height{ static_cast<size_t>(ANativeWindow_getHeight(window)) };
    BabylonNative::UpdateView(window, width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_updateMSAA(JNIEnv* env, jclass obj, jint value)
{
    BabylonNative::UpdateMSAA(static_cast<uint8_t>(value));
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_renderView(JNIEnv* env, jclass obj)
{
    BabylonNative::RenderView();
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_resetView(JNIEnv* env, jclass obj)
{
    BabylonNative::ResetView();
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_updateXRView(JNIEnv* env, jclass obj, jobject surface)
{
    ANativeWindow* window{};
    if (surface)
    {
        window = ANativeWindow_fromSurface(env, surface);
    }
    BabylonNative::UpdateXRView(window);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_isXRActive(JNIEnv* env, jclass obj)
{
    return BabylonNative::IsXRActive();
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_setTouchButtonState(JNIEnv* env, jclass obj, jint pointerId, jboolean isDown, jint x, jint y)
{
    BabylonNative::SetTouchButtonState(static_cast<uint32_t>(pointerId), isDown, static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_setTouchPosition(JNIEnv* env, jclass obj, jint pointerId, jint x, jint y)
{
    BabylonNative::SetTouchPosition(static_cast<uint32_t>(pointerId), static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}
