#import <Cocoa/Cocoa.h>
#import "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        dispatch_async(dispatch_get_main_queue(), ^{
            if (graphics)
            {
                graphics->FinishRenderingCurrentFrame();
            }
            runtime.reset();
            graphics.reset();
            UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"Validation Tests"
                                            message:(errorCode == 0)?@"Success!":@"Errors: Check logs!"
                                            preferredStyle:UIAlertControllerStyleAlert];

            UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault
                handler:^(UIAlertAction * ) {}];

            [alert addAction:defaultAction];
            UIViewController *rootController = [[[[UIApplication sharedApplication]delegate] window] rootViewController];
            [rootController presentViewController:alert animated:YES completion:nil];
        });
#else
        dispatch_async(dispatch_get_main_queue(), ^{
            [[_nativeWindowPtr window]close];
        });
#endif
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& /*info*/)
    {
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        std::string path = getenv("HOME");
        return Napi::Value::From(info.Env(), path);
    }
}