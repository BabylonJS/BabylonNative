# What Is Supported?

Babylon.js has many, many features; and while the goal of Babylon Native
is to support an overwhelming majority of those features, not all parts of
Babylon.js are equally well supported by Babylon Native at this time. This
document is intended to help readers "eyeball it," checking quickly whether
a given type of feature is likely supported by Babylon Native at this time.
For more precise information about current and upcoming support for
particular features, please don't hesitate to task us on
[the forum](https://forum.babylonjs.com/c/questions/).

## Core Features: <span style="color:green">Supported</span>

**Exceptions:**
-   Audio

The core engine features of Babylon.js (rendering, scene graph 
manipulation, frame-by-frame logic, etc.) are available in Babylon 
Native. As a rule, if it's in 
[`@babylonjs/core`](https://www.npmjs.com/package/@babylonjs/core),
you can probably use it in Babylon Native. The glaring exception to 
this is audio (`Sound`, etc.), which is not yet supported at all.

## Secondary Features: <span style="color:red">Not Supported</span>

**Exceptions:**
-   Text rendering (experimental)

Secondary engine features (GUI, the Inspector, 2D Controls, and other 
ancillary Babylon capabilities) are overwhelmingly not yet supported in
Babylon Native. If it's not a core engine capability and it doesn't fit 
into one of the other categories described below, it probably doesn't work 
in Babylon Native yet.

## XR: <span style="color:lightgreen">Mostly Supported</span>

**Exceptions:**
-   Oculus
-   SteamVR

XR support in Babylon Native is generally extremely robust -- sometimes
even exceeding what's possible with WebXR -- on the platforms where it's
available. Android and iOS are very well supported, and Windows Mixed
Reality (HoloLens) is also well supported. Oculus and other XR platforms 
are not supported yet.

## 3D Assets: <span style="color:lightgreen">Mostly Supported</span>

The bulk of Babylon's featureset for 3D assets (models, 
materials/shaders, etc.) is supported by Babylon Native. More specialized
scenarios may have more nuanced support stories (Draco compression has
*some* but not *full* support, for example), but in general if it's a 
feature of loading or using a 3D asset, it probably works.

## Non-3D Assets: <span style="color:yellow">Mostly Not Supported</span>

**Exceptions:**
-   Textures

Textures are a part of most 3D assets, so they are well supported, 
but other kinds of non-3D assets such as audio, video, vector graphics,
and the like are largely if not completely unsupported right now. 
If it's not a 3D asset or a part of a 3D asset, you should assume it 
can't be used in Babylon Native yet.

## Platforms: <span style="color:lightgreen">Mostly Supported</span>

**Exceptions:**
-   Vulkan

Babylon Native supports all major operating systems as deployment targets
(and, where appropriate, as development platforms). Babylon Native can also
target all modern graphics APIs except, at present, Vulkan.
