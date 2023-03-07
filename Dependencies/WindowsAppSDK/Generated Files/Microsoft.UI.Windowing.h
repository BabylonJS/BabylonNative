/* Header file automatically generated from Microsoft.UI.Windowing.idl */
/*
 * File built with Microsoft(R) MIDLRT Compiler Engine Version 10.00.0231 
 */

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include <rpc.h>
#include <rpcndr.h>

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include <windows.h>
#include <ole2.h>
#endif /*COM_NO_WINDOWS_H*/
#ifndef __Microsoft2EUI2EWindowing_h__
#define __Microsoft2EUI2EWindowing_h__
#ifndef __Microsoft2EUI2EWindowing_p_h__
#define __Microsoft2EUI2EWindowing_p_h__


#pragma once

// Ensure that the setting of the /ns_prefix command line switch is consistent for all headers.
// If you get an error from the compiler indicating "warning C4005: 'CHECK_NS_PREFIX_STATE': macro redefinition", this
// indicates that you have included two different headers with different settings for the /ns_prefix MIDL command line switch
#if !defined(DISABLE_NS_PREFIX_CHECKS)
#define CHECK_NS_PREFIX_STATE "always"
#endif // !defined(DISABLE_NS_PREFIX_CHECKS)


#pragma push_macro("MIDL_CONST_ID")
#undef MIDL_CONST_ID
#define MIDL_CONST_ID const __declspec(selectany)


//  API Contract Inclusion Definitions
#if !defined(SPECIFIC_API_CONTRACT_DEFINITIONS)
#if !defined(MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION)
#define MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION 0x10002
#endif // defined(MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)
#define WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)
#define WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION 0xe0000
#endif // defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)

#if !defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)
#define WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)
#define WINDOWS_PHONE_PHONECONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)
#define WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)

#if !defined(WINDOWS_UI_WEBUI_CORE_WEBUICOMMANDBARCONTRACT_VERSION)
#define WINDOWS_UI_WEBUI_CORE_WEBUICOMMANDBARCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_UI_WEBUI_CORE_WEBUICOMMANDBARCONTRACT_VERSION)

#endif // defined(SPECIFIC_API_CONTRACT_DEFINITIONS)


// Header files for imported files
#include "inspectable.h"
#include "AsyncInfo.h"
#include "EventToken.h"
#include "windowscontracts.h"
#include "Windows.Foundation.h"
#include "Microsoft.Foundation.h"
#include "Microsoft.UI.h"
#include "Windows.Graphics.h"
#include "Windows.UI.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindow;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow ABI::Microsoft::UI::Windowing::IAppWindow

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindow2;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 ABI::Microsoft::UI::Windowing::IAppWindow2

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowChangedEventArgs;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs ABI::Microsoft::UI::Windowing::IAppWindowChangedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowChangedEventArgs2;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 ABI::Microsoft::UI::Windowing::IAppWindowChangedEventArgs2

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowClosingEventArgs;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs ABI::Microsoft::UI::Windowing::IAppWindowClosingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter ABI::Microsoft::UI::Windowing::IAppWindowPresenter

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowPresenterFactory;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory ABI::Microsoft::UI::Windowing::IAppWindowPresenterFactory

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowStatics;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics ABI::Microsoft::UI::Windowing::IAppWindowStatics

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowTitleBar;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar ABI::Microsoft::UI::Windowing::IAppWindowTitleBar

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowTitleBar2;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 ABI::Microsoft::UI::Windowing::IAppWindowTitleBar2

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IAppWindowTitleBarStatics;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics ABI::Microsoft::UI::Windowing::IAppWindowTitleBarStatics

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface ICompactOverlayPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter ABI::Microsoft::UI::Windowing::ICompactOverlayPresenter

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface ICompactOverlayPresenterStatics;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics ABI::Microsoft::UI::Windowing::ICompactOverlayPresenterStatics

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IDisplayArea;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea ABI::Microsoft::UI::Windowing::IDisplayArea

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IDisplayAreaStatics;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics ABI::Microsoft::UI::Windowing::IDisplayAreaStatics

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IDisplayAreaStatics2;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 ABI::Microsoft::UI::Windowing::IDisplayAreaStatics2

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IDisplayAreaWatcher;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IFullScreenPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter ABI::Microsoft::UI::Windowing::IFullScreenPresenter

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IFullScreenPresenterStatics;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics ABI::Microsoft::UI::Windowing::IFullScreenPresenterStatics

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IOverlappedPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter ABI::Microsoft::UI::Windowing::IOverlappedPresenter

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IOverlappedPresenter2;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 ABI::Microsoft::UI::Windowing::IOverlappedPresenter2

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IOverlappedPresenterStatics;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics ABI::Microsoft::UI::Windowing::IOverlappedPresenterStatics

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                interface IOverlappedPresenterStatics2;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 ABI::Microsoft::UI::Windowing::IOverlappedPresenterStatics2

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class DisplayArea;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_USE
#define DEF___FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("3d833884-2258-5c48-8d45-3642fe8c6755"))
IIterator<ABI::Microsoft::UI::Windowing::DisplayArea*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::DisplayArea*, ABI::Microsoft::UI::Windowing::IDisplayArea*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Microsoft.UI.Windowing.DisplayArea>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Microsoft::UI::Windowing::DisplayArea*> __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_t;
#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::Collections::__FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Windowing::IDisplayArea*>
//#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_t ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Windowing::IDisplayArea*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_USE
#define DEF___FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("fb253085-6cfc-5fa6-948e-97383f5be36c"))
IIterable<ABI::Microsoft::UI::Windowing::DisplayArea*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::DisplayArea*, ABI::Microsoft::UI::Windowing::IDisplayArea*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Microsoft.UI.Windowing.DisplayArea>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Microsoft::UI::Windowing::DisplayArea*> __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_t;
#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::Collections::__FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Windowing::IDisplayArea*>
//#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_t ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Windowing::IDisplayArea*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_USE
#define DEF___FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("63490e13-d075-5fe2-b6a4-86a44095215f"))
IVectorView<ABI::Microsoft::UI::Windowing::DisplayArea*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::DisplayArea*, ABI::Microsoft::UI::Windowing::IDisplayArea*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Microsoft.UI.Windowing.DisplayArea>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Microsoft::UI::Windowing::DisplayArea*> __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_t;
#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::Collections::__FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Windowing::IDisplayArea*>
//#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Windowing::IDisplayArea*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class AppWindow;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("eddd665f-7d53-5b2d-901d-9c12010816e7"))
ITypedEventHandler<ABI::Microsoft::UI::Windowing::AppWindow*,IInspectable*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::AppWindow*, ABI::Microsoft::UI::Windowing::IAppWindow*>,IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Windowing.AppWindow, Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Windowing::AppWindow*,IInspectable*> __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IAppWindow*,IInspectable*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IAppWindow*,IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class AppWindowChangedEventArgs;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("cda07756-1584-5049-9ad1-cca782242d34"))
ITypedEventHandler<ABI::Microsoft::UI::Windowing::AppWindow*,ABI::Microsoft::UI::Windowing::AppWindowChangedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::AppWindow*, ABI::Microsoft::UI::Windowing::IAppWindow*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::AppWindowChangedEventArgs*, ABI::Microsoft::UI::Windowing::IAppWindowChangedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Windowing.AppWindow, Microsoft.UI.Windowing.AppWindowChangedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Windowing::AppWindow*,ABI::Microsoft::UI::Windowing::AppWindowChangedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IAppWindow*,ABI::Microsoft::UI::Windowing::IAppWindowChangedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IAppWindow*,ABI::Microsoft::UI::Windowing::IAppWindowChangedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class AppWindowClosingEventArgs;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("fd0e72c8-8daa-50a2-959f-4d41446f1fc7"))
ITypedEventHandler<ABI::Microsoft::UI::Windowing::AppWindow*,ABI::Microsoft::UI::Windowing::AppWindowClosingEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::AppWindow*, ABI::Microsoft::UI::Windowing::IAppWindow*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::AppWindowClosingEventArgs*, ABI::Microsoft::UI::Windowing::IAppWindowClosingEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Windowing.AppWindow, Microsoft.UI.Windowing.AppWindowClosingEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Windowing::AppWindow*,ABI::Microsoft::UI::Windowing::AppWindowClosingEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IAppWindow*,ABI::Microsoft::UI::Windowing::IAppWindowClosingEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IAppWindow*,ABI::Microsoft::UI::Windowing::IAppWindowClosingEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class DisplayAreaWatcher;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("2ddb3359-9837-5ba8-803e-eb5ab1ccb67f"))
ITypedEventHandler<ABI::Microsoft::UI::Windowing::DisplayAreaWatcher*,IInspectable*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::DisplayAreaWatcher*, ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher*>,IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Windowing.DisplayAreaWatcher, Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Windowing::DisplayAreaWatcher*,IInspectable*> __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher*,IInspectable*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher*,IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("1ab60859-caaf-5b34-bab0-3d9035eb80cb"))
ITypedEventHandler<ABI::Microsoft::UI::Windowing::DisplayAreaWatcher*,ABI::Microsoft::UI::Windowing::DisplayArea*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::DisplayAreaWatcher*, ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Windowing::DisplayArea*, ABI::Microsoft::UI::Windowing::IDisplayArea*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Windowing.DisplayAreaWatcher, Microsoft.UI.Windowing.DisplayArea>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Windowing::DisplayAreaWatcher*,ABI::Microsoft::UI::Windowing::DisplayArea*> __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher*,ABI::Microsoft::UI::Windowing::IDisplayArea*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher*,ABI::Microsoft::UI::Windowing::IDisplayArea*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Windows {
        namespace UI {
            struct Color;
            
        } /* UI */
    } /* Windows */
} /* ABI */


