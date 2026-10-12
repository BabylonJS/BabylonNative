#include "ShaderCacheImpl.h"

namespace
{
    void SaveString(std::ostream& stream, const std::string& string)
    {
        uint32_t stringSize{static_cast<uint32_t>(string.size())};
        stream.write(reinterpret_cast<const char*>(&stringSize), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(string.data()), string.size());
    }

    void LoadString(std::istream& stream, std::string& string)
    {
        uint32_t stringSize;
        stream.read(reinterpret_cast<char*>(&stringSize), sizeof(uint32_t));
        string.resize(stringSize);
        stream.read(string.data(), stringSize);
    }

    void SaveBytes(std::ostream& stream, const std::vector<uint8_t>& bytes)
    {
        uint32_t byteCount{static_cast<uint32_t>(bytes.size())};
        stream.write(reinterpret_cast<const char*>(&byteCount), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }

    void LoadBytes(std::istream& stream, std::vector<uint8_t>& bytes)
    {
        uint32_t byteCount{};
        stream.read(reinterpret_cast<char*>(&byteCount), sizeof(uint32_t));
        bytes.resize(byteCount);
        stream.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
    }

    std::string NormalizeLineEndings(std::string_view source)
    {
        std::string result;

        for (char ch : source)
        {
            if (ch != '\r')
            {
                result.push_back(ch);
            }
        }

        return result;
    }
}

namespace Babylon::Plugins::ShaderCache
{
    // 3: sampler uniforms are stored under their original GLSL name rather than the
    //    SPIRV-Cross-renamed identifier, which changes both the bgfx uniform table in
    //    Vertex/FragmentBytes and the keys of UniformStages.
    // 4: store the compiler-assigned built-in instance-data slots.
    // 5: shader binary packaging bumped to bgfx BGFX_SHADER_BIN_VERSION 12 (raw
    //    SRV/UAV masks + tex meta); stale v4 cache entries would be rejected by bgfx.
    // 6: FlipFragCoordY rewrites gl_FragCoord and injects bnFragCoordTargetSize;
    //    version-5 entries for those shaders skip compilation and omit the uniform.
    // 7: shader binaries are bgfx BGFX_SHADER_BIN_VERSION 13 and, on D3D, record each
    //    sampler's register index and its paired texture's dimension.
    // 8: preserve application uniform aliases and inject nearest-sampler state.
    // 9: bnFragCoordTargetSize became bnRenderTargetTransform, which vertex shaders also read
    //    to render cube faces in GL row order, and dFdy now scales by it.
    // 10: persist ComputeBytes, append a separately keyed section of transform feedback
    //     compute shaders, and key instanced variants by their routed attributes.
    static const uint32_t CACHE_VERSION = 10;

    void ShaderCacheImpl::Clear()
    {
        m_cache.clear();
        m_computeCache.clear();
    }

    uint32_t ShaderCacheImpl::Save(std::ostream& stream)
    {
        uint32_t cacheVersion{CACHE_VERSION};
        stream.write(reinterpret_cast<const char*>(&cacheVersion), sizeof(uint32_t));
        SaveEntries(stream, m_cache);
        SaveEntries(stream, m_computeCache);
        return static_cast<uint32_t>(m_cache.size() + m_computeCache.size());
    }

    uint32_t ShaderCacheImpl::Load(std::istream& stream)
    {
        uint32_t cacheVersion;
        stream.read(reinterpret_cast<char*>(&cacheVersion), sizeof(uint32_t));
        if (cacheVersion != CACHE_VERSION)
        {
            return 0;
        }

        const uint32_t programCount = LoadEntries(stream, m_cache);
        const uint32_t computeCount = LoadEntries(stream, m_computeCache);
        return programCount + computeCount;
    }

