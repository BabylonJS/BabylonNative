#pragma once

#include <Babylon/Graphics/BgfxShaderInfo.h>

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Babylon::Plugins::ShaderCache
{
    // Adds a shader to the cache with the given vertex and fragment shader sources and shader information.
    // instancedAttributes identifies a divisor-driven instanced variant (see ShaderCompiler::Compile);
    // an empty map is the base program.
    // Returns the added shader information from the cache.
    std::shared_ptr<Graphics::BgfxShaderInfo> AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo,
        const std::map<std::string, uint32_t>& instancedAttributes = {});

    // Gets the shader from the cache for the given vertex and fragment shader source code.
    // Returns the shader information from the cache or nullptr if not found.
    std::shared_ptr<Graphics::BgfxShaderInfo> GetShader(std::string_view vertexSource, std::string_view fragmentSource,
        const std::map<std::string, uint32_t>& instancedAttributes = {});

    // Adds/gets the compute shader emulating a transform feedback vertex shader that captures
    // varyings (see ShaderCompiler::CompileTransformFeedback). Keyed separately from vertex/fragment programs.
    std::shared_ptr<Graphics::BgfxShaderInfo> AddTransformFeedbackShader(std::string_view vertexSource, const std::vector<std::string>& varyings, Graphics::BgfxShaderInfo shaderInfo);
    std::shared_ptr<Graphics::BgfxShaderInfo> GetTransformFeedbackShader(std::string_view vertexSource, const std::vector<std::string>& varyings);
}