#ifndef DEF___FIReference_1_Windows__CUI__CColor_USE
#define DEF___FIReference_1_Windows__CUI__CColor_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("ab8e5d11-b0c1-5a21-95ae-f16bf3a37624"))
IReference<struct ABI::Windows::UI::Color> : IReference_impl<struct ABI::Windows::UI::Color> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.IReference`1<Windows.UI.Color>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IReference<struct ABI::Windows::UI::Color> __FIReference_1_Windows__CUI__CColor_t;
#define __FIReference_1_Windows__CUI__CColor ABI::Windows::Foundation::__FIReference_1_Windows__CUI__CColor_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIReference_1_Windows__CUI__CColor ABI::Windows::Foundation::IReference<ABI::Windows::UI::Color>
//#define __FIReference_1_Windows__CUI__CColor_t ABI::Windows::Foundation::IReference<ABI::Windows::UI::Color>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIReference_1_Windows__CUI__CColor_USE */








namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct DisplayId DisplayId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct IconId IconId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct WindowId WindowId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */



namespace ABI {
    namespace Windows {
        namespace Graphics {
            
            typedef struct PointInt32 PointInt32;
            
        } /* Graphics */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace Graphics {
            
            typedef struct RectInt32 RectInt32;
            
        } /* Graphics */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace Graphics {
            
            typedef struct SizeInt32 SizeInt32;
            
        } /* Graphics */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Windows {
        namespace UI {
            
            typedef struct Color Color;
            
        } /* UI */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum AppWindowPresenterKind : int AppWindowPresenterKind;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum CompactOverlaySize : int CompactOverlaySize;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum DisplayAreaFallback : int DisplayAreaFallback;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum DisplayAreaWatcherStatus : int DisplayAreaWatcherStatus;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum IconShowOptions : int IconShowOptions;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum OverlappedPresenterState : int OverlappedPresenterState;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                
                typedef enum TitleBarHeightOption : int TitleBarHeightOption;
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */



























namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class AppWindowPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class AppWindowTitleBar;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class CompactOverlayPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class FullScreenPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                class OverlappedPresenter;
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */










/*
 *
 * Struct Microsoft.UI.Windowing.AppWindowPresenterKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum AppWindowPresenterKind : int
                {
                    AppWindowPresenterKind_Default = 0,
                    AppWindowPresenterKind_CompactOverlay = 1,
                    AppWindowPresenterKind_FullScreen = 2,
                    AppWindowPresenterKind_Overlapped = 3,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.CompactOverlaySize
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum CompactOverlaySize : int
                {
                    CompactOverlaySize_Small = 0,
                    CompactOverlaySize_Medium = 1,
                    CompactOverlaySize_Large = 2,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.DisplayAreaFallback
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum DisplayAreaFallback : int
                {
                    DisplayAreaFallback_None = 0,
                    DisplayAreaFallback_Primary = 1,
                    DisplayAreaFallback_Nearest = 2,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.DisplayAreaWatcherStatus
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum DisplayAreaWatcherStatus : int
                {
                    DisplayAreaWatcherStatus_Created = 0,
                    DisplayAreaWatcherStatus_Started = 1,
                    DisplayAreaWatcherStatus_EnumerationCompleted = 2,
                    DisplayAreaWatcherStatus_Stopping = 3,
                    DisplayAreaWatcherStatus_Stopped = 4,
                    DisplayAreaWatcherStatus_Aborted = 5,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.IconShowOptions
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum IconShowOptions : int
                {
                    IconShowOptions_ShowIconAndSystemMenu = 0,
                    IconShowOptions_HideIconAndSystemMenu = 1,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.OverlappedPresenterState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum OverlappedPresenterState : int
                {
                    OverlappedPresenterState_Maximized = 0,
                    OverlappedPresenterState_Minimized = 1,
                    OverlappedPresenterState_Restored = 2,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.TitleBarHeightOption
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [v1_enum, contract] */
                enum TitleBarHeightOption : int
                {
                    TitleBarHeightOption_Standard = 0,
                    TitleBarHeightOption_Tall = 1,
                };
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindow
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindow
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindow[] = L"Microsoft.UI.Windowing.IAppWindow";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("CFA788B3-643B-5C5E-AD4E-321D48A82ACD"), exclusiveto, contract] */
                MIDL_INTERFACE("CFA788B3-643B-5C5E-AD4E-321D48A82ACD")
                IAppWindow : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Id(
                        /* [retval, out] */ABI::Microsoft::UI::WindowId * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsShownInSwitchers(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsShownInSwitchers(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsVisible(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OwnerWindowId(
                        /* [retval, out] */ABI::Microsoft::UI::WindowId * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Graphics::PointInt32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Presenter(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IAppWindowPresenter * * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Size(
                        /* [retval, out] */ABI::Windows::Graphics::SizeInt32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Title(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Title(
                        /* [in] */HSTRING value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TitleBar(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IAppWindowTitleBar * * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Destroy(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Hide(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Move(
                        /* [in] */ABI::Windows::Graphics::PointInt32 position
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE MoveAndResize(
                        /* [in] */ABI::Windows::Graphics::RectInt32 rect
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE MoveAndResizeRelativeToDisplayArea(
                        /* [in] */ABI::Windows::Graphics::RectInt32 rect,
                        /* [in] */ABI::Microsoft::UI::Windowing::IDisplayArea * displayarea
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Resize(
                        /* [in] */ABI::Windows::Graphics::SizeInt32 size
                        ) = 0;
                    /* [overload, default_overload] */virtual HRESULT STDMETHODCALLTYPE SetIcon(
                        /* [in] */HSTRING iconPath
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE SetIconWithIconId(
                        /* [in] */ABI::Microsoft::UI::IconId iconId
                        ) = 0;
                    /* [overload, default_overload] */virtual HRESULT STDMETHODCALLTYPE SetPresenter(
                        /* [in] */ABI::Microsoft::UI::Windowing::IAppWindowPresenter * appWindowPresenter
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE SetPresenterByKind(
                        /* [in] */ABI::Microsoft::UI::Windowing::AppWindowPresenterKind appWindowPresenterKind
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE Show(void) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE ShowWithActivation(
                        /* [in] */::boolean activateWindow
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Changed(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Changed(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Closing(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Closing(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Destroying(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Destroying(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindow=_uuidof(IAppWindow);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindow2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindow
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindow2[] = L"Microsoft.UI.Windowing.IAppWindow2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("6CD41292-794C-5CAC-8961-210D012C6EBC"), exclusiveto, contract] */
                MIDL_INTERFACE("6CD41292-794C-5CAC-8961-210D012C6EBC")
                IAppWindow2 : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ClientSize(
                        /* [retval, out] */ABI::Windows::Graphics::SizeInt32 * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveInZOrderAtBottom(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveInZOrderAtTop(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveInZOrderBelow(
                        /* [in] */ABI::Microsoft::UI::WindowId windowId
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ResizeClient(
                        /* [in] */ABI::Windows::Graphics::SizeInt32 size
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ShowOnceWithRequestedStartupState(void) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindow2=_uuidof(IAppWindow2);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowChangedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowChangedEventArgs[] = L"Microsoft.UI.Windowing.IAppWindowChangedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("2182BC5D-FDAC-5C3E-BF37-7D8D684E9D1D"), exclusiveto, contract] */
                MIDL_INTERFACE("2182BC5D-FDAC-5C3E-BF37-7D8D684E9D1D")
                IAppWindowChangedEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DidPositionChange(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DidPresenterChange(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DidSizeChange(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DidVisibilityChange(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowChangedEventArgs=_uuidof(IAppWindowChangedEventArgs);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowChangedEventArgs2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowChangedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowChangedEventArgs2[] = L"Microsoft.UI.Windowing.IAppWindowChangedEventArgs2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("A773AB4C-A5EC-50E8-98AC-247FE6CD4227"), exclusiveto, contract] */
                MIDL_INTERFACE("A773AB4C-A5EC-50E8-98AC-247FE6CD4227")
                IAppWindowChangedEventArgs2 : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DidZOrderChange(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsZOrderAtBottom(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsZOrderAtTop(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ZOrderBelowWindowId(
                        /* [retval, out] */ABI::Microsoft::UI::WindowId * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowChangedEventArgs2=_uuidof(IAppWindowChangedEventArgs2);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowClosingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowClosingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowClosingEventArgs[] = L"Microsoft.UI.Windowing.IAppWindowClosingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("0E09D90B-2261-590B-9AD1-8504991D8754"), exclusiveto, contract] */
                MIDL_INTERFACE("0E09D90B-2261-590B-9AD1-8504991D8754")
                IAppWindowClosingEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cancel(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Cancel(
                        /* [in] */::boolean value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowClosingEventArgs=_uuidof(IAppWindowClosingEventArgs);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowPresenter[] = L"Microsoft.UI.Windowing.IAppWindowPresenter";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("BC3042C2-C6C6-5632-8989-FF0EC6D3B40D"), exclusiveto, contract] */
                MIDL_INTERFACE("BC3042C2-C6C6-5632-8989-FF0EC6D3B40D")
                IAppWindowPresenter : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Kind(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::AppWindowPresenterKind * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowPresenter=_uuidof(IAppWindowPresenter);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowPresenterFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowPresenterFactory[] = L"Microsoft.UI.Windowing.IAppWindowPresenterFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("62082E3C-1368-5238-90D1-E932DC718A82"), exclusiveto, contract] */
                MIDL_INTERFACE("62082E3C-1368-5238-90D1-E932DC718A82")
                IAppWindowPresenterFactory : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowPresenterFactory=_uuidof(IAppWindowPresenterFactory);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindow
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowStatics[] = L"Microsoft.UI.Windowing.IAppWindowStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("3C315C24-D540-5D72-B518-B226B83627CB"), exclusiveto, contract] */
                MIDL_INTERFACE("3C315C24-D540-5D72-B518-B226B83627CB")
                IAppWindowStatics : public IInspectable
                {
                public:
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IAppWindow * * result
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE CreateWithPresenter(
                        /* [in] */ABI::Microsoft::UI::Windowing::IAppWindowPresenter * appWindowPresenter,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IAppWindow * * result
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE CreateWithPresenterAndOwner(
                        /* [in] */ABI::Microsoft::UI::Windowing::IAppWindowPresenter * appWindowPresenter,
                        /* [in] */ABI::Microsoft::UI::WindowId ownerWindowId,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IAppWindow * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetFromWindowId(
                        /* [in] */ABI::Microsoft::UI::WindowId windowId,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IAppWindow * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowStatics=_uuidof(IAppWindowStatics);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowTitleBar
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowTitleBar
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowTitleBar[] = L"Microsoft.UI.Windowing.IAppWindowTitleBar";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("5574EFA2-C91C-5700-A363-539C71A7AAF4"), exclusiveto, contract] */
                MIDL_INTERFACE("5574EFA2-C91C-5700-A363-539C71A7AAF4")
                IAppWindowTitleBar : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BackgroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_BackgroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonBackgroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonBackgroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonForegroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonForegroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonHoverBackgroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonHoverBackgroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonHoverForegroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonHoverForegroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonInactiveBackgroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonInactiveBackgroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonInactiveForegroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonInactiveForegroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonPressedBackgroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonPressedBackgroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ButtonPressedForegroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ButtonPressedForegroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ExtendsContentIntoTitleBar(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ExtendsContentIntoTitleBar(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ForegroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ForegroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Height(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IconShowOptions(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IconShowOptions * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IconShowOptions(
                        /* [in] */ABI::Microsoft::UI::Windowing::IconShowOptions value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InactiveBackgroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InactiveBackgroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InactiveForegroundColor(
                        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InactiveForegroundColor(
                        /* [in] */__FIReference_1_Windows__CUI__CColor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LeftInset(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RightInset(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ResetToDefault(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetDragRectangles(
                        /* [in] */UINT32 __valueSize,
                        /* [size_is(__valueSize), in] */ABI::Windows::Graphics::RectInt32 * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowTitleBar=_uuidof(IAppWindowTitleBar);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowTitleBar2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowTitleBar
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowTitleBar2[] = L"Microsoft.UI.Windowing.IAppWindowTitleBar2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("86FAED38-748A-5B4B-9CCF-3BA0496C9041"), exclusiveto, contract] */
                MIDL_INTERFACE("86FAED38-748A-5B4B-9CCF-3BA0496C9041")
                IAppWindowTitleBar2 : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PreferredHeightOption(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::TitleBarHeightOption * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PreferredHeightOption(
                        /* [in] */ABI::Microsoft::UI::Windowing::TitleBarHeightOption value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowTitleBar2=_uuidof(IAppWindowTitleBar2);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowTitleBarStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowTitleBar
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowTitleBarStatics[] = L"Microsoft.UI.Windowing.IAppWindowTitleBarStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("9E1DA52E-8B15-54D6-A886-F7B9F9D930B2"), exclusiveto, contract] */
                MIDL_INTERFACE("9E1DA52E-8B15-54D6-A886-F7B9F9D930B2")
                IAppWindowTitleBarStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE IsCustomizationSupported(
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IAppWindowTitleBarStatics=_uuidof(IAppWindowTitleBarStatics);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.ICompactOverlayPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.CompactOverlayPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_ICompactOverlayPresenter[] = L"Microsoft.UI.Windowing.ICompactOverlayPresenter";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("EFEB0812-6FC7-5B7D-BD92-CC8F9A6454C9"), exclusiveto, contract] */
                MIDL_INTERFACE("EFEB0812-6FC7-5B7D-BD92-CC8F9A6454C9")
                ICompactOverlayPresenter : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InitialSize(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::CompactOverlaySize * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InitialSize(
                        /* [in] */ABI::Microsoft::UI::Windowing::CompactOverlaySize value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ICompactOverlayPresenter=_uuidof(ICompactOverlayPresenter);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.ICompactOverlayPresenterStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.CompactOverlayPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_ICompactOverlayPresenterStatics[] = L"Microsoft.UI.Windowing.ICompactOverlayPresenterStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("EAB93186-4F6A-52F9-8C03-DA57A1522F6E"), exclusiveto, contract] */
                MIDL_INTERFACE("EAB93186-4F6A-52F9-8C03-DA57A1522F6E")
                ICompactOverlayPresenterStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::ICompactOverlayPresenter * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ICompactOverlayPresenterStatics=_uuidof(ICompactOverlayPresenterStatics);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayArea
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayArea
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayArea[] = L"Microsoft.UI.Windowing.IDisplayArea";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("5C7E0537-B621-5579-BCAE-A84AA8746167"), exclusiveto, contract] */
                MIDL_INTERFACE("5C7E0537-B621-5579-BCAE-A84AA8746167")
                IDisplayArea : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DisplayId(
                        /* [retval, out] */ABI::Microsoft::UI::DisplayId * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsPrimary(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OuterBounds(
                        /* [retval, out] */ABI::Windows::Graphics::RectInt32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_WorkArea(
                        /* [retval, out] */ABI::Windows::Graphics::RectInt32 * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayArea=_uuidof(IDisplayArea);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayAreaStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayArea
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayAreaStatics[] = L"Microsoft.UI.Windowing.IDisplayAreaStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("02AB4926-211E-5D49-8E4B-2AF193DAED09"), exclusiveto, contract] */
                MIDL_INTERFACE("02AB4926-211E-5D49-8E4B-2AF193DAED09")
                IDisplayAreaStatics : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Primary(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IDisplayArea * * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateWatcher(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IDisplayAreaWatcher * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE FindAll(
                        /* [retval, out] */__FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetFromWindowId(
                        /* [in] */ABI::Microsoft::UI::WindowId windowId,
                        /* [in] */ABI::Microsoft::UI::Windowing::DisplayAreaFallback displayAreaFallback,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IDisplayArea * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetFromPoint(
                        /* [in] */ABI::Windows::Graphics::PointInt32 point,
                        /* [in] */ABI::Microsoft::UI::Windowing::DisplayAreaFallback displayAreaFallback,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IDisplayArea * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetFromRect(
                        /* [in] */ABI::Windows::Graphics::RectInt32 rect,
                        /* [in] */ABI::Microsoft::UI::Windowing::DisplayAreaFallback displayAreaFallback,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IDisplayArea * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayAreaStatics=_uuidof(IDisplayAreaStatics);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayAreaStatics2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayArea
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayAreaStatics2[] = L"Microsoft.UI.Windowing.IDisplayAreaStatics2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("7207AD4B-890D-5DD7-BC18-78FFD9544D8F"), exclusiveto, contract] */
                MIDL_INTERFACE("7207AD4B-890D-5DD7-BC18-78FFD9544D8F")
                IDisplayAreaStatics2 : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE GetFromDisplayId(
                        /* [in] */ABI::Microsoft::UI::DisplayId displayId,
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IDisplayArea * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayAreaStatics2=_uuidof(IDisplayAreaStatics2);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayAreaWatcher
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayAreaWatcher
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayAreaWatcher[] = L"Microsoft.UI.Windowing.IDisplayAreaWatcher";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("83F6562F-D3A0-548B-8E4F-A99BE3D95C9C"), exclusiveto, contract] */
                MIDL_INTERFACE("83F6562F-D3A0-548B-8E4F-A99BE3D95C9C")
                IDisplayAreaWatcher : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Status(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::DisplayAreaWatcherStatus * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Start(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Added(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Added(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_EnumerationCompleted(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_EnumerationCompleted(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Removed(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Removed(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Stopped(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Stopped(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Updated(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Updated(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayAreaWatcher=_uuidof(IDisplayAreaWatcher);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IFullScreenPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.FullScreenPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IFullScreenPresenter[] = L"Microsoft.UI.Windowing.IFullScreenPresenter";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("FA9141FD-B8DD-5DA1-8B2B-7CDADB76F593"), exclusiveto, contract] */
                MIDL_INTERFACE("FA9141FD-B8DD-5DA1-8B2B-7CDADB76F593")
                IFullScreenPresenter : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IFullScreenPresenter=_uuidof(IFullScreenPresenter);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IFullScreenPresenterStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.FullScreenPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IFullScreenPresenterStatics[] = L"Microsoft.UI.Windowing.IFullScreenPresenterStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("2EC0D2C1-E086-55BB-A3B2-44942E231C67"), exclusiveto, contract] */
                MIDL_INTERFACE("2EC0D2C1-E086-55BB-A3B2-44942E231C67")
                IFullScreenPresenterStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IFullScreenPresenter * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IFullScreenPresenterStatics=_uuidof(IFullScreenPresenterStatics);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenter[] = L"Microsoft.UI.Windowing.IOverlappedPresenter";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("21693970-4F4C-5172-9E9D-682A2D174884"), exclusiveto, contract] */
                MIDL_INTERFACE("21693970-4F4C-5172-9E9D-682A2D174884")
                IOverlappedPresenter : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasBorder(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasTitleBar(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsAlwaysOnTop(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsAlwaysOnTop(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsMaximizable(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsMaximizable(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsMinimizable(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsMinimizable(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsModal(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsModal(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsResizable(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsResizable(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_State(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::OverlappedPresenterState * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Maximize(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Minimize(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Restore(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetBorderAndTitleBar(
                        /* [in] */::boolean hasBorder,
                        /* [in] */::boolean hasTitleBar
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IOverlappedPresenter=_uuidof(IOverlappedPresenter);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenter2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenter2[] = L"Microsoft.UI.Windowing.IOverlappedPresenter2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("5C6CCD93-4244-5CD2-B355-ED5EA34DF730"), exclusiveto, contract] */
                MIDL_INTERFACE("5C6CCD93-4244-5CD2-B355-ED5EA34DF730")
                IOverlappedPresenter2 : public IInspectable
                {
                public:
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE MinimizeWithActivation(
                        /* [in] */::boolean activateWindow
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE RestoreWithActivation(
                        /* [in] */::boolean activateWindow
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IOverlappedPresenter2=_uuidof(IOverlappedPresenter2);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenterStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenterStatics[] = L"Microsoft.UI.Windowing.IOverlappedPresenterStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("997225E4-7B00-5AEE-A4BE-D4068D1999E2"), exclusiveto, contract] */
                MIDL_INTERFACE("997225E4-7B00-5AEE-A4BE-D4068D1999E2")
                IOverlappedPresenterStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IOverlappedPresenter * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateForContextMenu(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IOverlappedPresenter * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateForDialog(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IOverlappedPresenter * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateForToolWindow(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::IOverlappedPresenter * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IOverlappedPresenterStatics=_uuidof(IOverlappedPresenterStatics);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenterStatics2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenterStatics2[] = L"Microsoft.UI.Windowing.IOverlappedPresenterStatics2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Windowing {
                /* [object, uuid("ED5C4F92-32F4-5D15-80D0-B2A5EFA04D39"), exclusiveto, contract] */
                MIDL_INTERFACE("ED5C4F92-32F4-5D15-80D0-B2A5EFA04D39")
                IOverlappedPresenterStatics2 : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RequestedStartupState(
                        /* [retval, out] */ABI::Microsoft::UI::Windowing::OverlappedPresenterState * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IOverlappedPresenterStatics2=_uuidof(IOverlappedPresenterStatics2);
                
            } /* Windowing */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Windowing.AppWindow
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IAppWindowStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindow ** Default Interface **
 *    Microsoft.UI.Windowing.IAppWindow2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindow_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindow_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindow[] = L"Microsoft.UI.Windowing.AppWindow";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowChangedEventArgs ** Default Interface **
 *    Microsoft.UI.Windowing.IAppWindowChangedEventArgs2
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowChangedEventArgs[] = L"Microsoft.UI.Windowing.AppWindowChangedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowClosingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowClosingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowClosingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowClosingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowClosingEventArgs[] = L"Microsoft.UI.Windowing.AppWindowClosingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowPresenter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowPresenter[] = L"Microsoft.UI.Windowing.AppWindowPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowTitleBar
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IAppWindowTitleBarStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowTitleBar ** Default Interface **
 *    Microsoft.UI.Windowing.IAppWindowTitleBar2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowTitleBar_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowTitleBar_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowTitleBar[] = L"Microsoft.UI.Windowing.AppWindowTitleBar";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.CompactOverlayPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.ICompactOverlayPresenterStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.ICompactOverlayPresenter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_CompactOverlayPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_CompactOverlayPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_CompactOverlayPresenter[] = L"Microsoft.UI.Windowing.CompactOverlayPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.DisplayArea
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IDisplayAreaStatics2 interface starting with version 1.2 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *   Static Methods exist on the Microsoft.UI.Windowing.IDisplayAreaStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IDisplayArea ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_DisplayArea_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_DisplayArea_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_DisplayArea[] = L"Microsoft.UI.Windowing.DisplayArea";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.DisplayAreaWatcher
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IDisplayAreaWatcher ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_DisplayAreaWatcher_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_DisplayAreaWatcher_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_DisplayAreaWatcher[] = L"Microsoft.UI.Windowing.DisplayAreaWatcher";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.FullScreenPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IFullScreenPresenterStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IFullScreenPresenter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_FullScreenPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_FullScreenPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_FullScreenPresenter[] = L"Microsoft.UI.Windowing.FullScreenPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.OverlappedPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IOverlappedPresenterStatics2 interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *   Static Methods exist on the Microsoft.UI.Windowing.IOverlappedPresenterStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IOverlappedPresenter ** Default Interface **
 *    Microsoft.UI.Windowing.IOverlappedPresenter2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_OverlappedPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_OverlappedPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_OverlappedPresenter[] = L"Microsoft.UI.Windowing.OverlappedPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000




#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2;

#endif // ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__)
#define ____FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea;

typedef struct __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl;

interface __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea
{
    CONST_VTBL struct __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__)
#define ____FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea;

typedef  struct __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Microsoft__CUI__CWindowing__CDisplayArea **first);

    END_INTERFACE
} __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl;

interface __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea
{
    CONST_VTBL struct __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea;

typedef struct __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
            /* [in] */ __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl;

interface __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea
{
    CONST_VTBL struct __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayAreaVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * sender,/* [in] */ __RPC__in_opt IInspectable * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectableVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * sender,/* [in] */ __RPC__in_opt IInspectable * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectableVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayAreaVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayAreaVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayAreaVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

struct __x_ABI_CWindows_CUI_CColor;

#if !defined(____FIReference_1_Windows__CUI__CColor_INTERFACE_DEFINED__)
#define ____FIReference_1_Windows__CUI__CColor_INTERFACE_DEFINED__

typedef interface __FIReference_1_Windows__CUI__CColor __FIReference_1_Windows__CUI__CColor;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIReference_1_Windows__CUI__CColor;

typedef struct __FIReference_1_Windows__CUI__CColorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIReference_1_Windows__CUI__CColor * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIReference_1_Windows__CUI__CColor * This );
    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIReference_1_Windows__CUI__CColor * This );

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIReference_1_Windows__CUI__CColor * This, 
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( __RPC__in __FIReference_1_Windows__CUI__CColor * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( __RPC__in __FIReference_1_Windows__CUI__CColor * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )(__RPC__in __FIReference_1_Windows__CUI__CColor * This, /* [retval][out] */ __RPC__out struct __x_ABI_CWindows_CUI_CColor *value);
    END_INTERFACE
} __FIReference_1_Windows__CUI__CColorVtbl;

interface __FIReference_1_Windows__CUI__CColor
{
    CONST_VTBL struct __FIReference_1_Windows__CUI__CColorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIReference_1_Windows__CUI__CColor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIReference_1_Windows__CUI__CColor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIReference_1_Windows__CUI__CColor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIReference_1_Windows__CUI__CColor_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIReference_1_Windows__CUI__CColor_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIReference_1_Windows__CUI__CColor_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIReference_1_Windows__CUI__CColor_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 
#endif /* COBJMACROS */


#endif // ____FIReference_1_Windows__CUI__CColor_INTERFACE_DEFINED__







typedef struct __x_ABI_CMicrosoft_CUI_CDisplayId __x_ABI_CMicrosoft_CUI_CDisplayId;


typedef struct __x_ABI_CMicrosoft_CUI_CIconId __x_ABI_CMicrosoft_CUI_CIconId;


typedef struct __x_ABI_CMicrosoft_CUI_CWindowId __x_ABI_CMicrosoft_CUI_CWindowId;




typedef struct __x_ABI_CWindows_CGraphics_CPointInt32 __x_ABI_CWindows_CGraphics_CPointInt32;


typedef struct __x_ABI_CWindows_CGraphics_CRectInt32 __x_ABI_CWindows_CGraphics_CRectInt32;


typedef struct __x_ABI_CWindows_CGraphics_CSizeInt32 __x_ABI_CWindows_CGraphics_CSizeInt32;




typedef struct __x_ABI_CWindows_CUI_CColor __x_ABI_CWindows_CUI_CColor;




typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_CAppWindowPresenterKind __x_ABI_CMicrosoft_CUI_CWindowing_CAppWindowPresenterKind;


typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_CCompactOverlaySize __x_ABI_CMicrosoft_CUI_CWindowing_CCompactOverlaySize;


typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaFallback __x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaFallback;


typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaWatcherStatus __x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaWatcherStatus;


typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_CIconShowOptions __x_ABI_CMicrosoft_CUI_CWindowing_CIconShowOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_COverlappedPresenterState __x_ABI_CMicrosoft_CUI_CWindowing_COverlappedPresenterState;


typedef enum __x_ABI_CMicrosoft_CUI_CWindowing_CTitleBarHeightOption __x_ABI_CMicrosoft_CUI_CWindowing_CTitleBarHeightOption;










































/*
 *
 * Struct Microsoft.UI.Windowing.AppWindowPresenterKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_CAppWindowPresenterKind
{
    AppWindowPresenterKind_Default = 0,
    AppWindowPresenterKind_CompactOverlay = 1,
    AppWindowPresenterKind_FullScreen = 2,
    AppWindowPresenterKind_Overlapped = 3,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.CompactOverlaySize
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_CCompactOverlaySize
{
    CompactOverlaySize_Small = 0,
    CompactOverlaySize_Medium = 1,
    CompactOverlaySize_Large = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.DisplayAreaFallback
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaFallback
{
    DisplayAreaFallback_None = 0,
    DisplayAreaFallback_Primary = 1,
    DisplayAreaFallback_Nearest = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.DisplayAreaWatcherStatus
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaWatcherStatus
{
    DisplayAreaWatcherStatus_Created = 0,
    DisplayAreaWatcherStatus_Started = 1,
    DisplayAreaWatcherStatus_EnumerationCompleted = 2,
    DisplayAreaWatcherStatus_Stopping = 3,
    DisplayAreaWatcherStatus_Stopped = 4,
    DisplayAreaWatcherStatus_Aborted = 5,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.IconShowOptions
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_CIconShowOptions
{
    IconShowOptions_ShowIconAndSystemMenu = 0,
    IconShowOptions_HideIconAndSystemMenu = 1,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.OverlappedPresenterState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_COverlappedPresenterState
{
    OverlappedPresenterState_Maximized = 0,
    OverlappedPresenterState_Minimized = 1,
    OverlappedPresenterState_Restored = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Windowing.TitleBarHeightOption
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CWindowing_CTitleBarHeightOption
{
    TitleBarHeightOption_Standard = 0,
    TitleBarHeightOption_Tall = 1,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindow
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindow
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindow[] = L"Microsoft.UI.Windowing.IAppWindow";
/* [object, uuid("CFA788B3-643B-5C5E-AD4E-321D48A82ACD"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Id )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowId * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsShownInSwitchers )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsShownInSwitchers )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsVisible )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OwnerWindowId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowId * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CPointInt32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Presenter )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Size )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CSizeInt32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Title )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Title )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TitleBar )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * * value
        );
    HRESULT ( STDMETHODCALLTYPE *Destroy )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This
        );
    HRESULT ( STDMETHODCALLTYPE *Hide )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This
        );
    HRESULT ( STDMETHODCALLTYPE *Move )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CPointInt32 position
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *MoveAndResize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CRectInt32 rect
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *MoveAndResizeRelativeToDisplayArea )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CRectInt32 rect,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * displayarea
        );
    HRESULT ( STDMETHODCALLTYPE *Resize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CSizeInt32 size
        );
    /* [overload, default_overload] */HRESULT ( STDMETHODCALLTYPE *SetIcon )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */HSTRING iconPath
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *SetIconWithIconId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CIconId iconId
        );
    /* [overload, default_overload] */HRESULT ( STDMETHODCALLTYPE *SetPresenter )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * appWindowPresenter
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *SetPresenterByKind )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CAppWindowPresenterKind appWindowPresenterKind
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *Show )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *ShowWithActivation )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */boolean activateWindow
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Changed )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowChangedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Changed )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Closing )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_Microsoft__CUI__CWindowing__CAppWindowClosingEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Closing )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Destroying )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CAppWindow_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Destroying )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_Id(This,value) \
    ( (This)->lpVtbl->get_Id(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_IsShownInSwitchers(This,value) \
    ( (This)->lpVtbl->get_IsShownInSwitchers(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_put_IsShownInSwitchers(This,value) \
    ( (This)->lpVtbl->put_IsShownInSwitchers(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_IsVisible(This,value) \
    ( (This)->lpVtbl->get_IsVisible(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_OwnerWindowId(This,value) \
    ( (This)->lpVtbl->get_OwnerWindowId(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_Presenter(This,value) \
    ( (This)->lpVtbl->get_Presenter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_Size(This,value) \
    ( (This)->lpVtbl->get_Size(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_Title(This,value) \
    ( (This)->lpVtbl->get_Title(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_put_Title(This,value) \
    ( (This)->lpVtbl->put_Title(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_get_TitleBar(This,value) \
    ( (This)->lpVtbl->get_TitleBar(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_Destroy(This) \
    ( (This)->lpVtbl->Destroy(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_Hide(This) \
    ( (This)->lpVtbl->Hide(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_Move(This,position) \
    ( (This)->lpVtbl->Move(This,position) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_MoveAndResize(This,rect) \
    ( (This)->lpVtbl->MoveAndResize(This,rect) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_MoveAndResizeRelativeToDisplayArea(This,rect,displayarea) \
    ( (This)->lpVtbl->MoveAndResizeRelativeToDisplayArea(This,rect,displayarea) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_Resize(This,size) \
    ( (This)->lpVtbl->Resize(This,size) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_SetIcon(This,iconPath) \
    ( (This)->lpVtbl->SetIcon(This,iconPath) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_SetIconWithIconId(This,iconId) \
    ( (This)->lpVtbl->SetIconWithIconId(This,iconId) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_SetPresenter(This,appWindowPresenter) \
    ( (This)->lpVtbl->SetPresenter(This,appWindowPresenter) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_SetPresenterByKind(This,appWindowPresenterKind) \
    ( (This)->lpVtbl->SetPresenterByKind(This,appWindowPresenterKind) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_Show(This) \
    ( (This)->lpVtbl->Show(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_ShowWithActivation(This,activateWindow) \
    ( (This)->lpVtbl->ShowWithActivation(This,activateWindow) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_add_Changed(This,handler,token) \
    ( (This)->lpVtbl->add_Changed(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_remove_Changed(This,token) \
    ( (This)->lpVtbl->remove_Changed(This,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_add_Closing(This,handler,token) \
    ( (This)->lpVtbl->add_Closing(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_remove_Closing(This,token) \
    ( (This)->lpVtbl->remove_Closing(This,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_add_Destroying(This,handler,token) \
    ( (This)->lpVtbl->add_Destroying(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_remove_Destroying(This,token) \
    ( (This)->lpVtbl->remove_Destroying(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindow2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindow
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindow2[] = L"Microsoft.UI.Windowing.IAppWindow2";
/* [object, uuid("6CD41292-794C-5CAC-8961-210D012C6EBC"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ClientSize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CSizeInt32 * value
        );
    HRESULT ( STDMETHODCALLTYPE *MoveInZOrderAtBottom )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This
        );
    HRESULT ( STDMETHODCALLTYPE *MoveInZOrderAtTop )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This
        );
    HRESULT ( STDMETHODCALLTYPE *MoveInZOrderBelow )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId
        );
    HRESULT ( STDMETHODCALLTYPE *ResizeClient )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CSizeInt32 size
        );
    HRESULT ( STDMETHODCALLTYPE *ShowOnceWithRequestedStartupState )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2 * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_get_ClientSize(This,value) \
    ( (This)->lpVtbl->get_ClientSize(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_MoveInZOrderAtBottom(This) \
    ( (This)->lpVtbl->MoveInZOrderAtBottom(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_MoveInZOrderAtTop(This) \
    ( (This)->lpVtbl->MoveInZOrderAtTop(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_MoveInZOrderBelow(This,windowId) \
    ( (This)->lpVtbl->MoveInZOrderBelow(This,windowId) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_ResizeClient(This,size) \
    ( (This)->lpVtbl->ResizeClient(This,size) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_ShowOnceWithRequestedStartupState(This) \
    ( (This)->lpVtbl->ShowOnceWithRequestedStartupState(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowChangedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowChangedEventArgs[] = L"Microsoft.UI.Windowing.IAppWindowChangedEventArgs";
/* [object, uuid("2182BC5D-FDAC-5C3E-BF37-7D8D684E9D1D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DidPositionChange )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DidPresenterChange )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DidSizeChange )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DidVisibilityChange )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs * This,
        /* [retval, out] */boolean * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_get_DidPositionChange(This,value) \
    ( (This)->lpVtbl->get_DidPositionChange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_get_DidPresenterChange(This,value) \
    ( (This)->lpVtbl->get_DidPresenterChange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_get_DidSizeChange(This,value) \
    ( (This)->lpVtbl->get_DidSizeChange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_get_DidVisibilityChange(This,value) \
    ( (This)->lpVtbl->get_DidVisibilityChange(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowChangedEventArgs2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowChangedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowChangedEventArgs2[] = L"Microsoft.UI.Windowing.IAppWindowChangedEventArgs2";
/* [object, uuid("A773AB4C-A5EC-50E8-98AC-247FE6CD4227"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DidZOrderChange )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsZOrderAtBottom )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsZOrderAtTop )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ZOrderBelowWindowId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2 * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowId * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_get_DidZOrderChange(This,value) \
    ( (This)->lpVtbl->get_DidZOrderChange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_get_IsZOrderAtBottom(This,value) \
    ( (This)->lpVtbl->get_IsZOrderAtBottom(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_get_IsZOrderAtTop(This,value) \
    ( (This)->lpVtbl->get_IsZOrderAtTop(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_get_ZOrderBelowWindowId(This,value) \
    ( (This)->lpVtbl->get_ZOrderBelowWindowId(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowChangedEventArgs2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowClosingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowClosingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowClosingEventArgs[] = L"Microsoft.UI.Windowing.IAppWindowClosingEventArgs";
/* [object, uuid("0E09D90B-2261-590B-9AD1-8504991D8754"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cancel )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Cancel )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs * This,
        /* [in] */boolean value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_get_Cancel(This,value) \
    ( (This)->lpVtbl->get_Cancel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_put_Cancel(This,value) \
    ( (This)->lpVtbl->put_Cancel(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowClosingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowPresenter[] = L"Microsoft.UI.Windowing.IAppWindowPresenter";
/* [object, uuid("BC3042C2-C6C6-5632-8989-FF0EC6D3B40D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Kind )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CAppWindowPresenterKind * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_get_Kind(This,value) \
    ( (This)->lpVtbl->get_Kind(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowPresenterFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowPresenterFactory[] = L"Microsoft.UI.Windowing.IAppWindowPresenterFactory";
/* [object, uuid("62082E3C-1368-5238-90D1-E932DC718A82"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenterFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindow
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowStatics[] = L"Microsoft.UI.Windowing.IAppWindowStatics";
/* [object, uuid("3C315C24-D540-5D72-B518-B226B83627CB"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [overload] */HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *CreateWithPresenter )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * appWindowPresenter,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *CreateWithPresenterAndOwner )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowPresenter * appWindowPresenter,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId ownerWindowId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetFromWindowId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindow * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_Create(This,result) \
    ( (This)->lpVtbl->Create(This,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_CreateWithPresenter(This,appWindowPresenter,result) \
    ( (This)->lpVtbl->CreateWithPresenter(This,appWindowPresenter,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_CreateWithPresenterAndOwner(This,appWindowPresenter,ownerWindowId,result) \
    ( (This)->lpVtbl->CreateWithPresenterAndOwner(This,appWindowPresenter,ownerWindowId,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_GetFromWindowId(This,windowId,result) \
    ( (This)->lpVtbl->GetFromWindowId(This,windowId,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowTitleBar
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowTitleBar
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowTitleBar[] = L"Microsoft.UI.Windowing.IAppWindowTitleBar";
/* [object, uuid("5574EFA2-C91C-5700-A363-539C71A7AAF4"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_BackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonHoverBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonHoverBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonHoverForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonHoverForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonInactiveBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonInactiveBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonInactiveForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonInactiveForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonPressedBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonPressedBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ButtonPressedForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ButtonPressedForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ExtendsContentIntoTitleBar )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ExtendsContentIntoTitleBar )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Height )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IconShowOptions )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIconShowOptions * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IconShowOptions )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CIconShowOptions value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InactiveBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InactiveBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InactiveForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InactiveForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LeftInset )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RightInset )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [retval, out] */INT32 * value
        );
    HRESULT ( STDMETHODCALLTYPE *ResetToDefault )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This
        );
    HRESULT ( STDMETHODCALLTYPE *SetDragRectangles )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar * This,
        /* [in] */UINT32 __valueSize,
        /* [size_is(__valueSize), in] */__x_ABI_CWindows_CGraphics_CRectInt32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_BackgroundColor(This,value) \
    ( (This)->lpVtbl->get_BackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_BackgroundColor(This,value) \
    ( (This)->lpVtbl->put_BackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonBackgroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonBackgroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonForegroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonForegroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonHoverBackgroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonHoverBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonHoverBackgroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonHoverBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonHoverForegroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonHoverForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonHoverForegroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonHoverForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonInactiveBackgroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonInactiveBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonInactiveBackgroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonInactiveBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonInactiveForegroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonInactiveForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonInactiveForegroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonInactiveForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonPressedBackgroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonPressedBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonPressedBackgroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonPressedBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ButtonPressedForegroundColor(This,value) \
    ( (This)->lpVtbl->get_ButtonPressedForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ButtonPressedForegroundColor(This,value) \
    ( (This)->lpVtbl->put_ButtonPressedForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ExtendsContentIntoTitleBar(This,value) \
    ( (This)->lpVtbl->get_ExtendsContentIntoTitleBar(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ExtendsContentIntoTitleBar(This,value) \
    ( (This)->lpVtbl->put_ExtendsContentIntoTitleBar(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_ForegroundColor(This,value) \
    ( (This)->lpVtbl->get_ForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_ForegroundColor(This,value) \
    ( (This)->lpVtbl->put_ForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_Height(This,value) \
    ( (This)->lpVtbl->get_Height(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_IconShowOptions(This,value) \
    ( (This)->lpVtbl->get_IconShowOptions(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_IconShowOptions(This,value) \
    ( (This)->lpVtbl->put_IconShowOptions(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_InactiveBackgroundColor(This,value) \
    ( (This)->lpVtbl->get_InactiveBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_InactiveBackgroundColor(This,value) \
    ( (This)->lpVtbl->put_InactiveBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_InactiveForegroundColor(This,value) \
    ( (This)->lpVtbl->get_InactiveForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_put_InactiveForegroundColor(This,value) \
    ( (This)->lpVtbl->put_InactiveForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_LeftInset(This,value) \
    ( (This)->lpVtbl->get_LeftInset(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_get_RightInset(This,value) \
    ( (This)->lpVtbl->get_RightInset(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_ResetToDefault(This) \
    ( (This)->lpVtbl->ResetToDefault(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_SetDragRectangles(This,__valueSize,value) \
    ( (This)->lpVtbl->SetDragRectangles(This,__valueSize,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowTitleBar2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowTitleBar
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowTitleBar2[] = L"Microsoft.UI.Windowing.IAppWindowTitleBar2";
/* [object, uuid("86FAED38-748A-5B4B-9CCF-3BA0496C9041"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PreferredHeightOption )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CTitleBarHeightOption * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PreferredHeightOption )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2 * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CTitleBarHeightOption value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_get_PreferredHeightOption(This,value) \
    ( (This)->lpVtbl->get_PreferredHeightOption(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_put_PreferredHeightOption(This,value) \
    ( (This)->lpVtbl->put_PreferredHeightOption(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBar2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IAppWindowTitleBarStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.AppWindowTitleBar
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IAppWindowTitleBarStatics[] = L"Microsoft.UI.Windowing.IAppWindowTitleBarStatics";
/* [object, uuid("9E1DA52E-8B15-54D6-A886-F7B9F9D930B2"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *IsCustomizationSupported )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics * This,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_IsCustomizationSupported(This,result) \
    ( (This)->lpVtbl->IsCustomizationSupported(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIAppWindowTitleBarStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.ICompactOverlayPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.CompactOverlayPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_ICompactOverlayPresenter[] = L"Microsoft.UI.Windowing.ICompactOverlayPresenter";
/* [object, uuid("EFEB0812-6FC7-5B7D-BD92-CC8F9A6454C9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InitialSize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CCompactOverlaySize * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InitialSize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CCompactOverlaySize value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_get_InitialSize(This,value) \
    ( (This)->lpVtbl->get_InitialSize(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_put_InitialSize(This,value) \
    ( (This)->lpVtbl->put_InitialSize(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.ICompactOverlayPresenterStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.CompactOverlayPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_ICompactOverlayPresenterStatics[] = L"Microsoft.UI.Windowing.ICompactOverlayPresenterStatics";
/* [object, uuid("EAB93186-4F6A-52F9-8C03-DA57A1522F6E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenter * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_Create(This,result) \
    ( (This)->lpVtbl->Create(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CICompactOverlayPresenterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayArea
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayArea
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayArea[] = L"Microsoft.UI.Windowing.IDisplayArea";
/* [object, uuid("5C7E0537-B621-5579-BCAE-A84AA8746167"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DisplayId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDisplayId * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsPrimary )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OuterBounds )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CRectInt32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_WorkArea )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CRectInt32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_get_DisplayId(This,value) \
    ( (This)->lpVtbl->get_DisplayId(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_get_IsPrimary(This,value) \
    ( (This)->lpVtbl->get_IsPrimary(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_get_OuterBounds(This,value) \
    ( (This)->lpVtbl->get_OuterBounds(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_get_WorkArea(This,value) \
    ( (This)->lpVtbl->get_WorkArea(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayAreaStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayArea
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayAreaStatics[] = L"Microsoft.UI.Windowing.IDisplayAreaStatics";
/* [object, uuid("02AB4926-211E-5D49-8E4B-2AF193DAED09"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Primary )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * * value
        );
    HRESULT ( STDMETHODCALLTYPE *CreateWatcher )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * * result
        );
    HRESULT ( STDMETHODCALLTYPE *FindAll )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
        /* [retval, out] */__FIVectorView_1_Microsoft__CUI__CWindowing__CDisplayArea * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetFromWindowId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaFallback displayAreaFallback,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetFromPoint )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CPointInt32 point,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaFallback displayAreaFallback,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetFromRect )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CRectInt32 rect,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaFallback displayAreaFallback,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_get_Primary(This,value) \
    ( (This)->lpVtbl->get_Primary(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_CreateWatcher(This,result) \
    ( (This)->lpVtbl->CreateWatcher(This,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_FindAll(This,result) \
    ( (This)->lpVtbl->FindAll(This,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_GetFromWindowId(This,windowId,displayAreaFallback,result) \
    ( (This)->lpVtbl->GetFromWindowId(This,windowId,displayAreaFallback,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_GetFromPoint(This,point,displayAreaFallback,result) \
    ( (This)->lpVtbl->GetFromPoint(This,point,displayAreaFallback,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_GetFromRect(This,rect,displayAreaFallback,result) \
    ( (This)->lpVtbl->GetFromRect(This,rect,displayAreaFallback,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayAreaStatics2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayArea
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayAreaStatics2[] = L"Microsoft.UI.Windowing.IDisplayAreaStatics2";
/* [object, uuid("7207AD4B-890D-5DD7-BC18-78FFD9544D8F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetFromDisplayId )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2 * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CDisplayId displayId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayArea * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_GetFromDisplayId(This,displayId,result) \
    ( (This)->lpVtbl->GetFromDisplayId(This,displayId,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaStatics2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.UI.Windowing.IDisplayAreaWatcher
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.DisplayAreaWatcher
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IDisplayAreaWatcher[] = L"Microsoft.UI.Windowing.IDisplayAreaWatcher";
/* [object, uuid("83F6562F-D3A0-548B-8E4F-A99BE3D95C9C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcherVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Status )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CDisplayAreaWatcherStatus * value
        );
    HRESULT ( STDMETHODCALLTYPE *Start )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This
        );
    HRESULT ( STDMETHODCALLTYPE *Stop )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Added )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Added )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_EnumerationCompleted )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_EnumerationCompleted )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Removed )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Removed )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Stopped )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Stopped )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Updated )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CWindowing__CDisplayAreaWatcher_Microsoft__CUI__CWindowing__CDisplayArea * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Updated )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcherVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcherVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_get_Status(This,value) \
    ( (This)->lpVtbl->get_Status(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_Start(This) \
    ( (This)->lpVtbl->Start(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_Stop(This) \
    ( (This)->lpVtbl->Stop(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_add_Added(This,handler,token) \
    ( (This)->lpVtbl->add_Added(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_remove_Added(This,token) \
    ( (This)->lpVtbl->remove_Added(This,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_add_EnumerationCompleted(This,handler,token) \
    ( (This)->lpVtbl->add_EnumerationCompleted(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_remove_EnumerationCompleted(This,token) \
    ( (This)->lpVtbl->remove_EnumerationCompleted(This,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_add_Removed(This,handler,token) \
    ( (This)->lpVtbl->add_Removed(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_remove_Removed(This,token) \
    ( (This)->lpVtbl->remove_Removed(This,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_add_Stopped(This,handler,token) \
    ( (This)->lpVtbl->add_Stopped(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_remove_Stopped(This,token) \
    ( (This)->lpVtbl->remove_Stopped(This,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_add_Updated(This,handler,token) \
    ( (This)->lpVtbl->add_Updated(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_remove_Updated(This,token) \
    ( (This)->lpVtbl->remove_Updated(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIDisplayAreaWatcher_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IFullScreenPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.FullScreenPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IFullScreenPresenter[] = L"Microsoft.UI.Windowing.IFullScreenPresenter";
/* [object, uuid("FA9141FD-B8DD-5DA1-8B2B-7CDADB76F593"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IFullScreenPresenterStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.FullScreenPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IFullScreenPresenterStatics[] = L"Microsoft.UI.Windowing.IFullScreenPresenterStatics";
/* [object, uuid("2EC0D2C1-E086-55BB-A3B2-44942E231C67"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenter * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_Create(This,result) \
    ( (This)->lpVtbl->Create(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIFullScreenPresenterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenter[] = L"Microsoft.UI.Windowing.IOverlappedPresenter";
/* [object, uuid("21693970-4F4C-5172-9E9D-682A2D174884"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasBorder )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasTitleBar )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsAlwaysOnTop )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsAlwaysOnTop )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsMaximizable )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsMaximizable )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsMinimizable )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsMinimizable )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsModal )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsModal )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsResizable )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsResizable )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_State )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_COverlappedPresenterState * value
        );
    HRESULT ( STDMETHODCALLTYPE *Maximize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This
        );
    HRESULT ( STDMETHODCALLTYPE *Minimize )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This
        );
    HRESULT ( STDMETHODCALLTYPE *Restore )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This
        );
    HRESULT ( STDMETHODCALLTYPE *SetBorderAndTitleBar )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * This,
        /* [in] */boolean hasBorder,
        /* [in] */boolean hasTitleBar
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_HasBorder(This,value) \
    ( (This)->lpVtbl->get_HasBorder(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_HasTitleBar(This,value) \
    ( (This)->lpVtbl->get_HasTitleBar(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_IsAlwaysOnTop(This,value) \
    ( (This)->lpVtbl->get_IsAlwaysOnTop(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_put_IsAlwaysOnTop(This,value) \
    ( (This)->lpVtbl->put_IsAlwaysOnTop(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_IsMaximizable(This,value) \
    ( (This)->lpVtbl->get_IsMaximizable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_put_IsMaximizable(This,value) \
    ( (This)->lpVtbl->put_IsMaximizable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_IsMinimizable(This,value) \
    ( (This)->lpVtbl->get_IsMinimizable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_put_IsMinimizable(This,value) \
    ( (This)->lpVtbl->put_IsMinimizable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_IsModal(This,value) \
    ( (This)->lpVtbl->get_IsModal(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_put_IsModal(This,value) \
    ( (This)->lpVtbl->put_IsModal(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_IsResizable(This,value) \
    ( (This)->lpVtbl->get_IsResizable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_put_IsResizable(This,value) \
    ( (This)->lpVtbl->put_IsResizable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_get_State(This,value) \
    ( (This)->lpVtbl->get_State(This,value) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_Maximize(This) \
    ( (This)->lpVtbl->Maximize(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_Minimize(This) \
    ( (This)->lpVtbl->Minimize(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_Restore(This) \
    ( (This)->lpVtbl->Restore(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_SetBorderAndTitleBar(This,hasBorder,hasTitleBar) \
    ( (This)->lpVtbl->SetBorderAndTitleBar(This,hasBorder,hasTitleBar) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenter2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenter2[] = L"Microsoft.UI.Windowing.IOverlappedPresenter2";
/* [object, uuid("5C6CCD93-4244-5CD2-B355-ED5EA34DF730"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [overload] */HRESULT ( STDMETHODCALLTYPE *MinimizeWithActivation )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This,
        /* [in] */boolean activateWindow
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *RestoreWithActivation )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2 * This,
        /* [in] */boolean activateWindow
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_MinimizeWithActivation(This,activateWindow) \
    ( (This)->lpVtbl->MinimizeWithActivation(This,activateWindow) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_RestoreWithActivation(This,activateWindow) \
    ( (This)->lpVtbl->RestoreWithActivation(This,activateWindow) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenterStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenterStatics[] = L"Microsoft.UI.Windowing.IOverlappedPresenterStatics";
/* [object, uuid("997225E4-7B00-5AEE-A4BE-D4068D1999E2"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateForContextMenu )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateForDialog )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateForToolWindow )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenter * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_Create(This,result) \
    ( (This)->lpVtbl->Create(This,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_CreateForContextMenu(This,result) \
    ( (This)->lpVtbl->CreateForContextMenu(This,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_CreateForDialog(This,result) \
    ( (This)->lpVtbl->CreateForDialog(This,result) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_CreateForToolWindow(This,result) \
    ( (This)->lpVtbl->CreateForToolWindow(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Windowing.IOverlappedPresenterStatics2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Windowing.OverlappedPresenter
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Windowing_IOverlappedPresenterStatics2[] = L"Microsoft.UI.Windowing.IOverlappedPresenterStatics2";
/* [object, uuid("ED5C4F92-32F4-5D15-80D0-B2A5EFA04D39"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RequestedStartupState )(
        __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2 * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CWindowing_COverlappedPresenterState * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_get_RequestedStartupState(This,value) \
    ( (This)->lpVtbl->get_RequestedStartupState(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CWindowing_CIOverlappedPresenterStatics2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Windowing.AppWindow
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IAppWindowStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindow ** Default Interface **
 *    Microsoft.UI.Windowing.IAppWindow2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindow_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindow_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindow[] = L"Microsoft.UI.Windowing.AppWindow";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowChangedEventArgs ** Default Interface **
 *    Microsoft.UI.Windowing.IAppWindowChangedEventArgs2
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowChangedEventArgs[] = L"Microsoft.UI.Windowing.AppWindowChangedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowClosingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowClosingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowClosingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowClosingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowClosingEventArgs[] = L"Microsoft.UI.Windowing.AppWindowClosingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowPresenter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowPresenter[] = L"Microsoft.UI.Windowing.AppWindowPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.AppWindowTitleBar
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IAppWindowTitleBarStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IAppWindowTitleBar ** Default Interface **
 *    Microsoft.UI.Windowing.IAppWindowTitleBar2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowTitleBar_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_AppWindowTitleBar_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_AppWindowTitleBar[] = L"Microsoft.UI.Windowing.AppWindowTitleBar";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.CompactOverlayPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.ICompactOverlayPresenterStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.ICompactOverlayPresenter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_CompactOverlayPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_CompactOverlayPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_CompactOverlayPresenter[] = L"Microsoft.UI.Windowing.CompactOverlayPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.DisplayArea
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IDisplayAreaStatics2 interface starting with version 1.2 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *   Static Methods exist on the Microsoft.UI.Windowing.IDisplayAreaStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IDisplayArea ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_DisplayArea_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_DisplayArea_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_DisplayArea[] = L"Microsoft.UI.Windowing.DisplayArea";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.DisplayAreaWatcher
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IDisplayAreaWatcher ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_DisplayAreaWatcher_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_DisplayAreaWatcher_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_DisplayAreaWatcher[] = L"Microsoft.UI.Windowing.DisplayAreaWatcher";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.FullScreenPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IFullScreenPresenterStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IFullScreenPresenter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_FullScreenPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_FullScreenPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_FullScreenPresenter[] = L"Microsoft.UI.Windowing.FullScreenPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Windowing.OverlappedPresenter
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Windowing.IOverlappedPresenterStatics2 interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *   Static Methods exist on the Microsoft.UI.Windowing.IOverlappedPresenterStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Windowing.IOverlappedPresenter ** Default Interface **
 *    Microsoft.UI.Windowing.IOverlappedPresenter2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Windowing_OverlappedPresenter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Windowing_OverlappedPresenter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Windowing_OverlappedPresenter[] = L"Microsoft.UI.Windowing.OverlappedPresenter";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000




#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EWindowing_p_h__

#endif // __Microsoft2EUI2EWindowing_h__
