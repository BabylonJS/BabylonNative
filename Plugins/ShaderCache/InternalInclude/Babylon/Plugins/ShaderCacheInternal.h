#pragma once

#include <Babylon/Graphics/BgfxShaderInfo.h>

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Babylon::Plugins::ShaderCache
{
    // Adds a shader to the cache with the given vertex and fragment shader sources and shader information.
    // Returns the added shader information from the cache.
    std::shared_ptr<Graphics::BgfxShaderInfo> AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo);

    // Gets the shader from the cache for the given vertex and fragment shader source code.
    // Returns the shader information from the cache or nullptr if not found.
    std::shared_ptr<Graphics::BgfxShaderInfo> GetShader(std::string_view vertexSource, std::string_view fragmentSource);
}
