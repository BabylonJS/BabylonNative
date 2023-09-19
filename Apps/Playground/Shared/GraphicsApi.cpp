#include "GraphicsApi.h"

#define STRINGIZEX(x) #x
#define STRINGIZE(x) STRINGIZEX(x)

namespace Playground::Internal
{
    namespace
    {
        constexpr auto JS_CLASS_NAME = "GraphicsApi";
    }

    class GraphicsApi final : public Napi::ObjectWrap<GraphicsApi>
    {
    public:
        static void Initialize(Napi::Env env)
        {
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    StaticAccessor("Name", &GraphicsApi::Name, nullptr)
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        GraphicsApi(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<GraphicsApi>{info}
        {
        }

    private:
        static Napi::Value Name(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(info.Env(), STRINGIZE(GRAPHICS_API));
        }
    };
}

namespace Playground::GraphicsApi
{
    void Initialize(Napi::Env env)
    {
        Playground::Internal::GraphicsApi::Initialize(env);
    }
}
