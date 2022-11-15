#pragma once

#include <napi/napi.h>
#include <optional>

namespace Babylon::Plugins {
    namespace Constraint
    {
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
        template <typename T>
        Napi::Value asNapiValue(Napi::Env env, T value) = delete;

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
    }

    // The CameraCapability class fulfills the Capabilities, Constraints, and Settings web API pattern.
    // A Capability represents a camera feature as it's available values (either a min/max or a set of allowed values).
    // A Constraint represents a user request to set a camera feature to an ideal or exact value.
    // A Setting represents a camera features current value.
    // For a better understanding of the Capabilities, Constraints, and Settings pattern see this article:
    // https://developer.mozilla.org/en-US/docs/Web/API/Media_Capture_and_Streams_API/Constraints
    class CameraCapability {
    public:
        enum Feature
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

        CameraCapability(Feature capability);
        virtual ~CameraCapability() = default;

        std::string getName();

        virtual MeetsConstraint meetsConstraints(Napi::Object constraints)=0;
        virtual bool applyConstraints(Napi::Object constraints)=0;
        virtual void addAsCapability(Napi::Object target)=0;
        virtual void addAsSetting(Napi::Object target)=0;

    protected:
        const Feature m_feature;
    };

    template <typename T>
    class CameraCapabilityTemplate : public CameraCapability
    {
    public:
        CameraCapabilityTemplate(Feature feature,
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
