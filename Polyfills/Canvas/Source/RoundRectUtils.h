#pragma once

#include <array>
#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    constexpr size_t ROUND_RECT_CORNER_COMPONENT_COUNT = 8;
    constexpr uint32_t UNIFORM_RADII_COUNT = 1;
    constexpr uint32_t OPPOSING_RADII_COUNT = 2;
    constexpr uint32_t THREE_RADII_COUNT = 3;
    constexpr uint32_t CORNER_RADII_COUNT = 4;

    enum class RoundRectRadiiType
    {
        Uniform,
        Varying,
        Elliptic,
    };

    struct RoundRectRadii
    {
        RoundRectRadiiType type;
        std::array<float, ROUND_RECT_CORNER_COMPONENT_COUNT> values{};
    };

    inline RoundRectRadii ParseRoundRectRadii(Napi::Env env, const Napi::Value& radii)
    {
        if (radii.IsNumber())
        {
            return {RoundRectRadiiType::Uniform, {radii.As<Napi::Number>().FloatValue()}};
        }

        if (radii.IsArray())
        {
            const auto array = radii.As<Napi::Array>();
            switch (array.Length())
            {
                case UNIFORM_RADII_COUNT:
                    return {RoundRectRadiiType::Uniform, {array[0u].As<Napi::Number>().FloatValue()}};
                case OPPOSING_RADII_COUNT:
                {
                    const auto topLeftBottomRight = array[0u].As<Napi::Number>().FloatValue();
                    const auto topRightBottomLeft = array[1u].As<Napi::Number>().FloatValue();
                    return {RoundRectRadiiType::Varying, {topLeftBottomRight, topRightBottomLeft, topLeftBottomRight, topRightBottomLeft}};
                }
                case THREE_RADII_COUNT:
                {
                    const auto topLeft = array[0u].As<Napi::Number>().FloatValue();
                    const auto topRightBottomLeft = array[1u].As<Napi::Number>().FloatValue();
                    const auto bottomRight = array[2u].As<Napi::Number>().FloatValue();
                    return {RoundRectRadiiType::Varying, {topLeft, topRightBottomLeft, bottomRight, topRightBottomLeft}};
                }
                case CORNER_RADII_COUNT:
                {
                    const auto topLeft = array[0u].As<Napi::Number>().FloatValue();
                    const auto topRight = array[1u].As<Napi::Number>().FloatValue();
                    const auto bottomRight = array[2u].As<Napi::Number>().FloatValue();
                    const auto bottomLeft = array[3u].As<Napi::Number>().FloatValue();
                    return {RoundRectRadiiType::Varying, {
                        topLeft, topRight, bottomRight, bottomLeft}};
                }
                default:
                    throw Napi::Error::New(env, "Invalid number of parameters for radii");
            }
        }

        if (radii.IsObject())
        {
            const auto point = radii.As<Napi::Object>();
            const auto x = point.Get("x").As<Napi::Number>().FloatValue();
            const auto y = point.Get("y").As<Napi::Number>().FloatValue();
            return {RoundRectRadiiType::Elliptic, {x, y, x, y, x, y, x, y}};
        }

        throw Napi::Error::New(env, "Invalid radii parameter");
    }
}
