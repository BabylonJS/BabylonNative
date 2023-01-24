#include "Capability.h"
#include <assert.h>

namespace Babylon::Plugins
{
    Capability::Capability(Feature capability)
        : m_feature{capability}
    {
    }

    std::string Capability::GetName()
    {
        switch (m_feature)
        {
            case FacingMode:
                return "facingMode";
            case Torch:
                return "torch";
            case Zoom:
                return "zoom";
            default:
                assert(false);
                return "";
        }
    }

    template<typename T>
    Constraint::Type CameraCapabilityTemplate<T>::GetConstraintType()
    {
        switch (m_feature)
        {
            case FacingMode:
                return Constraint::Type::Sequence;
            case Torch:
                return Constraint::Type::Sequence;
            case Zoom:
                return Constraint::Type::Range;
            default:
                assert(false);
                return Constraint::Type::Sequence;
        }
    }

    template<typename T>
    CameraCapabilityTemplate<T>::CameraCapabilityTemplate(
    Capability::Feature feature,
    T currentValue,
    T defaultValue,
    std::vector<T> acceptedValues,
    std::function<bool(T)> setterFunction)
        : Capability(feature)
        , m_currentValue{currentValue}
        , m_defaultValue{defaultValue}
        , m_acceptedValues{acceptedValues}
        , m_setterFunction{setterFunction}
    {
    }

    template<typename T>
    void CameraCapabilityTemplate<T>::AddAsCapability(Napi::Object target)
    {
        auto env{target.Env()};

        switch (GetConstraintType())
        {
            case Constraint::Type::Sequence:
            {
                auto capability{Napi::Array::New(env, m_acceptedValues.size())};
                for (uint32_t i = 0; i < m_acceptedValues.size(); i++)
                {
                    capability.Set(i, Constraint::AsNapiValue<T>(env, m_acceptedValues[i]));
                }

                target.Set(GetName(), capability);
                return;
            }
            case Constraint::Type::Range:
            {
                auto capability{Napi::Object::New(env)};
                capability.Set("min", Constraint::AsNapiValue<T>(env, m_acceptedValues[0]));
                capability.Set("max", Constraint::AsNapiValue<T>(env, m_acceptedValues[1]));

                target.Set(GetName(), capability);
                return;
            }
            default:
                assert(false);
                return;
        }
    }

    template<typename T>
    void CameraCapabilityTemplate<T>::AddAsSetting(Napi::Object target)
    {
        target.Set(GetName(), Constraint::AsNapiValue<T>(target.Env(), m_currentValue));
    }

