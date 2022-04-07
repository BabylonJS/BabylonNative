# NativeEngine

The NativeEngine plugin component is, in many respects, the "Babylon" 
part of Babylon Native. While other components like 
[JsRuntime](JsRuntime.md) and [AppRuntime](AppRuntime.md) provide the 
infrastructure required to scalably and extensibly bring Babylon Native's
Web-logic-on-native-implementation to life, NativeEngine is the component
which actually interfaces with Babylon.js and powers its rendering 
capabilities. This document provides a high-level overview of the design
of NativeEngine and some of the nuances involved in its workings.

## The C++/JavaScript Integration

At its lowest level, Babylon.js is built on top of WebGL. To do all its 
rendering and GPU work, Babylon.js directly calls WebGL APIs, manages 
state, etc. in order to draw graphics into the browser. Because of this, 
one possible way Babylon Native's graphics capabilities could have been
implemented would have been to polyfill WebGL, supplying all the types and 
APIs and assumptions Babylon.js relies on at its lowest level and 
implementing the required functionality in native.

Babylon Native does not do this. Instead, rather than integrate with 
Babylon.js at its lowest level (the library-platform border at WebGL), we 
instead chose to integrate Babylon Native with Babylon.js at the 
`Engine`-level. This approach has several advantages over a WebGL polyfill, 
the most prominent of which are that (1) `Engine`'s higher level of 
abstraction means the surface area of the integration can be minimized
and (2) `Engine`'s nature as a Babylon-specific concept means we can,
when necessary, introduce customizations to the JavaScript side of the 
relationship as well as the native side (which cannot be done with a 
polyfill that has to resemble an externally-defined standard). The point of
integration for Babylon Native, then, is between the `NativeEngine` class 
in Babylon.js and the `NativeEngine` class in Babylon Native.

During the initialization of the NativeEngine component when a Babylon 
Native is first starting up, the initializer exposes a N-API constructor
for the C++ `NativeEngine` type to the JavaScript in a standardized 
location in the JavaScript. This constructor is effectively an 
implementation detail of Babylon.js and is not intended to be used 
directly. The type Babylon Native JavaScript developers are intended to
use is the `NativeEngine` JavaScript type, which is aware of the location of
the native type constructor and will use that constructor to create and 
store a native `NativeEngine` instance as one of its members. The
JavaScript `NativeEngine` will then use this C++ `NativeEngine` instance to
execute all the rendering work required of a Babylon.js `Engine`, keeping 
all implementation details including the existence of the backing native
object hidden from the consuming code. Because of this, code that runs 
against Babylon.js's traditional Web-based engines should be able to run
identically against a C++-backed `NativeEngine` without requiring any 
modification.

## bgfx Integration

In the same way that `NativeEngine` is integrated "above" with JavaScript 
using Babylon.js's `Engine` abstraction, `NativeEngine` is integrated 
"below" with the native platform's graphics technology using
[bgfx](https://bkaradzic.github.io/bgfx/overview.html). bgfx (along with
its sibling libraries bimg and bx) is a lightweight third-party open source
graphics abstraction capable of targeting every major native graphics API.
By using bgfx to abstract away the underlying graphics API, `NativeEngine`
is able to write its "translation layer" connecting the Babylon.js
`Engine` abstraction with the underlying graphics abstraction only once, 
thereby minimizing the amount of platform-specific code that must be written
and maintained as part of the NativeEngine component.

However, a small amount of platform-specific behavior remains necessary, 
most prominently in order to transpile Babylon.js's WebGL shaders into 
other shader languages that can be used by bgfx natively on the various 
platforms. This platform-specific behavior occupies a small, isolated 
percentage of the NativeEngine component's source code (and none of its 
public-facing API); however, it is a sufficiently important and specialized 
part of the codebase that it warrants 
[its own dedicated documentation page](ShaderTranspilation.md).
