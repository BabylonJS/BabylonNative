2 levels of logging

# Code logging API
Works are a console log. Enable Debug Trace (off by default) and set an outputter(none set by default) then call Trace.
```
SetTraceOutput([](const char *trace){
printf("%s\n", trace);
});
SetDebugTrace(true);
...
Trace("Error detected.");
```

# Babylon code instrumentation
Babylon code is instrumented and will use Trace function described earlier IF `BABYLON_DEBUG_TRACE` preprocessor is defined. If it's not (default), no log will happen and no performance impact.

To summarize, user can log in his own code with Babylon logging API. Babylon Native and JSRuntimeHost code is instrumented but will only log if  `BABYLON_DEBUG_TRACE` preprocessor is defined and logging is enabled and an outputer function is set.

Thread safety in the logging output is the responsability of the user.
