/**
 * Shader Tool
 *
 * A cross-platform console tool that compiles GLSL shaders using
 * the ShaderCompiler component and saves the compiled results using ShaderCache.
 *
 * Usage:
 *   ShaderTool -o <output_file> [-v <vertex> <varyings>]... [[-i <attributes>] <vertex1> <fragment1> ...]
 */

#include <Babylon/Plugins/ShaderCompiler.h>
#include <Babylon/Plugins/ShaderCacheInternal.h>
#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Graphics/BgfxShaderInfo.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <sstream>
#include <vector>

namespace
{
    struct ShaderPair
    {
        std::filesystem::path vertexPath;
        std::filesystem::path fragmentPath;
        // Divisor-driven instanced attribute names; when set, the instanced variant is cached too.
        std::vector<std::string> instancedAttributes;
    };

    struct TransformFeedbackShader
    {
        std::filesystem::path vertexPath;
        std::vector<std::string> varyings;
    };

    std::vector<std::string> SplitList(std::string_view list)
    {
        std::vector<std::string> items;
        std::stringstream stream{std::string{list}};
        std::string item;
        while (std::getline(stream, item, ','))
        {
            if (!item.empty())
            {
                items.push_back(item);
            }
        }
        return items;
    }

    // Mirrors the runtime draw path (VertexBuffer::CreateInstanceDataLayout + NativeEngine::DrawInternal):
    // built-in instance attributes keep their compiler-assigned slots, and the remaining instanced
    // attributes take the following slots in descending attribute-location order.
    std::map<std::string, uint32_t> ResolveInstancedAttributes(const Babylon::Graphics::BgfxShaderInfo& baseShader, const std::vector<std::string>& names)
    {
        uint32_t builtInCount = static_cast<uint32_t>(baseShader.BuiltInInstanceDataSlots.size());
        if (builtInCount == 0)
        {
            for (const auto& [name, location] : baseShader.VertexAttributeLocations)
            {
                builtInCount += Babylon::Graphics::IsBuiltInInstanceAttributeName(name) ? 1 : 0;
            }
        }

        std::map<uint32_t, std::string> generic;
        for (const auto& name : names)
        {
            const auto location = baseShader.VertexAttributeLocations.find(name);
            if (location == baseShader.VertexAttributeLocations.end())
            {
                throw std::runtime_error("Instanced attribute is not a vertex shader input: " + name);
            }
            if (!Babylon::Graphics::IsBuiltInInstanceAttributeName(name))
            {
                generic.emplace(location->second, name);
            }
        }

        std::map<std::string, uint32_t> instancedAttributes;
        uint32_t slot = builtInCount;
        for (auto attribute = generic.rbegin(); attribute != generic.rend(); ++attribute, ++slot)
        {
            if (slot >= Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT)
            {
                throw std::runtime_error("Too many instanced attributes");
            }
            instancedAttributes.emplace(attribute->second, Babylon::Graphics::INSTANCE_DATA_FIRST_LOCATION - slot);
        }
        return instancedAttributes;
    }

    void PrintUsage(const char* programName)
    {
        std::cerr << "Babylon Native Shader Tool" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  " << programName << " -o <output_file> [-v <vertex> <varyings>]... [[-i <attributes>] <vertex1> <fragment1> ...]" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  -o <output_file>         Path to the output compiled shader cache file" << std::endl;
        std::cerr << "  -v <vertex> <varyings>   Transform feedback vertex shader and its comma-separated captured" << std::endl;
        std::cerr << "                           varyings (interleaved, in capture order); may be repeated" << std::endl;
        std::cerr << "  -i <attributes>          Comma-separated attributes the next vertex/fragment pair reads with" << std::endl;
        std::cerr << "                           a vertex divisor; caches that instanced variant as well" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Arguments:" << std::endl;
        std::cerr << "  <vertex> <fragment>   Pairs of vertex and fragment shader source files (GLSL)" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Examples:" << std::endl;
        std::cerr << "  " << programName << " -o cache.bin vertex.glsl fragment.glsl" << std::endl;
        std::cerr << "  " << programName << " -o cache.bin v1.glsl f1.glsl v2.glsl f2.glsl" << std::endl;
        std::cerr << "  " << programName << " -o cache.bin -v update.glsl outPosition,outAge -i position,age v1.glsl f1.glsl" << std::endl;
    }

