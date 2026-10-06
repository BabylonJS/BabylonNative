# Console
Implements parts of [`console`](https://developer.mozilla.org/en-US/docs/Web/API/console). Provides a way to output debug messages from JavaScript into C++.

Currently supports:
* `log()`
* `warn()`
* `error()`

When initializing, you should provide a callback which takes a message and a log level and outputs the message in whatever way you like. For example, you could initialize it like so:
```c++
Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
    fprintf(stdout, "%s", message);
    fflush(stdout);
});
```

Inside the callback you can optionally capture the JavaScript callstack at the originating `console.*` call site via `Babylon::Polyfills::Console::CaptureCurrentJsStack(env)`. The polyfill's own shim frames are skipped, so the top frame is the user's call site. Capture is best-effort -- the returned string can be empty (no JS context active, engine doesn't expose `Error.stack`, etc.), so always check before using. The capture is opt-in per-call -- hosts that don't need stacks pay nothing; hosts that want them on a specific level can branch on the `LogLevel` argument:
```c++
Babylon::Polyfills::Console::Initialize(env, [env](const char* message, auto level) {
    fprintf(stdout, "%s", message);
    if (level == Babylon::Polyfills::Console::LogLevel::Error) {
        auto stack = Babylon::Polyfills::Console::CaptureCurrentJsStack(env);
        if (!stack.empty()) {
            fprintf(stdout, "\n%s", stack.c_str());
        }
    }
    fflush(stdout);
});
```