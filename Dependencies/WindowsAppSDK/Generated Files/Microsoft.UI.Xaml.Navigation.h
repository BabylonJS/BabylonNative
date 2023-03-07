/* Header file automatically generated from Microsoft.UI.Xaml.Navigation.idl */
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
#ifndef __Microsoft2EUI2EXaml2ENavigation_h__
#define __Microsoft2EUI2EXaml2ENavigation_h__
#ifndef __Microsoft2EUI2EXaml2ENavigation_p_h__
#define __Microsoft2EUI2EXaml2ENavigation_p_h__


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

#if !defined(MICROSOFT_UI_XAML_WINUICONTRACT_VERSION)
#define MICROSOFT_UI_XAML_WINUICONTRACT_VERSION 0x30000
#endif // defined(MICROSOFT_UI_XAML_WINUICONTRACT_VERSION)

#if !defined(MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION)
#define MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION 0x30000
#endif // defined(MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATEDEVENTSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATEDEVENTSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATEDEVENTSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATIONCAMERASETTINGSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATIONCAMERASETTINGSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATIONCAMERASETTINGSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_CONTACTACTIVATEDEVENTSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_CONTACTACTIVATEDEVENTSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_CONTACTACTIVATEDEVENTSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_WEBUISEARCHACTIVATEDEVENTSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_WEBUISEARCHACTIVATEDEVENTSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_WEBUISEARCHACTIVATEDEVENTSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_BACKGROUND_BACKGROUNDALARMAPPLICATIONCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_BACKGROUND_BACKGROUNDALARMAPPLICATIONCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_BACKGROUND_BACKGROUNDALARMAPPLICATIONCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_BACKGROUND_CALLSBACKGROUNDCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_BACKGROUND_CALLSBACKGROUNDCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_BACKGROUND_CALLSBACKGROUNDCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSVOIPCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_CALLSVOIPCONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSVOIPCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_LOCKSCREENCALLCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_LOCKSCREENCALLCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_LOCKSCREENCALLCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_COMMUNICATIONBLOCKING_COMMUNICATIONBLOCKINGCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_COMMUNICATIONBLOCKING_COMMUNICATIONBLOCKINGCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_APPLICATIONMODEL_COMMUNICATIONBLOCKING_COMMUNICATIONBLOCKINGCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_FULLTRUSTAPPCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_FULLTRUSTAPPCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_APPLICATIONMODEL_FULLTRUSTAPPCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_SEARCH_SEARCHCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_SEARCH_SEARCHCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_SEARCH_SEARCHCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_STARTUPTASKCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_STARTUPTASKCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_APPLICATIONMODEL_STARTUPTASKCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_WALLET_WALLETCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_WALLET_WALLETCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_WALLET_WALLETCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_PRINTERS_EXTENSIONS_EXTENSIONSCONTRACT_VERSION)
#define WINDOWS_DEVICES_PRINTERS_EXTENSIONS_EXTENSIONSCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_DEVICES_PRINTERS_EXTENSIONS_EXTENSIONSCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDBACKGROUNDTRIGGERCONTRACT_VERSION)
#define WINDOWS_DEVICES_SMARTCARDS_SMARTCARDBACKGROUNDTRIGGERCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDBACKGROUNDTRIGGERCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDEMULATORCONTRACT_VERSION)
#define WINDOWS_DEVICES_SMARTCARDS_SMARTCARDEMULATORCONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDEMULATORCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_SMS_LEGACYSMSAPICONTRACT_VERSION)
#define WINDOWS_DEVICES_SMS_LEGACYSMSAPICONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_DEVICES_SMS_LEGACYSMSAPICONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)
#define WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)
#define WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION 0xe0000
#endif // defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)

#if !defined(WINDOWS_GAMING_INPUT_GAMINGINPUTPREVIEWCONTRACT_VERSION)
#define WINDOWS_GAMING_INPUT_GAMINGINPUTPREVIEWCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_GAMING_INPUT_GAMINGINPUTPREVIEWCONTRACT_VERSION)

