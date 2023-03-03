/* Header file automatically generated from Microsoft.UI.Composition.SystemBackdrops.idl */
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
#ifndef __Microsoft2EUI2EComposition2ESystemBackdrops_h__
#define __Microsoft2EUI2EComposition2ESystemBackdrops_h__
#ifndef __Microsoft2EUI2EComposition2ESystemBackdrops_p_h__
#define __Microsoft2EUI2EComposition2ESystemBackdrops_p_h__


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
#include "Microsoft.Foundation.h"
#include "Microsoft.UI.h"
#include "Microsoft.UI.Composition.h"
#include "Windows.UI.h"
#include "Windows.UI.Composition.h"
#include "Windows.UI.Core.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface IDesktopAcrylicController;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController ABI::Microsoft::UI::Composition::SystemBackdrops::IDesktopAcrylicController

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface IDesktopAcrylicController2;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 ABI::Microsoft::UI::Composition::SystemBackdrops::IDesktopAcrylicController2

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface IDesktopAcrylicControllerStatics;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics ABI::Microsoft::UI::Composition::SystemBackdrops::IDesktopAcrylicControllerStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface IMicaController;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController ABI::Microsoft::UI::Composition::SystemBackdrops::IMicaController

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface IMicaController2;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 ABI::Microsoft::UI::Composition::SystemBackdrops::IMicaController2

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface IMicaControllerStatics;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics ABI::Microsoft::UI::Composition::SystemBackdrops::IMicaControllerStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface ISystemBackdropConfiguration;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropConfiguration

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface ISystemBackdropController;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropController

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    interface ISystemBackdropControllerWithTargets;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("4c245142-08e6-5719-bacb-a2d2f18947c6"))
ITypedEventHandler<ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets*,IInspectable*> : ITypedEventHandler_impl<ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets*,IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets, Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets*,IInspectable*> __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets*,IInspectable*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropControllerWithTargets*,IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

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








#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                interface ICompositionSupportsSystemBackdrop;
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop ABI::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop_FWD_DEFINED__





namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct WindowId WindowId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */



#ifndef ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Foundation {
            interface IClosable;
        } /* Foundation */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CFoundation_CIClosable ABI::Windows::Foundation::IClosable

#endif // ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__




namespace ABI {
    namespace Windows {
        namespace UI {
            
            typedef struct Color Color;
            
        } /* UI */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Composition {
                class CompositionTarget;
            } /* Composition */
        } /* UI */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CUI_CComposition_CICompositionTarget_FWD_DEFINED__
#define ____x_ABI_CWindows_CUI_CComposition_CICompositionTarget_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Composition {
                interface ICompositionTarget;
            } /* Composition */
        } /* UI */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CUI_CComposition_CICompositionTarget ABI::Windows::UI::Composition::ICompositionTarget

#endif // ____x_ABI_CWindows_CUI_CComposition_CICompositionTarget_FWD_DEFINED__





namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Core {
                class CoreWindow;
            } /* Core */
        } /* UI */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CUI_CCore_CICoreWindow_FWD_DEFINED__
#define ____x_ABI_CWindows_CUI_CCore_CICoreWindow_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Core {
                interface ICoreWindow;
            } /* Core */
        } /* UI */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CUI_CCore_CICoreWindow ABI::Windows::UI::Core::ICoreWindow

#endif // ____x_ABI_CWindows_CUI_CCore_CICoreWindow_FWD_DEFINED__





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    
                    typedef enum MicaKind : int MicaKind;
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    
                    typedef enum SystemBackdropState : int SystemBackdropState;
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    
                    typedef enum SystemBackdropTheme : int SystemBackdropTheme;
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */










namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    class DesktopAcrylicController;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    class MicaController;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    class SystemBackdropConfiguration;
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */












/*
 *
 * Struct Microsoft.UI.Composition.SystemBackdrops.MicaKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [v1_enum, contract] */
                    enum MicaKind : int
                    {
                        MicaKind_Base = 0,
                        MicaKind_BaseAlt = 1,
                    };
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Struct Microsoft.UI.Composition.SystemBackdrops.SystemBackdropState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [v1_enum, contract] */
                    enum SystemBackdropState : int
                    {
                        SystemBackdropState_Active = 0,
                        SystemBackdropState_Fallback = 1,
                        SystemBackdropState_HighContrast = 2,
                    };
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Struct Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [v1_enum, contract] */
                    enum SystemBackdropTheme : int
                    {
                        SystemBackdropTheme_Default = 0,
                        SystemBackdropTheme_Light = 1,
                        SystemBackdropTheme_Dark = 2,
                    };
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IDesktopAcrylicController[] = L"Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("7C20A6AF-8EB3-5F08-BDFC-6D35E35DFE45"), exclusiveto, contract] */
                    MIDL_INTERFACE("7C20A6AF-8EB3-5F08-BDFC-6D35E35DFE45")
                    IDesktopAcrylicController : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FallbackColor(
                            /* [retval, out] */ABI::Windows::UI::Color * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FallbackColor(
                            /* [in] */ABI::Windows::UI::Color value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LuminosityOpacity(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_LuminosityOpacity(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TintColor(
                            /* [retval, out] */ABI::Windows::UI::Color * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TintColor(
                            /* [in] */ABI::Windows::UI::Color value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TintOpacity(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TintOpacity(
                            /* [in] */FLOAT value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IDesktopAcrylicController=_uuidof(IDesktopAcrylicController);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IDesktopAcrylicController2[] = L"Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("88E0A368-DFC7-5971-A50B-40DF5AA5F5C2"), exclusiveto, contract] */
                    MIDL_INTERFACE("88E0A368-DFC7-5971-A50B-40DF5AA5F5C2")
                    IDesktopAcrylicController2 : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE ResetProperties(void) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IDesktopAcrylicController2=_uuidof(IDesktopAcrylicController2);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicControllerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IDesktopAcrylicControllerStatics[] = L"Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicControllerStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("A9E8F790-79EF-5416-9B67-6BCFE867C8B7"), exclusiveto, contract] */
                    MIDL_INTERFACE("A9E8F790-79EF-5416-9B67-6BCFE867C8B7")
                    IDesktopAcrylicControllerStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE IsSupported(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IDesktopAcrylicControllerStatics=_uuidof(IDesktopAcrylicControllerStatics);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IMicaController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IMicaController[] = L"Microsoft.UI.Composition.SystemBackdrops.IMicaController";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("2DE996A9-0A2A-5889-A89C-1F84060A8CAB"), exclusiveto, contract] */
                    MIDL_INTERFACE("2DE996A9-0A2A-5889-A89C-1F84060A8CAB")
                    IMicaController : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FallbackColor(
                            /* [retval, out] */ABI::Windows::UI::Color * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FallbackColor(
                            /* [in] */ABI::Windows::UI::Color value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LuminosityOpacity(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_LuminosityOpacity(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TintColor(
                            /* [retval, out] */ABI::Windows::UI::Color * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TintColor(
                            /* [in] */ABI::Windows::UI::Color value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TintOpacity(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TintOpacity(
                            /* [in] */FLOAT value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IMicaController=_uuidof(IMicaController);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IMicaController2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IMicaController2[] = L"Microsoft.UI.Composition.SystemBackdrops.IMicaController2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("F1ED4A52-D9CA-506E-9586-CAAEFD3AA971"), exclusiveto, contract] */
                    MIDL_INTERFACE("F1ED4A52-D9CA-506E-9586-CAAEFD3AA971")
                    IMicaController2 : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Kind(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::SystemBackdrops::MicaKind * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Kind(
                            /* [in] */ABI::Microsoft::UI::Composition::SystemBackdrops::MicaKind value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE ResetProperties(void) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IMicaController2=_uuidof(IMicaController2);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IMicaControllerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IMicaControllerStatics[] = L"Microsoft.UI.Composition.SystemBackdrops.IMicaControllerStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("7D85D834-D514-5250-B7C4-0B7850D1EFDC"), exclusiveto, contract] */
                    MIDL_INTERFACE("7D85D834-D514-5250-B7C4-0B7850D1EFDC")
                    IMicaControllerStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE IsSupported(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IMicaControllerStatics=_uuidof(IMicaControllerStatics);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropConfiguration
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_ISystemBackdropConfiguration[] = L"Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropConfiguration";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("EBCCE1B9-0E0C-5431-AB0E-00F3F0669962"), exclusiveto, contract] */
                    MIDL_INTERFACE("EBCCE1B9-0E0C-5431-AB0E-00F3F0669962")
                    ISystemBackdropConfiguration : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HighContrastBackgroundColor(
                            /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_HighContrastBackgroundColor(
                            /* [in] */__FIReference_1_Windows__CUI__CColor * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsHighContrast(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsHighContrast(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsInputActive(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsInputActive(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Theme(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Theme(
                            /* [in] */ABI::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISystemBackdropConfiguration=_uuidof(ISystemBackdropConfiguration);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Windows.Foundation.IClosable
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_ISystemBackdropController[] = L"Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("5632D76C-0B74-5B52-AA33-80262068AEB2"), contract] */
                    MIDL_INTERFACE("5632D76C-0B74-5B52-AA33-80262068AEB2")
                    ISystemBackdropController : public IInspectable
                    {
                    public:
                        /* [overload, default_overload] */virtual HRESULT STDMETHODCALLTYPE SetTargetWithWindowId(
                            /* [in] */ABI::Microsoft::UI::WindowId windowId,
                            /* [in] */ABI::Windows::UI::Composition::ICompositionTarget * desktopWindowTarget,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        /* [overload] */virtual HRESULT STDMETHODCALLTYPE SetTargetWithCoreWindow(
                            /* [in] */ABI::Windows::UI::Core::ICoreWindow * coreWindow,
                            /* [in] */ABI::Windows::UI::Composition::ICompositionTarget * compositionTarget,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISystemBackdropController=_uuidof(ISystemBackdropController);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *     Windows.Foundation.IClosable
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_ISystemBackdropControllerWithTargets[] = L"Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace SystemBackdrops {
                    /* [object, uuid("9C56FE7C-98EB-5F89-AD97-DAD57FC30C8C"), contract] */
                    MIDL_INTERFACE("9C56FE7C-98EB-5F89-AD97-DAD57FC30C8C")
                    ISystemBackdropControllerWithTargets : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_State(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropState * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE AddSystemBackdropTarget(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop * systemBackdropTarget,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE RemoveAllSystemBackdropTargets(void) = 0;
                        virtual HRESULT STDMETHODCALLTYPE RemoveSystemBackdropTarget(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop * systemBackdropTarget,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetSystemBackdropConfiguration(
                            /* [in] */ABI::Microsoft::UI::Composition::SystemBackdrops::ISystemBackdropConfiguration * configuration
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_StateChanged(
                            /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_StateChanged(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISystemBackdropControllerWithTargets=_uuidof(ISystemBackdropControllerWithTargets);
                    
                } /* SystemBackdrops */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicControllerStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController ** Default Interface **
 *    Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController2
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *    Windows.Foundation.IClosable
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_DesktopAcrylicController_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_DesktopAcrylicController_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_SystemBackdrops_DesktopAcrylicController[] = L"Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.SystemBackdrops.IMicaControllerStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.SystemBackdrops.IMicaController ** Default Interface **
 *    Microsoft.UI.Composition.SystemBackdrops.IMicaController2
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *    Windows.Foundation.IClosable
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_MicaController_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_MicaController_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_SystemBackdrops_MicaController[] = L"Microsoft.UI.Composition.SystemBackdrops.MicaController";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropConfiguration ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_SystemBackdropConfiguration_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_SystemBackdropConfiguration_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_SystemBackdrops_SystemBackdropConfiguration[] = L"Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * sender,/* [in] */ __RPC__in_opt IInspectable * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectableVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

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






#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop __x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop_FWD_DEFINED__






typedef struct __x_ABI_CMicrosoft_CUI_CWindowId __x_ABI_CMicrosoft_CUI_CWindowId;



#ifndef ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIClosable __x_ABI_CWindows_CFoundation_CIClosable;

#endif // ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__





typedef struct __x_ABI_CWindows_CUI_CColor __x_ABI_CWindows_CUI_CColor;



#ifndef ____x_ABI_CWindows_CUI_CComposition_CICompositionTarget_FWD_DEFINED__
#define ____x_ABI_CWindows_CUI_CComposition_CICompositionTarget_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CUI_CComposition_CICompositionTarget __x_ABI_CWindows_CUI_CComposition_CICompositionTarget;

#endif // ____x_ABI_CWindows_CUI_CComposition_CICompositionTarget_FWD_DEFINED__





#ifndef ____x_ABI_CWindows_CUI_CCore_CICoreWindow_FWD_DEFINED__
#define ____x_ABI_CWindows_CUI_CCore_CICoreWindow_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CUI_CCore_CICoreWindow __x_ABI_CWindows_CUI_CCore_CICoreWindow;

#endif // ____x_ABI_CWindows_CUI_CCore_CICoreWindow_FWD_DEFINED__






typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CMicaKind __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CMicaKind;


typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropState __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropState;


typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropTheme __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropTheme;























/*
 *
 * Struct Microsoft.UI.Composition.SystemBackdrops.MicaKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CMicaKind
{
    MicaKind_Base = 0,
    MicaKind_BaseAlt = 1,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Struct Microsoft.UI.Composition.SystemBackdrops.SystemBackdropState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropState
{
    SystemBackdropState_Active = 0,
    SystemBackdropState_Fallback = 1,
    SystemBackdropState_HighContrast = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Struct Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropTheme
{
    SystemBackdropTheme_Default = 0,
    SystemBackdropTheme_Light = 1,
    SystemBackdropTheme_Dark = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IDesktopAcrylicController[] = L"Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController";
/* [object, uuid("7C20A6AF-8EB3-5F08-BDFC-6D35E35DFE45"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FallbackColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FallbackColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [in] */__x_ABI_CWindows_CUI_CColor value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LuminosityOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_LuminosityOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TintColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TintColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [in] */__x_ABI_CWindows_CUI_CColor value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TintOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TintOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController * This,
        /* [in] */FLOAT value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_get_FallbackColor(This,value) \
    ( (This)->lpVtbl->get_FallbackColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_put_FallbackColor(This,value) \
    ( (This)->lpVtbl->put_FallbackColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_get_LuminosityOpacity(This,value) \
    ( (This)->lpVtbl->get_LuminosityOpacity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_put_LuminosityOpacity(This,value) \
    ( (This)->lpVtbl->put_LuminosityOpacity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_get_TintColor(This,value) \
    ( (This)->lpVtbl->get_TintColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_put_TintColor(This,value) \
    ( (This)->lpVtbl->put_TintColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_get_TintOpacity(This,value) \
    ( (This)->lpVtbl->get_TintOpacity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_put_TintOpacity(This,value) \
    ( (This)->lpVtbl->put_TintOpacity(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IDesktopAcrylicController2[] = L"Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController2";
/* [object, uuid("88E0A368-DFC7-5971-A50B-40DF5AA5F5C2"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *ResetProperties )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2 * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_ResetProperties(This) \
    ( (This)->lpVtbl->ResetProperties(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicController2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicControllerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IDesktopAcrylicControllerStatics[] = L"Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicControllerStatics";
/* [object, uuid("A9E8F790-79EF-5416-9B67-6BCFE867C8B7"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *IsSupported )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics * This,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_IsSupported(This,result) \
    ( (This)->lpVtbl->IsSupported(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIDesktopAcrylicControllerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IMicaController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IMicaController[] = L"Microsoft.UI.Composition.SystemBackdrops.IMicaController";
/* [object, uuid("2DE996A9-0A2A-5889-A89C-1F84060A8CAB"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FallbackColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FallbackColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [in] */__x_ABI_CWindows_CUI_CColor value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LuminosityOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_LuminosityOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TintColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TintColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [in] */__x_ABI_CWindows_CUI_CColor value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TintOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TintOpacity )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController * This,
        /* [in] */FLOAT value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_get_FallbackColor(This,value) \
    ( (This)->lpVtbl->get_FallbackColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_put_FallbackColor(This,value) \
    ( (This)->lpVtbl->put_FallbackColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_get_LuminosityOpacity(This,value) \
    ( (This)->lpVtbl->get_LuminosityOpacity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_put_LuminosityOpacity(This,value) \
    ( (This)->lpVtbl->put_LuminosityOpacity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_get_TintColor(This,value) \
    ( (This)->lpVtbl->get_TintColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_put_TintColor(This,value) \
    ( (This)->lpVtbl->put_TintColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_get_TintOpacity(This,value) \
    ( (This)->lpVtbl->get_TintOpacity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_put_TintOpacity(This,value) \
    ( (This)->lpVtbl->put_TintOpacity(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IMicaController2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IMicaController2[] = L"Microsoft.UI.Composition.SystemBackdrops.IMicaController2";
/* [object, uuid("F1ED4A52-D9CA-506E-9586-CAAEFD3AA971"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Kind )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CMicaKind * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Kind )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CMicaKind value
        );
    HRESULT ( STDMETHODCALLTYPE *ResetProperties )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2 * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_get_Kind(This,value) \
    ( (This)->lpVtbl->get_Kind(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_put_Kind(This,value) \
    ( (This)->lpVtbl->put_Kind(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_ResetProperties(This) \
    ( (This)->lpVtbl->ResetProperties(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaController2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.IMicaControllerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_IMicaControllerStatics[] = L"Microsoft.UI.Composition.SystemBackdrops.IMicaControllerStatics";
/* [object, uuid("7D85D834-D514-5250-B7C4-0B7850D1EFDC"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *IsSupported )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics * This,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_IsSupported(This,result) \
    ( (This)->lpVtbl->IsSupported(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CIMicaControllerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropConfiguration
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_ISystemBackdropConfiguration[] = L"Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropConfiguration";
/* [object, uuid("EBCCE1B9-0E0C-5431-AB0E-00F3F0669962"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfigurationVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HighContrastBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [retval, out] */__FIReference_1_Windows__CUI__CColor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_HighContrastBackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [in] */__FIReference_1_Windows__CUI__CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsHighContrast )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsHighContrast )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsInputActive )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsInputActive )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Theme )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropTheme * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Theme )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropTheme value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfigurationVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfigurationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_get_HighContrastBackgroundColor(This,value) \
    ( (This)->lpVtbl->get_HighContrastBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_put_HighContrastBackgroundColor(This,value) \
    ( (This)->lpVtbl->put_HighContrastBackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_get_IsHighContrast(This,value) \
    ( (This)->lpVtbl->get_IsHighContrast(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_put_IsHighContrast(This,value) \
    ( (This)->lpVtbl->put_IsHighContrast(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_get_IsInputActive(This,value) \
    ( (This)->lpVtbl->get_IsInputActive(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_put_IsInputActive(This,value) \
    ( (This)->lpVtbl->put_IsInputActive(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_get_Theme(This,value) \
    ( (This)->lpVtbl->get_Theme(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_put_Theme(This,value) \
    ( (This)->lpVtbl->put_Theme(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Windows.Foundation.IClosable
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_ISystemBackdropController[] = L"Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController";
/* [object, uuid("5632D76C-0B74-5B52-AA33-80262068AEB2"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [overload, default_overload] */HRESULT ( STDMETHODCALLTYPE *SetTargetWithWindowId )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId,
        /* [in] */__x_ABI_CWindows_CUI_CComposition_CICompositionTarget * desktopWindowTarget,
        /* [retval, out] */boolean * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *SetTargetWithCoreWindow )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController * This,
        /* [in] */__x_ABI_CWindows_CUI_CCore_CICoreWindow * coreWindow,
        /* [in] */__x_ABI_CWindows_CUI_CComposition_CICompositionTarget * compositionTarget,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_SetTargetWithWindowId(This,windowId,desktopWindowTarget,result) \
    ( (This)->lpVtbl->SetTargetWithWindowId(This,windowId,desktopWindowTarget,result) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_SetTargetWithCoreWindow(This,coreWindow,compositionTarget,result) \
    ( (This)->lpVtbl->SetTargetWithCoreWindow(This,coreWindow,compositionTarget,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropController_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *     Windows.Foundation.IClosable
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_SystemBackdrops_ISystemBackdropControllerWithTargets[] = L"Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets";
/* [object, uuid("9C56FE7C-98EB-5F89-AD97-DAD57FC30C8C"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargetsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_State )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CSystemBackdropState * value
        );
    HRESULT ( STDMETHODCALLTYPE *AddSystemBackdropTarget )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop * systemBackdropTarget,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *RemoveAllSystemBackdropTargets )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This
        );
    HRESULT ( STDMETHODCALLTYPE *RemoveSystemBackdropTarget )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositionSupportsSystemBackdrop * systemBackdropTarget,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetSystemBackdropConfiguration )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropConfiguration * configuration
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_StateChanged )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CComposition__CSystemBackdrops__CISystemBackdropControllerWithTargets_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_StateChanged )(
        __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargetsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargetsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_get_State(This,value) \
    ( (This)->lpVtbl->get_State(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_AddSystemBackdropTarget(This,systemBackdropTarget,result) \
    ( (This)->lpVtbl->AddSystemBackdropTarget(This,systemBackdropTarget,result) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_RemoveAllSystemBackdropTargets(This) \
    ( (This)->lpVtbl->RemoveAllSystemBackdropTargets(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_RemoveSystemBackdropTarget(This,systemBackdropTarget,result) \
    ( (This)->lpVtbl->RemoveSystemBackdropTarget(This,systemBackdropTarget,result) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_SetSystemBackdropConfiguration(This,configuration) \
    ( (This)->lpVtbl->SetSystemBackdropConfiguration(This,configuration) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_add_StateChanged(This,handler,token) \
    ( (This)->lpVtbl->add_StateChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_remove_StateChanged(This,token) \
    ( (This)->lpVtbl->remove_StateChanged(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CSystemBackdrops_CISystemBackdropControllerWithTargets_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicControllerStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController ** Default Interface **
 *    Microsoft.UI.Composition.SystemBackdrops.IDesktopAcrylicController2
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *    Windows.Foundation.IClosable
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_DesktopAcrylicController_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_DesktopAcrylicController_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_SystemBackdrops_DesktopAcrylicController[] = L"Microsoft.UI.Composition.SystemBackdrops.DesktopAcrylicController";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.SystemBackdrops.MicaController
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.SystemBackdrops.IMicaControllerStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.SystemBackdrops.IMicaController ** Default Interface **
 *    Microsoft.UI.Composition.SystemBackdrops.IMicaController2
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropController
 *    Windows.Foundation.IClosable
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropControllerWithTargets
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_MicaController_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_MicaController_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_SystemBackdrops_MicaController[] = L"Microsoft.UI.Composition.SystemBackdrops.MicaController";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.SystemBackdrops.ISystemBackdropConfiguration ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_SystemBackdropConfiguration_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_SystemBackdrops_SystemBackdropConfiguration_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_SystemBackdrops_SystemBackdropConfiguration[] = L"Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EComposition2ESystemBackdrops_p_h__

#endif // __Microsoft2EUI2EComposition2ESystemBackdrops_h__
