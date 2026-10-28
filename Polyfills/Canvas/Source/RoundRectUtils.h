#pragma once

#include <array>
#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    enum class RoundRectRadiiType
    {
        Uniform,
        Varying,
        Elliptic,
    };

    struct RoundRectRadii
    {
        RoundRectRadiiType type;
        std::array<float, 8> values{};
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
                case 1:
                    return {RoundRectRadiiType::Uniform, {array[0u].As<Napi::Number>().FloatValue()}};
                case 2:
                {
                    const auto topLeftBottomRight = array[0u].As<Napi::Number>().FloatValue();
                    const auto topRightBottomLeft = array[1u].As<Napi::Number>().FloatValue();
                    return {RoundRectRadiiType::Varying, {topLeftBottomRight, topRightBottomLeft, topLeftBottomRight, topRightBottomLeft}};
                }
                case 3:
                {
                    const auto topLeft = array[0u].As<Napi::Number>().FloatValue();
                    const auto topRightBottomLeft = array[1u].As<Napi::Number>().FloatValue();
                    const auto bottomRight = array[2u].As<Napi::Number>().FloatValue();
                    return {RoundRectRadiiType::Varying, {topLeft, topRightBottomLeft, bottomRight, topRightBottomLeft}};
                }
                case 4:
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
