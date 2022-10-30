#include "CameraCapability.h"

namespace Babylon::Plugins
{
    CameraCapability::CameraCapability(Capability capability)
        : m_capability{ capability }
    {
    };

    std::string CameraCapability::getName()
    {
        switch (m_capability)
        {
            case FacingMode: return "facingMode";
            case Torch: return "torch";
            case Zoom: return "zoom";
        }
    }

    template <typename T>
    CameraCapability::ConstraintType CameraCapabilityTemplate<T>::getConstraintType() {
        switch (m_capability)
        {
            case FacingMode: return CameraCapabilityTemplate::Sequence;
            case Torch: return CameraCapabilityTemplate::Sequence;
            case Zoom: return CameraCapabilityTemplate::Range;
        }

    }

    template <typename T>
    CameraCapabilityTemplate<T>::CameraCapabilityTemplate(
            CameraCapability::Capability capability,
            T currentValue,
            T defaultValue,
            std::vector<T> acceptedValues,
            std::function<bool(T)> setterFunction)
            : CameraCapability(capability)
            , m_currentValue{ currentValue }
            , m_defaultValue{ defaultValue }
            , m_acceptedValues{ acceptedValues }
            , m_setterFunction{ setterFunction }
    {
    }

    template <typename T>
    void CameraCapabilityTemplate<T>::addAsCapability(Napi::Object target) {
        auto env{ target.Env() };
        
        switch(getConstraintType())
        {
            case ConstraintType::Sequence:
            {
                auto capability{ Napi::Array::New(env, m_acceptedValues.size()) };
                for (uint32_t i = 0; i < m_acceptedValues.size(); i++) {
                    capability.Set(i, asNapiValue<T>(env, m_acceptedValues[i]));
                }

                target.Set(getName(),capability);
            }
            case ConstraintType::Range:
            {
                auto capability{ Napi::Object::New(env) };
                capability.Set("min", asNapiValue<T>(env, m_acceptedValues[0]));
                capability.Set("max", asNapiValue<T>(env, m_acceptedValues[1]));

                target.Set(getName(),capability);
            }
        }

    }

    template <typename T>
    void CameraCapabilityTemplate<T>::addAsSetting(Napi::Object target) {
        target.Set(getName(), asNapiValue<T>(target.Env(), m_currentValue));
    }

