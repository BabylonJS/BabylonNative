# BabylonNative Script Debugging on Win32
You can use Chromium DevTools to debug your JavaScript while running BabylonNative on Win32 (x86/x64) with the V8 javascript engine.

## Set up BabylonNative to Use V8
When running CMake, be sure to specify the following flag:
`-D NAPI_JAVASCRIPT_ENGINE=V8`

For example:
```powershell
mkdir Build
cd Build
cmake -A Win32 -D NAPI_JAVASCRIPT_ENGINE=V8 ..
```

## Add the Remote Target URL
Depending on which Chromium-based browser you're using (Edge or Chrome), navigate to `edge://inspect` or `chrome://inspect`.
Click `Configure...` and in the target discovery settings add an entry `localhost:{port}`, where `{port}` is consistent with the one supplied to the call to [`StartInspector`](https://github.com/BabylonJS/BabylonNative/blob/2db465f5569d8eb833245a41030c5219cfa3fe59/Apps/Playground/Win32/App.cpp#L141-L144).

![Target discovery settings](Images/DevTools/chrome-targets.png)

## Click Inspect
Open the Playground application and wait for the DevTools page to recognize the inspector server spun up by the app. Once you see the app pop up in the list of Remote Targets, you may click inspect to open up a DevTools window.

![Ready to inspect](Images/DevTools/chrome-playground.png)

## View Logs, Set Breakpoints, Profile, and More
Once you have the DevTools window open, you can use it in the same way as if you were inspecting a web page.

![DevTools window](Images/DevTools/devtools-breakpoint.png)
