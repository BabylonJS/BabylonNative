#include "Constraint.h"

namespace Babylon::Plugins::Constraint {
    template<>
    ConstraintValue<bool> ParseConstraint<bool>(Napi::Value value) {
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

    template<>
    Napi::Value AsNapiValue<bool>(Napi::Env env, bool value) {
        return Napi::Boolean::New(env, value);
    }

    template<>
    ConstraintValue<std::string> ParseConstraint<std::string>(Napi::Value value) {
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

    template<>
    Napi::Value AsNapiValue<std::string>(Napi::Env env, std::string value) {
        return Napi::String::New(env, value);
    }

    template<>
    ConstraintValue<int32_t> ParseConstraint<int32_t>(Napi::Value value)
    {
        std::optional<int32_t> ideal{}, exact{}, min{}, max{};

        if (value.IsNumber())
        {
            ideal = value.As<Napi::Number>().Uint32Value();
        }
        else if (value.IsObject())
        {
            auto valueObject = value.As<Napi::Object>();
            if (valueObject.Get("ideal").IsNumber())
            {
                ideal = valueObject.Get("ideal").As<Napi::Number>().Int32Value();
            }
            if (valueObject.Get("exact").IsNumber())
            {
                exact = valueObject.Get("exact").As<Napi::Number>().Int32Value();
            }
            if (valueObject.Get("min").IsNumber())
            {
                min = valueObject.Get("min").As<Napi::Number>().Int32Value();
            }
            if (valueObject.Get("max").IsNumber())
            {
                max = valueObject.Get("max").As<Napi::Number>().Int32Value();
            }
        }

        return {ideal, exact, min, max};
    }

    template<>
    Napi::Value AsNapiValue<int32_t>(Napi::Env env, int32_t value)
    {
        return Napi::Number::New(env, value);
    }

    template<>
    ConstraintValue<double> ParseConstraint<double>(Napi::Value value)
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
    }

    template<>
    Napi::Value AsNapiValue<double>(Napi::Env env, double value)
    {
        return Napi::Number::New(env, value);
    }
}