    template<typename T>
    CameraCapability::MeetsConstraint CameraCapabilityTemplate<T>::meetsConstraints(Napi::Object constraints) {
        // Get the constraint matching this capability
        Napi::Value constraintValue{ constraints.Get(getName()) };
        
        if (constraintValue.IsEmpty() || constraintValue.IsUndefined() || constraintValue.IsNull())
        {
            // The constraints don't contain any restrictions for this capability.
            return CameraCapability::MeetsConstraint::Unconstrained;
        }

        Constraint constraint{ parseConstraint<T>(constraintValue) };

        switch(getConstraintType())
        {
            case ConstraintType::Sequence:
            {
                if (!constraint.ideal.has_value() && !constraint.exact.has_value())
                {
                    // The constraint couldn't be parsed, but was configured. Consider it a failure to meet the constraint.
                    return CameraCapability::MeetsConstraint::Unsatisfied;
                }

                if (constraint.exact.has_value())
                {
                    // The exact value (if set) suppercedes the ideal value. The constraint is either fully satisfied if the exact value is in the allowed sequence, or unsatisfied if it's not.
                    return std::find(m_acceptedValues.begin(), m_acceptedValues.end(), constraint.exact.value()) != m_acceptedValues.end() ? CameraCapability::MeetsConstraint::FullySatisfied : CameraCapability::MeetsConstraint::Unsatisfied;
                }

                // If the ideal constraint is in the allowed sequence then the constraint is still fully satisifed. If it's not then the constraint is still partially satisfied as the constraint didn't specify an exact requirement
                return std::find(m_acceptedValues.begin(), m_acceptedValues.end(), constraint.ideal.value()) != m_acceptedValues.end() ? CameraCapability::MeetsConstraint::FullySatisfied : CameraCapability::MeetsConstraint::PartiallySatisfied;
            }
            case ConstraintType::Range:
            {
                if (!constraint.ideal.has_value() && !constraint.exact.has_value() && !constraint.min.has_value() && !constraint.max.has_value())
                {
                    // The constraint couldn't be parsed, but was configured. Consider it a failure to meet the constraint.
                    return CameraCapability::MeetsConstraint::Unsatisfied;
                }
                
                T minAccepted = m_acceptedValues[0];
                T maxAccepted = m_acceptedValues[1];

                if (constraint.exact.has_value())
                {
                    // The exact value has highest priority the constraint is either fully satisfied or unsatisfied based on it
                    return constraint.exact.value() >= minAccepted && constraint.exact.value() <= maxAccepted ? CameraCapability::MeetsConstraint::FullySatisfied : CameraCapability::MeetsConstraint::Unsatisfied;

                }
                if ((constraint.min.has_value() && constraint.min.value() < minAccepted))
                {
                    // The constraints min value is lower than this capabilities min accepted value. The constraint is unsatisfied.
                    return CameraCapability::MeetsConstraint::Unsatisfied;
                }
                if ((constraint.max.has_value() && constraint.max.value() > maxAccepted))
                {
                    // The constraints max value is higher than this capabilities max accepted value. The constraint is unsatisfied.
                    return CameraCapability::MeetsConstraint::Unsatisfied;
                }

                // The constraints Min, Max, and Exact values are satisfied or unconstrained. If the ideal value is unset or met then the constraint is fully met, otherwise it is still
                // partially satisifed as it doesn't break the min, max, or exact requirements.
                return constraint.ideal.has_value() && constraint.ideal.value() >= minAccepted && constraint.ideal.value() <= maxAccepted ? CameraCapability::MeetsConstraint::FullySatisfied : CameraCapability::MeetsConstraint::PartiallySatisfied;
            }
        }

    }

    template <typename T>
    bool CameraCapabilityTemplate<T>::applyConstraints(Napi::Object constraints)
    {
        auto constraintSatisfaction{ meetsConstraints(constraints) };
        
        switch (constraintSatisfaction)
        {
            case CameraCapability::MeetsConstraint::Unsatisfied:
                // The constraint falls outside of the allowed values for the capability and cannot be set
                return false;
            case CameraCapability::MeetsConstraint::PartiallySatisfied:
                // The constraint can't be set, but doesn't specify any exact, min, or max hard requirements, consider it a success
                return true;
            case CameraCapability::MeetsConstraint::Unconstrained:
                // The constraints don't contain any restrictions for this capability. Ensure it's value is reset to the default.
                if (m_currentValue != m_defaultValue && m_setterFunction(m_defaultValue))
                {
                    m_currentValue = m_defaultValue;
                }
                
                return true;
            case CameraCapability::MeetsConstraint::FullySatisfied:
                // Get the constraint matching this capability
                Napi::Value constraintValue{ constraints.Get(getName()) };
                
                Constraint constraint{parseConstraint<T>(constraintValue) };

                // The target value should fallback between the different possible constraint values in the order of exact, ideal, max, min
                T targetValue = constraint.exact.has_value() ? constraint.exact.value()
                        : constraint.ideal.has_value() ? constraint.ideal.value()
                        : constraint.max.has_value() ? constraint.max.value()
                        : constraint.min.value();

                if (m_currentValue != targetValue)
                {
                    if (!m_setterFunction(targetValue))
                    {
                        // The value was valid, but setting it failed
                        return false;
                    }
                    m_currentValue = targetValue;
                }
                return true;
        }
    }

    template <>
    CameraCapability::Constraint<bool> CameraCapability::parseConstraint<bool>(Napi::Value value) {
        std::optional<bool> ideal{}, exact{};

        if (value.IsBoolean())
        {
            ideal = value.As<Napi::Boolean>().Value();
        }
        else if (value.IsObject())
        {
            auto valueObject = value.As<Napi::Object>();
            if (valueObject.Get("ideal").IsBoolean())
            {
                ideal = valueObject.Get("ideal").As<Napi::Boolean>().Value();
            }
            if (valueObject.Get("exact").IsBoolean())
            {
                exact = valueObject.Get("exact").As<Napi::Boolean>().Value();
            }
        }

        return {ideal, exact, {}, {}};
    }

