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

    struct ShaderHash
    {
        size_t vertexHash;
        size_t fragmentHash;
        bool operator < (const ShaderHash& other) const {
            if (vertexHash < other.vertexHash || (vertexHash == other.vertexHash && fragmentHash < other.fragmentHash))
                return true;
            return false;
        }
    };
}

namespace Babylon
{
    static const uint32_t CACHE_VERSION = 1;

    void ShaderCache::Impl::SerializeOutTo(std::ofstream& stream)
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
    }

    uint32_t ShaderCache::Impl::SerializeInFrom(std::ifstream& stream)
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

    const ShaderCompiler::BgfxShaderInfo* ShaderCache::Impl::GetShader(std::string_view vertexSource, std::string_view fragmentSource)
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

    ShaderCompiler::BgfxShaderInfo* ShaderCache::Impl::AddShader(std::string_view vertexSource, std::string_view fragmentSource, ShaderCompiler::BgfxShaderInfo shaderInfo)
    {
        const ShaderHash hash{
            std::hash<std::string_view>{}(vertexSource),
            std::hash<std::string_view>{}(fragmentSource)};
        const auto iter{Cache.find(hash)};
        if (iter == Cache.end())
        {
            Cache[hash] = shaderInfo;
        }
        return &Cache[hash];
    }

    static std::unique_ptr<ShaderCache::Impl> Instance{};
    ShaderCache::ShaderCache()
    {
        if (!Instance)
        {
            Instance = std::make_unique<ShaderCache::Impl>();
        }
    }

    ShaderCache::~ShaderCache()
    {
        Instance.reset();
    }

    ShaderCache::Impl* ShaderCache::GetImpl()
    {
        return Instance.get();
    }

    void ShaderCache::SerializeOutTo(std::ofstream& stream)
    {
        GetImpl()->SerializeOutTo(stream);
    }

    uint32_t ShaderCache::SerializeInFrom(std::ifstream& stream)
    {
        return GetImpl()->SerializeInFrom(stream);
    }
}