    std::string ReadFileContents(const std::filesystem::path& filePath)
    {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filePath.string());
        }

        const auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string contents(static_cast<size_t>(fileSize), '\0');
        if (!file.read(contents.data(), fileSize))
        {
            throw std::runtime_error("Failed to read file: " + filePath.string());
        }

        return contents;
    }

    bool ValidateFilePath(const std::filesystem::path& filePath, const char* shaderType)
    {
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "Error: " << shaderType << " file does not exist: " << filePath.string() << std::endl;
            return false;
        }

        if (!std::filesystem::is_regular_file(filePath))
        {
            std::cerr << "Error: " << shaderType << " path is not a regular file: " << filePath.string() << std::endl;
            return false;
        }

        return true;
    }

    bool ValidateOutputPath(const std::filesystem::path& outputPath)
    {
        const auto parentPath = outputPath.parent_path();
        if (!parentPath.empty() && !std::filesystem::exists(parentPath))
        {
            std::cerr << "Error: Output directory does not exist: " << parentPath.string() << std::endl;
            return false;
        }

        return true;
    }

    bool ParseArguments(int argc, char* argv[], std::filesystem::path& outputPath, std::vector<ShaderPair>& shaderPairs,
        std::vector<TransformFeedbackShader>& transformFeedbackShaders)
    {
        bool hasOutput = false;
        std::vector<const char*> remaining;
        std::map<size_t, std::vector<std::string>> instancedAttributes;
        for (int i = 1; i < argc; i++)
        {
            if (std::strcmp(argv[i], "-v") == 0)
            {
                if (i + 2 >= argc)
                {
                    std::cerr << "Error: -v requires a vertex shader and a varyings list" << std::endl;
                    return false;
                }
                TransformFeedbackShader shader{argv[i + 1], SplitList(argv[i + 2])};
                if (shader.varyings.empty())
                {
                    std::cerr << "Error: -v requires at least one varying" << std::endl;
                    return false;
                }
                transformFeedbackShaders.push_back(std::move(shader));
                i += 2;
                continue;
            }
            if (std::strcmp(argv[i], "-i") == 0)
            {
                if (i + 1 >= argc || remaining.size() % 2 != 0)
                {
                    std::cerr << "Error: -i requires an attribute list and must precede a vertex/fragment pair" << std::endl;
                    return false;
                }
                instancedAttributes[remaining.size() / 2] = SplitList(argv[++i]);
                continue;
            }
            if (std::strcmp(argv[i], "-o") == 0)
            {
                if (i + 1 >= argc)
                {
                    std::cerr << "Error: Missing value for -o option" << std::endl;
                    return false;
                }
                if (hasOutput)
                {
                    std::cerr << "Error: -o may only be specified once" << std::endl;
                    return false;
                }
                hasOutput = true;
                outputPath = argv[++i];
                continue;
            }
            remaining.push_back(argv[i]);
        }

        if (!hasOutput)
        {
            std::cerr << "Error: Missing required -o <output_file> option" << std::endl;
            return false;
        }

        if (remaining.empty() && transformFeedbackShaders.empty())
        {
            std::cerr << "Error: No shader files specified" << std::endl;
            return false;
        }

        // Positional pairs
        if (remaining.size() % 2 != 0)
        {
            std::cerr << "Error: Shader files must be specified in vertex/fragment pairs" << std::endl;
            return false;
        }

        for (size_t i = 0; i < remaining.size(); i += 2)
        {
            shaderPairs.push_back({std::filesystem::path(remaining[i]), std::filesystem::path(remaining[i + 1]), {}});
        }

        for (auto& [pairIndex, names] : instancedAttributes)
        {
            if (pairIndex >= shaderPairs.size() || names.empty())
            {
                std::cerr << "Error: -i must be followed by a vertex/fragment pair and list at least one attribute" << std::endl;
                return false;
            }
            shaderPairs[pairIndex].instancedAttributes = std::move(names);
        }

        return true;
    }
}

