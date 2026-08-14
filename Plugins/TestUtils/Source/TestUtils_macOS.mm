#include "TestUtils.h"

#include <filesystem>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        auto exitCode = info[0].As<Napi::Number>().Int32Value();

        InvokeExitCallback(exitCode);

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
            // width/height are the desired framebuffer size in pixels (the
            // validation reference images are authored at this exact size).
            // Babylon Native sizes the bgfx backbuffer to the view's logical
            // point extent (the host drives View::Resize with the layout
            // bounds in points), so set the content size directly in points
            // rather than dividing by backingScaleFactor. Dividing by the
            // scale here would produce a half-resolution backbuffer on Retina
            // displays and make the pixel-diff readback mismatch the
            // reference image dimensions.
            [window setContentSize:NSMakeSize(width, height)];
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
