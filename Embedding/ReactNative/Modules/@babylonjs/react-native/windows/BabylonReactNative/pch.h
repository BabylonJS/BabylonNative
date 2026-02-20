#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif

#include <unknwn.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Navigation.h>
#include <winrt/Windows.UI.Xaml.h>

#include <winrt/Microsoft.ReactNative.h>

// BabylonNative
#ifndef NODE_ADDON_API_DISABLE_NODE_SPECIFIC
#define NODE_ADDON_API_DISABLE_NODE_SPECIFIC
#endif

#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
#define NODE_ADDON_API_DISABLE_DEPRECATED
#endif

#include "BabylonNative.h"