/* Header file automatically generated from Microsoft.UI.Dispatching.idl */
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
#ifndef __Microsoft2EUI2EDispatching_h__
#define __Microsoft2EUI2EDispatching_h__
#ifndef __Microsoft2EUI2EDispatching_p_h__
#define __Microsoft2EUI2EDispatching_p_h__


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
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueueHandler;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler ABI::Microsoft::UI::Dispatching::IDispatcherQueueHandler

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueue;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue ABI::Microsoft::UI::Dispatching::IDispatcherQueue

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueue2;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 ABI::Microsoft::UI::Dispatching::IDispatcherQueue2

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueueController;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController ABI::Microsoft::UI::Dispatching::IDispatcherQueueController

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueueControllerStatics;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics ABI::Microsoft::UI::Dispatching::IDispatcherQueueControllerStatics

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueueShutdownStartingEventArgs;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs ABI::Microsoft::UI::Dispatching::IDispatcherQueueShutdownStartingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueueStatics;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics ABI::Microsoft::UI::Dispatching::IDispatcherQueueStatics

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                interface IDispatcherQueueTimer;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer ABI::Microsoft::UI::Dispatching::IDispatcherQueueTimer

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                class DispatcherQueue;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("3bdaf5dd-3da4-5b44-adb3-6990540afac6"))
ITypedEventHandler<ABI::Microsoft::UI::Dispatching::DispatcherQueue*,IInspectable*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Dispatching::DispatcherQueue*, ABI::Microsoft::UI::Dispatching::IDispatcherQueue*>,IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Dispatching.DispatcherQueue, Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Dispatching::DispatcherQueue*,IInspectable*> __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Dispatching::IDispatcherQueue*,IInspectable*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Dispatching::IDispatcherQueue*,IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                class DispatcherQueueShutdownStartingEventArgs;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("ecd63a61-4dbf-57bc-880c-a255dfe352c3"))
ITypedEventHandler<ABI::Microsoft::UI::Dispatching::DispatcherQueue*,ABI::Microsoft::UI::Dispatching::DispatcherQueueShutdownStartingEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Dispatching::DispatcherQueue*, ABI::Microsoft::UI::Dispatching::IDispatcherQueue*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Dispatching::DispatcherQueueShutdownStartingEventArgs*, ABI::Microsoft::UI::Dispatching::IDispatcherQueueShutdownStartingEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Dispatching.DispatcherQueue, Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Dispatching::DispatcherQueue*,ABI::Microsoft::UI::Dispatching::DispatcherQueueShutdownStartingEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Dispatching::IDispatcherQueue*,ABI::Microsoft::UI::Dispatching::IDispatcherQueueShutdownStartingEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Dispatching::IDispatcherQueue*,ABI::Microsoft::UI::Dispatching::IDispatcherQueueShutdownStartingEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                class DispatcherQueueTimer;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("2182a2ac-7545-566a-984f-b10f07bab089"))
ITypedEventHandler<ABI::Microsoft::UI::Dispatching::DispatcherQueueTimer*,IInspectable*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Dispatching::DispatcherQueueTimer*, ABI::Microsoft::UI::Dispatching::IDispatcherQueueTimer*>,IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Dispatching.DispatcherQueueTimer, Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Dispatching::DispatcherQueueTimer*,IInspectable*> __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Dispatching::IDispatcherQueueTimer*,IInspectable*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Dispatching::IDispatcherQueueTimer*,IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000






namespace ABI {
    namespace Windows {
        namespace Foundation {
            class Deferral;
        } /* Foundation */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CFoundation_CIDeferral_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIDeferral_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Foundation {
            interface IDeferral;
        } /* Foundation */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CFoundation_CIDeferral ABI::Windows::Foundation::IDeferral

#endif // ____x_ABI_CWindows_CFoundation_CIDeferral_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Foundation {
            interface IAsyncAction;
        } /* Foundation */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CFoundation_CIAsyncAction ABI::Windows::Foundation::IAsyncAction

#endif // ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__


namespace ABI {
    namespace Windows {
        namespace Foundation {
            
