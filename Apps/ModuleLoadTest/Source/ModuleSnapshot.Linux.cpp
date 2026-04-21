#include "ModuleSnapshot.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>

#include <link.h>

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
        // (priorities 0-100 are reserved for the implementation). This runs
        // before user-default-priority constructors (e.g. bx's) in *this*
        // image and in images whose constructors have default priority.
        //
        // Caveat (unlike Win32 TLS callbacks): constructors in a *separate*
        // shared object may still run before this one if the dynamic linker
        // orders that object's init ahead of ours. As a result the baseline
        // may include modules loaded by those early initializers. For this
        // test that's acceptable — anything in the baseline is NOT counted
        // against BN's boot delta, and the signal we want (new modules
        // pulled in by Graphics::Device + AppRuntime + polyfills during
        // actual boot) still appears in the delta.
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

        int IteratePhdrCallback(struct dl_phdr_info* info, size_t /*size*/, void* data)
        {
            auto& snapshot = *static_cast<ModuleSnapshot*>(data);

            const char* name = info->dlpi_name;
            if (name == nullptr || name[0] == '\0')
            {
                // Main executable: dl_iterate_phdr reports an empty name for
                // it. Skip so the test's own binary isn't counted as a "BN
                // module"; the test's own binary is never a regression.
                return 0;
            }

            // Skip pseudo-images like "linux-vdso.so.1" that are kernel-provided.
            if (std::strstr(name, "linux-vdso") != nullptr ||
                std::strstr(name, "linux-gate") != nullptr)
            {
                return 0;
            }

            const char* slash = std::strrchr(name, '/');
            const char* base = slash ? slash + 1 : name;
            if (*base == '\0')
            {
                return 0;
            }

            snapshot.insert(ToLower(base));
            return 0;
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
        ::dl_iterate_phdr(&IteratePhdrCallback, &snapshot);
        return snapshot;
    }
}
