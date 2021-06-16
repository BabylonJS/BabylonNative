#pragma once

#include <napi/env.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>

#if _MSC_VER
#pragma warning( disable : 4324 ) // 'bx::DirectoryReader': structure was padded due to alignment specifier
#endif

#include <bx/file.h>

#include <functional>
#include <sstream>
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics.h>
#include <atomic>

namespace
{
    std::atomic<bool> doExit{};
    int errorCode{};

#if defined(__cplusplus_winrt)
    std::string WStringToString(const wchar_t* w, size_t size)
    {
        auto length{ ::WideCharToMultiByte(CP_UTF8, 0, w,static_cast<int>(size), nullptr, 0, nullptr, nullptr) };

        std::string ret{};
        if (length)
        {
            ret.resize(length);
            ::WideCharToMultiByte(CP_UTF8, 0, w, static_cast<int>(size), &ret[0], length, nullptr, nullptr);
        }
        return ret;
#else
#ifdef WIN32
    std::filesystem::path GetModulePath()
    {
        char buffer[1024];
        ::GetModuleFileNameA(nullptr, buffer, ARRAYSIZE(buffer));
        return std::filesystem::path{ buffer }.parent_path();
    }
#endif
#endif
}

// can't externalize variable with ObjC++. Using a function instead.
int GetExitCode()
{
    return errorCode;
}

namespace Babylon
{
    class TestUtils final : public Napi::ObjectWrap<TestUtils>
    {
    public:
        static inline constexpr const char* JS_INSTANCE_NAME{ "TestUtils" };

        using ParentT = Napi::ObjectWrap<TestUtils>;

        static void CreateInstance(Napi::Env env, WindowType nativeWindowPtr)
        {
            _nativeWindowPtr = nativeWindowPtr;
            Napi::HandleScope scope{ env };

            Napi::Function func = ParentT::DefineClass(
                env,
                "TestUtilsClass",
                {
                    ParentT::InstanceMethod("exit", &TestUtils::Exit),
                    ParentT::InstanceMethod("updateSize", &TestUtils::UpdateSize),
                    ParentT::InstanceMethod("setTitle", &TestUtils::SetTitle),
                    ParentT::InstanceMethod("writePNG", &TestUtils::WritePNG),
                    ParentT::InstanceMethod("decodeImage", &TestUtils::DecodeImage),
                    ParentT::InstanceMethod("getImageData", &TestUtils::GetImageData),
                    ParentT::InstanceMethod("getOutputDirectory", &TestUtils::GetOutputDirectory),
                });
            env.Global().Set(JS_INSTANCE_NAME, func.New({}));
        }

        explicit TestUtils(const Napi::CallbackInfo& info)
            : ParentT{ info }
        {
        }

    private:
        static inline Napi::FunctionReference constructor{};

        void Exit(const Napi::CallbackInfo& info)
        {
            const int32_t exitCode = info[0].As<Napi::Number>().Int32Value();
            doExit = true;
            errorCode = exitCode;
#if defined(__cplusplus_winrt)
            // ceguille: I didn't find a better way to do it for UWP
            exit(errorCode);
#elif ANDROID
#else
#ifdef WIN32
            PostMessageW(_nativeWindowPtr, WM_DESTROY, 0, 0);
#elif __linux__
            Display* display = XOpenDisplay(NULL);
            XClientMessageEvent dummyEvent;
            memset(&dummyEvent, 0, sizeof(XClientMessageEvent));
            dummyEvent.type = ClientMessage;
            dummyEvent.window = (Window)_nativeWindowPtr;
            dummyEvent.format = 32;
            XSendEvent(display, (Window)_nativeWindowPtr, 0, 0, (XEvent*)&dummyEvent);
            XFlush(display);
#elif __APPLE__
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
            dispatch_async(dispatch_get_main_queue(), ^{
                if (graphics)
                {
                    graphics->FinishRenderingCurrentFrame();
                }
                runtime.reset();
                graphics.reset();
                UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"Validation Tests"
                                               message:(errorCode == 0)?@"Success!":@"Errors: Check logs!"
                                               preferredStyle:UIAlertControllerStyleAlert];

                UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault
                   handler:^(UIAlertAction * ) {}];

                [alert addAction:defaultAction];
                UIViewController *rootController = [[[[UIApplication sharedApplication]delegate] window] rootViewController];
                [rootController presentViewController:alert animated:YES completion:nil];
            });
#else
            dispatch_async(dispatch_get_main_queue(), ^{
                [[_nativeWindowPtr window]close];
            });
#endif
#else
            // TODO: handle exit for other platforms
#endif
#endif
        }

