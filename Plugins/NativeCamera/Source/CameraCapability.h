#pragma once

#include <napi/napi.h>

namespace Babylon::Plugins {
    class CameraCapability {
    public:
        enum Capability {
            Width,
            Height,
            FacingMode,
            Torch,
        };

        enum Type {
            Bool,
            UInt,
            String,
        };

        CameraCapability(Capability capability);
        virtual ~CameraCapability() = default;

        std::string getName();
        Type getType();
        Capability getCapability();

        virtual bool setValue(Napi::Value value)=0;
        virtual bool resetValue()=0;
        virtual Napi::Value asConstrainable(Napi::Env env)=0;
        virtual Napi::Value asSetting(Napi::Env env)=0;

    private:
        const Capability m_capability;
    };

    class CameraCapabilityBool : public CameraCapability
    {
    public:
        CameraCapabilityBool(Capability capability,
                             bool currentValue,
                             bool defaultValue,
                             std::vector<bool> acceptedValues,
                             std::optional<std::function<bool(bool)>> setterFunction);

        bool setValue(Napi::Value value);
        bool resetValue();
        Napi::Value asConstrainable(Napi::Env env);
        Napi::Value asSetting(Napi::Env env);

    private:
        struct Constraint
        {
            std::optional<bool> ideal;
            std::optional<bool> exact;
        };

        Constraint parseConstraint(Napi::Value value);

        bool m_currentValue;
        const bool m_defaultValue;
        const std::vector<bool> m_acceptedValues;
        std::optional<std::function<bool(bool)>> m_setterFunction;
    };

    class CameraCapabilityString : public CameraCapability
    {
    public:
        CameraCapabilityString(Capability capability,
        std::string currentValue, std::string defaultValue,
                std::vector<std::string> acceptedValues);

        bool setValue(Napi::Value value);
        bool resetValue();
        Napi::Value asConstrainable(Napi::Env env);
        Napi::Value asSetting(Napi::Env env);

        std::string currentValue;
        const std::string defaultValue;
        const std::vector<std::string> acceptedValues;
    };
}