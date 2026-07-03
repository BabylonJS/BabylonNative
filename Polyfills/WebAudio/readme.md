# WebAudio

Implements the subset of the Web Audio API used by Babylon.js, backed by [miniaudio](https://github.com/mackron/miniaudio).

## Platform support

| Platform | Status |
|----------|--------|
| Windows / macOS / Linux (desktop) | Validated via unit tests (null backend) and Playground `audio.js` (real device). Suitable for buffer-based Babylon audio. |
| iOS / Android | Built into Embedding with `AVAudioSession` (iOS) and Android audio-focus hooks. Playground loads `audio.js` on both platforms for on-device validation. |
| CI / headless | Use `BABYLON_NATIVE_WEBAUDIO_BACKEND=null` so graph logic runs without a playback device. |

Enable with `BABYLON_NATIVE_POLYFILL_WEBAUDIO=ON` (default). The polyfill is initialized from `Embedding` when linked.

## Lifecycle (browser-aligned)

- `AudioContext` starts in the `"suspended"` state; call `resume()` after a user gesture on mobile.
- The native playback device is opened lazily on the first successful `resume()`.
- `Embedding::Runtime::Suspend/Resume` pauses and resumes the shared audio device when the host app backgrounds.
- iOS activates `AVAudioSession` and handles interruptions; Android requests audio focus before playback.
- A minimal `document` stub (`createElement("audio")`, no-op event listeners) is installed automatically when `globalThis.document` is undefined.

## Supported surface

- `AudioContext` / `webkitAudioContext`
- `AudioBuffer`, `AudioBufferSourceNode`, `GainNode`, `PannerNode`, `StereoPannerNode`
- `AudioListener`, `AudioParam` (automation timeline: `setValueAtTime`, ramps, `setValueCurveAtTime`, `cancelScheduledValues`)
- `AnalyserNode` (time-domain snapshot + Hann-windowed FFT frequency data with smoothing)
- `AudioDestinationNode`
- `Audio` (`canPlayType` for format detection)
- `AudioBufferSourceNode.start(when)` scheduled start times

Decoding supports the formats provided by miniaudio (wav, mp3, flac, ogg, and others depending on build).

Set `BABYLON_NATIVE_WEBAUDIO_BACKEND=null` to force a software clock for headless CI (no hardware device). When no playback device is available at runtime, the polyfill falls back to miniaudio's null backend automatically so construction never crashes.

## Babylon.js coverage

Works for the paths Babylon actually uses on native:

- Legacy `BABYLON.Sound` with `ArrayBuffer` / `decodeAudioData`
- v9 `CreateAudioEngineAsync` + `CreateSoundAsync` (when present in the Babylon build)
- 3D spatial audio (`PannerNode`, `AudioListener`, distance attenuation)
- Volume / pitch ramps via `AudioParam` automation
- Audio analysers (`getByteFrequencyData`, `getFloatFrequencyData`, time-domain getters)

## Not supported (yet)

- `OfflineAudioContext`
- `MediaElementAudioSourceNode` / `MediaStream` sources
- URL streaming via `HTMLAudioElement` (`play` / `pause` are stubs; use `decodeAudioData` instead)
- Full Web Audio spec (`setTargetAtTime`, HRTF panning, `AudioWorklet`, etc.)

## App integration

Embedding hosts that link `BabylonNativeEmbedding` receive platform audio-session setup automatically. No per-app `document` stub is required.

For custom hosts that initialize the polyfill directly, `WebAudio::Initialize` still installs the built-in `document` stub when needed.

On mobile, ensure the first audible playback follows a user gesture and call `AudioContext.resume()` from that gesture if autoplay is blocked. See `Apps/Playground/Scripts/audio.js` for a working example (pointer tap resumes audio).

## Playground demo

Playground loads `app:///Scripts/audio.js` on desktop, iOS, and Android. The script generates an in-memory beep WAV and plays it through legacy `BABYLON.Sound` and/or `CreateSoundAsync` while orbiting a spatialized source around the camera.

## Mobile validation checklist

Use this checklist on physical devices with the speaker unmuted.

### Build and wiring

- [ ] `BABYLON_NATIVE_POLYFILL_WEBAUDIO=ON` in the app CMake configuration
- [ ] App links `Embedding` / `BabylonNativeEmbedding` so `WebAudio::Initialize` and platform hooks run
- [ ] JavaScript bundle includes Babylon audio code paths you intend to use (`Sound` and/or `CreateSoundAsync`)

### JavaScript bootstrap

- [ ] `typeof AudioContext !== "undefined"` after runtime startup
- [ ] `await new AudioContext().resume()` resolves without error after a user gesture

### Functional smoke tests

- [ ] Decode a small in-memory WAV via `decodeAudioData` and play through `bufferSource → gain → destination`
- [ ] Legacy `new BABYLON.Sound(name, arrayBuffer, scene, null, { autoplay: true })` is audible
- [ ] v9 `CreateAudioEngineAsync` + `CreateSoundAsync` is audible (if used)
- [ ] Moving the listener / camera changes spatialized sound level
- [ ] `ended` fires when a short non-looping buffer finishes
- [ ] App survives background/foreground without crashing (audio may stay suspended until user interaction)

### Platform-specific

**iOS**

- [ ] `AVAudioSession` category `playback` activates on first play (handled by Embedding)
- [ ] Audio plays after device silent switch is on
- [ ] First play after cold start works after user gesture + `AudioContext.resume()`
- [ ] No audio glitches when rotating, locking, or receiving a phone call

**Android**

- [ ] `RECORD_AUDIO` is **not** required for playback-only apps
- [ ] Audio focus handled (pause on focus loss, resume when regained if contexts are running)
- [ ] Playback works on API 24+ targets used by the app
- [ ] First play after cold start works after user interaction on restrictive OEM builds

### Regression / CI

- [ ] `BABYLON_NATIVE_WEBAUDIO_BACKEND=null ./UnitTests --gtest_filter=JavaScript.All` passes on your CI host

### Known gaps to watch for

- Shared engine clock across `AudioContext` instances (same as a single native device)
- Panner uses simplified equal-power azimuth, not browser HRTF
