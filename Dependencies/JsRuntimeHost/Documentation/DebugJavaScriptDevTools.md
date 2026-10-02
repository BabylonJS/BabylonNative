# JsRuntimeHost Script Debugging
You can use Chromium DevTools to debug your JavaScript while running with the V8 JavaScript engine (Default Engine for Android).

## Add the Remote Target URL
On whichever Chromium-based browser you're using (Edge or Chrome), navigate to `about://inspect`.
Click `Configure...` and in the target discovery settings add an entry `localhost:5643`.

![Target discovery settings](Images/DevTools/chrome-targets.png)

## Setting Up To Debug
Make sure to change the line in [tests.js](https://github.com/BabylonJS/JsRuntimeHost/blob/f487c7b3f89b407e95a53543a06a34f1a1fbb860/Tests/UnitTests/Scripts/tests.js#L2) to true, or else the DevTools won't have enough time to attach to the JavaScript Instance. If you are debugging your own script, you can also use [setTimeout()](https://developer.mozilla.org/en-US/docs/web/api/settimeout) to delay the execution of the code you are trying to debug while you wait to attach the debugger.

## Debugging on Android/Emulator

Install [ADB](https://developer.android.com/tools/releases/platform-tools) and run the following command with an Android device plugged in or Android emulator running.
```
adb forward tcp:5643 tcp:5643
```

Note, every time the Android device is unplugged and re-plugged, this ADB command need to be run again.

## Click Inspect
Run the UnitTests application and wait for the DevTools page to recognize the inspector server spun up by the app. Once you see the app pop up in the list of Remote Targets, you may click inspect to open up a DevTools window.

![Ready to inspect](Images/DevTools/chrome-inspect.png)

## DevTool Settings

There is also a setting that needs to be enabled in the DevTools. Go into the settings by clicking on the gear in the top right corner and check this box in the preferences tab. This only needs to be done once.

![DevTools window](Images/DevTools/devtools-settings.png)

## View Logs, Set Breakpoints, Profile, and More
Once you have the DevTools window open, you can use it in the same way as if you were inspecting a web page.
For more information, see this documentation from Google on [how to debug JavaScript using Chrome DevTools](https://developer.chrome.com/docs/devtools/javascript/).

![DevTools window](Images/DevTools/chrome-debugger.png)