#if !defined(WINDOWS_GLOBALIZATION_GLOBALIZATIONJAPANESEPHONETICANALYZERCONTRACT_VERSION)
#define WINDOWS_GLOBALIZATION_GLOBALIZATIONJAPANESEPHONETICANALYZERCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_GLOBALIZATION_GLOBALIZATIONJAPANESEPHONETICANALYZERCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_APPBROADCASTCONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_APPBROADCASTCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_MEDIA_CAPTURE_APPBROADCASTCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_APPCAPTURECONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_APPCAPTURECONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_MEDIA_CAPTURE_APPCAPTURECONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_APPCAPTUREMETADATACONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_APPCAPTUREMETADATACONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_CAPTURE_APPCAPTUREMETADATACONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_CAMERACAPTUREUICONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_CAMERACAPTUREUICONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_CAPTURE_CAMERACAPTUREUICONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_GAMEBARCONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_GAMEBARCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_CAPTURE_GAMEBARCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_DEVICES_CALLCONTROLCONTRACT_VERSION)
#define WINDOWS_MEDIA_DEVICES_CALLCONTROLCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_DEVICES_CALLCONTROLCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_MEDIACONTROLCONTRACT_VERSION)
#define WINDOWS_MEDIA_MEDIACONTROLCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_MEDIACONTROLCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_PROTECTION_PROTECTIONRENEWALCONTRACT_VERSION)
#define WINDOWS_MEDIA_PROTECTION_PROTECTIONRENEWALCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_PROTECTION_PROTECTIONRENEWALCONTRACT_VERSION)

#if !defined(WINDOWS_NETWORKING_CONNECTIVITY_WWANCONTRACT_VERSION)
#define WINDOWS_NETWORKING_CONNECTIVITY_WWANCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_NETWORKING_CONNECTIVITY_WWANCONTRACT_VERSION)

#if !defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)
#define WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)
#define WINDOWS_PHONE_PHONECONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)
#define WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)

#if !defined(WINDOWS_SECURITY_ENTERPRISEDATA_ENTERPRISEDATACONTRACT_VERSION)
#define WINDOWS_SECURITY_ENTERPRISEDATA_ENTERPRISEDATACONTRACT_VERSION 0x50000
#endif // defined(WINDOWS_SECURITY_ENTERPRISEDATA_ENTERPRISEDATACONTRACT_VERSION)

#if !defined(WINDOWS_STORAGE_PROVIDER_CLOUDFILESCONTRACT_VERSION)
#define WINDOWS_STORAGE_PROVIDER_CLOUDFILESCONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_STORAGE_PROVIDER_CLOUDFILESCONTRACT_VERSION)

#if !defined(WINDOWS_SYSTEM_SYSTEMMANAGEMENTCONTRACT_VERSION)
#define WINDOWS_SYSTEM_SYSTEMMANAGEMENTCONTRACT_VERSION 0x70000
#endif // defined(WINDOWS_SYSTEM_SYSTEMMANAGEMENTCONTRACT_VERSION)

#if !defined(WINDOWS_UI_CORE_COREWINDOWDIALOGSCONTRACT_VERSION)
#define WINDOWS_UI_CORE_COREWINDOWDIALOGSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_UI_CORE_COREWINDOWDIALOGSCONTRACT_VERSION)

