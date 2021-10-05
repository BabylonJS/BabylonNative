#include <napi/env.h>

#if _MSC_VER
#pragma warning( disable : 4324 ) // 'bx::DirectoryReader': structure was padded due to alignment specifier
#endif

#include <bx/file.h>
#include <bimg/bimg.h>
#include <Babylon/Graphics.h>

namespace Babylon::Plugins::Internal
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

        inline static WindowType _nativeWindowPtr{};
        inline static bx::DefaultAllocator allocator{};

        void Exit(const Napi::CallbackInfo& info);
        void UpdateSize(const Napi::CallbackInfo& info);
        void SetTitle(const Napi::CallbackInfo& info);
        Napi::Value GetOutputDirectory(const Napi::CallbackInfo& info);

        void WritePNG(const Napi::CallbackInfo& info);
        Napi::Value DecodeImage(const Napi::CallbackInfo& info);
        Napi::Value GetImageData(const Napi::CallbackInfo& info);

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
    };
} // namespace