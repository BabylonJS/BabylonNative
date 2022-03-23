# When to Use Babylon Native

## High Level Overview

-   Babylon Native technologies are specialized and will be differently
    suited to different usages.
-   For developers coming from a Web-first perspective, Babylon Native
    technologies should likely be "last resort" choices, selected only
    when simpler hybrid app alternatives will not suffice.
-   For developers coming from a native-first perspective, Babylon Native
    is a prime candidate and will frequently be among the simplest and 
    easiest ways to feature-rich rendering to native apps.

## Introduction

Babylon Native and Babylon React Native are specialized technologies with
specialized use cases. Both belong to the larger family of platforms
which allow Web technologies to be integrated into non-Web applications, 
including native applications on multiple platforms. Sometimes, Babylon 
Native or Babylon React Native will be the best technologies to enable
a certain use case; in other cases, an alternative from the same technology
family would be a more suitable choice. This document explores some of the
considerations involved in this choice of platform and attempts to provide 
a thought framework for deciding when to use Babylon Native. The bulk of
this document will be dedicated to the perspective of a developer coming
from a Web-first background as that is the case with more ambiguity; for
a developer with a native-first perspective, the choices are clearer and
are covered in 
[a dedicated later section](#the-native-first-perspective).

## Alternative Platform Choices

From a Web-first perspective, there are many platform options that may, in 
different scenarios, provide alternatives to Babylon Native/Babylon React 
Native, the most prominent of which include the following:

-   [Progressive Web Apps](https://en.wikipedia.org/wiki/Progressive_web_application)
-   [Ionic](https://ionicframework.com/)
-   [Electron](https://www.electronjs.org/)
-   [React Native](https://reactnative.dev/) (or any other true native 
    platform) using a WebView

Each of these platforms has its own unique strengths and weaknesses, and
exploring those attributes is beyond the scope of this document. An 
important similarity they share, however, is that *any* of these platforms
will tend to be a simpler and easier choice than a Babylon Native 
technology *for usages where Babylon Native's strengths are not needed*.
In other words, if it is possible to create a given experience using one of
these other platforms, doing so is likely to be a simpler -- and in that 
sense better -- alternative to using a Babylon Native technology. This is
because the costs associated with using Babylon Native technologies 
overwhelmingly come in the form of complexity, and it is of course better to
avoid paying those costs when possible. However, there are a number of 
usages where no other alternative we're aware of will be acceptable, and in
these circumstances using Babylon Native technologies will be the best -- 
and in some cases the only -- viable choice.

## Babylon Native's Costs: When to *Not* Use Babylon Native

Babylon Native's and Babylon React Native's complexity costs arise from the
fact that both sit further toward the "native" side of the hybrid app 
spectrum than any of the alternatives mentioned above.

![Hybrid app platform spectrum](./Images/hybrid_app_spectrum.png)

These complexities can be approximately quantified as "distance" from a 
true Web app: the more different from raw Web a platform is, the more
complex we consider it to be. (Again, this is from the perspective of a 
developer who favors Web technologies over native technologies; for a 
native-first perspective, please skip to 
[the native app section](#the-native-first-perspective).) 
Generally speaking, a developer using any hybrid app platfrom will need to 
be broadly familiar with Web development concepts *as well as* some 
quantity of additional concepts proportional to the platform's difference
from a true Web app. A PWA developer needs to understand pretty much 
everything a raw Web developer needs to understand, plus additional 
concepts pertaining to making the PWA installable; an Ionic developer needs
to understand what a PWA developer understands as well as how to use Ionic
to generate Android and/or iOS apps; a React Native developer needs to 
understand much of the same Web-like practices as the others, but also
requires quite a bit of specialized context about React Native's bespoke
systems; and a Babylon React Native developer needs to understand everything
a React Native developer understands plus additional information specific to
using Babylon React Native.

This leads us to repeat our earlier assertion: if you *can* use a different 
platform (without sacrificing experience quality or features), you probably 
*should* use a different platform. In this sense, Babylon Native 
technologies are platforms of last resort, so they should be chosen when no
easier option will suit your needs.

## So When *Should* I Use Babylon Native?

In exchange for the complexities, Babylon Native technologies provide the 
power to do things that other hybrid app platforms simply cannot do. The
most prominent example of this comes from Babylon React Native: Babylon
React Native allows for the creation of XR (i.e., ARKit) apps that run
on iOS, which is simply not achievable using any other hybrid app platform
today. Thus, if your intended usage is to provide an AR experience built 
with Web-like technologies that runs on iOS, that is an excellent reason
to use Babylon React Native as it is currently the only viable choice.

More generally, Babylon Native technologies allow developers to reach 
through the fundamental limitations of Web-like platforms to directly 
expose native functionality to the Web-like hybrid app layer. Any time your 
experience requires a native capability that isn't exposed to typical 
Web-like usage at all, Babylon Native may be a good choice for exposing it; 
and if the feature you require is in any way associated with graphics or 
rendering, Babylon Native is likely among the best available choices due to 
the infrastructure it provides for integrating Web-like and native rendering
capabilities.

As an example, consider the challenges of OpenXR integration. 
Simplistically speaking, OpenXR typically works by having the XR experience
render to a shared native texture, which is then handed off to a separate 
process to be presented on the XR device (HMD, etc.). Using Web-like 
technologies to render to raw native textures (especially ones passed in 
from outside sources) is not something that's easy to set up from scratch. 
However, because Babylon Native already provides the infrastructure to use 
Babylon.js to render to native textures, this makes it easy to encapsulate
the native texture into something that can be targeted by Web-like 
rendering while the underlying relationship with the presenting process
can be managed entirely natively. It is this very capability that 
motivated the development of Babylon React Native in the first place.

While native graphics-related capabilities are certainly Babylon Native and 
Babylon React Native's greatest strengths, they are by no means the only
features for which Babylon Native is the right choice. Other native-only
capabilities, advanced asynchrony, multi-JavaScript context... Because
Babylon Native technologies provide access to and through the platform in
ways that other platforms preclude, any usage that requires the app to go 
beyond the "normal" set of allowed behaviors for Web-like applications is
a good candidate for development in Babylon Native technologies. The more
"normal" an app's scenario is, the less likely it needs or should use 
Babylon Native's flexibility and power; but when "normal" isn't enough, 
Babylon Native technologies will often be the best -- and sometimes the
only -- viable choice.

## The Native-First Perspective

While Babylon Native technologies should largely be "last resorts" for 
developers approaching the hybrid app space from a Web-first perspective,
the opposite is true from a native-first perspective. From such a 
perspective, the goal of a hybrid app is to enable Web-like rendering
capabilities while incurring as few of the costs associated with Web
technologies as possible; and at this goal, Babylon Native excels.
Babylon Native is designed to make very few assumptions about the 
environment into which it is integrated, which makes it possible to 
"inject" Babylon Native into existing native applications which want
to *use* Web-like rendering capabilities without pivoting to *become*
Web-like hybrid apps. As an example, consider an academic codebase written
in C (as so many of them are) for use in some spatially-inclined scientific
field such as astronomy. Adding a visualizer to this existing codebase
would be extremely difficult to do from scratch, and the resulting 
visualizer would likely be very rudimentary in its appearance and 
capabilities. However, a visualizer built on Babylon Native could very
easily be encapsulated behind a C API and added to the existing codebase,
providing the full rendering power of Babylon.js at a very, very minimal
code cost.

In short, in all the ways that Babylon Native technologies are "last 
resort" choices from a Web-first perspective, Babylon Native should be
among the first options considered when looking at adding rendering from 
a native-first perspective. Of course, Babylon Native will not be as fast
or powerful as a full-native gaming solution like Unity or Unreal, so
native usages that require that kind of top-tier power will likely not be 
able to make do with Babylon Native. However, for native apps looking to 
quickly and easily add feature-rich rendering, few alternatives will 
provide as much rendering capability for as little native infrastructure 
cost as Babylon Native can offer.
