# Babylon Native Components

This document gives a high-level overview of the components provided by 
default in the Babylon Native repository. For information about the 
definition of _component_ and the different categories of components, 
please read the page on the 
[Babylon Native build system](BuildSystem.md). For information about how to 
create new components and add or reference them in Babylon Native, please
read the page on 
[extending Babylon Native](Extending.md).

## Dependencies

Components in this category encapsulate functionality brought in from 
dependencies which are not logically a part of Babylon Native. Most of these
are external libraries, and many of them are included as submodules.

### arcana

[Arcana](https://github.com/microsoft/arcana.cpp) is a collection of C++ 
utilities. Most prominently, it includes an open-source and cross-platform 
task and asynchrony system modelled after the 
[PPL](https://docs.microsoft.com/en-us/cpp/parallel/concrt/parallel-patterns-library-ppl?view=vs-2019), 
which Babylon Native makes extensive use of internally. Babylon Native also 
makes internal use of several other utilities and data structures provided 
by Arcana. However, no Arcana types are exposed by Babylon Native's APIs.

### bgfx

Babylon Native uses the 
[bgfx](https://bkaradzic.github.io/bgfx/overview.html) family of 
technologies internally as a graphics abstraction for cross-platform native
rendering. Babylon Native consumes these dependencies through a 
[fork](https://github.com/BabylonJS/bgfx.cmake) of the 
[bgfx.cmake](https://github.com/widberg/bgfx.cmake) project, which provides
an easy integration layer by which bgfx can be brought into Babylon 
Native's CMake-based build system. The bgfx family of technologies are used
internally by Babylon Native components that need a cross-platform 
abstraction for GPU access and rendering work. However, no bgfx types are
exposed by Babylon Native's APIs. 

### glslang

[glslang](https://github.com/KhronosGroup/glslang) is the reference compiler
for GLSL provided by the Khronos Group. It is used internally by Babylon 
Native as part of the 
[shader transpilation pipeline](ShaderTranspilation.md).

### gsl

The [Guidelines Support Library](https://github.com/microsoft/gsl) is a 
C++ utility library supplying several useful types and capabilities that
are not supplied by the STL but are conceptually similar. It is used 
internally throughout Babylon Native, but no GSL types are exposed by any
Babylon Native API.

### napi

N-API is an integration layer that allows C and C++ native libraries to 
interface with JavaScript engines in an engine-agnostic manner. Babylon
Native's consumption of N-API is a heavily-modified spinoff of the 
original [Node.js Addon API](https://github.com/nodejs/node-addon-api).
N-API types -- particularly the `Env` type -- are central to Babylon Native
and **are** exposed as part of the contract of most Babylon Native 
components.

### SPIRV-Cross

[SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross) is a Khronos
Group tool that allows shaders to be translated from one shader language to 
another via SPIR-V, a platform-agnostic intermediate representation. 
SPIRV-Cross is used internally as part of Babylon Native's shader 
transpilation pipeline and is not exposed through any API.

### UrlLib

UrlLib is a minimal custom URL-fetching abstraction allowing resources
located at URLs to be retrieved across different platforms using (beneath 
the abstraction) implementations native to each platform in question. It is
used internally by Babylon Native to power most file retrieval operations, 
both from C++ and from JavaScript.

### XR

The XR dependency is a minimal custom AR/VR/MR/XR abstraction used 
internally by Babylon Native to power Babylon.js's XR capabilities on
multiple platforms. In the immediate term, the goal of this abstraction is
to hide the underlying XR implementation from consuming Babylon Native
components because the implementations vary significantly across platforms.
As [OpenXR](https://www.khronos.org/openxr/) gains adoption, the variance 
of underlying implementations is expected to diminish until eventually 
(ideally) the XR dependency will either cease to exist or persist only as 
an extremely thin platform-agnostic integration layer to facilitate 
consumption of the OpenXR logic underneath.

## Core

Components in this category house the most fundamental capabilities exposed 
by Babylon Native to consuming C++ code.

### AppRuntime

AppRuntime can be thought of as the centerpiece of a Babylon Native app 
_under the most common circumstances_: it provides a `JsRuntime`, 
encapsulates a JavaScript engine, and manages the lifetimes of the 
resources required to run a Babylon Native application. Note that it is
not as foundational as the JsRuntime component because it is possible to 
create a Babylon Native application without using AppRuntime. The 
AppRuntime merely exists to codify and encapsulate the "canonical" usage
of a `JsRuntime` and other resources for a typical Babylon Native app.
For in-depth inforomation, please read 
[`AppRuntime`'s dedicated documentation page](AppRuntime.md).

### JsRuntime

JsRuntime is the most foundational of all Babylon Native components and 
types. This component exposes N-API as part of its contract and is consumed
by almost every other Babylon Native component. For in-depth information, 
please read 
[`JsRuntime`'s dedicated documentation page](JsRuntime.md).

### ScriptLoader

The ScriptLoader is a small utility intended to make it easy to load 
JavaScript files into a N-API `Env` owned by a Babylon Native `AppRuntime`.
In particular, the `ScriptLoader` type was created to abstract away the
asynchronous nature of file loading to ensure that JavaScript files and code
would always be run in the correct order. For example, consider the files
**A.js** and **B.js**, where the latter uses types defined in the former.
Intuitively, the operations to be done in order to load B.js successfully 
are, "First load A.js, then load B.js." However, because loading a file into
JavaScript is a multi-step operation (and may even involve network calls 
depending on where a script is located and what it does) where steps happen
asynchronously on a number of different threads, we must be careful that we 
do not start loading B.js _into JavaScript_ before A.js finishes loading 
_into JavaScript_. In other words, there are nuances to task ordering in 
script loading that are difficult to intuitively express without using 
asynchronous concepts. To sidestep this potentially needless complexity,
`ScriptLoader` uses Arcana to maintain its own mechanism of enforcing
order-of-operations. Calls made to `ScriptLoader::LoadScript` or 
`ScriptLoader::Eval` are enqueued, and each enqueued work item will _fully_ 
complete before the `ScriptLoader` allows the next work item to even begin.
This has tricky nuances in edge cases and is somewhat strict within the 
context of itself, but it allows for extremely safe and simple script 
loading without forcing consumers to deal directly with asynchrony concerns.

## Plugins

Components in this category provide essential Babylon Native functionality
to consuming JavaScript (and, to a lesser extent, C++) code. Many of these
plugins are depended upon directly by Babylon.js itself.

### NativeEngine

NativeEngine is, by a wide margin, the most elaborate, sophisticated, and 
important plugin component provided by Babylon Native. At a high level, 
the NativeEngine component is used directly by Babylon.js to provide all the
graphics functionality needed to fulfill Babylon's core rendering promises
on native platforms. For an in-depth discussion of NativeEngine, please
read its [dedicated documentation page](NativeEngine.md).

### NativeWindow

Not to be confused with the Window polyfill, the NativeWindow plugin exists
to expose information to JavaScript about the operating system window in 
which the application is being hosted. At present, this information 
predominately concerns resolution, and NativeWindow is the primary vector
by which information about window resizing events is passed to consuming 
code in JavaScript.

### NativeXr

NativeXr does for XR rendering what NativeEngine does for conventional 
rendering: it provides a native implementation layer that works with 
Babylon.js to ensure that Babylon's rendering promises are fulfilled in the 
given situation. In the case of XR, NativeXr serves as an intermediary layer
between the XR dependency and Babylon Native's requirements of XR from the
underlying platform. In this way, NativeXr serves some of the same purpose 
that a WebXR polyfill might serve. However, where a WebXR polyfill would be
obliged to meet an established standard in order to provide functionality 
to theoretically any consumer, NativeXr exists exclusively to provide the
minimal implementation required to support Babylon.js's promises and use
cases. As with the XR dependency, it is expected that the role and 
importance of NativeXr should diminish over time as the need to "bridge the
gap" between abstractions shrinks with usage; and should a true WebXR 
polyfill ever become available for Babylon Native, that will almost 
certainly render NativeXr completely obsolete, at which point this plugin 
should be deleted entirely.

## Polyfills

Components in this category supply isolated browser-like capabilities to 
Babylon Native in order to fulfill dependencies of various JavaScript
libraries. As such, these components are overwhelmingly very simple and
commonly understood.

### Console

This polyfill allows the use of the `console.log(...)` API in JavaScript
and ensures that messages logged in this way are routed to an output
mechanism specified by the consuming C++ code.

### Window

Not to be confused with the NativeWindow plugin, this polyfill provides
a small selection of `Window` capabilities familiar from browsers -- 
including `setTimeout(...)`, `atob(...)`, and event listeners -- to 
consuming JavaScript code.

### XMLHttpRequest

This polyfill provides a partial `XMLHttpRequest` implementation which 
allows JavaScript code to fetch files from URLs. This polyfill is capable
of fetching file from many different kinds of locations, including from the
Internet or from local storage, depending on the kind of URL provided.

## Apps

These are the consuming experiences provided with Babylon Native. They 
are predominantly intended to be used for testing, for running custom 
experiences designed for a particular use case, and as examples for how
Babylon Native applications can be built.

### Playground

The Playground app is the primary "test harness" mechanism for Babylon 
Native. When work is being done in the repository it is almost always 
initially tested by modifying **experience.js** to run the new behavior in 
the Playground app. It is also possible to launch a pre-built Playground
app with command line arguments of the locations of external JavaScript
scripts, which will cause the app to load those scripts and execute them
instead of experience.js.

The longer-term vision for the Babylon Native Playground app is much more
sophisticated and beyond the scope of this overview. For more information,
please read the 
[dedicated documentation page for the Playground app](Playground.md).

### ValidationTests

The ValidationTests app is exactly what its name implies: it is a mechanism
for easily running validation tests to ensure Babylon Native continues to 
run and render correctly under expected circumstances. The ValidationTests
are designed to be run by the Babylon Native team as part of common 
workflows and CI and are not intended to be highly customizable. The only
supported platform for the validation tests is Win32.
