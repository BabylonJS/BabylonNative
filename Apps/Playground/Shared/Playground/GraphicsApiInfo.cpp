#include "GraphicsApiInfo.h"

#define STRINGIZEX(x) #x
#define STRINGIZE(x) STRINGIZEX(x)

namespace Playground::Internal
{
    namespace
    {
        constexpr auto JS_CLASS_NAME = "GraphicsApiInfo";
    }

    class GraphicsApiInfo final : public Napi::ObjectWrap<GraphicsApiInfo>
    {
    public:
        static void Initialize(Napi::Env env)
        {
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    StaticAccessor("Name", &GraphicsApiInfo::Name, nullptr)
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        GraphicsApiInfo(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<GraphicsApiInfo>{info}
        {
        }

    private:
        static Napi::Value Name(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(info.Env(), STRINGIZE(GRAPHICS_API));
        }
    };
}

namespace Playground::GraphicsApiInfo
{
    void Initialize(Napi::Env env)
    {
        Playground::Internal::GraphicsApiInfo::Initialize(env);
    }
}
