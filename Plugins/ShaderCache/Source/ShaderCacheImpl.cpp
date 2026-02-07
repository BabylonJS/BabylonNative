#include "ShaderCacheImpl.h"

namespace
{
    void SaveString(std::ofstream& stream, const std::string& string)
    {
        uint32_t stringSize{static_cast<uint32_t>(string.size())};
        stream.write(reinterpret_cast<const char*>(&stringSize), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(string.data()), string.size());
    }

    void LoadString(std::ifstream& stream, std::string& string)
    {
        uint32_t stringSize;
        stream.read(reinterpret_cast<char*>(&stringSize), sizeof(uint32_t));
        string.resize(stringSize);
        stream.read(string.data(), stringSize);
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
    static const uint32_t CACHE_VERSION = 2;

    void ShaderCacheImpl::Clear()
    {
        m_cache.clear();
    }

    uint32_t ShaderCacheImpl::Save(std::ofstream& stream)
    {
        uint32_t cacheVersion{CACHE_VERSION};
        stream.write(reinterpret_cast<const char*>(&cacheVersion), sizeof(uint32_t));
        uint32_t cacheSize{static_cast<uint32_t>(m_cache.size())};
        stream.write(reinterpret_cast<const char*>(&cacheSize), sizeof(uint32_t));
        for (auto& entry : m_cache)
        {
            stream.write(reinterpret_cast<const char*>(&entry.first), sizeof(ShaderHash));
            const auto& info = entry.second;
            uint32_t vertexBytes{static_cast<uint32_t>(info->VertexBytes.size())};
            stream.write(reinterpret_cast<const char*>(&vertexBytes), sizeof(uint32_t));
            stream.write((const char*)info->VertexBytes.data(), info->VertexBytes.size());

            uint32_t fragmentBytes{static_cast<uint32_t>(info->FragmentBytes.size())};
            stream.write(reinterpret_cast<const char*>(&fragmentBytes), sizeof(uint32_t));
            stream.write((const char*)info->FragmentBytes.data(), info->FragmentBytes.size());

            uint32_t vertexAttributeLocationCount{static_cast<uint32_t>(info->VertexAttributeLocations.size())};
            stream.write(reinterpret_cast<const char*>(&vertexAttributeLocationCount), sizeof(uint32_t));
            for (auto& attributeLocation : info->VertexAttributeLocations)
            {
                SaveString(stream, attributeLocation.first);
                stream.write(reinterpret_cast<const char*>(&attributeLocation.second), sizeof(uint32_t));
            }

            uint32_t stageCount{static_cast<uint32_t>(info->UniformStages.size())};
            stream.write(reinterpret_cast<const char*>(&stageCount), sizeof(uint32_t));
            for (auto& uniformStages : info->UniformStages)
            {
                SaveString(stream, uniformStages.first);
                stream.write(reinterpret_cast<const char*>(&uniformStages.second), sizeof(uint8_t));
            }
        }
        return cacheSize;
    }

    uint32_t ShaderCacheImpl::Load(std::ifstream& stream)
    {
        uint32_t cacheVersion;
        stream.read(reinterpret_cast<char*>(&cacheVersion), sizeof(uint32_t));
        if (cacheVersion != CACHE_VERSION)
        {
            return 0;
        }

        uint32_t cacheSize;
        stream.read(reinterpret_cast<char*>(&cacheSize), sizeof(uint32_t));
        for (unsigned int i = 0; i < cacheSize; i++)
        {
            ShaderHash hash;
            stream.read(reinterpret_cast<char*>(&hash), sizeof(ShaderHash));
            std::shared_ptr<Graphics::BgfxShaderInfo> info = std::make_shared<Graphics::BgfxShaderInfo>();
            uint32_t vertexBytes;
            stream.read(reinterpret_cast<char*>(&vertexBytes), sizeof(uint32_t));
            info->VertexBytes.resize(vertexBytes);
            stream.read(reinterpret_cast<char*>(info->VertexBytes.data()), info->VertexBytes.size());
            uint32_t fragmentBytes;
            stream.read(reinterpret_cast<char*>(&fragmentBytes), sizeof(uint32_t));
            info->FragmentBytes.resize(fragmentBytes);
            stream.read(reinterpret_cast<char*>(info->FragmentBytes.data()), info->FragmentBytes.size());
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

            m_cache.emplace(hash, std::move(info));
        }
        return cacheSize;
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderCacheImpl::AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo)
    {
        return m_cache.try_emplace(Hash(vertexSource, fragmentSource), std::make_shared<Graphics::BgfxShaderInfo>(std::move(shaderInfo))).first->second;
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> ShaderCacheImpl::GetShader(std::string_view vertexSource, std::string_view fragmentSource)
    {
        const auto iter = m_cache.find(Hash(vertexSource, fragmentSource));
        return (iter == m_cache.end() ? nullptr : iter->second);
    }

    ShaderCacheImpl::ShaderHash ShaderCacheImpl::Hash(std::string_view vertexSource, std::string_view fragmentSource)
    {
        std::string normalizeVertexSource = NormalizeLineEndings(vertexSource);
        std::string normalizeFragmentSource = NormalizeLineEndings(fragmentSource);
        return {XXH3_64bits(normalizeVertexSource.data(), normalizeVertexSource.size()),
                XXH3_64bits(normalizeFragmentSource.data(), normalizeFragmentSource.size())};
    }
}
