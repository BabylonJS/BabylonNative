2 levels of logging

# Code logging API
Works are a console log. Enable Debug Trace (off by default) and set an outputter(none set by default) then call Trace.
```
SetTraceOutput([](const char *trace) {
    printf("%s\n", trace);
});
SetDebugTrace(true);
...
DEBUG_TRACE("Error detected.");
```

# Babylon code instrumentation
Babylon code is instrumented and will use the trace function described earlier _if_ `BABYLON_DEBUG_TRACE` preprocessor is defined. If it's not (default), no log will happen and no performance impact.

Users can add logs to their own code using the Babylon logging API, but logging will only occur if it's enabled and an output function is set.

Thread safety in the logging output is the responsibility of the user.
