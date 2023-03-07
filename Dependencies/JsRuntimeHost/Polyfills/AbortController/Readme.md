# AbortController
Implements parts of [`AbortController`](https://developer.mozilla.org/en-US/docs/Web/API/AbortController/) and [`AbortSignal`](https://developer.mozilla.org/en-US/docs/Web/API/AbortSignal). Provides a way to trigger the abort signal. *Work In Progress*

Currently not implemented:
* [`ThrowIfAborted`](https://developer.mozilla.org/en-US/docs/Web/API/AbortSignal/throwIfAborted)
* [`Timeout`](https://developer.mozilla.org/en-US/docs/Web/API/AbortSignal/timeout)
* [`Abort`](https://developer.mozilla.org/en-US/docs/Web/API/AbortSignal/abort)

Both the AbortController and AbortSignal polyfills are initialized inside AbortController's initialize method:
```c++
Babylon::Polyfills::AbortController::Initialize(env);
```
