#pragma once

#include <napi/napi.h>

namespace Babylon::Plugins {
    class CameraCapability {
    public:
        enum Capability
        {
            Width,
            Height,
            FacingMode,
            Torch,
        };
        
        enum MeetsConstraint
        {
            FullySatisfied, // The constraint can be fully met
            PartiallySatisfied, // The constraint cannot be met, but satisfies the Exact, Min, and Max
            Unsatisfied, // The constraint cannot be met, and fails the Exact, Min, or Max
            Unconstrained, // There are no constraints that apply
        };

        enum ConstraintType
        {
            Sequence,
            Range,
        };

        CameraCapability(Capability capability);
        virtual ~CameraCapability() = default;

        std::string getName();

        virtual MeetsConstraint meetsConstraints(Napi::Object constraints)=0;
        virtual bool applyConstraints(Napi::Object constraints)=0;
        virtual Napi::Value asCapability(Napi::Env env)=0;
        virtual Napi::Value asSetting(Napi::Env env)=0;

        template <typename T>
        struct Constraint
        {
            std::optional<T> ideal;
            std::optional<T> exact;
            std::optional<T> min;
            std::optional<T> max;
        };

        template <typename T>
        Constraint<T> parseConstraint(Napi::Value value) = delete;

        template <>
        Constraint<uint32_t> parseConstraint(Napi::Value value);

        template <>
        Constraint<bool> parseConstraint(Napi::Value value);

        template <>
        Constraint<std::string> parseConstraint(Napi::Value value);

        template <typename T>
        Napi::Value asNapiValue(Napi::Env env, T value) = delete;

        template <>
        Napi::Value asNapiValue(Napi::Env env, uint32_t value);

        template <>
        Napi::Value asNapiValue(Napi::Env env, bool value);

        template <>
        Napi::Value asNapiValue(Napi::Env env, std::string value);

    protected:
        const Capability m_capability;
    };

    template <typename T>
    class CameraCapabilityTemplate : public CameraCapability
    {
    public:
        CameraCapabilityTemplate(Capability capability,
                                 T currentValue,
                                 T defaultValue,
                                 std::vector<T> acceptedValues,
                                 std::function<bool(T)> setterFunction = [](T){return true;});

        MeetsConstraint meetsConstraints(Napi::Object constraints);
        bool applyConstraints(Napi::Object constraints);
        Napi::Value asCapability(Napi::Env env);
        Napi::Value asSetting(Napi::Env env);

    private:
        ConstraintType getConstraintType();

        T m_currentValue;
        const T m_defaultValue;
        const std::vector<T> m_acceptedValues;
        std::function<bool(T)> m_setterFunction;
    };
}
