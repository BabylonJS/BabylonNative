package com.babylonjs.embedding;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.os.Build;

/**
 * Requests Android audio focus before WebAudio playback starts.
 */
public final class AudioFocusHelper {
    private static AudioManager audioManager;
    private static AudioFocusRequest audioFocusRequest;
    private static final AudioManager.OnAudioFocusChangeListener focusListener = focusChange -> {
        switch (focusChange) {
            case AudioManager.AUDIOFOCUS_LOSS:
            case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
            case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                nativeNotifyAudioInterruption(true);
                break;
            case AudioManager.AUDIOFOCUS_GAIN:
                nativeNotifyAudioInterruption(false);
                break;
            default:
                break;
        }
    };

    static {
        System.loadLibrary("BabylonNativeEmbedding");
    }

    private AudioFocusHelper() {}

    public static void initialize(Context context) {
        audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
    }

    public static void requestPlaybackFocus() {
        if (audioManager == null) {
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (audioFocusRequest == null) {
                AudioAttributes attributes = new AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_GAME)
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .build();
                audioFocusRequest = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
                        .setAudioAttributes(attributes)
                        .setOnAudioFocusChangeListener(focusListener)
                        .build();
            }

            audioManager.requestAudioFocus(audioFocusRequest);
        } else {
            audioManager.requestAudioFocus(
                    focusListener,
                    AudioManager.STREAM_MUSIC,
                    AudioManager.AUDIOFOCUS_GAIN);
        }
    }

    private static native void nativeNotifyAudioInterruption(boolean began);
}
