#include <napi/env.h>

#if _MSC_VER
#pragma warning( disable : 4324 ) // 'bx::DirectoryReader': structure was padded due to alignment specifier
#endif

#include <bx/allocator.h>
#include <bimg/bimg.h>

namespace Babylon::Plugins::Internal
{
    class TestUtils final : public Napi::ObjectWrap<TestUtils>
    {
    public:
        class ImplData;

        static inline constexpr const char* JS_INSTANCE_NAME{ "TestUtils" };

        using ParentT = Napi::ObjectWrap<TestUtils>;

        static void CreateInstance(Napi::Env env, std::shared_ptr<ImplData> implData)
        {
            m_implData = std::move(implData);
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

        inline static std::shared_ptr<ImplData> m_implData;
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