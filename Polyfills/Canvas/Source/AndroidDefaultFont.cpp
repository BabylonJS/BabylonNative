#include "AndroidDefaultFont.h"
#include "nanovg/nanovg.h"
#include <fstream>
#include <limits>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        std::vector<unsigned char> ReadFont(const char* path)
        {
            std::ifstream file{path, std::ios::binary | std::ios::ate};
            const std::streamoff size = file.tellg();
            if (size <= 0 || size > std::numeric_limits<int>::max())
            {
                return {};
            }
            std::vector<unsigned char> bytes(static_cast<size_t>(size));
            if (!file.seekg(0).read(reinterpret_cast<char*>(bytes.data()), size))
            {
                return {};
            }
            return bytes;
        }

        std::vector<unsigned char> GetDefaultFont()
        {
            for (const char* path : {"/system/fonts/Roboto-Regular.ttf", "/system/fonts/DroidSans.ttf"})
            {
                auto bytes = ReadFont(path);
                if (!bytes.empty())
                {
                    return bytes;
                }
            }
            return {};
        }
    }

    int LoadAndroidDefaultFont(NVGcontext* context)
    {
        // Fontstash borrows these bytes for the lifetime of each Canvas context.
        static auto bytes = GetDefaultFont();
        return bytes.empty() ? -1 : nvgCreateFontMem(context, "sans-serif", bytes.data(), static_cast<int>(bytes.size()), 0);
    }
}