int main(int argc, char* argv[])
{
    std::filesystem::path outputPath;
    std::vector<ShaderPair> shaderPairs;
    std::vector<TransformFeedbackShader> transformFeedbackShaders;

    // Parse command line arguments
    if (!ParseArguments(argc, argv, outputPath, shaderPairs, transformFeedbackShaders))
    {
        std::cerr << std::endl;
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // Validate output path
    if (!ValidateOutputPath(outputPath))
    {
        return EXIT_FAILURE;
    }

    // Validate all shader files exist
    for (const auto& pair : shaderPairs)
    {
        if (!ValidateFilePath(pair.vertexPath, "Vertex shader") ||
            !ValidateFilePath(pair.fragmentPath, "Fragment shader"))
        {
            return EXIT_FAILURE;
        }
    }

    for (const auto& shader : transformFeedbackShaders)
    {
        if (!ValidateFilePath(shader.vertexPath, "Transform feedback vertex shader"))
        {
            return EXIT_FAILURE;
        }
    }

    try
    {
        // Enable the shader cache
        Babylon::Plugins::ShaderCache::Enable();

        // Create the shader compiler
        Babylon::Plugins::ShaderCompiler compiler;

        // Compile all shader pairs
        for (const auto& pair : shaderPairs)
        {
            std::cout << "Compiling: " << pair.vertexPath.string() << " + " << pair.fragmentPath.string() << std::endl;

            std::string vertexSource = ReadFileContents(pair.vertexPath);
            std::string fragmentSource = ReadFileContents(pair.fragmentPath);
            const auto baseShader = Babylon::Plugins::ShaderCache::AddShader(vertexSource, fragmentSource, compiler.Compile(vertexSource, fragmentSource));
            if (!pair.instancedAttributes.empty())
            {
                const auto instancedAttributes = ResolveInstancedAttributes(*baseShader, pair.instancedAttributes);
                if (!instancedAttributes.empty())
                {
                    std::cout << "Compiling instanced variant: " << pair.vertexPath.string() << std::endl;
                    Babylon::Plugins::ShaderCache::AddShader(vertexSource, fragmentSource,
                        compiler.Compile(vertexSource, fragmentSource, instancedAttributes), instancedAttributes);
                }
            }
        }

        for (const auto& shader : transformFeedbackShaders)
        {
            std::cout << "Compiling transform feedback: " << shader.vertexPath.string() << std::endl;

            std::string vertexSource = ReadFileContents(shader.vertexPath);
            Babylon::Plugins::ShaderCache::AddTransformFeedbackShader(vertexSource, shader.varyings,
                compiler.CompileTransformFeedback(vertexSource, shader.varyings));
        }

        // Save the shader cache to the output file
        std::cout << "Saving compiled shaders to: " << outputPath.string() << std::endl;
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open())
        {
            throw std::runtime_error("Failed to open output file for writing: " + outputPath.string());
        }

        const uint32_t savedEntries = Babylon::Plugins::ShaderCache::Save(outputFile);
        outputFile.close();

        std::cout << "Successfully compiled and saved " << savedEntries << " shader(s) to " << outputPath.string() << std::endl;

        // Clean up
        Babylon::Plugins::ShaderCache::Disable();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        Babylon::Plugins::ShaderCache::Disable();
        return EXIT_FAILURE;
    }
}