    void ShaderCacheImpl::SaveEntries(std::ostream& stream, const ShaderMap& entries)
    {
        uint32_t cacheSize{static_cast<uint32_t>(entries.size())};
        stream.write(reinterpret_cast<const char*>(&cacheSize), sizeof(uint32_t));
        for (auto& entry : entries)
        {
            stream.write(reinterpret_cast<const char*>(&entry.first), sizeof(ShaderHash));
            const auto& info = entry.second;
            SaveBytes(stream, info->VertexBytes);
            SaveBytes(stream, info->FragmentBytes);
            SaveBytes(stream, info->ComputeBytes);

            uint32_t vertexAttributeLocationCount{static_cast<uint32_t>(info->VertexAttributeLocations.size())};
            stream.write(reinterpret_cast<const char*>(&vertexAttributeLocationCount), sizeof(uint32_t));
            for (auto& attributeLocation : info->VertexAttributeLocations)
            {
                SaveString(stream, attributeLocation.first);
                stream.write(reinterpret_cast<const char*>(&attributeLocation.second), sizeof(uint32_t));
            }

            uint32_t builtInInstanceDataSlotCount{static_cast<uint32_t>(info->BuiltInInstanceDataSlots.size())};
            stream.write(reinterpret_cast<const char*>(&builtInInstanceDataSlotCount), sizeof(uint32_t));
            for (const auto& [name, slot] : info->BuiltInInstanceDataSlots)
            {
                SaveString(stream, name);
                stream.write(reinterpret_cast<const char*>(&slot), sizeof(uint32_t));
            }

            uint32_t stageCount{static_cast<uint32_t>(info->UniformStages.size())};
            stream.write(reinterpret_cast<const char*>(&stageCount), sizeof(uint32_t));
            for (auto& uniformStages : info->UniformStages)
            {
                SaveString(stream, uniformStages.first);
                stream.write(reinterpret_cast<const char*>(&uniformStages.second), sizeof(uint8_t));
            }
            const auto nameCount = static_cast<uint32_t>(info->UniformNames.size());
            stream.write(reinterpret_cast<const char*>(&nameCount), sizeof(nameCount));
            for (const auto& [compiledName, originalName] : info->UniformNames)
            {
                SaveString(stream, compiledName);
                SaveString(stream, originalName);
            }
        }
    }

