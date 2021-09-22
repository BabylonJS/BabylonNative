# Console
Implements parts of [`console`](https://developer.mozilla.org/en-US/docs/Web/API/console). Provides a way to output debug messages from JavaScript into C++.

Currently supports:
* `log()`
* `warn()`
* `error()`

When initializing, you should provide a callback which takes a message and a log level and outputs the message in whatever way you like. For example, you could initialize it like so:
```c++
Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
    printf("%s", message);
    fflush(stdout);
});
```
