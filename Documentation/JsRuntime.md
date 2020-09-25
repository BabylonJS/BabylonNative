# The JsRuntime Component

JsRuntime (which, throughout this document, will be styled "JsRuntime" 
for the component and "`JsRuntime`" for the C++ type) is arguably _the_ 
quintessential Babylon Native component. At the most fundamental level,
Babylon Native is a technology suite designed to allow truly native
capabilities (for example, graphics) to be invoked cross-platform using
the same sorts of tools that can be used on the Web (for example, 
Babylon.js). The foundation of such technology is the ability to run 
JavaScript "business logic" code in a native app and supply the JavaScript 
with capabilities backed by true native implementations.

`JsRuntime` codifies this ability: it is an abstraction that encapsulates 
the state and functionality that other Babylon Native components can always 
depend on being available in every Babylon Native app. In this way, 
JsRuntime can be thought of as a _de facto_ definition: if an app is a 
Babylon Native app, then it has a `JsRuntime`. The following sections 
provide additional context for the motivation, implementation, and usages 
of the JsRuntime component.

## Motivation: Interfacing JavaScript and Native

As mentioned above, one of the fundamental characteristics of a Babylon
Native app is the involvement of a natively-hosted JavaScript engine. Note 
the use of the word _involvement_; Babylon Native (through JsRuntime) goes 
to great lengths to avoid being prescriptive about any of the following 
implementation details:

- What JavaScript engine is used.
- How the JavaScript engine is initialized/owned.
- How threading is controlled.

By avoiding taking a dependency on any of these implementation-level 
specifics, Babylon Native enables the creation of a `JsRuntime` (and, 
likewise, Babylon Native components which depend on `JsRuntime`) in a 
wide variety of circumstances. Several of these usages are discussed in 
more detail [below](#usages-owning-and-"piggybacking"-babylon-native-apps).

Without any specific knowledge of implementation details, then, 
`JsRuntime` must answer two fundamental questions for dependent Babylon
Native components: how do I get on the correct thread to safely call
JavaScript functions, and how do I call JavaScript functions once I'm on
the right thread? `JsRuntime` answers both of these questions with a 
single function:

```
void JsRuntime::Dispatch(std::function<void(Napi::Env)>);
```

This function expresses everything that components need to know and can 
depend on in order to communicate safely and reliably between native and 
JavaScript in any Babylon Native app. Any function provided as an argument 
to `Dispatch(...)` will be executed asynchronously on the JavaScript 
thread, at which time it will be provided a `Napi::Env` as its own argument
which it can use to access JavaScript state and resources. This one-line 
contract is the cornerstone upon which nearly all Babylon Native 
components are built.

## Implementation: `Dispatch` and Lifecycle

There are several nuances that should be taken into consideration when 
understanding the implementation of the JsRuntime component. The choice 
to use N-API as the underlying JavaScript abstraction will not be explored
in this section beyond saying that it was deemed preferable for being 
(at the time the choice was made) lightweight, fast, usable, extensible,
and relatively mature. The two nuances that will be discussed more deeply
in this section concern the dispatch function and the `JsRuntime`'s 
lifecycle.

Considering its centrality to Babylon Native, the implementation of 
`JsRuntime` is notable for containing almost no functionality of its own,
particularly in the implementation of `Dispatch(...)`. This is by design.
JsRuntime is an abstraction of what can be done in any Babylon Native app,
not a specification of how it can be done. The `Dispatch(...)` method, 
then, represents the assumption that, whenever there is a JavaScript
engine with a JavaScript thread, then there must be _some way_ to get onto
that thread. `Dispatch(...)` provides a unified, reliable, and thread-safe
way to encapsulate that assumption, and the actual implementation of the
`std::function` that does the dispatching is entirely dependent on the
[usage](#usages-owning-and-"piggybacking"-babylon-native-apps).

What little functionality does exist in the implementation of `JsRuntime`
is almost entirely constructors, and the reason for this implementation is
because of the lifecycle of a `JsRuntime` object. Perhaps the most subtle
nuance of the `JsRuntime` is that it is actually owned by the JavaScript
engine, and its lifecycle is consequently dependent upon -- and _strictly
less than_ -- that of the JavaScript engine that owns it. A `JsRuntime`
_cannot_ be created before there is a JavaScript engine instance (in 
implementation, a `Napi::Env`) to own it, and the `JsRuntime` will 
correspondingly be destroyed when the `Napi::Env` that owns it is torn 
down. The reason for this is that the `std::function` that underlies
`Dispatch(...)` must necessarily take a copy of a `Napi::Env` object 
in order to supply that `Napi::Env` to its dispatched callbacks, and 
`Napi::Env` copies cannot outlive the JavaScript engine instance to 
which they refer. This must be taken into consideration when using a 
`JsRuntime` to dispatch functionality back to the JavaScript thread from
other threads (for example, in the case of asynchronous work): it is 
possible to take and hold a non-const reference to a `JsRuntime` and use 
that to return to the JavaScript thread later in execution, but it is
not possible to take ownership of the `JsRuntime` or "pin" it into 
existence should the owning JavaScript context be torn down, so care must
be taken to correctly cancel asynchronous work as necessary in order to 
avoid calling `Dispatch(...)` on a `JsRuntime` that has already been 
destroyed.

## Usages: Owning and "Piggybacking" Babylon Native Apps

The canonical Babylon Native app (and, by extension, the canonical use 
case for a JsRuntime) is one in which a Babylon Native-aware portion of 
the app owns both the JavaScript engine and the thread on which it runs.
This use case is encapsulated by the AppRuntime component and is discussed
in more detail in that component's 
[dedicated documentation page](AppRuntime.md); it is, however, not the 
only usage.

It is also possible to create `JsRuntime` objects (and thus Babylon Native 
apps) that "piggyback" on other app infrastructures. For example, the 
[Babylon React Native](https://github.com/BabylonJS/BabylonReactNative/)
project integrates Babylon Native into React Native apps, which come with
their own pre-existing JavaScript engine instances and threads over which
Babylon Native has no control whatsoever. In situations like these, the 
minimalism of the JsRuntime contract proves essential. Because Babylon 
Native's dependencies are predominantly on the JsRuntime, which abstracts
away almost all implementation details including which JavaScript engine 
is in use and how dispatching to the JavaScript thread is done, it is 
possible to make a `JsRuntime` using 
[vastly different implementation logic](https://github.com/BabylonJS/BabylonReactNative/blob/6ba5397d0ed9464dd98dc34dd6b68bda2bdfe535/Modules/%40babylonjs/react-native/android/src/main/cpp/BabylonNativeInterop.cpp#L41-L50)
than is used 
[in the canonical case](https://github.com/BabylonJS/BabylonNative/blob/1b40d7b59b6e4d01dd4b9bb1db4f0349a5d03cc6/Core/AppRuntime/Source/AppRuntime.cpp#L11); and once such a `JsRuntime` is created, the rest of the desired
Babylon Native components can depend upon it and use it identically in 
either case, without having change their behavior or expectations 
based on whether Babylon Native owns or "piggybacks" on the underlying app 
infrastructure.
