#include <Babylon/ShaderCache.h>
#include <map>
#include "ShaderCompiler.h"
#include "ShaderCache.h"

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
}

namespace Babylon
{
    static const uint32_t CACHE_VERSION = 1;

    uint32_t ShaderCacheImpl::Serialize(std::ofstream& stream)
    {
        uint32_t cacheVersion{CACHE_VERSION};
        stream.write(reinterpret_cast<const char*>(&cacheVersion), sizeof(uint32_t));
        uint32_t cacheSize{static_cast<uint32_t>(Cache.size())};
        stream.write(reinterpret_cast<const char*>(&cacheSize), sizeof(uint32_t));
        for (auto& entry : Cache)
        {
            stream.write(reinterpret_cast<const char*>(&entry.first), sizeof(ShaderHash));
            const auto& infos = entry.second;
            uint32_t vertexBytes{static_cast<uint32_t>(infos.VertexBytes.size())};
            stream.write(reinterpret_cast<const char*>(&vertexBytes), sizeof(uint32_t));
            stream.write((const char*)infos.VertexBytes.data(), infos.VertexBytes.size());

            uint32_t fragmentBytes{static_cast<uint32_t>(infos.FragmentBytes.size())};
            stream.write(reinterpret_cast<const char*>(&fragmentBytes), sizeof(uint32_t));
            stream.write((const char*)infos.FragmentBytes.data(), infos.FragmentBytes.size());

            uint32_t vertexAttributeLocationCount{static_cast<uint32_t>(infos.VertexAttributeLocations.size())};
            stream.write(reinterpret_cast<const char*>(&vertexAttributeLocationCount), sizeof(uint32_t));
            for (auto& attributeLocation : infos.VertexAttributeLocations)
            {
                SaveString(stream, attributeLocation.first);
                stream.write(reinterpret_cast<const char*>(&attributeLocation.second), sizeof(uint32_t));
            }

            uint32_t stageCount{static_cast<uint32_t>(infos.UniformStages.size())};
            stream.write(reinterpret_cast<const char*>(&stageCount), sizeof(uint32_t));
            for (auto& uniformStages : infos.UniformStages)
            {
                SaveString(stream, uniformStages.first);
                stream.write(reinterpret_cast<const char*>(&uniformStages.second), sizeof(uint8_t));
            }
        }
        return cacheSize;
    }

    uint32_t ShaderCacheImpl::Deserialize(std::ifstream& stream)
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
            ShaderCompiler::BgfxShaderInfo infos;
            uint32_t vertexBytes;
            stream.read(reinterpret_cast<char*>(&vertexBytes), sizeof(uint32_t));
            infos.VertexBytes.resize(vertexBytes);
            stream.read(reinterpret_cast<char*>(infos.VertexBytes.data()), infos.VertexBytes.size());
            uint32_t fragmentBytes;
            stream.read(reinterpret_cast<char*>(&fragmentBytes), sizeof(uint32_t));
            infos.FragmentBytes.resize(fragmentBytes);
            stream.read(reinterpret_cast<char*>(infos.FragmentBytes.data()), infos.FragmentBytes.size());

            uint32_t vertexAttributeLocationCount;
            stream.read(reinterpret_cast<char*>(&vertexAttributeLocationCount), sizeof(uint32_t));
            for (unsigned int vertexAttributeLocation = 0; vertexAttributeLocation < vertexAttributeLocationCount; vertexAttributeLocation++)
            {
                std::string locationName;
                LoadString(stream, locationName);
                uint32_t locationIndex;
                stream.read(reinterpret_cast<char*>(&locationIndex), sizeof(uint32_t));
                infos.VertexAttributeLocations[locationName] = locationIndex;
            }

            uint32_t stageCount;
            stream.read(reinterpret_cast<char*>(&stageCount), sizeof(uint32_t));
            for (unsigned int stage = 0; stage < stageCount; stage++)
            {
                std::string stageName;
                LoadString(stream, stageName);
                uint8_t stageIndex;
                stream.read(reinterpret_cast<char*>(&stageIndex), sizeof(uint8_t));
                infos.UniformStages[stageName] = stageIndex;
            }

            Cache[hash] = infos;
        }
        return cacheSize;
    }

    const ShaderCompiler::BgfxShaderInfo* ShaderCacheImpl::GetShader(std::string_view vertexSource, std::string_view fragmentSource)
    {
        const ShaderHash hash{
            std::hash<std::string_view>{}(vertexSource),
            std::hash<std::string_view>{}(fragmentSource)};
        const auto iter{Cache.find(hash)};
        if (iter == Cache.end())
        {
            return nullptr;
        }
        return &iter->second;
    }

    void ShaderCacheImpl::AddShader(std::string_view vertexSource, std::string_view fragmentSource, ShaderCompiler::BgfxShaderInfo shaderInfo)
    {
        const ShaderHash hash{
            std::hash<std::string_view>{}(vertexSource),
            std::hash<std::string_view>{}(fragmentSource)};
        const auto iter{Cache.find(hash)};
        if (iter == Cache.end())
        {
            Cache[hash] = shaderInfo;
        }
    }

    ShaderCacheImpl* ShaderCacheImpl::GetImpl()
    {
        return Instance.get();
    }

    namespace ShaderCache
    {
        void Enabled(bool enabled)
        {
            if (enabled && !ShaderCacheImpl::GetImpl())
            {
                ShaderCacheImpl::Instance = std::make_unique<ShaderCacheImpl>();
            }
            else if (!enabled)
            {
                ShaderCacheImpl::Instance.reset();
            }
        }

        bool Enabled()
        {
            return ShaderCacheImpl::GetImpl() != nullptr;
        }

        uint32_t Serialize(std::ofstream& stream)
        {
            auto impl = ShaderCacheImpl::GetImpl();
            if (!impl)
            {
                // shall we throw here?
                return 0;
            }
            return impl->Serialize(stream);
        }

        uint32_t Deserialize(std::ifstream& stream)
        {
            auto impl = ShaderCacheImpl::GetImpl();
            if (!impl)
            {
                // shall we throw here?
                return 0;
            }
            return impl->Deserialize(stream);
        }
    }
}