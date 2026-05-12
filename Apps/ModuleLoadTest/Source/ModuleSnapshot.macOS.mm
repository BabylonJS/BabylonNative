#include "ModuleSnapshot.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

#include <mach-o/dyld.h>

namespace ModuleLoadTest
{
    namespace
    {
        std::string ToLower(std::string value)
        {
            std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return value;
        }

        // The baseline is captured by a constructor with priority 101
        // (priorities 0-100 are reserved for the implementation). On Darwin
        // the constructor-priority attribute is permitted by the compiler
        // but dyld's image-init ordering does not honor it strictly across
        // images: dylibs loaded before this one (including bx-style static
        // initializers in linked dylibs) may have already run by the time
        // this fires. As a result the baseline may include modules loaded
        // by those early initializers. For this test that's acceptable —
        // anything in the baseline is NOT counted against BN's boot delta,
        // and the signal we want (new images loaded by Graphics::Device +
        // AppRuntime + polyfills during actual boot) still appears.
        ModuleSnapshot* g_preInitSnapshot = nullptr;

        void CapturePreInitSnapshot() __attribute__((constructor(101)));
        void CapturePreInitSnapshot()
        {
            if (g_preInitSnapshot == nullptr)
            {
                g_preInitSnapshot = new ModuleSnapshot(CaptureSnapshot());
            }
        }

        void DisposePreInitSnapshot() __attribute__((destructor(101)));
        void DisposePreInitSnapshot()
        {
            delete g_preInitSnapshot;
            g_preInitSnapshot = nullptr;
        }
    }

    const ModuleSnapshot& GetPreInitBaseline()
    {
        static const ModuleSnapshot empty{};
        return g_preInitSnapshot != nullptr ? *g_preInitSnapshot : empty;
    }

    ModuleSnapshot CaptureSnapshot()
    {
        ModuleSnapshot snapshot{};

        const uint32_t count = ::_dyld_image_count();
        for (uint32_t i = 0; i < count; ++i)
        {
            const char* path = ::_dyld_get_image_name(i);
            if (path == nullptr || *path == '\0')
            {
                continue;
            }

            const char* slash = std::strrchr(path, '/');
            const char* base = slash ? slash + 1 : path;
            if (*base == '\0')
            {
                continue;
            }

            snapshot.insert(ToLower(base));
        }

        return snapshot;
    }
}
