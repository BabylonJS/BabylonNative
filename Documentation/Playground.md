# The Playground App

The Playground app, which is the only in-repo Babylon Native app supported 
on all platforms, evolved from a simple test harness designed to run 
small bespoke JavaScript scripts in order to exercise under-development
code paths. This is still the most common usage of the Playground, and 
will likely remain so for many developers. However, the long-term vision
of the Playground app is significantly more sophisticated and may open the
door for new use cases, both for the Playground app and for Babylon Native 
in general.

## The Current State

At present, the Playground app is primarily used to execute 
[experience.js](../Apps/Playground/Scripts/experience.js); most commonly, 
developers will modify experience.js to test whatever feature is currently
being developed, sometimes checking in the change with an enabling flag if
they feel that other developers may also benefit from the same test. 
There are, however, a few additional features and use cases that are less 
obvious.

Though designed to execute experience.js by default, the Playground app is 
also capable of loading and executing arbitrary JavaScript files. How this 
is done varies by platform -- Win32 allows JS files to be passed as 
command line arguments while UWP allows them to be executed via protocal 
invocation, for example -- but the behavior is the same on all platforms
where invocation is supported: the app will first load the Babylon.js 
scripts (Babylon itself, the glTF loaders, etc.), then execute the 
requested external JS file, then execute a special "playground runner" 
intended to call behaviors that may have been defined in the requested JS
file. Essentially, this allows for externally-defined JS files to control 
the behavior of the Playground app as an alternative to experience.js --
at runtime and without the need to recompile or redeploy.

The Playground app also features a "hot reload" capability on certain
platforms (Win32 and UWP, for now). The current implementation is that 
pressing a button (`R` at present, likely to be remapped to `F5` in the 
future) while the app is in focus causes the Playground to tear down 
its entire `AppRuntime` object -- which includes the JavaScript context -- 
and reload the experience from disk without actually requiring a restart of
the app. This feature is of little use when executing experience.js, which 
is colocated with the Playground app and is rarely modified except when 
working in the Babylon Native repository. However, if the Playground app
was launched to execute an external JavaScript file, the hot reload feature
will reload _that file_ from disk, and any changes made to it since the last
time it was loaded will be immediately visible in the experience.

This combination of features is how the Playground app got its name. With
external file execution and hot reload, it is very possible to have a 
JavaScript file open in a text editor and loaded into the Playground app 
at the same time, side by side. Edits can then be made and saved in the
JavaScript file, and all that's necessary to see that code running live is
to click on the Playground app's window and press `R`. This capability 
deliberately emulates the 
[Babylon.js Playground](https://playground.babylonjs.com/) to the point that
it's often possible to take the exact same code that works on the Web
playground (assuming it doesn't directly invoke browser APIs such as the 
DOM), save it to a local JavaScript file, and successfully run it in the 
Babylon Native Playground app. (Enabling this is the purpose of the 
"playground runner" script mentioned above. JavaScript files created in
this way characteristically define a `createScene()` function and consume
externally defined `engine` and `scene` variables, so the "playground 
runner" checks for the presence of a `createScene()` function and, if it 
exists, provides the required variables and calls the function.)

## A Standalone Playground

The capabilities described above are fairly rudimentary, but they have been 
added as a proof-of-concept for the larger and more long-term vision of the 
Playground app the Babylon team intends to actualize. Ultimately, the 
Playground app is planned to acquire features that may make it worthwhile
to release as a standalone app -- perhaps through the Windows 10 app store 
or other means -- and might power novel usage and development scenarios 
including real app development on devices on which no developer experience 
currently exists.

In particular, the Babylon team is interested in exploring the possibility 
of rapid and approachable development on and for self-contained XR devices
such as the Microsoft HoloLens and the Oculus Quest. At present, developing 
experiences for such devices requires a degree of tooling and expertise 
that reduces the approachability of the experience: users must download 
SDKs, perhaps game engines, connect their devices to separate development
machines, etc. in order to even begin to see results from their immersive
native XR development efforts. In theory, however, all that's needed to 
develop an experience for the Babylon Native Playground app is the 
Playground app itself and a text editor. (A physical keyboard may also 
prove helpful when writing code.)

This prospect -- an easy, accessible, on-device development experience 
powered by a standalone Playground app -- opens the door for several 
scenarios. With such a capability, it should be possible to develop
fully-functional native XR experiences on the XR device itself, without 
requiring external tools or even a separate development computer. It 
should be possible for developers to jump into the process extremely 
easily and quickly: download the Playground app, download a source code
editor, write a simple JS file, then launch that file in the Playground.
It should be possible for such developers to leverage the same JavaScript
and Babylon.js development experience that can also serve them on the Web;
and at the same time their native Playground apps can leverage capabilities
that are not available on the Web (nonstandardized platform capabilities, 
file access, etc.). Furthermore, it should be possible to do such 
development completely offline, and potentially even in the field.

In summary (though a number features must still be added to bring this 
vision to life), the currently-intended vision for the Babylon Native 
Playground app is that it should become a runtime of sorts -- a sort of 
common-denominator Babylon Native compiled feature set that allows a 
certain kind of real Babylon Native apps to be written and run in scenarios 
that might not have been accessible otherwise.

## Packaging and "Apps"

Implicit to the above discussion of developing and running Playground-based 
experiences as independent concepts is the idea of a "Playground App." 
In order to fully support the usages discussed above, the Playground app
must be able to load experiences that are more complex than a single JS 
file. The groundwork for such experiences is already in place: the 
Windows Playgrounds are all technically capable of loading multiple
JS files and already support relative paths so that resources adjacent to
the external JavaScript file can be referenced easily. However, what is
currently in place is both too minimal and too cryptic to be useful.

To fully support the use cases described above, it is likely that a formal
notion of a "Playground App" must be codified. What this notion would 
entail is an ongoing discussion, and so this section of this documentation 
page is expected to be expanded in the future as discussions progress.
However, the following high-level specifications are currently considered
priorities for whatever "App" mechanism is ultimately resolved upon.

- Apps must be easy (both conceptually and technically) to create.
- It must be possible -- and ideally easy -- to redistribute apps, 
    preferably as single files.
- It must be easy to launch a Playground app. The mechanism by which such 
    an app might be easily launched is likely to vary by deployment 
    platform.
- It should be possible to unpack and customize an app, thereby enabling 
    knowlegeable users to adapt distributed apps on-the-fly to new 
    capabilities and usage scenarios.

As mentioned above, discussions around the Playground app are ongoing, and
this page will be updated as new information becomes available. For any 
inquiries, please contact the Babylon team by posting a question in on the 
[forum](https://forum.babylonjs.com/).
