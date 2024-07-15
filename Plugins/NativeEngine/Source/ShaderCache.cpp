#include "ShaderCache.h"
#include "xxhash.h"
#include <iostream>
#include <fstream>

namespace {
    XXH64_hash_t hashStringView(std::string_view stringView)
    {
        // Allocate a state struct. Do not just use malloc() or new.
        XXH3_state_t* state = XXH3_createState();
        assert(state != NULL && "Out of memory!");
        // Reset the state to start a new hashing session.
        XXH3_64bits_reset(state);
        // Read the file in chunks
        //while ((count = fread(buffer, 1, sizeof(buffer), f)) != 0) {
        //for (unsigned int i = 0; i < stringView.size(); i++)
            // Run update() as many times as necessary to process the data
        XXH3_64bits_update(state, stringView.data(), stringView.size());
        //}
        // Retrieve the finalized hash. This will not change the state.
        XXH64_hash_t result = XXH3_64bits_digest(state);
        // Free the state. Do not use free().
        XXH3_freeState(state);
        return result;
    }

    void SaveString(std::ofstream& stream, const std::string& string)
    {
        uint32_t stringSize{ static_cast<uint32_t>(string.size()) };
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

    ShaderCache::ShaderCache()
    {
        std::ifstream stream("shaderCache.bin", std::ios::binary);
        if (stream)
        {
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
        }
    }

    ShaderCache::~ShaderCache()
    {
        std::ofstream stream("shaderCache.bin", std::ios::binary);
        if (stream)
        {
            uint32_t cacheSize{ static_cast<uint32_t>(Cache.size()) };
            stream.write(reinterpret_cast<const char*>(&cacheSize), sizeof(uint32_t));
            for (auto& entry : Cache)
            {
                stream.write(reinterpret_cast<const char*>(&entry.first), sizeof(ShaderHash));
                const auto& infos = entry.second;
                uint32_t vertexBytes{ static_cast<uint32_t>(infos.VertexBytes.size()) };
                stream.write(reinterpret_cast<const char*>(&vertexBytes), sizeof(uint32_t));
                stream.write((const char*)infos.VertexBytes.data(), infos.VertexBytes.size());

                uint32_t fragmentBytes{ static_cast<uint32_t>(infos.FragmentBytes.size()) };
                stream.write(reinterpret_cast<const char*>(&fragmentBytes), sizeof(uint32_t));
                stream.write((const char*)infos.FragmentBytes.data(), infos.FragmentBytes.size());

                uint32_t vertexAttributeLocationCount{ static_cast<uint32_t>(infos.VertexAttributeLocations.size()) };
                stream.write(reinterpret_cast<const char*>(&vertexAttributeLocationCount), sizeof(uint32_t));
                for (auto& attributeLocation : infos.VertexAttributeLocations)
                {
                    SaveString(stream, attributeLocation.first);
                    stream.write(reinterpret_cast<const char*>(&attributeLocation.second), sizeof(uint32_t));
                }

                uint32_t stageCount{ static_cast<uint32_t>(infos.UniformStages.size()) };
                stream.write(reinterpret_cast<const char*>(&stageCount), sizeof(uint32_t));
                for (auto& uniformStages : infos.UniformStages)
                {
                    SaveString(stream, uniformStages.first);
                    stream.write(reinterpret_cast<const char*>(&uniformStages.second), sizeof(uint8_t));
                }
            }
        }
    }

    const ShaderCompiler::BgfxShaderInfo* ShaderCache::GetShader(std::string_view vertexSource, std::string_view fragmentSource)
    {
        const ShaderHash hash{
            hashStringView(vertexSource),
            hashStringView(fragmentSource)};
        const auto iter{Cache.find(hash)};
        if (iter == Cache.end())
        {
            return nullptr;
        }
        return &iter->second;
    }

    ShaderCompiler::BgfxShaderInfo* ShaderCache::AddShader(std::string_view vertexSource, std::string_view fragmentSource, ShaderCompiler::BgfxShaderInfo shaderInfo)
    {
        const ShaderHash hash{
            hashStringView(vertexSource),
            hashStringView(fragmentSource) };
        const auto iter{ Cache.find(hash) };
        if (iter == Cache.end())
        {
            Cache[hash] = shaderInfo;
        }
        return &Cache[hash];
    }
}