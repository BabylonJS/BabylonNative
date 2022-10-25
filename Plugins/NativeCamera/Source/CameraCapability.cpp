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
            case Width: return "width";
            case Height: return "height";
            case FacingMode: return "facingMode";
            case Torch: return "torch";
        }
    }

    CameraCapability::Type CameraCapability::getType()
    {
        switch (m_capability) {
            case Width:
            case Height:
                return Type::UInt;
            case FacingMode:
                return Type::String;
            case Torch:
                return Type::Bool;
        }
    }

    CameraCapability::Capability CameraCapability::getCapability() {
        return m_capability;
    }

    CameraCapabilityBool::CameraCapabilityBool(CameraCapability::Capability capability, bool currentValue, bool defaultValue, std::vector<bool> acceptedValues, std::optional<std::function<bool(bool)>> setterFunction)
            : CameraCapability(capability)
            , m_currentValue{ currentValue }
            , m_defaultValue{ defaultValue }
            , m_acceptedValues{ acceptedValues }
            , m_setterFunction{ setterFunction}
    {
    }

    Napi::Value CameraCapabilityBool::asConstrainable(Napi::Env env) {
        auto constrainable{ Napi::Array::New(env, m_acceptedValues.size()) };
        for(uint32_t i=0; i < m_acceptedValues.size(); i++)
        {
            constrainable.Set(i, Napi::Boolean::New(env, m_acceptedValues[i]));
        }

        return std::move(constrainable);
    }

    Napi::Value CameraCapabilityBool::asSetting(Napi::Env env) {
        return Napi::Boolean::From(env, m_currentValue);
    }

    bool CameraCapabilityBool::setValue(Napi::Value value) {
        Constraint constraint{parseConstraint(value) };

        if (!constraint.ideal.has_value() && !constraint.exact.has_value())
        {
            // The constraint couldn't be parsed return false
            return false;
        }

        bool targetValue = constraint.exact.has_value() ? constraint.exact.value() : constraint.ideal.value();

        if (m_setterFunction.has_value() && std::find(m_acceptedValues.begin(), m_acceptedValues.end(), targetValue) != m_acceptedValues.end())
        {
            if (m_currentValue != targetValue)
            {
                if (!m_setterFunction.value()(targetValue))
                {
                    return false;
                }
                m_currentValue = targetValue;
            }
            return true;
        }

        // The new value could not be applied return false if it was an exact requirement
        return constraint.exact.has_value() ? false : true;
    }

    bool CameraCapabilityBool::resetValue() {
        if (m_setterFunction.has_value() && m_setterFunction.value()(m_defaultValue))
        {
            m_currentValue = m_defaultValue;
            return true;
        }

        return false;
    }

    CameraCapabilityBool::Constraint CameraCapabilityBool::parseConstraint(Napi::Value value) {
        std::optional<bool> ideal{};
        std::optional<bool> exact{};

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

        return {ideal, exact};
    }

    CameraCapabilityString::CameraCapabilityString(CameraCapability::Capability capability, std::string currentValue, std::string defaultValue, std::vector<std::string> acceptedValues)
            : CameraCapability(capability)
            , currentValue{ currentValue }
            , defaultValue{ defaultValue }
            , acceptedValues { acceptedValues }
    {
    }

    Napi::Value CameraCapabilityString::asConstrainable(Napi::Env env) {
        auto constrainable{ Napi::Array::New(env, acceptedValues.size()) };
        for(uint32_t i=0; i < acceptedValues.size(); i++)
        {
            constrainable.Set(i, Napi::String::New(env, acceptedValues[i]));
        }

        return std::move(constrainable);
    }

    Napi::Value CameraCapabilityString::asSetting(Napi::Env env) {
        return Napi::Boolean::From(env, currentValue);
    }

    bool CameraCapabilityString::setValue(Napi::Value /*value*/) {
        return false;
    }

    bool CameraCapabilityString::resetValue() {
        return false;
    }
}