    template <>
    Napi::Value CameraCapability::asNapiValue<bool>(Napi::Env env, bool value) {
        return Napi::Boolean::New(env, value);
    }

    template <>
    CameraCapability::Constraint<std::string> CameraCapability::parseConstraint<std::string>(Napi::Value value) {
        std::optional<std::string> ideal{}, exact{};

        if (value.IsString())
        {
            ideal = value.As<Napi::String>().Utf8Value();
        }
        else if (value.IsObject())
        {
            auto valueObject = value.As<Napi::Object>();
            if (valueObject.Get("ideal").IsString())
            {
                ideal = valueObject.Get("ideal").As<Napi::String>().Utf8Value();
            }
            if (valueObject.Get("exact").IsString())
            {
                exact = valueObject.Get("exact").As<Napi::String>().Utf8Value();
            }
        }

        return {ideal, exact, {}, {}};
    }

    template <>
    Napi::Value CameraCapability::asNapiValue<std::string>(Napi::Env env, std::string value) {
        return Napi::String::New(env, value);
    }

    template <>
    CameraCapability::Constraint<uint32_t> CameraCapability::parseConstraint<uint32_t>(Napi::Value value)
    {
        std::optional<uint32_t> ideal{}, exact{}, min{}, max{};

        if (value.IsNumber())
        {
            ideal = value.As<Napi::Number>().Uint32Value();
        }
        else if (value.IsObject())
        {
            auto valueObject = value.As<Napi::Object>();
            if (valueObject.Get("ideal").IsNumber())
            {
                ideal = valueObject.Get("ideal").As<Napi::Number>().Uint32Value();
            }
            if (valueObject.Get("exact").IsNumber())
            {
                exact = valueObject.Get("exact").As<Napi::Number>().Uint32Value();
            }
            if (valueObject.Get("min").IsNumber())
            {
                min = valueObject.Get("min").As<Napi::Number>().Uint32Value();
            }
            if (valueObject.Get("max").IsNumber())
            {
                max = valueObject.Get("max").As<Napi::Number>().Uint32Value();
            }
        }

        return {ideal, exact, min, max};
    };

    template <>
    Napi::Value CameraCapability::asNapiValue<uint32_t>(Napi::Env env, uint32_t value)
    {
        return Napi::Number::New(env, value);
    }

    template <>
    CameraCapability::Constraint<double> CameraCapability::parseConstraint<double>(Napi::Value value)
    {
        std::optional<double> ideal{}, exact{}, min{}, max{};

        if (value.IsNumber())
        {
            ideal = value.As<Napi::Number>().DoubleValue();
        }
        else if (value.IsObject())
        {
            auto valueObject = value.As<Napi::Object>();
            if (valueObject.Get("ideal").IsNumber())
            {
                ideal = valueObject.Get("ideal").As<Napi::Number>().DoubleValue();
            }
            if (valueObject.Get("exact").IsNumber())
            {
                exact = valueObject.Get("exact").As<Napi::Number>().DoubleValue();
            }
            if (valueObject.Get("min").IsNumber())
            {
                min = valueObject.Get("min").As<Napi::Number>().DoubleValue();
            }
            if (valueObject.Get("max").IsNumber())
            {
                max = valueObject.Get("max").As<Napi::Number>().DoubleValue();
            }
        }

        return {ideal, exact, min, max};
    };

    template <>
    Napi::Value CameraCapability::asNapiValue<double>(Napi::Env env, double value)
    {
        return Napi::Number::New(env, value);
    }

    // Explicitly instantiate the supported template types. The alternative is to put all template
    // implemenations in the header file
    template class CameraCapabilityTemplate<bool>;
    template class CameraCapabilityTemplate<std::string>;
    template class CameraCapabilityTemplate<uint32_t>;
    template class CameraCapabilityTemplate<double>;
}
