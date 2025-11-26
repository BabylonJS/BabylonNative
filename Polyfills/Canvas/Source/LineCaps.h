#pragma once
#include <regex>
#include "nanovg/nanovg.h"

namespace Babylon::Polyfills::Internal
{
    static NVGlineCap StringToLineCap(Napi::Env env, const std::string& lineCapString)
    {
        std::string str = lineCapString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

        static const std::unordered_map<std::string, NVGlineCap> lineCaps = {
            {"butt", NVG_BUTT},
            {"round", NVG_ROUND},
            {"square", NVG_SQUARE}};

		auto iter = lineCaps.find(str);
		if (iter != lineCaps.end())
		{
			auto lineCap = iter->second;
			return lineCap;
		}

        // fallback default 'butt'
        return NVG_BUTT;
    }

    static NVGlineCap StringToLineJoin(Napi::Env env, const std::string& lineJoinString)
    {
        std::string str = lineJoinString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

        static const std::unordered_map<std::string, NVGlineCap> lineJoins = {
            {"bevel", NVG_BEVEL},
            {"round", NVG_ROUND},
            {"miter", NVG_MITER}};

		auto iter = lineJoins.find(str);
		if (iter != lineJoins.end())
		{
			auto lineJoin = iter->second;
			return lineJoin;
		}

        // fallback default 'miter'
        return NVG_MITER;
    }
}
