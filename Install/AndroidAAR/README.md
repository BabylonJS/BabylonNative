# Babylon Native — Android AAR

This Gradle project produces a publishable Android AAR
(`BabylonNative-release.aar`) embedding the [Babylon Native](https://github.com/BabylonJS/BabylonNative)
runtime, intended to be embedded inside any Android application.

The build is tuned for the **smallest possible footprint** suitable for
production deployment.

## Footprint optimizations

* QuickJS JavaScript engine (no V8 / JSC dependency).
* libc++ statically linked into `libBabylonNativeJNI.so` (no
  `libc++_shared.so` companion).
* Only `arm64-v8a` and `x86_64` ABIs.
* OpenGL ES 3.0 backend only — Vulkan, D3D11, D3D12 and Metal are not
  compiled into bgfx.
* Image loading, shader compiler, NativeInput and XMLHttpRequest plugins
  are disabled.
* Release build uses `MinSizeRel` (`-Os`), function/data sections,
  hidden visibility, dead-code elimination and `--strip-all`.
* No `Babylon::Plugins::NativeXr` (no ARCore dependency).

## Shader cache

The runtime expects a precompiled shader cache to be provided by the
embedding application as an asset named **`shadercache.bin`**:

```
app/src/main/assets/shadercache.bin
```

If present, it is loaded at engine startup. If absent the runtime still
works but every shader will be compiled on first use.

## Public Java API

Two classes are exposed under `com.library.babylonnative`:

* `BabylonView` — a `FrameLayout` wrapping a Babylon Native rendering
  surface. Forward `onPause()` / `onResume()` /
  `onRequestPermissionsResult()` from your `Activity`.
* `Wrapper` — a thin JNI bridge. Most consumers do not need to use it
  directly.

The consuming application is expected to call
`BabylonView.loadScript(...)` (or `eval(...)`) explicitly from Java —
**no script is auto-loaded by the AAR**.

### Minimal usage

```java
public class MyActivity extends Activity implements BabylonView.ViewDelegate {
    private BabylonView view;

    @Override
    protected void onCreate(Bundle s) {
        super.onCreate(s);
        view = new BabylonView(getApplication(), this, this);
        setContentView(view);
    }

    @Override public void onViewReady() {
        view.loadScript("app:///Scripts/myScene.js");
    }

    @Override protected void onPause()  { view.onPause();  super.onPause();  }
    @Override protected void onResume() { super.onResume(); view.onResume(); }
}
```

## Building

From this directory:

```powershell
.\build-release.ps1
```

or directly:

```powershell
.\gradlew :BabylonNative:assembleRelease
```

The script then prints the size of the produced `.aar` and of every
`libBabylonNativeJNI.so` it contains.

The AAR is written to:

```
BabylonNative\build\outputs\aar\BabylonNative-release.aar
```
