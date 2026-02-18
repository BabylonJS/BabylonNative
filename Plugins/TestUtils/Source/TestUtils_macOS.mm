#include "TestUtils.h"

#include <filesystem>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        auto exitCode = info[0].As<Napi::Number>().Int32Value();
        if (exitCode != 0)
        {
            std::quick_exit(exitCode);
        }

        dispatch_async(dispatch_get_main_queue(), ^{
            [NSApp terminate:nil];
        });
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& info)
    {
        const int32_t width = info[0].As<Napi::Number>().Int32Value();
        const int32_t height = info[1].As<Napi::Number>().Int32Value();

        dispatch_async(dispatch_get_main_queue(), ^{
            auto* window = [(NSView*)((__bridge CAMetalLayer*)m_window).delegate window];
            CGFloat scale = window.backingScaleFactor;
            [window setContentSize:NSMakeSize(width / scale, height / scale)];
        });
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& info)
    {
        NSString* title = @(info[0].As<Napi::String>().Utf8Value().c_str());
        dispatch_async(dispatch_get_main_queue(), ^{
            auto* window = [(NSView*)((__bridge CAMetalLayer*)m_window).delegate window];
            [window setTitle:title];
        });
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto path = std::filesystem::path{[[[NSBundle mainBundle] executablePath] UTF8String]}
            .parent_path().parent_path().parent_path().parent_path().parent_path().generic_string();
        return Napi::Value::From(info.Env(), path);
    }
}
