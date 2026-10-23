#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Plugins/TestUtils.h>
#include <Shared/CommandLine.h>
#include <Shared/Diagnostics.h>
#include <Shared/PlaygroundScripts.h>

#include <cstdio>
#include <utility>

namespace
{
    void RunHeadless(const PlaygroundOptions& options)
    {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyProhibited];
        [NSApp finishLaunching];

        NSWindow* window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(0, 0, 600, 400)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:NO];
        MTKView* metalView = [[MTKView alloc] initWithFrame:window.contentView.bounds device:nil];
        metalView.autoResizeDrawable = NO;
        metalView.paused = YES;
        metalView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        window.contentView = metalView;

        Babylon::Embedding::RuntimeOptions runtimeOptions{};
        runtimeOptions.enableDebugTrace = options.DebugTrace.value_or(false);
        runtimeOptions.log = Playground::MakeLogCallback([](std::string_view message) {
            std::printf("%.*s\n", static_cast<int>(message.size()), message.data());
        });
        Babylon::Embedding::Runtime runtime{std::move(runtimeOptions)};
        Playground::Initialize(options);
        Playground::QueueOptions(runtime, options);
        Playground::LoadBootstrapScripts(runtime);
        if (options.Scripts.empty())
        {
            runtime.LoadScript(options.ListTests ? "app:///Scripts/validation_native.js" : "app:///Scripts/experience.js");
        }
        else
        {
            for (const auto& script : options.Scripts)
            {
                NSString* path = @(script.c_str());
                NSURL* url = [NSURL URLWithString:path];
                if (url.scheme.length == 0)
                {
                    url = [NSURL fileURLWithPath:path];
                }
                runtime.LoadScript(url.absoluteString.UTF8String);
            }
            runtime.LoadScript("app:///Scripts/playground_runner.js");
        }

        Babylon::Embedding::View view{runtime, (__bridge CA::MetalLayer*)metalView.layer};
        CGSize size = metalView.bounds.size;
        view.Resize(static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height), Babylon::Embedding::CoordinateUnits::Logical);
        for (;;)
        {
            @autoreleasepool
            {
                NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                    untilDate:[NSDate dateWithTimeIntervalSinceNow:0.001]
                    inMode:NSDefaultRunLoopMode
                    dequeue:YES];
                if (event != nil)
                {
                    [NSApp sendEvent:event];
                }
                const CGSize nextSize = metalView.bounds.size;
                if (!CGSizeEqualToSize(size, nextSize))
                {
                    size = nextSize;
                    view.Resize(static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height), Babylon::Embedding::CoordinateUnits::Logical);
                }
                view.RenderFrame();
            }
        }
    }
}

int main(int argc, const char* argv[])
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);
    Diagnostics::Initialize();
    Babylon::Plugins::TestUtils::SetExitCallback([](int code) {
        Diagnostics::SetExitCode(code);
        Diagnostics::PrintFinishLine();
    });

    const auto options = CommandLine::Parse(argc, argv);
    if (options.ParseError)
    {
        std::fprintf(stderr, "%s\n", options.ErrorMessage.c_str());
        Diagnostics::SetExitCode(2);
        return 2;
    }
    if (options.ShowHelp)
    {
        CommandLine::PrintUsage(argv[0]);
        return 0;
    }
    if (options.Headless || options.ListTests)
    {
        @autoreleasepool
        {
            RunHeadless(options);
        }
    }
    return NSApplicationMain(argc, argv);
}
