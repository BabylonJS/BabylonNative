/**
 * Shader Tool
 *
 * A cross-platform console tool that compiles GLSL shaders using
 * the ShaderCompiler component and saves the compiled results using ShaderCache.
 *
 * Usage:
 *   ShaderTool -o <output_file> <vertex1> <fragment1> [<vertex2> <fragment2> ...]
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
#include <vector>

namespace
{
    struct ShaderPair
    {
        std::filesystem::path vertexPath;
        std::filesystem::path fragmentPath;
    };

    void PrintUsage(const char* programName)
    {
        std::cerr << "Babylon Native Shader Tool" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  " << programName << " -o <output_file> <vertex1> <fragment1> [<vertex2> <fragment2> ...]" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  -o <output_file>   Path to the output compiled shader cache file" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Arguments:" << std::endl;
        std::cerr << "  <vertex> <fragment>   Pairs of vertex and fragment shader source files (GLSL)" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Examples:" << std::endl;
        std::cerr << "  " << programName << " -o cache.bin vertex.glsl fragment.glsl" << std::endl;
        std::cerr << "  " << programName << " -o cache.bin v1.glsl f1.glsl v2.glsl f2.glsl" << std::endl;
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

    bool ParseArguments(int argc, char* argv[], std::filesystem::path& outputPath, std::vector<ShaderPair>& shaderPairs)
    {
        // Find -o flag
        int outputIndex = -1;
        for (int i = 1; i < argc - 1; i++)
        {
            if (std::strcmp(argv[i], "-o") == 0)
            {
                outputIndex = i;
                outputPath = argv[i + 1];
                break;
            }
        }

        if (outputIndex == -1)
        {
            std::cerr << "Error: Missing required -o <output_file> option" << std::endl;
            return false;
        }

        // Collect remaining arguments (excluding -o and its value) directly into shader pairs
        std::vector<const char*> remaining;
        for (int i = 1; i < argc; i++)
        {
            if (i == outputIndex)
            {
                i++; // Skip -o and its value
                continue;
            }
            remaining.push_back(argv[i]);
        }

        if (remaining.empty())
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
            shaderPairs.push_back({std::filesystem::path(remaining[i]), std::filesystem::path(remaining[i + 1])});
        }

        return true;
    }
}

int main(int argc, char* argv[])
{
    std::filesystem::path outputPath;
    std::vector<ShaderPair> shaderPairs;

    // Parse command line arguments
    if (!ParseArguments(argc, argv, outputPath, shaderPairs))
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
            Babylon::Plugins::ShaderCache::AddShader(vertexSource, fragmentSource, compiler.Compile(vertexSource, fragmentSource));
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
