#include "App.h"
#include "ModuleSnapshot.h"

#include <Babylon/DebugTrace.h>
#include <Babylon/Graphics/Device.h>

#import <Metal/Metal.hpp>

#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string_view>

#import <Foundation/Foundation.h>

namespace ModuleLoadTest
{
    // Apple equivalent of IsDebuggerPresent() — non-invasive.
    // https://developer.apple.com/library/archive/qa/qa1361/_index.html
    bool IsBeingTraced()
    {
        int name[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
        struct kinfo_proc info{};
        size_t size = sizeof(info);
        if (sysctl(name, 4, &info, &size, nullptr, 0) != 0)
        {
            return false;
        }
        return (info.kp_proc.p_flag & P_TRACED) != 0;
    }

    const ModuleSnapshot& GetExpectedBootModules()
    {
        // Seeded from CI on macos-latest (ARM64 paravirtualized GPU runner).
        // Append entries here as new configs surface deltas.
        static const ModuleSnapshot kModules{
            "appleparavirtgpumetaliogpufamily",
            "iogpu",
        };
        return kModules;
    }

    // On macOS the interesting per-SKU variation is the Metal/GPU driver
    // bundles (AMDMTLBronzeDriver, AppleIntelKBLGraphicsMTLDriver, ...), which
    // dyld loads by path under /System/Library/Extensions/. Base names still
    // differ between Apple Silicon and Intel. Seed a broad carve-out.
    bool IsAllowedOptionalModule(std::string_view name)
    {
        static constexpr std::string_view kPrefixes[] = {
            // Metal GPU driver bundles
            "amdmtl",
            "appleintel",
            "applem1",
            "applem2",
            "applem3",
            "nvmtl",
            // Ambient/IOSurface layer
            "iosurface",
        };
        for (const auto& prefix : kPrefixes)
        {
            if (name.size() >= prefix.size() && name.compare(0, prefix.size(), prefix) == 0)
            {
                return true;
            }
        }
        return false;
    }
    std::optional<Babylon::Graphics::Configuration> CreateGraphicsConfig()
    {
        // MTL::Device must outlive RunBoot. Park in function-local static
        // storage so it lives for the duration of the process.
        static NS::SharedPtr<MTL::Device> device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());
        if (!device)
        {
            std::cout << "ModuleLoadTest: SKIP - no Metal device available." << std::endl;
            return std::nullopt;
        }

        Babylon::DebugTrace::EnableDebugTrace(true);
        Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

        Babylon::Graphics::Configuration config{};
        config.Device = device.get();
        config.Width = 600;
        config.Height = 400;
        return config;
    }
}
