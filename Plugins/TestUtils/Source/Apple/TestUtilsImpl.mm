#include "../TestUtilsImplData.h"
#include <filesystem>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        auto errorCode = info[0].As<Napi::Number>().Int32Value();

        dispatch_async(dispatch_get_main_queue(), ^{
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
            NSString* message = (errorCode == 0) ? @"Success!" : [NSString stringWithFormat:@"Error code: %d. Check logs!", errorCode];
            UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"Validation Tests" message:message preferredStyle:UIAlertControllerStyleAlert];
            UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * ) {}];
            [alert addAction:defaultAction];
            UIViewController *rootController = [[[[UIApplication sharedApplication]delegate] window] rootViewController];
            [rootController presentViewController:alert animated:YES completion:nil];
#else
            if (errorCode != 0)
            {
                std::quick_exit(errorCode);
            }

            [NSApp terminate:nil];
#endif
        });
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& info)
    {
        const int32_t width = info[0].As<Napi::Number>().Int32Value();
        const int32_t height = info[1].As<Napi::Number>().Int32Value();

        dispatch_async(dispatch_get_main_queue(), ^{
            NSWindow* window = [(NSView*)((CAMetalLayer*)m_implData->m_window).delegate window];
            CGFloat scale = window.backingScaleFactor;
            [window setContentSize:NSMakeSize(width / scale, height / scale)];
        });
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& info)
    {
        NSString* title = @(info[0].As<Napi::String>().Utf8Value().c_str());
        dispatch_async(dispatch_get_main_queue(), ^{
            NSWindow* window = [(NSView*)((CAMetalLayer*)m_implData->m_window).delegate window];
            [window setTitle:title];
        });
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto path = std::filesystem::path{[[[NSBundle mainBundle] executablePath] UTF8String]}
            .parent_path().parent_path().parent_path().parent_path().parent_path().generic_string();
        return Napi::Value::From(info.Env(), path);
    }

    void TestUtils::PostProcessFrameBufferData(std::vector<uint8_t>& data)
    {
        // swizzle the data from BGRA to RGBA on Apple platforms
        for (size_t i = 0; i < data.size(); i += 4)
        {
            std::swap(data[i], data[i + 2]);
        }
    }
}

namespace Babylon::Plugins::TestUtils
{
    void Initialize(Napi::Env env, Graphics::WindowT window)
    {
        auto implData{std::make_shared<Internal::TestUtils::ImplData>(window)};
        Internal::TestUtils::CreateInstance(env, implData);
    }
}
