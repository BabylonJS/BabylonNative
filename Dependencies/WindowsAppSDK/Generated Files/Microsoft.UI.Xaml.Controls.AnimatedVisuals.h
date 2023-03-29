/* Header file automatically generated from Microsoft.UI.Xaml.Controls.AnimatedVisuals.idl */
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
#ifndef __Microsoft2EUI2EXaml2EControls2EAnimatedVisuals_h__
#define __Microsoft2EUI2EXaml2EControls2EAnimatedVisuals_h__
#ifndef __Microsoft2EUI2EXaml2EControls2EAnimatedVisuals_p_h__
#define __Microsoft2EUI2EXaml2EControls2EAnimatedVisuals_p_h__


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
#include "Microsoft.UI.Xaml.Controls.h"

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    interface IAnimatedVisualSource;
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource ABI::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    interface IAnimatedVisualSource2;
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2 ABI::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource2

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2_FWD_DEFINED__










namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedAcceptVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedBackVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedChevronDownSmallVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedChevronRightDownSmallVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedChevronUpDownSmallVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedFindVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedGlobalNavigationButtonVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    namespace AnimatedVisuals {
                        class AnimatedSettingsVisualSource;
                    } /* AnimatedVisuals */
                } /* Controls */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */








/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedAcceptVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedAcceptVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedAcceptVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedAcceptVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedAcceptVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedBackVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedBackVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedBackVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedBackVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedBackVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronDownSmallVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronDownSmallVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronDownSmallVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronDownSmallVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronDownSmallVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronRightDownSmallVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronRightDownSmallVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronRightDownSmallVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronRightDownSmallVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronRightDownSmallVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronUpDownSmallVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronUpDownSmallVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronUpDownSmallVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronUpDownSmallVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronUpDownSmallVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedFindVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedFindVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedFindVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedFindVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedFindVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedGlobalNavigationButtonVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedGlobalNavigationButtonVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedGlobalNavigationButtonVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedGlobalNavigationButtonVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedGlobalNavigationButtonVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedSettingsVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedSettingsVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedSettingsVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedSettingsVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedSettingsVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000






#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource __x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2 __x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CControls_CIAnimatedVisualSource2_FWD_DEFINED__
























/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedAcceptVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedAcceptVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedAcceptVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedAcceptVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedAcceptVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedBackVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedBackVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedBackVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedBackVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedBackVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronDownSmallVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronDownSmallVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronDownSmallVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronDownSmallVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronDownSmallVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronRightDownSmallVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronRightDownSmallVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronRightDownSmallVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronRightDownSmallVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronRightDownSmallVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronUpDownSmallVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronUpDownSmallVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronUpDownSmallVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedChevronUpDownSmallVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedChevronUpDownSmallVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedFindVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedFindVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedFindVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedFindVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedFindVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedGlobalNavigationButtonVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedGlobalNavigationButtonVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedGlobalNavigationButtonVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedGlobalNavigationButtonVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedGlobalNavigationButtonVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedSettingsVisualSource
 *
 * Introduced to Microsoft.UI.Xaml.XamlContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.XamlContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource ** Default Interface **
 *    Microsoft.UI.Xaml.Controls.IAnimatedVisualSource2
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedSettingsVisualSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedSettingsVisualSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Controls_AnimatedVisuals_AnimatedSettingsVisualSource[] = L"Microsoft.UI.Xaml.Controls.AnimatedVisuals.AnimatedSettingsVisualSource";
#endif
#endif // MICROSOFT_UI_XAML_XAMLCONTRACT_VERSION >= 0x10000






#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EControls2EAnimatedVisuals_p_h__

#endif // __Microsoft2EUI2EXaml2EControls2EAnimatedVisuals_h__