            typedef struct TimeSpan TimeSpan;
            
        } /* Foundation */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                
                typedef enum DispatcherQueuePriority : int DispatcherQueuePriority;
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */










namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                class DispatcherQueueController;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */












/*
 *
 * Struct Microsoft.UI.Dispatching.DispatcherQueuePriority
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [v1_enum, contract] */
                enum DispatcherQueuePriority : int
                {
                    DispatcherQueuePriority_Low = -10,
                    DispatcherQueuePriority_Normal = 0,
                    DispatcherQueuePriority_High = 10,
                };
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Dispatching.DispatcherQueueHandler
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("2E0872A9-4E29-5F14-B688-FB96D5F9D5F8"), contract] */
                MIDL_INTERFACE("2E0872A9-4E29-5F14-B688-FB96D5F9D5F8")
                IDispatcherQueueHandler : public IUnknown
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Invoke(void) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueueHandler=_uuidof(IDispatcherQueueHandler);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueue
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueue
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueue[] = L"Microsoft.UI.Dispatching.IDispatcherQueue";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("F6EBF8FA-BE1C-5BF6-A467-73DA28738AE8"), exclusiveto, contract] */
                MIDL_INTERFACE("F6EBF8FA-BE1C-5BF6-A467-73DA28738AE8")
                IDispatcherQueue : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE CreateTimer(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueueTimer * * result
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE TryEnqueue(
                        /* [in] */ABI::Microsoft::UI::Dispatching::IDispatcherQueueHandler  * callback,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    /* [overload] */virtual HRESULT STDMETHODCALLTYPE TryEnqueueWithPriority(
                        /* [in] */ABI::Microsoft::UI::Dispatching::DispatcherQueuePriority priority,
                        /* [in] */ABI::Microsoft::UI::Dispatching::IDispatcherQueueHandler  * callback,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ShutdownStarting(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ShutdownStarting(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ShutdownCompleted(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ShutdownCompleted(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueue=_uuidof(IDispatcherQueue);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueue2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueue
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueue2[] = L"Microsoft.UI.Dispatching.IDispatcherQueue2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("0CF48751-F1AC-59B8-BA52-6CE7A1444D6F"), exclusiveto, contract] */
                MIDL_INTERFACE("0CF48751-F1AC-59B8-BA52-6CE7A1444D6F")
                IDispatcherQueue2 : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasThreadAccess(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueue2=_uuidof(IDispatcherQueue2);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueController[] = L"Microsoft.UI.Dispatching.IDispatcherQueueController";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("BCE8178D-2183-584C-9E5B-F9366F6AE484"), exclusiveto, contract] */
                MIDL_INTERFACE("BCE8178D-2183-584C-9E5B-F9366F6AE484")
                IDispatcherQueueController : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DispatcherQueue(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueue * * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ShutdownQueueAsync(
                        /* [retval, out] */ABI::Windows::Foundation::IAsyncAction * * operation
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueueController=_uuidof(IDispatcherQueueController);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueControllerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueControllerStatics[] = L"Microsoft.UI.Dispatching.IDispatcherQueueControllerStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("F18D6145-722B-593D-BCF2-A61E713F0037"), exclusiveto, contract] */
                MIDL_INTERFACE("F18D6145-722B-593D-BCF2-A61E713F0037")
                IDispatcherQueueControllerStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE CreateOnDedicatedThread(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueueController * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateOnCurrentThread(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueueController * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueueControllerStatics=_uuidof(IDispatcherQueueControllerStatics);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueShutdownStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueShutdownStartingEventArgs[] = L"Microsoft.UI.Dispatching.IDispatcherQueueShutdownStartingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("32519BE5-072B-5660-A70E-8835C9B8157D"), exclusiveto, contract] */
                MIDL_INTERFACE("32519BE5-072B-5660-A70E-8835C9B8157D")
                IDispatcherQueueShutdownStartingEventArgs : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE GetDeferral(
                        /* [retval, out] */ABI::Windows::Foundation::IDeferral * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueueShutdownStartingEventArgs=_uuidof(IDispatcherQueueShutdownStartingEventArgs);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueue
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueStatics[] = L"Microsoft.UI.Dispatching.IDispatcherQueueStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("CD3382EA-A455-5124-B63A-CA40D34CA23C"), exclusiveto, contract] */
                MIDL_INTERFACE("CD3382EA-A455-5124-B63A-CA40D34CA23C")
                IDispatcherQueueStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE GetForCurrentThread(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueue * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueueStatics=_uuidof(IDispatcherQueueStatics);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueTimer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueTimer
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueTimer[] = L"Microsoft.UI.Dispatching.IDispatcherQueueTimer";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                /* [object, uuid("AD4D63FD-88FE-541F-AC11-BF2DC1ED2CE5"), exclusiveto, contract] */
                MIDL_INTERFACE("AD4D63FD-88FE-541F-AC11-BF2DC1ED2CE5")
                IDispatcherQueueTimer : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Interval(
                        /* [retval, out] */ABI::Windows::Foundation::TimeSpan * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Interval(
                        /* [in] */ABI::Windows::Foundation::TimeSpan value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsRunning(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsRepeating(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsRepeating(
                        /* [in] */::boolean value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Start(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Tick(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Tick(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDispatcherQueueTimer=_uuidof(IDispatcherQueueTimer);
                
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueue
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Dispatching.IDispatcherQueueStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueue ** Default Interface **
 *    Microsoft.UI.Dispatching.IDispatcherQueue2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueue_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueue_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueue[] = L"Microsoft.UI.Dispatching.DispatcherQueue";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueueController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Dispatching.IDispatcherQueueControllerStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueueController ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueController_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueController_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueController[] = L"Microsoft.UI.Dispatching.DispatcherQueueController";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueueShutdownStartingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueShutdownStartingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueShutdownStartingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueShutdownStartingEventArgs[] = L"Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueueTimer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueueTimer ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueTimer_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueTimer_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueTimer[] = L"Microsoft.UI.Dispatching.DispatcherQueueTimer";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000




#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * sender,/* [in] */ __RPC__in_opt IInspectable * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectableVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * sender,/* [in] */ __RPC__in_opt IInspectable * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectableVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000





#ifndef ____x_ABI_CWindows_CFoundation_CIDeferral_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIDeferral_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIDeferral __x_ABI_CWindows_CFoundation_CIDeferral;

#endif // ____x_ABI_CWindows_CFoundation_CIDeferral_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIAsyncAction __x_ABI_CWindows_CFoundation_CIAsyncAction;

#endif // ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__



typedef struct __x_ABI_CWindows_CFoundation_CTimeSpan __x_ABI_CWindows_CFoundation_CTimeSpan;




typedef enum __x_ABI_CMicrosoft_CUI_CDispatching_CDispatcherQueuePriority __x_ABI_CMicrosoft_CUI_CDispatching_CDispatcherQueuePriority;





















/*
 *
 * Struct Microsoft.UI.Dispatching.DispatcherQueuePriority
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CDispatching_CDispatcherQueuePriority
{
    DispatcherQueuePriority_Low = -10,
    DispatcherQueuePriority_Normal = 0,
    DispatcherQueuePriority_High = 10,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Dispatching.DispatcherQueueHandler
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_INTERFACE_DEFINED__
/* [object, uuid("2E0872A9-4E29-5F14-B688-FB96D5F9D5F8"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_Invoke(This) \
    ( (This)->lpVtbl->Invoke(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueue
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueue
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueue[] = L"Microsoft.UI.Dispatching.IDispatcherQueue";
/* [object, uuid("F6EBF8FA-BE1C-5BF6-A467-73DA28738AE8"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateTimer )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *TryEnqueue )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler  * callback,
        /* [retval, out] */boolean * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *TryEnqueueWithPriority )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CDispatching_CDispatcherQueuePriority priority,
        /* [in] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueHandler  * callback,
        /* [retval, out] */boolean * result
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ShutdownStarting )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_Microsoft__CUI__CDispatching__CDispatcherQueueShutdownStartingEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ShutdownStarting )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ShutdownCompleted )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueue_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ShutdownCompleted )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_CreateTimer(This,result) \
    ( (This)->lpVtbl->CreateTimer(This,result) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_TryEnqueue(This,callback,result) \
    ( (This)->lpVtbl->TryEnqueue(This,callback,result) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_TryEnqueueWithPriority(This,priority,callback,result) \
    ( (This)->lpVtbl->TryEnqueueWithPriority(This,priority,callback,result) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_add_ShutdownStarting(This,handler,token) \
    ( (This)->lpVtbl->add_ShutdownStarting(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_remove_ShutdownStarting(This,token) \
    ( (This)->lpVtbl->remove_ShutdownStarting(This,token) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_add_ShutdownCompleted(This,handler,token) \
    ( (This)->lpVtbl->add_ShutdownCompleted(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_remove_ShutdownCompleted(This,token) \
    ( (This)->lpVtbl->remove_ShutdownCompleted(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueue2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueue
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueue2[] = L"Microsoft.UI.Dispatching.IDispatcherQueue2";
/* [object, uuid("0CF48751-F1AC-59B8-BA52-6CE7A1444D6F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasThreadAccess )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2 * This,
        /* [retval, out] */boolean * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_get_HasThreadAccess(This,value) \
    ( (This)->lpVtbl->get_HasThreadAccess(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueController[] = L"Microsoft.UI.Dispatching.IDispatcherQueueController";
/* [object, uuid("BCE8178D-2183-584C-9E5B-F9366F6AE484"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DispatcherQueue )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * * value
        );
    HRESULT ( STDMETHODCALLTYPE *ShutdownQueueAsync )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIAsyncAction * * operation
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_get_DispatcherQueue(This,value) \
    ( (This)->lpVtbl->get_DispatcherQueue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_ShutdownQueueAsync(This,operation) \
    ( (This)->lpVtbl->ShutdownQueueAsync(This,operation) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueControllerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueControllerStatics[] = L"Microsoft.UI.Dispatching.IDispatcherQueueControllerStatics";
/* [object, uuid("F18D6145-722B-593D-BCF2-A61E713F0037"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateOnDedicatedThread )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateOnCurrentThread )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueController * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_CreateOnDedicatedThread(This,result) \
    ( (This)->lpVtbl->CreateOnDedicatedThread(This,result) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_CreateOnCurrentThread(This,result) \
    ( (This)->lpVtbl->CreateOnCurrentThread(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueControllerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueShutdownStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueShutdownStartingEventArgs[] = L"Microsoft.UI.Dispatching.IDispatcherQueueShutdownStartingEventArgs";
/* [object, uuid("32519BE5-072B-5660-A70E-8835C9B8157D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetDeferral )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIDeferral * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_GetDeferral(This,result) \
    ( (This)->lpVtbl->GetDeferral(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueShutdownStartingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueue
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueStatics[] = L"Microsoft.UI.Dispatching.IDispatcherQueueStatics";
/* [object, uuid("CD3382EA-A455-5124-B63A-CA40D34CA23C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetForCurrentThread )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_GetForCurrentThread(This,result) \
    ( (This)->lpVtbl->GetForCurrentThread(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Dispatching.IDispatcherQueueTimer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Dispatching.DispatcherQueueTimer
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Dispatching_IDispatcherQueueTimer[] = L"Microsoft.UI.Dispatching.IDispatcherQueueTimer";
/* [object, uuid("AD4D63FD-88FE-541F-AC11-BF2DC1ED2CE5"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Interval )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CTimeSpan * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Interval )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CTimeSpan value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsRunning )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsRepeating )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsRepeating )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [in] */boolean value
        );
    HRESULT ( STDMETHODCALLTYPE *Start )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This
        );
    HRESULT ( STDMETHODCALLTYPE *Stop )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Tick )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CDispatching__CDispatcherQueueTimer_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Tick )(
        __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimerVtbl;

interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_get_Interval(This,value) \
    ( (This)->lpVtbl->get_Interval(This,value) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_put_Interval(This,value) \
    ( (This)->lpVtbl->put_Interval(This,value) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_get_IsRunning(This,value) \
    ( (This)->lpVtbl->get_IsRunning(This,value) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_get_IsRepeating(This,value) \
    ( (This)->lpVtbl->get_IsRepeating(This,value) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_put_IsRepeating(This,value) \
    ( (This)->lpVtbl->put_IsRepeating(This,value) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_Start(This) \
    ( (This)->lpVtbl->Start(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_Stop(This) \
    ( (This)->lpVtbl->Stop(This) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_add_Tick(This,handler,token) \
    ( (This)->lpVtbl->add_Tick(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_remove_Tick(This,token) \
    ( (This)->lpVtbl->remove_Tick(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueueTimer_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueue
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Dispatching.IDispatcherQueueStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueue ** Default Interface **
 *    Microsoft.UI.Dispatching.IDispatcherQueue2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueue_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueue_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueue[] = L"Microsoft.UI.Dispatching.DispatcherQueue";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueueController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Dispatching.IDispatcherQueueControllerStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueueController ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueController_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueController_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueController[] = L"Microsoft.UI.Dispatching.DispatcherQueueController";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueueShutdownStartingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueShutdownStartingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueShutdownStartingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueShutdownStartingEventArgs[] = L"Microsoft.UI.Dispatching.DispatcherQueueShutdownStartingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Dispatching.DispatcherQueueTimer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Dispatching.IDispatcherQueueTimer ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueTimer_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Dispatching_DispatcherQueueTimer_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueTimer[] = L"Microsoft.UI.Dispatching.DispatcherQueueTimer";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000




#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EDispatching_p_h__

#endif // __Microsoft2EUI2EDispatching_h__
