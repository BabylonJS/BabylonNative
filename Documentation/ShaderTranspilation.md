# Shader Transpilation

One of the most unusual problems Babylon Native's NativeEngine component 
has to solve is bridging the gap between its "above" and "below" graphics 
abstractions with regards to shaders. "Above," NativeEngine is connected to 
Babylon.js's `Engine` abstraction, which was designed with WebGL in mind 
and so provides all its shaders in ESSL. "Below," Babylon Native uses bgfx 
as a graphics abstraction, which uses platform-dependent native shaders 
(HLSL, GLSL, MSL, etc.) bundled into custom packages. To allow these 
abstractions to work together, NativeEngine must be able to, at runtime,
transpile shaders from the ESSL Babylon Native provides to the 
custom-packaged native shaders bgfx requires.

## The Transpilation Pipeline

The task of shader transpilation is translating Babylon.js's ESSL shader
source into the native shader language of the target graphics platform:
HLSL for DirectX, GLSL for OpenGL (and someday Vulkan), and MSL for 
Metal on Apple platforms. NativeEngine accomplishes this task by first 
compiling the original ESSL shader to SPIR-V, an intermediate 
representation, then disassembling that intermediate representation to the 
shader language of choice. Two tools are used in this process: 
[glslang](https://github.com/KhronosGroup/glslang) and 
[SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross).

glslang is the Khronos Group's official reference compiler for GLSL, of
which ESSL is a specific dialect. Among other features, glslang is able to 
parse ESSL to a syntax tree representation and compile it to SPIR-V. The
syntax tree representation will be discussed further in 
[the next section](#The-glslang-AST-Traversers); for direct shader 
transpilation, the most important feature is the SPIR-V representation.

SPIRV-Cross is a project intended to allow the disassembly of SPIR-V shader 
representations into 
["readable, usable and efficient"](https://github.com/KhronosGroup/SPIRV-Cross#features)
native shading languages including HLSL and Metal. (SPIRV-Cross is also 
capable of outputting GLSL, but we don't use that capability because
glslang can already do that). Once we have a SPIR-V representation of our
shader from glslang, then, we use SPIRV-Cross to disassemble the shader
into the required native shader language, which can then be compiled 
and/or passed as source to bgfx as necessary.

Note that, as partly alluded to above, GLSL-consuming platforms (OpenGL
and Vulkan) in theory may not require the use of SPIRV-Cross because
glslang includes all the required features to produce consumable shader
artifacts. However, at this point SPIRV-Cross is still used for OpenGL
(Vulkan support is not yet implemented) because we also currently use
SPIRV-Cross compiler output to produce the metadata required to populate
the [bgfx custom shader packaging](#bgfx-Custom-Shader-Packaging). This
is a temporary implementation detail tracked by Babylon Native
[issue 300](https://github.com/BabylonJS/BabylonNative/issues/300). When 
this issue is resolved, it should be possible to exclude SPIRV-Cross from
OpenGL- and Vulkan-targeting platforms altogether, retrieving the required
shader metadata instead from the glslang AST traversers.

## The glslang AST Traversers

The prior section described the process of "directly" transpiling shaders 
using glslang and SPIRV-Cross; this is how shaders can be transpiled 
unmodified. However, for some platforms (whether because of requirements 
of the platform or of bgfx's abstraction on top of it), directly 
transpiling the shaders will not produce a usable result. In such cases, as 
an intermediate step in the above process, the shaders must be modified.

When modifying shaders at runtime, NativeEngine makes use of the `Traverser`
mechanism built into glslang to inspect and modify the shader program's
[AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree). Different 
platforms require different shader modifications, all of which are beyond 
the scope of this document and so are heavily commented in the 
implementation code. At a high level, though, each `Traverser` walks 
through the shader's parsed syntax tree, inspects the structure and 
content, and then modifies the syntax tree in some way to effect a change 
in the shader code (renaming a variable, reshaping a type, etc.). This 
operation is done in the middle of the glslang step mentioned above, 
after parsing the original ESSL but before generating the SPIR-V.

## bgfx Custom Shader Packaging

bgfx, as mentioned above, has a customized format that it expects shaders 
to be "packaged" in before they are delivered to the abstraction. This
package is actually the structure of the files that, in more conventional
use cases, are written as the output of bgfx's custom shader preprocessor,
`shaderc`. In typical use cases, bgfx actually expects its shaders to be
written in a custom intermediate language which will then be preprocessed
by `shaderc` and packaged into custom files. Since this is not an option
for Babylon Native, we instead replicate a subset of `shaderc`'s behavior
at runtime in order to package our transpiled native shaders into the 
format bgfx expects.

The specifics of the bgfx shader format are beyond the scope of this 
document to explain, but they typically involve bundling the shader itself 
(which, depending on the platform, may be source code or compiled binaries) 
with headers and footers which contain information about the shader 
(uniforms, attributes, etc.). Note that, by packaging shaders in this 
format, Babylon Native is taking a direct dependency on something which is
technically an implementation detail of bgfx. However, the `shaderc` output
format is relatively stable, so it's not expected to change often; and it's
versioned, so even when it does change, it should be possible to "follow
along" behind changes without being constantly broken by them.
