#pragma once

#include <napi/napi.h>
#include <optional>

namespace Babylon::Plugins {
    class CameraCapability {
    public:
        enum Capability
        {
            FacingMode,
            Torch,
            Zoom
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
        virtual void addAsCapability(Napi::Object target)=0;
        virtual void addAsSetting(Napi::Object target)=0;

        template <typename T>
        struct Constraint
        {
            std::optional<T> ideal;
            std::optional<T> exact;
            std::optional<T> min;
            std::optional<T> max;
        };

        template <typename T>
        static Constraint<T> parseConstraint(Napi::Value value) = delete;
        template <typename T>
        static Napi::Value asNapiValue(Napi::Env env, T value) = delete;

        // Uint32
        template <>
        Constraint<uint32_t> parseConstraint(Napi::Value value);
        template <>
        Napi::Value asNapiValue(Napi::Env env, uint32_t value);
        
        // Double
        template <>
        Constraint<double> parseConstraint(Napi::Value value);
        template <>
        Napi::Value asNapiValue(Napi::Env env, double value);

        // Bool
        template <>
        Constraint<bool> parseConstraint(Napi::Value value);
        template <>
        Napi::Value asNapiValue(Napi::Env env, bool value);

        // String
        template <>
        Constraint<std::string> parseConstraint(Napi::Value value);
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
        
        void addAsCapability(Napi::Object target);
        void addAsSetting(Napi::Object target);

    private:
        ConstraintType getConstraintType();

        T m_currentValue;
        const T m_defaultValue;
        const std::vector<T> m_acceptedValues;
        std::function<bool(T)> m_setterFunction;
    };
}