        void UpdateSize(const Napi::CallbackInfo& info)
        {
#if defined(__cplusplus_winrt)
            (void)info;
#elif WIN32
            const int32_t width = info[0].As<Napi::Number>().Int32Value();
            const int32_t height = info[1].As<Napi::Number>().Int32Value();

            HWND hwnd = _nativeWindowPtr;
            RECT rc{ 0, 0, width, height };
            AdjustWindowRectEx(&rc, GetWindowStyle(hwnd), GetMenu(hwnd) != NULL, GetWindowExStyle(hwnd));
            SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
#else
            // TODO: handle resize for other platforms
            (void)info;
#endif
        }

        void SetTitle(const Napi::CallbackInfo& info)
        {
            const auto title = info[0].As<Napi::String>().Utf8Value();
#if defined(__cplusplus_winrt)
#elif WIN32
            SetWindowTextA(_nativeWindowPtr, title.c_str());
#elif ANDROID
            (void)info;
#elif __linux__
            Display* display = XOpenDisplay(NULL);
            XStoreName(display, (Window)_nativeWindowPtr, title.c_str());
#else
            // TODO: handle title for other platforms
#endif
        }

        void WritePNG(const Napi::CallbackInfo& info)
        {
            const auto buffer = info[0].As<Napi::Uint8Array>();
            const auto width = info[1].As<Napi::Number>().Uint32Value();
            const auto height = info[2].As<Napi::Number>().Uint32Value();
            const auto filename = info[3].As<Napi::String>().Utf8Value();

            if (buffer.ByteLength() < (width * height * 4))
            {
                return;
            }

            bx::MemoryBlock mb(&allocator);
            bx::FileWriter writer;
            bx::FilePath filepath(filename.c_str());
            bx::Error err;
            if (writer.open(filepath, false, &err))
            {
                bimg::imageWritePng(&writer, width, height, width * 4, buffer.Data(), bimg::TextureFormat::RGBA8, false);
                writer.close();
            }
        }

        struct Image
        {
            Image() = default;
            ~Image()
            {
                if (m_Image)
                {
                    bimg::imageFree(m_Image);
                    m_Image = nullptr;
                }
            }
            bimg::ImageContainer* m_Image{};
        };

        Napi::Value DecodeImage(const Napi::CallbackInfo& info)
        {
            Image* image = new Image;
            const auto buffer = info[0].As<Napi::ArrayBuffer>();

            image->m_Image = bimg::imageParse(&allocator, buffer.Data(), static_cast<uint32_t>(buffer.ByteLength()));

            auto finalizer = [](Napi::Env, Image* image) { delete image;};
            return Napi::External<Image>::New(info.Env(), image, std::move(finalizer));
        }

        Napi::Value GetImageData(const Napi::CallbackInfo& info)
        {
            const auto imageData = info[0].As<Napi::External<Image>>().Data();

            if (!imageData || !imageData->m_Image || !imageData->m_Image->m_size)
            {
                return info.Env().Undefined();
            }

            auto data = Napi::Uint8Array::New(info.Env(), imageData->m_Image->m_size);
            const auto ptr = static_cast<uint8_t*>(imageData->m_Image->m_data);
            memcpy(data.Data(), ptr, imageData->m_Image->m_size);

            return Napi::Value::From(info.Env(), data);
        }

        Napi::Value GetOutputDirectory(const Napi::CallbackInfo& info)
        {
#if defined(__cplusplus_winrt)
            using namespace Windows::Storage;
            StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;
            std::wstring wpath = localFolder->Path->Data();
            std::string path{WStringToString(wpath.data(), wpath.size())};
#elif ANDROID
            auto path = "/data/data/com.android.babylonnative.validationtests/cache";
#elif __APPLE__
            std::string path = getenv("HOME");
#elif __linux__
            char exe[1024];
            int ret = readlink("/proc/self/exe", exe, sizeof(exe)-1);
            if(ret == -1)
            {
                throw Napi::Error::New(info.Env(), "Unable to get executable location");
            }
            exe[ret] = 0;

            auto path = std::filesystem::path{exe}.parent_path().generic_string();
#elif WIN32
            auto path = GetModulePath().parent_path().generic_string();
#endif
            return Napi::Value::From(info.Env(), path);
        }

        inline static WindowType _nativeWindowPtr{};
        inline static bx::DefaultAllocator allocator{};
    };
}
