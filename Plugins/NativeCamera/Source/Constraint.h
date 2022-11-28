#pragma once

#include <napi/napi.h>
#include <optional>

// The constraint namespace is a collection of helpers to facilitate the web API Constraint pattern.
// A constraint is a request to set a capability to a specific value/range or an ideal target value
// that the platoform should try to match as closely as possible.
// More info on the constraint pattern can be found here:
// https://developer.mozilla.org/en-US/docs/Web/API/Media_Capture_and_Streams_API/Constraints#applying_constraints
namespace Babylon::Plugins::Constraint {
    enum Type
    {
        Sequence,
        Range,
    };

    template<typename T>
    struct ConstraintValue
    {
        std::optional<T> ideal{};
        std::optional<T> exact{};
        std::optional<T> min{};
        std::optional<T> max{};
    };

    // Default template, intentionally inaccessible as only explicitly defined specializations should be used
    template<typename T>
    ConstraintValue<T> ParseConstraint(Napi::Value value) = delete;
    template<typename T>
    Napi::Value AsNapiValue(Napi::Env env, T value) = delete;

    // Bool specialziation
    template<>
    ConstraintValue<bool> ParseConstraint<bool>(Napi::Value value);
    template<>
    Napi::Value AsNapiValue<bool>(Napi::Env env, bool value);

    // String specialization
    template<>
    ConstraintValue<std::string> ParseConstraint<std::string>(Napi::Value value);
    template<>
    Napi::Value AsNapiValue<std::string>(Napi::Env env, std::string value);

    // Int32 specialization
    template<>
    ConstraintValue<int32_t> ParseConstraint<int32_t>(Napi::Value value);
    template<>
    Napi::Value AsNapiValue<int32_t>(Napi::Env env, int32_t value);

    // Double specialization
    template<>
    ConstraintValue<double> ParseConstraint<double>(Napi::Value value);
    template<>
    Napi::Value AsNapiValue<double>(Napi::Env env, double value);
}
