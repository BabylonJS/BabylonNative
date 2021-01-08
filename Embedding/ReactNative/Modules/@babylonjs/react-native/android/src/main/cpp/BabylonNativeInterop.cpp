#include <jni.h>

#include <Babylon/Graphics.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Plugins/NativeXr.h>
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

    Babylon::Initialize(*jsiRuntime, std::move(jsDispatcher));
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_deinitialize(JNIEnv* env, jclass obj)
{
    Babylon::Deinitialize();
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
    ANativeWindow* windowPtr{ ANativeWindow_fromSurface(env, surface) };
    auto width{ static_cast<size_t>(ANativeWindow_getWidth(windowPtr)) };
    auto height{ static_cast<size_t>(ANativeWindow_getHeight(windowPtr)) };
    Babylon::UpdateView(windowPtr, width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_setPointerButtonState(JNIEnv* env, jclass obj, jint pointerId, jint buttonId, jboolean isDown, jint x, jint y)
{
    Babylon::SetPointerButtonState(static_cast<uint32_t>(pointerId), static_cast<uint32_t>(buttonId), isDown, static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}

extern "C" JNIEXPORT void JNICALL Java_com_babylonreactnative_BabylonNativeInterop_00024BabylonNative_setPointerPosition(JNIEnv* env, jclass obj, jint pointerId, jint x, jint y)
{
    Babylon::SetPointerPosition(static_cast<uint32_t>(pointerId), static_cast<uint32_t>(x), static_cast<uint32_t>(y));
}
