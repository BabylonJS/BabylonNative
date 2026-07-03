#include <Babylon/Polyfills/AudioPlatform.h>

#import <AVFoundation/AVFoundation.h>

namespace
{
    void ConfigureAudioSession()
    {
        AVAudioSession* session = [AVAudioSession sharedInstance];
        NSError* error = nil;
        [session setCategory:AVAudioSessionCategoryPlayback error:&error];
        [session setActive:YES error:&error];
    }

    void HandleInterruption(NSNotification* notification)
    {
        const auto type = static_cast<AVAudioSessionInterruptionType>(
            [notification.userInfo[AVAudioSessionInterruptionTypeKey] unsignedIntegerValue]);

        if (type == AVAudioSessionInterruptionTypeBegan)
        {
            Babylon::Polyfills::Internal::NotifyAudioInterruption(true);
        }
        else if (type == AVAudioSessionInterruptionTypeEnded)
        {
            ConfigureAudioSession();
            Babylon::Polyfills::Internal::NotifyAudioInterruption(false);
        }
    }

    struct AudioSessionRegistrar
    {
        AudioSessionRegistrar()
        {
            Babylon::Polyfills::RegisterAudioPreparePlaybackHandler([]() {
                ConfigureAudioSession();
            });

            [[NSNotificationCenter defaultCenter]
                addObserverForName:AVAudioSessionInterruptionNotification
                            object:[AVAudioSession sharedInstance]
                             queue:nil
                        usingBlock:^(NSNotification* notification) {
                            HandleInterruption(notification);
                        }];
        }
    };

    AudioSessionRegistrar g_audioSessionRegistrar{};
}