#if !defined(WINDOWS_UI_VIEWMANAGEMENT_VIEWMANAGEMENTVIEWSCALINGCONTRACT_VERSION)
#define WINDOWS_UI_VIEWMANAGEMENT_VIEWMANAGEMENTVIEWSCALINGCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_UI_VIEWMANAGEMENT_VIEWMANAGEMENTVIEWSCALINGCONTRACT_VERSION)

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
#include "Microsoft.UI.Xaml.h"
#include "Microsoft.UI.Xaml.Media.Animation.h"
#include "Windows.UI.Xaml.Interop.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigatedEventHandler;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler ABI::Microsoft::UI::Xaml::Navigation::INavigatedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigatingCancelEventHandler;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler ABI::Microsoft::UI::Xaml::Navigation::INavigatingCancelEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigationFailedEventHandler;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler ABI::Microsoft::UI::Xaml::Navigation::INavigationFailedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigationStoppedEventHandler;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler ABI::Microsoft::UI::Xaml::Navigation::INavigationStoppedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface IFrameNavigationOptions;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions ABI::Microsoft::UI::Xaml::Navigation::IFrameNavigationOptions

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface IFrameNavigationOptionsFactory;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory ABI::Microsoft::UI::Xaml::Navigation::IFrameNavigationOptionsFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigatingCancelEventArgs;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs ABI::Microsoft::UI::Xaml::Navigation::INavigatingCancelEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigationEventArgs;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs ABI::Microsoft::UI::Xaml::Navigation::INavigationEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface INavigationFailedEventArgs;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs ABI::Microsoft::UI::Xaml::Navigation::INavigationFailedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface IPageStackEntry;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface IPageStackEntryFactory;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntryFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    interface IPageStackEntryStatics;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntryStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    class PageStackEntry;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#define DEF___FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("162c5e92-295f-532c-af30-0e782e535a05"))
IIterator<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*, ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Microsoft.UI.Xaml.Navigation.PageStackEntry>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t;
#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::__FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
//#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#define DEF___FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c4f16816-858c-5ec6-bb69-f27e2e71fc25"))
IIterable<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*, ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Microsoft.UI.Xaml.Navigation.PageStackEntry>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t;
#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::__FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
//#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#define DEF___FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("1a07e864-47dd-50bc-8948-7d2afb8c332f"))
IVectorView<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*, ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Microsoft.UI.Xaml.Navigation.PageStackEntry>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t;
#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::__FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
//#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#define DEF___FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("866bb888-10b1-5413-adb4-3d3b197ada8b"))
IVector<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*, ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Microsoft.UI.Xaml.Navigation.PageStackEntry>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<ABI::Microsoft::UI::Xaml::Navigation::PageStackEntry*> __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t;
#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::__FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
//#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_t ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                class DependencyProperty;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IDependencyProperty;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty ABI::Microsoft::UI::Xaml::IDependencyProperty

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Animation {
                        class NavigationTransitionInfo;
                    } /* Animation */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Animation {
                        interface INavigationTransitionInfo;
                    } /* Animation */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo_FWD_DEFINED__