    template<typename T>
    Capability::MeetsConstraint CameraCapabilityTemplate<T>::MeetsConstraints(Napi::Object constraints)
    {
        // Get the constraint matching this capability
        Napi::Value constraintValue{constraints.Get(GetName())};

        if (constraintValue.IsEmpty() || constraintValue.IsUndefined() || constraintValue.IsNull())
        {
            // The constraints don't contain any restrictions for this capability.
            return Capability::MeetsConstraint::Unconstrained;
        }

        Constraint::ConstraintValue<T> constraint{Constraint::ParseConstraint<T>(constraintValue)};

        switch (GetConstraintType())
        {
            case Constraint::Type::Sequence:
            {
                if (!constraint.ideal.has_value() && !constraint.exact.has_value())
                {
                    // The constraint couldn't be parsed, but was configured. Consider it a failure to meet the constraint.
                    return Capability::MeetsConstraint::Unsatisfied;
                }

                if (constraint.exact.has_value())
                {
                    // The exact value (if set) suppercedes the ideal value. The constraint is either fully satisfied if the exact value is in the allowed sequence, or unsatisfied if it's not.
                    return std::find(m_acceptedValues.begin(), m_acceptedValues.end(), constraint.exact.value()) != m_acceptedValues.end() ? Capability::MeetsConstraint::FullySatisfied : Capability::MeetsConstraint::Unsatisfied;
                }

                // If the ideal constraint is in the allowed sequence then the constraint is still fully satisifed. If it's not then the constraint is still partially satisfied as the constraint didn't specify an exact requirement
                return std::find(m_acceptedValues.begin(), m_acceptedValues.end(), constraint.ideal.value()) != m_acceptedValues.end() ? Capability::MeetsConstraint::FullySatisfied : Capability::MeetsConstraint::PartiallySatisfied;
            }
            case Constraint::Type::Range:
            {
                if (!constraint.ideal.has_value() && !constraint.exact.has_value() && !constraint.min.has_value() && !constraint.max.has_value())
                {
                    // The constraint couldn't be parsed, but was configured. Consider it a failure to meet the constraint.
                    return Capability::MeetsConstraint::Unsatisfied;
                }

                T minAccepted = m_acceptedValues[0];
                T maxAccepted = m_acceptedValues[1];

                if (constraint.exact.has_value())
                {
                    // The exact value has highest priority the constraint is either fully satisfied or unsatisfied based on it
                    return constraint.exact.value() >= minAccepted && constraint.exact.value() <= maxAccepted ? Capability::MeetsConstraint::FullySatisfied : Capability::MeetsConstraint::Unsatisfied;
                }
                if ((constraint.min.has_value() && constraint.min.value() < minAccepted))
                {
                    // The constraints min value is lower than this capabilities min accepted value. The constraint is unsatisfied.
                    return Capability::MeetsConstraint::Unsatisfied;
                }
                if ((constraint.max.has_value() && constraint.max.value() > maxAccepted))
                {
                    // The constraints max value is higher than this capabilities max accepted value. The constraint is unsatisfied.
                    return Capability::MeetsConstraint::Unsatisfied;
                }

                // The constraints Min, Max, and Exact values are satisfied or unconstrained. If the ideal value is unset or met then the constraint is fully met, otherwise it is still
                // partially satisifed as it doesn't break the min, max, or exact requirements.
                return constraint.ideal.has_value() && constraint.ideal.value() >= minAccepted && constraint.ideal.value() <= maxAccepted ? Capability::MeetsConstraint::FullySatisfied : Capability::MeetsConstraint::PartiallySatisfied;
            }
            default:
                assert(false);
                return Capability::MeetsConstraint::Unsatisfied;
        }
    }

    template<typename T>
    bool CameraCapabilityTemplate<T>::ApplyConstraints(Napi::Object constraints)
    {
        auto constraintSatisfaction{MeetsConstraints(constraints)};

        switch (constraintSatisfaction)
        {
            case Capability::MeetsConstraint::Unsatisfied:
                // The constraint falls outside of the allowed values for the capability and cannot be set
                return false;
            case Capability::MeetsConstraint::PartiallySatisfied:
                // The constraint can't be set, but doesn't specify any exact, min, or max hard requirements, consider it a success
                return true;
            case Capability::MeetsConstraint::Unconstrained:
                // The constraints don't contain any restrictions for this capability. Ensure it's value is reset to the default.
                if (m_currentValue != m_defaultValue && m_setterFunction(m_defaultValue))
                {
                    m_currentValue = m_defaultValue;
                }

                return true;
            case Capability::MeetsConstraint::FullySatisfied:
            {
                // Get the constraint matching this capability
                Napi::Value constraintValue{constraints.Get(GetName())};

                Constraint::ConstraintValue<T> constraint{Constraint::ParseConstraint<T>(constraintValue)};

                // The target value should fallback between the different possible constraint values in the order of exact, ideal, max, min
                T targetValue =
                constraint.exact.has_value() ? constraint.exact.value() : constraint.ideal.has_value() ? constraint.ideal.value() : constraint.max.has_value() ? constraint.max.value() : constraint.min.value();

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
            default:
                assert(false);
                return false;
        }
    }

    // Explicitly instantiate the supported template types. The alternative is to put all template
    // implemenations in the header file
    template class CameraCapabilityTemplate<bool>;
    template class CameraCapabilityTemplate<std::string>;
    template class CameraCapabilityTemplate<int32_t>;
    template class CameraCapabilityTemplate<double>;
}