    uint32_t ShaderCacheImpl::LoadEntries(std::istream& stream, ShaderMap& entries)
    {
        uint32_t cacheSize{};
        stream.read(reinterpret_cast<char*>(&cacheSize), sizeof(uint32_t));
        for (unsigned int i = 0; i < cacheSize; i++)
        {
            ShaderHash hash;
            stream.read(reinterpret_cast<char*>(&hash), sizeof(ShaderHash));
            std::shared_ptr<Graphics::BgfxShaderInfo> info = std::make_shared<Graphics::BgfxShaderInfo>();
            LoadBytes(stream, info->VertexBytes);
            LoadBytes(stream, info->FragmentBytes);
            LoadBytes(stream, info->ComputeBytes);
            uint32_t vertexAttributeLocationCount;
            stream.read(reinterpret_cast<char*>(&vertexAttributeLocationCount), sizeof(uint32_t));
            for (unsigned int vertexAttributeLocation = 0; vertexAttributeLocation < vertexAttributeLocationCount; vertexAttributeLocation++)
            {
                std::string locationName;
                LoadString(stream, locationName);
                uint32_t locationIndex;
                stream.read(reinterpret_cast<char*>(&locationIndex), sizeof(uint32_t));
                info->VertexAttributeLocations[locationName] = locationIndex;
            }

            uint32_t builtInInstanceDataSlotCount;
            stream.read(reinterpret_cast<char*>(&builtInInstanceDataSlotCount), sizeof(uint32_t));
            for (unsigned int index = 0; index < builtInInstanceDataSlotCount; ++index)
            {
                std::string name;
                LoadString(stream, name);
                uint32_t slot;
                stream.read(reinterpret_cast<char*>(&slot), sizeof(uint32_t));
                info->BuiltInInstanceDataSlots[name] = slot;
            }

            uint32_t stageCount;
            stream.read(reinterpret_cast<char*>(&stageCount), sizeof(uint32_t));
            for (unsigned int stage = 0; stage < stageCount; stage++)
            {
                std::string stageName;
                LoadString(stream, stageName);
                uint8_t stageIndex;
                stream.read(reinterpret_cast<char*>(&stageIndex), sizeof(uint8_t));
                info->UniformStages[stageName] = stageIndex;
            }

            uint32_t nameCount{};
            stream.read(reinterpret_cast<char*>(&nameCount), sizeof(nameCount));
            for (uint32_t index = 0; index < nameCount; ++index)
            {
                std::string compiledName;
                std::string originalName;
                LoadString(stream, compiledName);
                LoadString(stream, originalName);
                info->UniformNames.emplace(std::move(compiledName), std::move(originalName));
            }
            entries.emplace(hash, std::move(info));
        }
        return cacheSize;
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderCacheImpl::AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo, const std::map<std::string, uint32_t>& instancedAttributes)
    {
        return m_cache.try_emplace(Hash(vertexSource, fragmentSource, instancedAttributes), std::make_shared<Graphics::BgfxShaderInfo>(std::move(shaderInfo))).first->second;
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderCacheImpl::GetShader(std::string_view vertexSource, std::string_view fragmentSource, const std::map<std::string, uint32_t>& instancedAttributes)
    {
        const auto iter = m_cache.find(Hash(vertexSource, fragmentSource, instancedAttributes));
        return (iter == m_cache.end() ? nullptr : iter->second);
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderCacheImpl::AddTransformFeedbackShader(std::string_view vertexSource, const std::vector<std::string>& varyings, Graphics::BgfxShaderInfo shaderInfo)
    {
        return m_computeCache.try_emplace(Hash(vertexSource, varyings), std::make_shared<Graphics::BgfxShaderInfo>(std::move(shaderInfo))).first->second;
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderCacheImpl::GetTransformFeedbackShader(std::string_view vertexSource, const std::vector<std::string>& varyings)
    {
        const auto iter = m_computeCache.find(Hash(vertexSource, varyings));
        return (iter == m_computeCache.end() ? nullptr : iter->second);
    }

    ShaderCacheImpl::ShaderHash ShaderCacheImpl::Hash(std::string_view vertexSource, std::string_view fragmentSource, const std::map<std::string, uint32_t>& instancedAttributes)
    {
        std::string normalizeVertexSource = NormalizeLineEndings(vertexSource);
        // Instanced variants share the base sources; the routed attributes select the variant.
        // The NUL separator cannot occur in GLSL source, so variant keys never alias base keys.
        if (!instancedAttributes.empty())
        {
            normalizeVertexSource.push_back('\0');
            normalizeVertexSource += "instanced:";
            for (const auto& [name, location] : instancedAttributes)
            {
                normalizeVertexSource += name + "=" + std::to_string(location) + ";";
            }
        }
        std::string normalizeFragmentSource = NormalizeLineEndings(fragmentSource);
        return {XXH3_64bits(normalizeVertexSource.data(), normalizeVertexSource.size()),
                XXH3_64bits(normalizeFragmentSource.data(), normalizeFragmentSource.size())};
    }

    ShaderCacheImpl::ShaderHash ShaderCacheImpl::Hash(std::string_view vertexSource, const std::vector<std::string>& varyings)
    {
        // The NUL-separated varyings suffix keeps programs capturing different varyings distinct.
        std::string key = NormalizeLineEndings(vertexSource);
        key.push_back('\0');
        key += "transform-feedback:";
        for (const auto& varying : varyings)
        {
            key += varying + ",";
        }
        const auto hash = XXH3_128bits(key.data(), key.size());
        return {hash.low64, hash.high64};
    }
}