namespace ABI {
    namespace Windows {
        namespace Foundation {
            class Uri;
        } /* Foundation */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Foundation {
            interface IUriRuntimeClass;
        } /* Foundation */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CFoundation_CIUriRuntimeClass ABI::Windows::Foundation::IUriRuntimeClass

#endif // ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__




namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    
                    typedef struct TypeName TypeName;
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Windows */
} /* ABI */





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    
                    typedef enum NavigationCacheMode : int NavigationCacheMode;
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    
                    typedef enum NavigationMode : int NavigationMode;
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */













namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    class FrameNavigationOptions;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    class NavigatingCancelEventArgs;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    class NavigationEventArgs;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    class NavigationFailedEventArgs;
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */













/*
 *
 * Struct Microsoft.UI.Xaml.Navigation.NavigationCacheMode
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [v1_enum, contract] */
                    enum NavigationCacheMode : int
                    {
                        NavigationCacheMode_Disabled = 0,
                        NavigationCacheMode_Required = 1,
                        NavigationCacheMode_Enabled = 2,
                    };
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Navigation.NavigationMode
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [v1_enum, contract] */
                    enum NavigationMode : int
                    {
                        NavigationMode_New = 0,
                        NavigationMode_Back = 1,
                        NavigationMode_Forward = 2,
                        NavigationMode_Refresh = 3,
                    };
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigatedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("8631B517-6D8E-58EE-82FE-D4034D1BD7C1"), contract] */
                    MIDL_INTERFACE("8631B517-6D8E-58EE-82FE-D4034D1BD7C1")
                    INavigatedEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Navigation::INavigationEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigatedEventHandler=_uuidof(INavigatedEventHandler);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigatingCancelEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("FCAE1401-EC94-565F-9F48-7C4B6272B3B1"), contract] */
                    MIDL_INTERFACE("FCAE1401-EC94-565F-9F48-7C4B6272B3B1")
                    INavigatingCancelEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Navigation::INavigatingCancelEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigatingCancelEventHandler=_uuidof(INavigatingCancelEventHandler);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigationFailedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("97CA2B56-D6EB-5FD2-A675-A339640EEDBA"), contract] */
                    MIDL_INTERFACE("97CA2B56-D6EB-5FD2-A675-A339640EEDBA")
                    INavigationFailedEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Navigation::INavigationFailedEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigationFailedEventHandler=_uuidof(INavigationFailedEventHandler);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigationStoppedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("B9E796A6-7FFE-5A63-AEF4-CBC331663B66"), contract] */
                    MIDL_INTERFACE("B9E796A6-7FFE-5A63-AEF4-CBC331663B66")
                    INavigationStoppedEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Navigation::INavigationEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigationStoppedEventHandler=_uuidof(INavigationStoppedEventHandler);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IFrameNavigationOptions
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.FrameNavigationOptions
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IFrameNavigationOptions[] = L"Microsoft.UI.Xaml.Navigation.IFrameNavigationOptions";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("390DE593-14CF-5312-AF99-6CD8D59EC5D5"), exclusiveto, contract] */
                    MIDL_INTERFACE("390DE593-14CF-5312-AF99-6CD8D59EC5D5")
                    IFrameNavigationOptions : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsNavigationStackEnabled(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsNavigationStackEnabled(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TransitionInfoOverride(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TransitionInfoOverride(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IFrameNavigationOptions=_uuidof(IFrameNavigationOptions);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IFrameNavigationOptionsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.FrameNavigationOptions
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IFrameNavigationOptionsFactory[] = L"Microsoft.UI.Xaml.Navigation.IFrameNavigationOptionsFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("DDF3F748-7127-5CEE-9F79-AC281A234632"), exclusiveto, contract] */
                    MIDL_INTERFACE("DDF3F748-7127-5CEE-9F79-AC281A234632")
                    IFrameNavigationOptionsFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Navigation::IFrameNavigationOptions * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IFrameNavigationOptionsFactory=_uuidof(IFrameNavigationOptionsFactory);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.INavigatingCancelEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.NavigatingCancelEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_INavigatingCancelEventArgs[] = L"Microsoft.UI.Xaml.Navigation.INavigatingCancelEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("172FDE12-E06F-5DF6-930E-5FACF7B3FBE7"), exclusiveto, contract] */
                    MIDL_INTERFACE("172FDE12-E06F-5DF6-930E-5FACF7B3FBE7")
                    INavigatingCancelEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cancel(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Cancel(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NavigationMode(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Navigation::NavigationMode * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SourcePageType(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Parameter(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NavigationTransitionInfo(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigatingCancelEventArgs=_uuidof(INavigatingCancelEventArgs);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.INavigationEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.NavigationEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_INavigationEventArgs[] = L"Microsoft.UI.Xaml.Navigation.INavigationEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("876B70B4-2923-5785-9CEA-2E44AA0761BD"), exclusiveto, contract] */
                    MIDL_INTERFACE("876B70B4-2923-5785-9CEA-2E44AA0761BD")
                    INavigationEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Content(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Parameter(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NavigationTransitionInfo(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SourcePageType(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NavigationMode(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Navigation::NavigationMode * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Uri(
                            /* [retval, out] */ABI::Windows::Foundation::IUriRuntimeClass * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Uri(
                            /* [in] */ABI::Windows::Foundation::IUriRuntimeClass * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigationEventArgs=_uuidof(INavigationEventArgs);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.INavigationFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.NavigationFailedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_INavigationFailedEventArgs[] = L"Microsoft.UI.Xaml.Navigation.INavigationFailedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("F808F9A0-130C-5974-87F8-4433271A35A9"), exclusiveto, contract] */
                    MIDL_INTERFACE("F808F9A0-130C-5974-87F8-4433271A35A9")
                    INavigationFailedEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Exception(
                            /* [retval, out] */HRESULT * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Handled(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Handled(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SourcePageType(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INavigationFailedEventArgs=_uuidof(INavigationFailedEventArgs);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IPageStackEntry
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IPageStackEntry[] = L"Microsoft.UI.Xaml.Navigation.IPageStackEntry";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("D591F56E-4262-5C91-9D79-29165CD82100"), exclusiveto, contract] */
                    MIDL_INTERFACE("D591F56E-4262-5C91-9D79-29165CD82100")
                    IPageStackEntry : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SourcePageType(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Parameter(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NavigationTransitionInfo(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPageStackEntry=_uuidof(IPageStackEntry);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IPageStackEntryFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IPageStackEntryFactory[] = L"Microsoft.UI.Xaml.Navigation.IPageStackEntryFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("7E5A9469-6108-5E92-A499-5EE9F065A68A"), exclusiveto, contract] */
                    MIDL_INTERFACE("7E5A9469-6108-5E92-A499-5EE9F065A68A")
                    IPageStackEntryFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */ABI::Windows::UI::Xaml::Interop::TypeName sourcePageType,
                            /* [in] */IInspectable * parameter,
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::Animation::INavigationTransitionInfo * navigationTransitionInfo,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Navigation::IPageStackEntry * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPageStackEntryFactory=_uuidof(IPageStackEntryFactory);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IPageStackEntryStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IPageStackEntryStatics[] = L"Microsoft.UI.Xaml.Navigation.IPageStackEntryStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Navigation {
                    /* [object, uuid("2F1D4CB7-923B-59BB-BFC4-750933F28385"), exclusiveto, contract] */
                    MIDL_INTERFACE("2F1D4CB7-923B-59BB-BFC4-750933F28385")
                    IPageStackEntryStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SourcePageTypeProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPageStackEntryStatics=_uuidof(IPageStackEntryStatics);
                    
                } /* Navigation */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.FrameNavigationOptions
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.IFrameNavigationOptions ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_FrameNavigationOptions_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_FrameNavigationOptions_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_FrameNavigationOptions[] = L"Microsoft.UI.Xaml.Navigation.FrameNavigationOptions";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.NavigatingCancelEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.INavigatingCancelEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigatingCancelEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigatingCancelEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_NavigatingCancelEventArgs[] = L"Microsoft.UI.Xaml.Navigation.NavigatingCancelEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.NavigationEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.INavigationEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_NavigationEventArgs[] = L"Microsoft.UI.Xaml.Navigation.NavigationEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.NavigationFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.INavigationFailedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationFailedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationFailedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_NavigationFailedEventArgs[] = L"Microsoft.UI.Xaml.Navigation.NavigationFailedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Navigation.IPageStackEntryFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Navigation.IPageStackEntryStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.IPageStackEntry ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_PageStackEntry_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_PageStackEntry_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_PageStackEntry[] = L"Microsoft.UI.Xaml.Navigation.PageStackEntry";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__)
#define ____FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

typedef struct __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl;

interface __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry
{
    CONST_VTBL struct __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__)
#define ____FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

typedef  struct __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry **first);

    END_INTERFACE
} __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl;

interface __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry
{
    CONST_VTBL struct __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

typedef struct __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
            /* [in] */ __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl;

interface __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry
{
    CONST_VTBL struct __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__)
#define ____FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

typedef interface __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry;

typedef struct __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This, /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * *value);

    END_INTERFACE
} __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl;

interface __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry
{
    CONST_VTBL struct __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntryVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_Microsoft__CUI__CXaml__CNavigation__CPageStackEntry_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__





#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo_FWD_DEFINED__











#ifndef ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIUriRuntimeClass __x_ABI_CWindows_CFoundation_CIUriRuntimeClass;

#endif // ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__





typedef struct __x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName __x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName;






typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationCacheMode __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationCacheMode;


typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationMode __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationMode;




























/*
 *
 * Struct Microsoft.UI.Xaml.Navigation.NavigationCacheMode
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationCacheMode
{
    NavigationCacheMode_Disabled = 0,
    NavigationCacheMode_Required = 1,
    NavigationCacheMode_Enabled = 2,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Navigation.NavigationMode
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationMode
{
    NavigationMode_New = 0,
    NavigationMode_Back = 1,
    NavigationMode_Forward = 2,
    NavigationMode_Refresh = 3,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigatedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_INTERFACE_DEFINED__
/* [object, uuid("8631B517-6D8E-58EE-82FE-D4034D1BD7C1"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigatingCancelEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_INTERFACE_DEFINED__
/* [object, uuid("FCAE1401-EC94-565F-9F48-7C4B6272B3B1"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigationFailedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_INTERFACE_DEFINED__
/* [object, uuid("97CA2B56-D6EB-5FD2-A675-A339640EEDBA"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Navigation.NavigationStoppedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_INTERFACE_DEFINED__
/* [object, uuid("B9E796A6-7FFE-5A63-AEF4-CBC331663B66"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationStoppedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IFrameNavigationOptions
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.FrameNavigationOptions
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IFrameNavigationOptions[] = L"Microsoft.UI.Xaml.Navigation.IFrameNavigationOptions";
/* [object, uuid("390DE593-14CF-5312-AF99-6CD8D59EC5D5"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsNavigationStackEnabled )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsNavigationStackEnabled )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TransitionInfoOverride )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TransitionInfoOverride )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_get_IsNavigationStackEnabled(This,value) \
    ( (This)->lpVtbl->get_IsNavigationStackEnabled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_put_IsNavigationStackEnabled(This,value) \
    ( (This)->lpVtbl->put_IsNavigationStackEnabled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_get_TransitionInfoOverride(This,value) \
    ( (This)->lpVtbl->get_TransitionInfoOverride(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_put_TransitionInfoOverride(This,value) \
    ( (This)->lpVtbl->put_TransitionInfoOverride(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IFrameNavigationOptionsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.FrameNavigationOptions
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IFrameNavigationOptionsFactory[] = L"Microsoft.UI.Xaml.Navigation.IFrameNavigationOptionsFactory";
/* [object, uuid("DDF3F748-7127-5CEE-9F79-AC281A234632"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptions * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIFrameNavigationOptionsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.INavigatingCancelEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.NavigatingCancelEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_INavigatingCancelEventArgs[] = L"Microsoft.UI.Xaml.Navigation.INavigatingCancelEventArgs";
/* [object, uuid("172FDE12-E06F-5DF6-930E-5FACF7B3FBE7"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cancel )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Cancel )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NavigationMode )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationMode * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SourcePageType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Parameter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NavigationTransitionInfo )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_get_Cancel(This,value) \
    ( (This)->lpVtbl->get_Cancel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_put_Cancel(This,value) \
    ( (This)->lpVtbl->put_Cancel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_get_NavigationMode(This,value) \
    ( (This)->lpVtbl->get_NavigationMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_get_SourcePageType(This,value) \
    ( (This)->lpVtbl->get_SourcePageType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_get_Parameter(This,value) \
    ( (This)->lpVtbl->get_Parameter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_get_NavigationTransitionInfo(This,value) \
    ( (This)->lpVtbl->get_NavigationTransitionInfo(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigatingCancelEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.INavigationEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.NavigationEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_INavigationEventArgs[] = L"Microsoft.UI.Xaml.Navigation.INavigationEventArgs";
/* [object, uuid("876B70B4-2923-5785-9CEA-2E44AA0761BD"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Content )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Parameter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NavigationTransitionInfo )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SourcePageType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NavigationMode )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CNavigationMode * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Uri )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Uri )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_get_Content(This,value) \
    ( (This)->lpVtbl->get_Content(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_get_Parameter(This,value) \
    ( (This)->lpVtbl->get_Parameter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_get_NavigationTransitionInfo(This,value) \
    ( (This)->lpVtbl->get_NavigationTransitionInfo(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_get_SourcePageType(This,value) \
    ( (This)->lpVtbl->get_SourcePageType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_get_NavigationMode(This,value) \
    ( (This)->lpVtbl->get_NavigationMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_get_Uri(This,value) \
    ( (This)->lpVtbl->get_Uri(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_put_Uri(This,value) \
    ( (This)->lpVtbl->put_Uri(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.INavigationFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.NavigationFailedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_INavigationFailedEventArgs[] = L"Microsoft.UI.Xaml.Navigation.INavigationFailedEventArgs";
/* [object, uuid("F808F9A0-130C-5974-87F8-4433271A35A9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Exception )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
        /* [retval, out] */HRESULT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Handled )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Handled )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SourcePageType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_get_Exception(This,value) \
    ( (This)->lpVtbl->get_Exception(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_get_Handled(This,value) \
    ( (This)->lpVtbl->get_Handled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_put_Handled(This,value) \
    ( (This)->lpVtbl->put_Handled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_get_SourcePageType(This,value) \
    ( (This)->lpVtbl->get_SourcePageType(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CINavigationFailedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IPageStackEntry
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IPageStackEntry[] = L"Microsoft.UI.Xaml.Navigation.IPageStackEntry";
/* [object, uuid("D591F56E-4262-5C91-9D79-29165CD82100"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SourcePageType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Parameter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NavigationTransitionInfo )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_get_SourcePageType(This,value) \
    ( (This)->lpVtbl->get_SourcePageType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_get_Parameter(This,value) \
    ( (This)->lpVtbl->get_Parameter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_get_NavigationTransitionInfo(This,value) \
    ( (This)->lpVtbl->get_NavigationTransitionInfo(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IPageStackEntryFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IPageStackEntryFactory[] = L"Microsoft.UI.Xaml.Navigation.IPageStackEntryFactory";
/* [object, uuid("7E5A9469-6108-5E92-A499-5EE9F065A68A"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory * This,
        /* [in] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName sourcePageType,
        /* [in] */IInspectable * parameter,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CAnimation_CINavigationTransitionInfo * navigationTransitionInfo,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntry * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_CreateInstance(This,sourcePageType,parameter,navigationTransitionInfo,value) \
    ( (This)->lpVtbl->CreateInstance(This,sourcePageType,parameter,navigationTransitionInfo,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Navigation.IPageStackEntryStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Navigation_IPageStackEntryStatics[] = L"Microsoft.UI.Xaml.Navigation.IPageStackEntryStatics";
/* [object, uuid("2F1D4CB7-923B-59BB-BFC4-750933F28385"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SourcePageTypeProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_get_SourcePageTypeProperty(This,value) \
    ( (This)->lpVtbl->get_SourcePageTypeProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CNavigation_CIPageStackEntryStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.FrameNavigationOptions
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.IFrameNavigationOptions ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_FrameNavigationOptions_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_FrameNavigationOptions_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_FrameNavigationOptions[] = L"Microsoft.UI.Xaml.Navigation.FrameNavigationOptions";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.NavigatingCancelEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.INavigatingCancelEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigatingCancelEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigatingCancelEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_NavigatingCancelEventArgs[] = L"Microsoft.UI.Xaml.Navigation.NavigatingCancelEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.NavigationEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.INavigationEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_NavigationEventArgs[] = L"Microsoft.UI.Xaml.Navigation.NavigationEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.NavigationFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.INavigationFailedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationFailedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_NavigationFailedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_NavigationFailedEventArgs[] = L"Microsoft.UI.Xaml.Navigation.NavigationFailedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Navigation.PageStackEntry
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Navigation.IPageStackEntryFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Navigation.IPageStackEntryStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Navigation.IPageStackEntry ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_PageStackEntry_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Navigation_PageStackEntry_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Navigation_PageStackEntry[] = L"Microsoft.UI.Xaml.Navigation.PageStackEntry";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2ENavigation_p_h__

#endif // __Microsoft2EUI2EXaml2ENavigation_h__
