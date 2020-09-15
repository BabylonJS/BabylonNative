#pragma once

#include <napi/env.h>
#include <functional>

template<typename RuntimeT>
class InputManager final : public Napi::ObjectWrap<InputManager<RuntimeT>>
{
public:
    class InputBuffer
    {
    public:
        InputBuffer(RuntimeT& rt)
            : m_runtime{ rt }
        {}
        InputBuffer(const InputBuffer&) = delete;
        InputBuffer& operator=(const InputBuffer&) = delete;

        void SetPointerPosition(int x, int y)
        {
            m_runtime.Dispatch([x, y, this](Napi::Env)
            {
                m_pointerX = x;
                m_pointerY = y;
            });
        }

        void SetPointerDown(bool isPointerDown)
        {
            m_runtime.Dispatch([isPointerDown, this](Napi::Env)
            {
                m_isPointerDown = isPointerDown;
            });
        }

        int GetPointerX() const
        {
            return m_pointerX;
        }

        int GetPointerY() const
        {
            return m_pointerY;
        }

        bool IsPointerDown() const
        {
            return m_isPointerDown;
        }

    private:
        RuntimeT& m_runtime;

        int m_pointerX{};
        int m_pointerY{};
        bool m_isPointerDown{};
    };

    static void Initialize(Napi::Env env, InputBuffer& buffer)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = Napi::ObjectWrap<InputManager>::DefineClass(
            env,
            "InputManager",
            {
                Napi::ObjectWrap<InputManager>::InstanceAccessor("pointerX", &InputManager::PointerX, nullptr),
                Napi::ObjectWrap<InputManager>::InstanceAccessor("pointerY", &InputManager::PointerY, nullptr),
                Napi::ObjectWrap<InputManager>::InstanceAccessor("isPointerDown", &InputManager::IsPointerDown, nullptr),
            },
            &buffer);

        env.Global().Set("InputManager", func);
    }

    explicit InputManager(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<InputManager>{info}
        , m_buffer{static_cast<InputBuffer*>(info.Data())}
    {
    }

private:
    InputBuffer* m_buffer{};

    Napi::Value PointerX(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_buffer->GetPointerX());
    }

    Napi::Value PointerY(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_buffer->GetPointerY());
    }

    Napi::Value IsPointerDown(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_buffer->IsPointerDown());
    }
};
