/* Header file automatically generated from Microsoft.UI.Input.idl */
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
#ifndef __Microsoft2EUI2EInput_h__
#define __Microsoft2EUI2EInput_h__
#ifndef __Microsoft2EUI2EInput_p_h__
#define __Microsoft2EUI2EInput_p_h__


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
#include "Microsoft.UI.Dispatching.h"
#include "Windows.System.h"
#include "Windows.UI.Core.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface ICrossSlidingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs ABI::Microsoft::UI::Input::ICrossSlidingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IDraggingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs ABI::Microsoft::UI::Input::IDraggingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IGestureRecognizer;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer ABI::Microsoft::UI::Input::IGestureRecognizer

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IHoldingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs ABI::Microsoft::UI::Input::IHoldingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputActivationListener;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener ABI::Microsoft::UI::Input::IInputActivationListener

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputActivationListenerActivationChangedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs ABI::Microsoft::UI::Input::IInputActivationListenerActivationChangedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputActivationListenerStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics ABI::Microsoft::UI::Input::IInputActivationListenerStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor ABI::Microsoft::UI::Input::IInputCursor

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputCursorFactory;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory ABI::Microsoft::UI::Input::IInputCursorFactory

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputCursorStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics ABI::Microsoft::UI::Input::IInputCursorStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputCustomCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor ABI::Microsoft::UI::Input::IInputCustomCursor

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputCustomCursorFactory;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory ABI::Microsoft::UI::Input::IInputCustomCursorFactory

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputDesktopNamedResourceCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor ABI::Microsoft::UI::Input::IInputDesktopNamedResourceCursor

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputDesktopNamedResourceCursorStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics ABI::Microsoft::UI::Input::IInputDesktopNamedResourceCursorStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputDesktopResourceCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor ABI::Microsoft::UI::Input::IInputDesktopResourceCursor

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputDesktopResourceCursorStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics ABI::Microsoft::UI::Input::IInputDesktopResourceCursorStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputKeyboardSource;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource ABI::Microsoft::UI::Input::IInputKeyboardSource

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputKeyboardSourceStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics ABI::Microsoft::UI::Input::IInputKeyboardSourceStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputLightDismissAction;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction ABI::Microsoft::UI::Input::IInputLightDismissAction

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputLightDismissActionStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics ABI::Microsoft::UI::Input::IInputLightDismissActionStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputLightDismissEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs ABI::Microsoft::UI::Input::IInputLightDismissEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputObject;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject ABI::Microsoft::UI::Input::IInputObject

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputObjectFactory;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory ABI::Microsoft::UI::Input::IInputObjectFactory

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputPointerSource;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource ABI::Microsoft::UI::Input::IInputPointerSource

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputSystemCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor ABI::Microsoft::UI::Input::IInputSystemCursor

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IInputSystemCursorStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics ABI::Microsoft::UI::Input::IInputSystemCursorStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IManipulationCompletedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs ABI::Microsoft::UI::Input::IManipulationCompletedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IManipulationInertiaStartingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs ABI::Microsoft::UI::Input::IManipulationInertiaStartingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IManipulationStartedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs ABI::Microsoft::UI::Input::IManipulationStartedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IManipulationUpdatedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs ABI::Microsoft::UI::Input::IManipulationUpdatedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IMouseWheelParameters;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters ABI::Microsoft::UI::Input::IMouseWheelParameters

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IPointerEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs ABI::Microsoft::UI::Input::IPointerEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IPointerPoint;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint ABI::Microsoft::UI::Input::IPointerPoint

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IPointerPointProperties;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties ABI::Microsoft::UI::Input::IPointerPointProperties

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IPointerPointTransform;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform ABI::Microsoft::UI::Input::IPointerPointTransform

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IPointerPredictor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor ABI::Microsoft::UI::Input::IPointerPredictor

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IPointerPredictorStatics;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics ABI::Microsoft::UI::Input::IPointerPredictorStatics

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface IRightTappedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs ABI::Microsoft::UI::Input::IRightTappedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                interface ITappedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs ABI::Microsoft::UI::Input::ITappedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class PointerPoint;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_USE
#define DEF___FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("5b63939d-11cf-56c4-b0d0-11df9dc487d1"))
IIterator<ABI::Microsoft::UI::Input::PointerPoint*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::PointerPoint*, ABI::Microsoft::UI::Input::IPointerPoint*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Microsoft.UI.Input.PointerPoint>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Microsoft::UI::Input::PointerPoint*> __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_t;
#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::__FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Input::IPointerPoint*>
//#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_t ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Input::IPointerPoint*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_USE
#define DEF___FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("645acc33-ffce-5ad3-be2b-c49b9c27c35d"))
IIterable<ABI::Microsoft::UI::Input::PointerPoint*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::PointerPoint*, ABI::Microsoft::UI::Input::IPointerPoint*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Microsoft.UI.Input.PointerPoint>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Microsoft::UI::Input::PointerPoint*> __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_t;
#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::__FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Input::IPointerPoint*>
//#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_t ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Input::IPointerPoint*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_USE
#define DEF___FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("5fbb224b-8430-5086-ab85-e71dfaa84c90"))
IVectorView<ABI::Microsoft::UI::Input::PointerPoint*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::PointerPoint*, ABI::Microsoft::UI::Input::IPointerPoint*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Microsoft.UI.Input.PointerPoint>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Microsoft::UI::Input::PointerPoint*> __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_t;
#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::__FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Input::IPointerPoint*>
//#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Input::IPointerPoint*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVector_1_Microsoft__CUI__CInput__CPointerPoint_USE
#define DEF___FIVector_1_Microsoft__CUI__CInput__CPointerPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("8220cf33-5d76-5607-bb7d-b7cd07d27f33"))
IVector<ABI::Microsoft::UI::Input::PointerPoint*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::PointerPoint*, ABI::Microsoft::UI::Input::IPointerPoint*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Microsoft.UI.Input.PointerPoint>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<ABI::Microsoft::UI::Input::PointerPoint*> __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_t;
#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::__FIVector_1_Microsoft__CUI__CInput__CPointerPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Input::IPointerPoint*>
//#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_t ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Input::IPointerPoint*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_Microsoft__CUI__CInput__CPointerPoint_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class GestureRecognizer;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class CrossSlidingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("2c68b524-bff5-5b9e-ab2d-9b1cac2d3876"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::CrossSlidingEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::CrossSlidingEventArgs*, ABI::Microsoft::UI::Input::ICrossSlidingEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.CrossSlidingEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::CrossSlidingEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::ICrossSlidingEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::ICrossSlidingEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class DraggingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("7fc18fed-898b-58cf-9996-2baa40e7dd3d"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::DraggingEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::DraggingEventArgs*, ABI::Microsoft::UI::Input::IDraggingEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.DraggingEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::DraggingEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IDraggingEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IDraggingEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class HoldingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("41508cc9-7826-58f9-a6e9-63be531e95d3"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::HoldingEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::HoldingEventArgs*, ABI::Microsoft::UI::Input::IHoldingEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.HoldingEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::HoldingEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IHoldingEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IHoldingEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class ManipulationCompletedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("14bbef09-d8fd-56a9-ab2f-01dbf0187593"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationCompletedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::ManipulationCompletedEventArgs*, ABI::Microsoft::UI::Input::IManipulationCompletedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.ManipulationCompletedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationCompletedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationCompletedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationCompletedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class ManipulationInertiaStartingEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("5e7f485a-4248-574a-aee5-62ff0cf2d881"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationInertiaStartingEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::ManipulationInertiaStartingEventArgs*, ABI::Microsoft::UI::Input::IManipulationInertiaStartingEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.ManipulationInertiaStartingEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationInertiaStartingEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationInertiaStartingEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationInertiaStartingEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class ManipulationStartedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("44900647-40d5-5897-8957-310e220da555"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationStartedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::ManipulationStartedEventArgs*, ABI::Microsoft::UI::Input::IManipulationStartedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.ManipulationStartedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationStartedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationStartedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationStartedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class ManipulationUpdatedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("dd71c5ac-5ae1-5069-877a-ace37b5a4720"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationUpdatedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::ManipulationUpdatedEventArgs*, ABI::Microsoft::UI::Input::IManipulationUpdatedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.ManipulationUpdatedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::ManipulationUpdatedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationUpdatedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IManipulationUpdatedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class RightTappedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("71b4ad41-872a-5afd-9332-704311f261b5"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::RightTappedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::RightTappedEventArgs*, ABI::Microsoft::UI::Input::IRightTappedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.RightTappedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::RightTappedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IRightTappedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::IRightTappedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class TappedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("43cfd88f-74b1-570b-a71c-d78e6815cf0a"))
ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::TappedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::GestureRecognizer*, ABI::Microsoft::UI::Input::IGestureRecognizer*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::TappedEventArgs*, ABI::Microsoft::UI::Input::ITappedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.GestureRecognizer, Microsoft.UI.Input.TappedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::GestureRecognizer*,ABI::Microsoft::UI::Input::TappedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::ITappedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IGestureRecognizer*,ABI::Microsoft::UI::Input::ITappedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputActivationListener;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputActivationListenerActivationChangedEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("25b49081-1922-5447-b90f-cd4f9b03161b"))
ITypedEventHandler<ABI::Microsoft::UI::Input::InputActivationListener*,ABI::Microsoft::UI::Input::InputActivationListenerActivationChangedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::InputActivationListener*, ABI::Microsoft::UI::Input::IInputActivationListener*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::InputActivationListenerActivationChangedEventArgs*, ABI::Microsoft::UI::Input::IInputActivationListenerActivationChangedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.InputActivationListener, Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::InputActivationListener*,ABI::Microsoft::UI::Input::InputActivationListenerActivationChangedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IInputActivationListener*,ABI::Microsoft::UI::Input::IInputActivationListenerActivationChangedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IInputActivationListener*,ABI::Microsoft::UI::Input::IInputActivationListenerActivationChangedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputLightDismissAction;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputLightDismissEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("91cb240d-f32c-5efb-8090-5ab5f1733d21"))
ITypedEventHandler<ABI::Microsoft::UI::Input::InputLightDismissAction*,ABI::Microsoft::UI::Input::InputLightDismissEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::InputLightDismissAction*, ABI::Microsoft::UI::Input::IInputLightDismissAction*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::InputLightDismissEventArgs*, ABI::Microsoft::UI::Input::IInputLightDismissEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.InputLightDismissAction, Microsoft.UI.Input.InputLightDismissEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::InputLightDismissAction*,ABI::Microsoft::UI::Input::InputLightDismissEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IInputLightDismissAction*,ABI::Microsoft::UI::Input::IInputLightDismissEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IInputLightDismissAction*,ABI::Microsoft::UI::Input::IInputLightDismissEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputPointerSource;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class PointerEventArgs;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("040480be-4741-587d-9888-8694414a2650"))
ITypedEventHandler<ABI::Microsoft::UI::Input::InputPointerSource*,ABI::Microsoft::UI::Input::PointerEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::InputPointerSource*, ABI::Microsoft::UI::Input::IInputPointerSource*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Input::PointerEventArgs*, ABI::Microsoft::UI::Input::IPointerEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Input.InputPointerSource, Microsoft.UI.Input.PointerEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Input::InputPointerSource*,ABI::Microsoft::UI::Input::PointerEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IInputPointerSource*,ABI::Microsoft::UI::Input::IPointerEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Input::IInputPointerSource*,ABI::Microsoft::UI::Input::IPointerEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000






namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Dispatching {
                class DispatcherQueue;
            } /* Dispatching */
        } /* UI */
    } /* Microsoft */
} /* ABI */

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
        namespace Foundation {
            
            typedef struct Point Point;
            
        } /* Foundation */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace Foundation {
            
            typedef struct Rect Rect;
            
        } /* Foundation */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace Foundation {
            
            typedef struct TimeSpan TimeSpan;
            
        } /* Foundation */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Windows {
        namespace System {
            
            typedef enum VirtualKey : int VirtualKey;
            
        } /* System */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace System {
            
            typedef enum VirtualKeyModifiers : unsigned int VirtualKeyModifiers;
            
        } /* System */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Core {
                class CoreCursor;
            } /* Core */
        } /* UI */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CUI_CCore_CICoreCursor_FWD_DEFINED__
#define ____x_ABI_CWindows_CUI_CCore_CICoreCursor_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Core {
                interface ICoreCursor;
            } /* Core */
        } /* UI */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CUI_CCore_CICoreCursor ABI::Windows::UI::Core::ICoreCursor

#endif // ____x_ABI_CWindows_CUI_CCore_CICoreCursor_FWD_DEFINED__


namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Core {
                
                typedef enum CoreVirtualKeyStates : unsigned int CoreVirtualKeyStates;
                
            } /* Core */
        } /* UI */
    } /* Windows */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum CrossSlidingState : int CrossSlidingState;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum DraggingState : int DraggingState;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum GestureSettings : unsigned int GestureSettings;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum HoldingState : int HoldingState;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum InputActivationState : int InputActivationState;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum InputPointerSourceDeviceKinds : unsigned int InputPointerSourceDeviceKinds;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum InputSystemCursorShape : int InputSystemCursorShape;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum PointerDeviceType : int PointerDeviceType;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef enum PointerUpdateKind : int PointerUpdateKind;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef struct CrossSlideThresholds CrossSlideThresholds;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef struct ManipulationDelta ManipulationDelta;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                
                typedef struct ManipulationVelocities ManipulationVelocities;
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */














































namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputCustomCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputDesktopNamedResourceCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputDesktopResourceCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputKeyboardSource;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputObject;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class InputSystemCursor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */






namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class MouseWheelParameters;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class PointerPointProperties;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                class PointerPredictor;
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */












/*
 *
 * Struct Microsoft.UI.Input.CrossSlidingState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum CrossSlidingState : int
                {
                    CrossSlidingState_Started = 0,
                    CrossSlidingState_Dragging = 1,
                    CrossSlidingState_Selecting = 2,
                    CrossSlidingState_SelectSpeedBumping = 3,
                    CrossSlidingState_SpeedBumping = 4,
                    CrossSlidingState_Rearranging = 5,
                    CrossSlidingState_Completed = 6,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.DraggingState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum DraggingState : int
                {
                    DraggingState_Started = 0,
                    DraggingState_Continuing = 1,
                    DraggingState_Completed = 2,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.GestureSettings
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, flags, contract] */
                enum GestureSettings : unsigned int
                {
                    GestureSettings_None = 0,
                    GestureSettings_Tap = 0x1,
                    GestureSettings_DoubleTap = 0x2,
                    GestureSettings_Hold = 0x4,
                    GestureSettings_HoldWithMouse = 0x8,
                    GestureSettings_RightTap = 0x10,
                    GestureSettings_Drag = 0x20,
                    GestureSettings_ManipulationTranslateX = 0x40,
                    GestureSettings_ManipulationTranslateY = 0x80,
                    GestureSettings_ManipulationTranslateRailsX = 0x100,
                    GestureSettings_ManipulationTranslateRailsY = 0x200,
                    GestureSettings_ManipulationRotate = 0x400,
                    GestureSettings_ManipulationScale = 0x800,
                    GestureSettings_ManipulationTranslateInertia = 0x1000,
                    GestureSettings_ManipulationRotateInertia = 0x2000,
                    GestureSettings_ManipulationScaleInertia = 0x4000,
                    GestureSettings_CrossSlide = 0x8000,
                    GestureSettings_ManipulationMultipleFingerPanning = 0x10000,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(GestureSettings)
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.HoldingState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum HoldingState : int
                {
                    HoldingState_Started = 0,
                    HoldingState_Completed = 1,
                    HoldingState_Canceled = 2,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.InputActivationState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum InputActivationState : int
                {
                    InputActivationState_None = 0,
                    InputActivationState_Deactivated = 1,
                    InputActivationState_Activated = 2,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Struct Microsoft.UI.Input.InputPointerSourceDeviceKinds
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, flags, contract] */
                enum InputPointerSourceDeviceKinds : unsigned int
                {
                    InputPointerSourceDeviceKinds_None = 0,
                    InputPointerSourceDeviceKinds_Touch = 0x1,
                    InputPointerSourceDeviceKinds_Pen = 0x2,
                    InputPointerSourceDeviceKinds_Mouse = 0x4,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(InputPointerSourceDeviceKinds)
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.InputSystemCursorShape
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum InputSystemCursorShape : int
                {
                    InputSystemCursorShape_Arrow = 0,
                    InputSystemCursorShape_Cross = 1,
                    InputSystemCursorShape_Hand = 3,
                    InputSystemCursorShape_Help = 4,
                    InputSystemCursorShape_IBeam = 5,
                    InputSystemCursorShape_SizeAll = 6,
                    InputSystemCursorShape_SizeNortheastSouthwest = 7,
                    InputSystemCursorShape_SizeNorthSouth = 8,
                    InputSystemCursorShape_SizeNorthwestSoutheast = 9,
                    InputSystemCursorShape_SizeWestEast = 10,
                    InputSystemCursorShape_UniversalNo = 11,
                    InputSystemCursorShape_UpArrow = 12,
                    InputSystemCursorShape_Wait = 13,
                    InputSystemCursorShape_Pin = 14,
                    InputSystemCursorShape_Person = 15,
                    InputSystemCursorShape_AppStarting = 16,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.PointerDeviceType
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum PointerDeviceType : int
                {
                    PointerDeviceType_Touch = 0,
                    PointerDeviceType_Pen = 1,
                    PointerDeviceType_Mouse = 2,
                    PointerDeviceType_Touchpad = 3,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.PointerUpdateKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [v1_enum, contract] */
                enum PointerUpdateKind : int
                {
                    PointerUpdateKind_Other = 0,
                    PointerUpdateKind_LeftButtonPressed = 1,
                    PointerUpdateKind_LeftButtonReleased = 2,
                    PointerUpdateKind_RightButtonPressed = 3,
                    PointerUpdateKind_RightButtonReleased = 4,
                    PointerUpdateKind_MiddleButtonPressed = 5,
                    PointerUpdateKind_MiddleButtonReleased = 6,
                    PointerUpdateKind_XButton1Pressed = 7,
                    PointerUpdateKind_XButton1Released = 8,
                    PointerUpdateKind_XButton2Pressed = 9,
                    PointerUpdateKind_XButton2Released = 10,
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.CrossSlideThresholds
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [contract] */
                struct CrossSlideThresholds
                {
                    FLOAT SelectionStart;
                    FLOAT SpeedBumpStart;
                    FLOAT SpeedBumpEnd;
                    FLOAT RearrangeStart;
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.ManipulationDelta
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [contract] */
                struct ManipulationDelta
                {
                    ABI::Windows::Foundation::Point Translation;
                    FLOAT Scale;
                    FLOAT Rotation;
                    FLOAT Expansion;
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.ManipulationVelocities
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [contract] */
                struct ManipulationVelocities
                {
                    ABI::Windows::Foundation::Point Linear;
                    FLOAT Angular;
                    FLOAT Expansion;
                };
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.ICrossSlidingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.CrossSlidingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_ICrossSlidingEventArgs[] = L"Microsoft.UI.Input.ICrossSlidingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("7679641F-BA9F-543C-A7C8-6229A98F89EF"), exclusiveto, contract] */
                MIDL_INTERFACE("7679641F-BA9F-543C-A7C8-6229A98F89EF")
                ICrossSlidingEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CrossSlidingState(
                        /* [retval, out] */ABI::Microsoft::UI::Input::CrossSlidingState * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ICrossSlidingEventArgs=_uuidof(ICrossSlidingEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IDraggingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.DraggingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IDraggingEventArgs[] = L"Microsoft.UI.Input.IDraggingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("3EFB1B75-3D3B-550E-963D-0828CA76128A"), exclusiveto, contract] */
                MIDL_INTERFACE("3EFB1B75-3D3B-550E-963D-0828CA76128A")
                IDraggingEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DraggingState(
                        /* [retval, out] */ABI::Microsoft::UI::Input::DraggingState * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDraggingEventArgs=_uuidof(IDraggingEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IGestureRecognizer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.GestureRecognizer
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IGestureRecognizer[] = L"Microsoft.UI.Input.IGestureRecognizer";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("CDA89AFC-6BD0-595C-BA37-545FCE5BF016"), exclusiveto, contract] */
                MIDL_INTERFACE("CDA89AFC-6BD0-595C-BA37-545FCE5BF016")
                IGestureRecognizer : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AutoProcessInertia(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AutoProcessInertia(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CrossSlideExact(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CrossSlideExact(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CrossSlideHorizontally(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CrossSlideHorizontally(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CrossSlideThresholds(
                        /* [retval, out] */ABI::Microsoft::UI::Input::CrossSlideThresholds * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CrossSlideThresholds(
                        /* [in] */ABI::Microsoft::UI::Input::CrossSlideThresholds value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_GestureSettings(
                        /* [retval, out] */ABI::Microsoft::UI::Input::GestureSettings * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_GestureSettings(
                        /* [in] */ABI::Microsoft::UI::Input::GestureSettings value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsActive(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsInertial(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PivotCenter(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PivotCenter(
                        /* [in] */ABI::Windows::Foundation::Point value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PivotRadius(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PivotRadius(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InertiaExpansionDeceleration(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InertiaExpansionDeceleration(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InertiaExpansion(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InertiaExpansion(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InertiaRotationAngle(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InertiaRotationAngle(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InertiaRotationDeceleration(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InertiaRotationDeceleration(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InertiaTranslationDeceleration(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InertiaTranslationDeceleration(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InertiaTranslationDisplacement(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InertiaTranslationDisplacement(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ManipulationExact(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ManipulationExact(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MouseWheelParameters(
                        /* [retval, out] */ABI::Microsoft::UI::Input::IMouseWheelParameters * * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ShowGestureFeedback(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ShowGestureFeedback(
                        /* [in] */::boolean value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CanBeDoubleTap(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPoint * value,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CompleteGesture(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ProcessDownEvent(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPoint * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ProcessMoveEvents(
                        /* [in] */__FIVector_1_Microsoft__CUI__CInput__CPointerPoint * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ProcessMouseWheelEvent(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPoint * value,
                        /* [in] */::boolean isShiftKeyDown,
                        /* [in] */::boolean isControlKeyDown
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ProcessInertia(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ProcessUpEvent(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPoint * value
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Tapped(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Tapped(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_RightTapped(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_RightTapped(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Holding(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Holding(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Dragging(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Dragging(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ManipulationStarted(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ManipulationStarted(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ManipulationUpdated(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ManipulationUpdated(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ManipulationInertiaStarting(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ManipulationInertiaStarting(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ManipulationCompleted(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ManipulationCompleted(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_CrossSliding(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_CrossSliding(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IGestureRecognizer=_uuidof(IGestureRecognizer);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IHoldingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.HoldingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IHoldingEventArgs[] = L"Microsoft.UI.Input.IHoldingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("8E449E85-D223-533C-B0B2-BF7C6D10C2DB"), exclusiveto, contract] */
                MIDL_INTERFACE("8E449E85-D223-533C-B0B2-BF7C6D10C2DB")
                IHoldingEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HoldingState(
                        /* [retval, out] */ABI::Microsoft::UI::Input::HoldingState * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IHoldingEventArgs=_uuidof(IHoldingEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputActivationListener
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputActivationListener
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputActivationListener[] = L"Microsoft.UI.Input.IInputActivationListener";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("3B818627-6CE7-5E0D-A0F5-6684FD1AEC78"), exclusiveto, contract] */
                MIDL_INTERFACE("3B818627-6CE7-5E0D-A0F5-6684FD1AEC78")
                IInputActivationListener : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_State(
                        /* [retval, out] */ABI::Microsoft::UI::Input::InputActivationState * value
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_InputActivationChanged(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_InputActivationChanged(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputActivationListener=_uuidof(IInputActivationListener);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputActivationListenerActivationChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputActivationListenerActivationChangedEventArgs[] = L"Microsoft.UI.Input.IInputActivationListenerActivationChangedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("7978526B-00B6-5303-8F7D-55BEF36DA786"), exclusiveto, contract] */
                MIDL_INTERFACE("7978526B-00B6-5303-8F7D-55BEF36DA786")
                IInputActivationListenerActivationChangedEventArgs : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputActivationListenerActivationChangedEventArgs=_uuidof(IInputActivationListenerActivationChangedEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputActivationListenerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputActivationListener
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputActivationListenerStatics[] = L"Microsoft.UI.Input.IInputActivationListenerStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("C4249843-F053-5C99-9D51-720ADE94224D"), exclusiveto, contract] */
                MIDL_INTERFACE("C4249843-F053-5C99-9D51-720ADE94224D")
                IInputActivationListenerStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE GetForWindowId(
                        /* [in] */ABI::Microsoft::UI::WindowId windowId,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputActivationListener * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputActivationListenerStatics=_uuidof(IInputActivationListenerStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCursor[] = L"Microsoft.UI.Input.IInputCursor";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("359B15F9-19C2-5714-8432-75176826406B"), exclusiveto, contract] */
                MIDL_INTERFACE("359B15F9-19C2-5714-8432-75176826406B")
                IInputCursor : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputCursor=_uuidof(IInputCursor);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputCursorFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCursorFactory[] = L"Microsoft.UI.Input.IInputCursorFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("2F47647B-4BE0-53E9-BE7E-C38D5459DB6B"), exclusiveto, contract] */
                MIDL_INTERFACE("2F47647B-4BE0-53E9-BE7E-C38D5459DB6B")
                IInputCursorFactory : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputCursorFactory=_uuidof(IInputCursorFactory);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCursorStatics[] = L"Microsoft.UI.Input.IInputCursorStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("92F6A552-099F-55FB-8C31-E450284C9643"), exclusiveto, contract] */
                MIDL_INTERFACE("92F6A552-099F-55FB-8C31-E450284C9643")
                IInputCursorStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE CreateFromCoreCursor(
                        /* [in] */ABI::Windows::UI::Core::ICoreCursor * cursor,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputCursor * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputCursorStatics=_uuidof(IInputCursorStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputCustomCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCustomCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCustomCursor[] = L"Microsoft.UI.Input.IInputCustomCursor";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("5486F042-7E1A-5DC8-8041-E47B609A5BA1"), exclusiveto, contract] */
                MIDL_INTERFACE("5486F042-7E1A-5DC8-8041-E47B609A5BA1")
                IInputCustomCursor : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputCustomCursor=_uuidof(IInputCustomCursor);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputCustomCursorFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCustomCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCustomCursorFactory[] = L"Microsoft.UI.Input.IInputCustomCursorFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("6F402882-66E0-57D3-89D0-AA5E2FF917BC"), exclusiveto, contract] */
                MIDL_INTERFACE("6F402882-66E0-57D3-89D0-AA5E2FF917BC")
                IInputCustomCursorFactory : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputCustomCursorFactory=_uuidof(IInputCustomCursorFactory);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopNamedResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopNamedResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopNamedResourceCursor[] = L"Microsoft.UI.Input.IInputDesktopNamedResourceCursor";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("F40EA93B-0ED7-5B3A-BFE2-14E2B5AD88A3"), exclusiveto, contract] */
                MIDL_INTERFACE("F40EA93B-0ED7-5B3A-BFE2-14E2B5AD88A3")
                IInputDesktopNamedResourceCursor : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ModuleName(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ResourceName(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputDesktopNamedResourceCursor=_uuidof(IInputDesktopNamedResourceCursor);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopNamedResourceCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopNamedResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopNamedResourceCursorStatics[] = L"Microsoft.UI.Input.IInputDesktopNamedResourceCursorStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("E8B6D5AA-898B-5E69-B01F-383A0943E3E4"), exclusiveto, contract] */
                MIDL_INTERFACE("E8B6D5AA-898B-5E69-B01F-383A0943E3E4")
                IInputDesktopNamedResourceCursorStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [in] */HSTRING resourceName,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputDesktopNamedResourceCursor * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateFromModule(
                        /* [in] */HSTRING moduleName,
                        /* [in] */HSTRING resourceName,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputDesktopNamedResourceCursor * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputDesktopNamedResourceCursorStatics=_uuidof(IInputDesktopNamedResourceCursorStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopResourceCursor[] = L"Microsoft.UI.Input.IInputDesktopResourceCursor";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("1DF2777F-7C90-58FC-A7A3-D5736C6510FD"), exclusiveto, contract] */
                MIDL_INTERFACE("1DF2777F-7C90-58FC-A7A3-D5736C6510FD")
                IInputDesktopResourceCursor : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ModuleName(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ResourceId(
                        /* [retval, out] */UINT32 * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputDesktopResourceCursor=_uuidof(IInputDesktopResourceCursor);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopResourceCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopResourceCursorStatics[] = L"Microsoft.UI.Input.IInputDesktopResourceCursorStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("F440DC37-A0B6-56EB-BCEC-B024F2233D47"), exclusiveto, contract] */
                MIDL_INTERFACE("F440DC37-A0B6-56EB-BCEC-B024F2233D47")
                IInputDesktopResourceCursorStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [in] */UINT32 resourceId,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputDesktopResourceCursor * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateFromModule(
                        /* [in] */HSTRING moduleName,
                        /* [in] */UINT32 resourceId,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputDesktopResourceCursor * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputDesktopResourceCursorStatics=_uuidof(IInputDesktopResourceCursorStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputKeyboardSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputKeyboardSource
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputKeyboardSource[] = L"Microsoft.UI.Input.IInputKeyboardSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("ED61B906-16AD-5DF7-A550-5E6F7D2229F7"), exclusiveto, contract] */
                MIDL_INTERFACE("ED61B906-16AD-5DF7-A550-5E6F7D2229F7")
                IInputKeyboardSource : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputKeyboardSource=_uuidof(IInputKeyboardSource);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputKeyboardSourceStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputKeyboardSource
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputKeyboardSourceStatics[] = L"Microsoft.UI.Input.IInputKeyboardSourceStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("F4E1563D-8C2E-5BCD-B784-47ADEAA3CD7E"), exclusiveto, contract] */
                MIDL_INTERFACE("F4E1563D-8C2E-5BCD-B784-47ADEAA3CD7E")
                IInputKeyboardSourceStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE GetKeyStateForCurrentThread(
                        /* [in] */ABI::Windows::System::VirtualKey virtualKey,
                        /* [retval, out] */ABI::Windows::UI::Core::CoreVirtualKeyStates * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputKeyboardSourceStatics=_uuidof(IInputKeyboardSourceStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputLightDismissAction
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputLightDismissAction
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputLightDismissAction[] = L"Microsoft.UI.Input.IInputLightDismissAction";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("E8A39502-A860-502F-8C10-3646D43AECF1"), exclusiveto, contract] */
                MIDL_INTERFACE("E8A39502-A860-502F-8C10-3646D43AECF1")
                IInputLightDismissAction : public IInspectable
                {
                public:
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Dismissed(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Dismissed(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputLightDismissAction=_uuidof(IInputLightDismissAction);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputLightDismissActionStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputLightDismissAction
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputLightDismissActionStatics[] = L"Microsoft.UI.Input.IInputLightDismissActionStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("ED9B8DEF-6496-5169-984D-D44B4E690623"), exclusiveto, contract] */
                MIDL_INTERFACE("ED9B8DEF-6496-5169-984D-D44B4E690623")
                IInputLightDismissActionStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE GetForWindowId(
                        /* [in] */ABI::Microsoft::UI::WindowId windowId,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputLightDismissAction * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputLightDismissActionStatics=_uuidof(IInputLightDismissActionStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputLightDismissEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputLightDismissEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputLightDismissEventArgs[] = L"Microsoft.UI.Input.IInputLightDismissEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("078660EE-07CA-5808-B982-E6E899CF098C"), exclusiveto, contract] */
                MIDL_INTERFACE("078660EE-07CA-5808-B982-E6E899CF098C")
                IInputLightDismissEventArgs : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputLightDismissEventArgs=_uuidof(IInputLightDismissEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputObject[] = L"Microsoft.UI.Input.IInputObject";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("42EDBC88-D386-544D-B1B8-68617FE68282"), exclusiveto, contract] */
                MIDL_INTERFACE("42EDBC88-D386-544D-B1B8-68617FE68282")
                IInputObject : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DispatcherQueue(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueue * * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputObject=_uuidof(IInputObject);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputObject;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputObjectFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputObjectFactory[] = L"Microsoft.UI.Input.IInputObjectFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("F7786BC2-B0B8-5961-9A57-AE199D452106"), exclusiveto, contract] */
                MIDL_INTERFACE("F7786BC2-B0B8-5961-9A57-AE199D452106")
                IInputObjectFactory : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IInputObjectFactory=_uuidof(IInputObjectFactory);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputPointerSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputPointerSource
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputPointerSource[] = L"Microsoft.UI.Input.IInputPointerSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("6A6C2764-C3F4-5BE5-8447-C9A98766C240"), exclusiveto, contract] */
                MIDL_INTERFACE("6A6C2764-C3F4-5BE5-8447-C9A98766C240")
                IInputPointerSource : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cursor(
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputCursor * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Cursor(
                        /* [in] */ABI::Microsoft::UI::Input::IInputCursor * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DeviceKinds(
                        /* [retval, out] */ABI::Microsoft::UI::Input::InputPointerSourceDeviceKinds * value
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerCaptureLost(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerCaptureLost(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerEntered(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerEntered(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerExited(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerExited(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerMoved(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerMoved(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerPressed(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerPressed(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerReleased(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerReleased(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerRoutedAway(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerRoutedAway(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerRoutedReleased(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerRoutedReleased(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerRoutedTo(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerRoutedTo(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PointerWheelChanged(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PointerWheelChanged(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputPointerSource=_uuidof(IInputPointerSource);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputSystemCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputSystemCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputSystemCursor[] = L"Microsoft.UI.Input.IInputSystemCursor";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("59F538E7-C500-59AB-8B54-0BC6100FD49E"), exclusiveto, contract] */
                MIDL_INTERFACE("59F538E7-C500-59AB-8B54-0BC6100FD49E")
                IInputSystemCursor : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CursorShape(
                        /* [retval, out] */ABI::Microsoft::UI::Input::InputSystemCursorShape * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputSystemCursor=_uuidof(IInputSystemCursor);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputSystemCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputSystemCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputSystemCursorStatics[] = L"Microsoft.UI.Input.IInputSystemCursorStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("D3860BB6-698A-5814-AEDD-C2FA8BBA5A02"), exclusiveto, contract] */
                MIDL_INTERFACE("D3860BB6-698A-5814-AEDD-C2FA8BBA5A02")
                IInputSystemCursorStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE Create(
                        /* [in] */ABI::Microsoft::UI::Input::InputSystemCursorShape type,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IInputSystemCursor * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IInputSystemCursorStatics=_uuidof(IInputSystemCursorStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationCompletedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationCompletedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationCompletedEventArgs[] = L"Microsoft.UI.Input.IManipulationCompletedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("0E0249D4-46E4-5559-AEE3-FA45CE2A7F56"), exclusiveto, contract] */
                MIDL_INTERFACE("0E0249D4-46E4-5559-AEE3-FA45CE2A7F56")
                IManipulationCompletedEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cumulative(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationDelta * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Velocities(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationVelocities * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IManipulationCompletedEventArgs=_uuidof(IManipulationCompletedEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationInertiaStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationInertiaStartingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationInertiaStartingEventArgs[] = L"Microsoft.UI.Input.IManipulationInertiaStartingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("ACF9EF71-6E15-56AB-9260-F0D3CE5F66E8"), exclusiveto, contract] */
                MIDL_INTERFACE("ACF9EF71-6E15-56AB-9260-F0D3CE5F66E8")
                IManipulationInertiaStartingEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cumulative(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationDelta * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Delta(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationDelta * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Velocities(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationVelocities * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IManipulationInertiaStartingEventArgs=_uuidof(IManipulationInertiaStartingEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationStartedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationStartedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationStartedEventArgs[] = L"Microsoft.UI.Input.IManipulationStartedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("4A616613-EEF1-5F1B-A768-0775478D49D4"), exclusiveto, contract] */
                MIDL_INTERFACE("4A616613-EEF1-5F1B-A768-0775478D49D4")
                IManipulationStartedEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cumulative(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationDelta * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IManipulationStartedEventArgs=_uuidof(IManipulationStartedEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationUpdatedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationUpdatedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationUpdatedEventArgs[] = L"Microsoft.UI.Input.IManipulationUpdatedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("406E1961-0C98-5FC0-B3D8-116492EF0053"), exclusiveto, contract] */
                MIDL_INTERFACE("406E1961-0C98-5FC0-B3D8-116492EF0053")
                IManipulationUpdatedEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cumulative(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationDelta * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Delta(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationDelta * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Velocities(
                        /* [retval, out] */ABI::Microsoft::UI::Input::ManipulationVelocities * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IManipulationUpdatedEventArgs=_uuidof(IManipulationUpdatedEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IMouseWheelParameters
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.MouseWheelParameters
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IMouseWheelParameters[] = L"Microsoft.UI.Input.IMouseWheelParameters";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("6D98BE40-1D56-51D1-AA0D-F325439CD009"), exclusiveto, contract] */
                MIDL_INTERFACE("6D98BE40-1D56-51D1-AA0D-F325439CD009")
                IMouseWheelParameters : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CharTranslation(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CharTranslation(
                        /* [in] */ABI::Windows::Foundation::Point value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DeltaScale(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DeltaScale(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DeltaRotationAngle(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DeltaRotationAngle(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PageTranslation(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PageTranslation(
                        /* [in] */ABI::Windows::Foundation::Point value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IMouseWheelParameters=_uuidof(IMouseWheelParameters);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerEventArgs[] = L"Microsoft.UI.Input.IPointerEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("865B188C-2ED5-5DF8-829F-AC0701D5C51A"), exclusiveto, contract] */
                MIDL_INTERFACE("865B188C-2ED5-5DF8-829F-AC0701D5C51A")
                IPointerEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CurrentPoint(
                        /* [retval, out] */ABI::Microsoft::UI::Input::IPointerPoint * * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Handled(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Handled(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_KeyModifiers(
                        /* [retval, out] */ABI::Windows::System::VirtualKeyModifiers * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetIntermediatePoints(
                        /* [retval, out] */__FIVector_1_Microsoft__CUI__CInput__CPointerPoint * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetIntermediateTransformedPoints(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPointTransform * transform,
                        /* [retval, out] */__FIVector_1_Microsoft__CUI__CInput__CPointerPoint * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IPointerEventArgs=_uuidof(IPointerEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPoint
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPoint
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPoint[] = L"Microsoft.UI.Input.IPointerPoint";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("0D430EE6-252C-59A4-B2A2-D44264DC6A40"), exclusiveto, contract] */
                MIDL_INTERFACE("0D430EE6-252C-59A4-B2A2-D44264DC6A40")
                IPointerPoint : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FrameId(
                        /* [retval, out] */UINT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsInContact(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerId(
                        /* [retval, out] */UINT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Properties(
                        /* [retval, out] */ABI::Microsoft::UI::Input::IPointerPointProperties * * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Timestamp(
                        /* [retval, out] */UINT64 * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetTransformedPoint(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPointTransform * transform,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IPointerPoint * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IPointerPoint=_uuidof(IPointerPoint);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPointProperties
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPointProperties
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPointProperties[] = L"Microsoft.UI.Input.IPointerPointProperties";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("D760ED77-4B10-57A5-B3CC-D9BF3413E996"), exclusiveto, contract] */
                MIDL_INTERFACE("D760ED77-4B10-57A5-B3CC-D9BF3413E996")
                IPointerPointProperties : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ContactRect(
                        /* [retval, out] */ABI::Windows::Foundation::Rect * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsBarrelButtonPressed(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsCanceled(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsEraser(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsHorizontalMouseWheel(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsInRange(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsInverted(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsLeftButtonPressed(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsMiddleButtonPressed(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsPrimary(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsRightButtonPressed(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsXButton1Pressed(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsXButton2Pressed(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MouseWheelDelta(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Orientation(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerUpdateKind(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerUpdateKind * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Pressure(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TouchConfidence(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Twist(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_XTilt(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_YTilt(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IPointerPointProperties=_uuidof(IPointerPointProperties);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPointTransform
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPointTransform[] = L"Microsoft.UI.Input.IPointerPointTransform";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("DB4791BC-994D-54C7-92EF-66EA1DE9B43C"), contract] */
                MIDL_INTERFACE("DB4791BC-994D-54C7-92EF-66EA1DE9B43C")
                IPointerPointTransform : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Inverse(
                        /* [retval, out] */ABI::Microsoft::UI::Input::IPointerPointTransform * * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE TryTransform(
                        /* [in] */ABI::Windows::Foundation::Point inPoint,
                        /* [out] */ABI::Windows::Foundation::Point * outPoint,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE TryTransformBounds(
                        /* [in] */ABI::Windows::Foundation::Rect inRect,
                        /* [out] */ABI::Windows::Foundation::Rect * outRect,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IPointerPointTransform=_uuidof(IPointerPointTransform);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPredictor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPredictor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPredictor[] = L"Microsoft.UI.Input.IPointerPredictor";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("12C100EC-2100-565F-A60C-F1187F438828"), exclusiveto, contract] */
                MIDL_INTERFACE("12C100EC-2100-565F-A60C-F1187F438828")
                IPointerPredictor : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PredictionTime(
                        /* [retval, out] */ABI::Windows::Foundation::TimeSpan * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PredictionTime(
                        /* [in] */ABI::Windows::Foundation::TimeSpan value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetPredictedPoints(
                        /* [in] */ABI::Microsoft::UI::Input::IPointerPoint * point,
                        /* [out] */UINT32 * __resultSize,
                        /* [size_is(, *(__resultSize)), retval, out] */ABI::Microsoft::UI::Input::IPointerPoint * * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IPointerPredictor=_uuidof(IPointerPredictor);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPredictorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPredictor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPredictorStatics[] = L"Microsoft.UI.Input.IPointerPredictorStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("78A8EF30-3E5C-55CD-8F85-65AC09B1A987"), exclusiveto, contract] */
                MIDL_INTERFACE("78A8EF30-3E5C-55CD-8F85-65AC09B1A987")
                IPointerPredictorStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE CreateForInputPointerSource(
                        /* [in] */ABI::Microsoft::UI::Input::IInputPointerSource * inputPointerSource,
                        /* [retval, out] */ABI::Microsoft::UI::Input::IPointerPredictor * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IPointerPredictorStatics=_uuidof(IPointerPredictorStatics);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IRightTappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.RightTappedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IRightTappedEventArgs[] = L"Microsoft.UI.Input.IRightTappedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("8FF73B39-887E-50A4-8500-77953039DCB4"), exclusiveto, contract] */
                MIDL_INTERFACE("8FF73B39-887E-50A4-8500-77953039DCB4")
                IRightTappedEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IRightTappedEventArgs=_uuidof(IRightTappedEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.ITappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.TappedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_ITappedEventArgs[] = L"Microsoft.UI.Input.ITappedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Input {
                /* [object, uuid("C3A01BB5-6076-5E0F-871A-9D94A6A8F82B"), exclusiveto, contract] */
                MIDL_INTERFACE("C3A01BB5-6076-5E0F-871A-9D94A6A8F82B")
                ITappedEventArgs : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointerDeviceType(
                        /* [retval, out] */ABI::Microsoft::UI::Input::PointerDeviceType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TapCount(
                        /* [retval, out] */UINT32 * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITappedEventArgs=_uuidof(ITappedEventArgs);
                
            } /* Input */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.CrossSlidingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.ICrossSlidingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_CrossSlidingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_CrossSlidingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_CrossSlidingEventArgs[] = L"Microsoft.UI.Input.CrossSlidingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.DraggingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IDraggingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_DraggingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_DraggingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_DraggingEventArgs[] = L"Microsoft.UI.Input.DraggingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.GestureRecognizer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IGestureRecognizer ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_GestureRecognizer_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_GestureRecognizer_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_GestureRecognizer[] = L"Microsoft.UI.Input.GestureRecognizer";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.HoldingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IHoldingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_HoldingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_HoldingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_HoldingEventArgs[] = L"Microsoft.UI.Input.HoldingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputActivationListener
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputActivationListenerStatics interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputActivationListener ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputActivationListener_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputActivationListener_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputActivationListener[] = L"Microsoft.UI.Input.InputActivationListener";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputActivationListenerActivationChangedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputActivationListenerActivationChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputActivationListenerActivationChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputActivationListenerActivationChangedEventArgs[] = L"Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputCursorStatics interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputCursor ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputCursor[] = L"Microsoft.UI.Input.InputCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputCustomCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputCustomCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputCustomCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputCustomCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputCustomCursor[] = L"Microsoft.UI.Input.InputCustomCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputDesktopNamedResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputDesktopNamedResourceCursorStatics interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputDesktopNamedResourceCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputDesktopNamedResourceCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputDesktopNamedResourceCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputDesktopNamedResourceCursor[] = L"Microsoft.UI.Input.InputDesktopNamedResourceCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputDesktopResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputDesktopResourceCursorStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputDesktopResourceCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputDesktopResourceCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputDesktopResourceCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputDesktopResourceCursor[] = L"Microsoft.UI.Input.InputDesktopResourceCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputKeyboardSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputKeyboardSourceStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputKeyboardSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputKeyboardSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputKeyboardSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputKeyboardSource[] = L"Microsoft.UI.Input.InputKeyboardSource";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputLightDismissAction
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputLightDismissActionStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputLightDismissAction ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissAction_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissAction_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputLightDismissAction[] = L"Microsoft.UI.Input.InputLightDismissAction";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputLightDismissEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputLightDismissEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputLightDismissEventArgs[] = L"Microsoft.UI.Input.InputLightDismissEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputObject ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputObject_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputObject_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputObject[] = L"Microsoft.UI.Input.InputObject";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputPointerSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputPointerSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputPointerSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputPointerSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputPointerSource[] = L"Microsoft.UI.Input.InputPointerSource";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputSystemCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputSystemCursorStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputSystemCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputSystemCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputSystemCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputSystemCursor[] = L"Microsoft.UI.Input.InputSystemCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationCompletedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationCompletedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationCompletedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationCompletedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationCompletedEventArgs[] = L"Microsoft.UI.Input.ManipulationCompletedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationInertiaStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationInertiaStartingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationInertiaStartingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationInertiaStartingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationInertiaStartingEventArgs[] = L"Microsoft.UI.Input.ManipulationInertiaStartingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationStartedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationStartedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationStartedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationStartedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationStartedEventArgs[] = L"Microsoft.UI.Input.ManipulationStartedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationUpdatedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationUpdatedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationUpdatedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationUpdatedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationUpdatedEventArgs[] = L"Microsoft.UI.Input.ManipulationUpdatedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.MouseWheelParameters
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IMouseWheelParameters ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_MouseWheelParameters_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_MouseWheelParameters_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_MouseWheelParameters[] = L"Microsoft.UI.Input.MouseWheelParameters";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerEventArgs[] = L"Microsoft.UI.Input.PointerEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerPoint
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerPoint ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerPoint_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerPoint_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerPoint[] = L"Microsoft.UI.Input.PointerPoint";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerPointProperties
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerPointProperties ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerPointProperties_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerPointProperties_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerPointProperties[] = L"Microsoft.UI.Input.PointerPointProperties";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerPredictor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IPointerPredictorStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerPredictor ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerPredictor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerPredictor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerPredictor[] = L"Microsoft.UI.Input.PointerPredictor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.RightTappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IRightTappedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_RightTappedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_RightTappedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_RightTappedEventArgs[] = L"Microsoft.UI.Input.RightTappedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.TappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.ITappedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_TappedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_TappedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_TappedEventArgs[] = L"Microsoft.UI.Input.TappedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000




#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputObject __x_ABI_CMicrosoft_CUI_CInput_CIInputObject;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__)
#define ____FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Microsoft__CUI__CInput__CPointerPoint;

typedef struct __FIIterator_1_Microsoft__CUI__CInput__CPointerPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Microsoft__CUI__CInput__CPointerPointVtbl;

interface __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint
{
    CONST_VTBL struct __FIIterator_1_Microsoft__CUI__CInput__CPointerPointVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__)
#define ____FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Microsoft__CUI__CInput__CPointerPoint;

typedef  struct __FIIterable_1_Microsoft__CUI__CInput__CPointerPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Microsoft__CUI__CInput__CPointerPoint **first);

    END_INTERFACE
} __FIIterable_1_Microsoft__CUI__CInput__CPointerPointVtbl;

interface __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint
{
    CONST_VTBL struct __FIIterable_1_Microsoft__CUI__CInput__CPointerPointVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint;

typedef struct __FIVectorView_1_Microsoft__CUI__CInput__CPointerPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
            /* [in] */ __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Microsoft__CUI__CInput__CPointerPointVtbl;

interface __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint
{
    CONST_VTBL struct __FIVectorView_1_Microsoft__CUI__CInput__CPointerPointVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVector_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__)
#define ____FIVector_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

typedef interface __FIVector_1_Microsoft__CUI__CInput__CPointerPoint __FIVector_1_Microsoft__CUI__CInput__CPointerPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_Microsoft__CUI__CInput__CPointerPoint;

typedef struct __FIVector_1_Microsoft__CUI__CInput__CPointerPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_Microsoft__CUI__CInput__CPointerPoint **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This, /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_Microsoft__CUI__CInput__CPointerPoint * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * *value);

    END_INTERFACE
} __FIVector_1_Microsoft__CUI__CInput__CPointerPointVtbl;

interface __FIVector_1_Microsoft__CUI__CInput__CPointerPoint
{
    CONST_VTBL struct __FIVector_1_Microsoft__CUI__CInput__CPointerPointVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_Microsoft__CUI__CInput__CPointerPoint_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_Microsoft__CUI__CInput__CPointerPoint_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000





#ifndef ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue __x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue;

#endif // ____x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue_FWD_DEFINED__






typedef struct __x_ABI_CMicrosoft_CUI_CWindowId __x_ABI_CMicrosoft_CUI_CWindowId;



#ifndef ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIClosable __x_ABI_CWindows_CFoundation_CIClosable;

#endif // ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__



typedef struct __x_ABI_CWindows_CFoundation_CPoint __x_ABI_CWindows_CFoundation_CPoint;


typedef struct __x_ABI_CWindows_CFoundation_CRect __x_ABI_CWindows_CFoundation_CRect;


typedef struct __x_ABI_CWindows_CFoundation_CTimeSpan __x_ABI_CWindows_CFoundation_CTimeSpan;




typedef enum __x_ABI_CWindows_CSystem_CVirtualKey __x_ABI_CWindows_CSystem_CVirtualKey;


typedef enum __x_ABI_CWindows_CSystem_CVirtualKeyModifiers __x_ABI_CWindows_CSystem_CVirtualKeyModifiers;



#ifndef ____x_ABI_CWindows_CUI_CCore_CICoreCursor_FWD_DEFINED__
#define ____x_ABI_CWindows_CUI_CCore_CICoreCursor_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CUI_CCore_CICoreCursor __x_ABI_CWindows_CUI_CCore_CICoreCursor;

#endif // ____x_ABI_CWindows_CUI_CCore_CICoreCursor_FWD_DEFINED__



typedef enum __x_ABI_CWindows_CUI_CCore_CCoreVirtualKeyStates __x_ABI_CWindows_CUI_CCore_CCoreVirtualKeyStates;





typedef enum __x_ABI_CMicrosoft_CUI_CInput_CCrossSlidingState __x_ABI_CMicrosoft_CUI_CInput_CCrossSlidingState;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CDraggingState __x_ABI_CMicrosoft_CUI_CInput_CDraggingState;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CGestureSettings __x_ABI_CMicrosoft_CUI_CInput_CGestureSettings;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CHoldingState __x_ABI_CMicrosoft_CUI_CInput_CHoldingState;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CInputActivationState __x_ABI_CMicrosoft_CUI_CInput_CInputActivationState;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CInputPointerSourceDeviceKinds __x_ABI_CMicrosoft_CUI_CInput_CInputPointerSourceDeviceKinds;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CInputSystemCursorShape __x_ABI_CMicrosoft_CUI_CInput_CInputSystemCursorShape;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType __x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType;


typedef enum __x_ABI_CMicrosoft_CUI_CInput_CPointerUpdateKind __x_ABI_CMicrosoft_CUI_CInput_CPointerUpdateKind;


typedef struct __x_ABI_CMicrosoft_CUI_CInput_CCrossSlideThresholds __x_ABI_CMicrosoft_CUI_CInput_CCrossSlideThresholds;


typedef struct __x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta __x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta;


typedef struct __x_ABI_CMicrosoft_CUI_CInput_CManipulationVelocities __x_ABI_CMicrosoft_CUI_CInput_CManipulationVelocities;











































































/*
 *
 * Struct Microsoft.UI.Input.CrossSlidingState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CCrossSlidingState
{
    CrossSlidingState_Started = 0,
    CrossSlidingState_Dragging = 1,
    CrossSlidingState_Selecting = 2,
    CrossSlidingState_SelectSpeedBumping = 3,
    CrossSlidingState_SpeedBumping = 4,
    CrossSlidingState_Rearranging = 5,
    CrossSlidingState_Completed = 6,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.DraggingState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CDraggingState
{
    DraggingState_Started = 0,
    DraggingState_Continuing = 1,
    DraggingState_Completed = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.GestureSettings
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, flags, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CGestureSettings
{
    GestureSettings_None = 0,
    GestureSettings_Tap = 0x1,
    GestureSettings_DoubleTap = 0x2,
    GestureSettings_Hold = 0x4,
    GestureSettings_HoldWithMouse = 0x8,
    GestureSettings_RightTap = 0x10,
    GestureSettings_Drag = 0x20,
    GestureSettings_ManipulationTranslateX = 0x40,
    GestureSettings_ManipulationTranslateY = 0x80,
    GestureSettings_ManipulationTranslateRailsX = 0x100,
    GestureSettings_ManipulationTranslateRailsY = 0x200,
    GestureSettings_ManipulationRotate = 0x400,
    GestureSettings_ManipulationScale = 0x800,
    GestureSettings_ManipulationTranslateInertia = 0x1000,
    GestureSettings_ManipulationRotateInertia = 0x2000,
    GestureSettings_ManipulationScaleInertia = 0x4000,
    GestureSettings_CrossSlide = 0x8000,
    GestureSettings_ManipulationMultipleFingerPanning = 0x10000,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.HoldingState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CHoldingState
{
    HoldingState_Started = 0,
    HoldingState_Completed = 1,
    HoldingState_Canceled = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.InputActivationState
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CInputActivationState
{
    InputActivationState_None = 0,
    InputActivationState_Deactivated = 1,
    InputActivationState_Activated = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Struct Microsoft.UI.Input.InputPointerSourceDeviceKinds
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, flags, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CInputPointerSourceDeviceKinds
{
    InputPointerSourceDeviceKinds_None = 0,
    InputPointerSourceDeviceKinds_Touch = 0x1,
    InputPointerSourceDeviceKinds_Pen = 0x2,
    InputPointerSourceDeviceKinds_Mouse = 0x4,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.InputSystemCursorShape
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CInputSystemCursorShape
{
    InputSystemCursorShape_Arrow = 0,
    InputSystemCursorShape_Cross = 1,
    InputSystemCursorShape_Hand = 3,
    InputSystemCursorShape_Help = 4,
    InputSystemCursorShape_IBeam = 5,
    InputSystemCursorShape_SizeAll = 6,
    InputSystemCursorShape_SizeNortheastSouthwest = 7,
    InputSystemCursorShape_SizeNorthSouth = 8,
    InputSystemCursorShape_SizeNorthwestSoutheast = 9,
    InputSystemCursorShape_SizeWestEast = 10,
    InputSystemCursorShape_UniversalNo = 11,
    InputSystemCursorShape_UpArrow = 12,
    InputSystemCursorShape_Wait = 13,
    InputSystemCursorShape_Pin = 14,
    InputSystemCursorShape_Person = 15,
    InputSystemCursorShape_AppStarting = 16,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.PointerDeviceType
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType
{
    PointerDeviceType_Touch = 0,
    PointerDeviceType_Pen = 1,
    PointerDeviceType_Mouse = 2,
    PointerDeviceType_Touchpad = 3,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.PointerUpdateKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CInput_CPointerUpdateKind
{
    PointerUpdateKind_Other = 0,
    PointerUpdateKind_LeftButtonPressed = 1,
    PointerUpdateKind_LeftButtonReleased = 2,
    PointerUpdateKind_RightButtonPressed = 3,
    PointerUpdateKind_RightButtonReleased = 4,
    PointerUpdateKind_MiddleButtonPressed = 5,
    PointerUpdateKind_MiddleButtonReleased = 6,
    PointerUpdateKind_XButton1Pressed = 7,
    PointerUpdateKind_XButton1Released = 8,
    PointerUpdateKind_XButton2Pressed = 9,
    PointerUpdateKind_XButton2Released = 10,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.CrossSlideThresholds
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CInput_CCrossSlideThresholds
{
    FLOAT SelectionStart;
    FLOAT SpeedBumpStart;
    FLOAT SpeedBumpEnd;
    FLOAT RearrangeStart;
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.ManipulationDelta
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta
{
    __x_ABI_CWindows_CFoundation_CPoint Translation;
    FLOAT Scale;
    FLOAT Rotation;
    FLOAT Expansion;
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Input.ManipulationVelocities
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CInput_CManipulationVelocities
{
    __x_ABI_CWindows_CFoundation_CPoint Linear;
    FLOAT Angular;
    FLOAT Expansion;
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.ICrossSlidingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.CrossSlidingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_ICrossSlidingEventArgs[] = L"Microsoft.UI.Input.ICrossSlidingEventArgs";
/* [object, uuid("7679641F-BA9F-543C-A7C8-6229A98F89EF"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CrossSlidingState )(
        __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CCrossSlidingState * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_get_CrossSlidingState(This,value) \
    ( (This)->lpVtbl->get_CrossSlidingState(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CICrossSlidingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IDraggingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.DraggingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IDraggingEventArgs[] = L"Microsoft.UI.Input.IDraggingEventArgs";
/* [object, uuid("3EFB1B75-3D3B-550E-963D-0828CA76128A"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DraggingState )(
        __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CDraggingState * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_get_DraggingState(This,value) \
    ( (This)->lpVtbl->get_DraggingState(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIDraggingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IGestureRecognizer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.GestureRecognizer
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IGestureRecognizer[] = L"Microsoft.UI.Input.IGestureRecognizer";
/* [object, uuid("CDA89AFC-6BD0-595C-BA37-545FCE5BF016"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AutoProcessInertia )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AutoProcessInertia )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CrossSlideExact )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CrossSlideExact )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CrossSlideHorizontally )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CrossSlideHorizontally )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CrossSlideThresholds )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CCrossSlideThresholds * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CrossSlideThresholds )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CCrossSlideThresholds value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_GestureSettings )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CGestureSettings * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_GestureSettings )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CGestureSettings value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsActive )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsInertial )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PivotCenter )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PivotCenter )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PivotRadius )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PivotRadius )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InertiaExpansionDeceleration )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InertiaExpansionDeceleration )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InertiaExpansion )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InertiaExpansion )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InertiaRotationAngle )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InertiaRotationAngle )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InertiaRotationDeceleration )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InertiaRotationDeceleration )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InertiaTranslationDeceleration )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InertiaTranslationDeceleration )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InertiaTranslationDisplacement )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InertiaTranslationDisplacement )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ManipulationExact )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ManipulationExact )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MouseWheelParameters )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ShowGestureFeedback )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ShowGestureFeedback )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */boolean value
        );
    HRESULT ( STDMETHODCALLTYPE *CanBeDoubleTap )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * value,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *CompleteGesture )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This
        );
    HRESULT ( STDMETHODCALLTYPE *ProcessDownEvent )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * value
        );
    HRESULT ( STDMETHODCALLTYPE *ProcessMoveEvents )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FIVector_1_Microsoft__CUI__CInput__CPointerPoint * value
        );
    HRESULT ( STDMETHODCALLTYPE *ProcessMouseWheelEvent )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * value,
        /* [in] */boolean isShiftKeyDown,
        /* [in] */boolean isControlKeyDown
        );
    HRESULT ( STDMETHODCALLTYPE *ProcessInertia )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This
        );
    HRESULT ( STDMETHODCALLTYPE *ProcessUpEvent )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Tapped )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CTappedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Tapped )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_RightTapped )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CRightTappedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_RightTapped )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Holding )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CHoldingEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Holding )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Dragging )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CDraggingEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Dragging )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ManipulationStarted )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationStartedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ManipulationStarted )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ManipulationUpdated )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationUpdatedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ManipulationUpdated )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ManipulationInertiaStarting )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationInertiaStartingEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ManipulationInertiaStarting )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ManipulationCompleted )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CManipulationCompletedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ManipulationCompleted )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_CrossSliding )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CGestureRecognizer_Microsoft__CUI__CInput__CCrossSlidingEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_CrossSliding )(
        __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizerVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_AutoProcessInertia(This,value) \
    ( (This)->lpVtbl->get_AutoProcessInertia(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_AutoProcessInertia(This,value) \
    ( (This)->lpVtbl->put_AutoProcessInertia(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_CrossSlideExact(This,value) \
    ( (This)->lpVtbl->get_CrossSlideExact(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_CrossSlideExact(This,value) \
    ( (This)->lpVtbl->put_CrossSlideExact(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_CrossSlideHorizontally(This,value) \
    ( (This)->lpVtbl->get_CrossSlideHorizontally(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_CrossSlideHorizontally(This,value) \
    ( (This)->lpVtbl->put_CrossSlideHorizontally(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_CrossSlideThresholds(This,value) \
    ( (This)->lpVtbl->get_CrossSlideThresholds(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_CrossSlideThresholds(This,value) \
    ( (This)->lpVtbl->put_CrossSlideThresholds(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_GestureSettings(This,value) \
    ( (This)->lpVtbl->get_GestureSettings(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_GestureSettings(This,value) \
    ( (This)->lpVtbl->put_GestureSettings(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_IsActive(This,value) \
    ( (This)->lpVtbl->get_IsActive(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_IsInertial(This,value) \
    ( (This)->lpVtbl->get_IsInertial(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_PivotCenter(This,value) \
    ( (This)->lpVtbl->get_PivotCenter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_PivotCenter(This,value) \
    ( (This)->lpVtbl->put_PivotCenter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_PivotRadius(This,value) \
    ( (This)->lpVtbl->get_PivotRadius(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_PivotRadius(This,value) \
    ( (This)->lpVtbl->put_PivotRadius(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_InertiaExpansionDeceleration(This,value) \
    ( (This)->lpVtbl->get_InertiaExpansionDeceleration(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_InertiaExpansionDeceleration(This,value) \
    ( (This)->lpVtbl->put_InertiaExpansionDeceleration(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_InertiaExpansion(This,value) \
    ( (This)->lpVtbl->get_InertiaExpansion(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_InertiaExpansion(This,value) \
    ( (This)->lpVtbl->put_InertiaExpansion(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_InertiaRotationAngle(This,value) \
    ( (This)->lpVtbl->get_InertiaRotationAngle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_InertiaRotationAngle(This,value) \
    ( (This)->lpVtbl->put_InertiaRotationAngle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_InertiaRotationDeceleration(This,value) \
    ( (This)->lpVtbl->get_InertiaRotationDeceleration(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_InertiaRotationDeceleration(This,value) \
    ( (This)->lpVtbl->put_InertiaRotationDeceleration(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_InertiaTranslationDeceleration(This,value) \
    ( (This)->lpVtbl->get_InertiaTranslationDeceleration(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_InertiaTranslationDeceleration(This,value) \
    ( (This)->lpVtbl->put_InertiaTranslationDeceleration(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_InertiaTranslationDisplacement(This,value) \
    ( (This)->lpVtbl->get_InertiaTranslationDisplacement(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_InertiaTranslationDisplacement(This,value) \
    ( (This)->lpVtbl->put_InertiaTranslationDisplacement(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_ManipulationExact(This,value) \
    ( (This)->lpVtbl->get_ManipulationExact(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_ManipulationExact(This,value) \
    ( (This)->lpVtbl->put_ManipulationExact(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_MouseWheelParameters(This,value) \
    ( (This)->lpVtbl->get_MouseWheelParameters(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_get_ShowGestureFeedback(This,value) \
    ( (This)->lpVtbl->get_ShowGestureFeedback(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_put_ShowGestureFeedback(This,value) \
    ( (This)->lpVtbl->put_ShowGestureFeedback(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_CanBeDoubleTap(This,value,result) \
    ( (This)->lpVtbl->CanBeDoubleTap(This,value,result) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_CompleteGesture(This) \
    ( (This)->lpVtbl->CompleteGesture(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_ProcessDownEvent(This,value) \
    ( (This)->lpVtbl->ProcessDownEvent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_ProcessMoveEvents(This,value) \
    ( (This)->lpVtbl->ProcessMoveEvents(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_ProcessMouseWheelEvent(This,value,isShiftKeyDown,isControlKeyDown) \
    ( (This)->lpVtbl->ProcessMouseWheelEvent(This,value,isShiftKeyDown,isControlKeyDown) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_ProcessInertia(This) \
    ( (This)->lpVtbl->ProcessInertia(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_ProcessUpEvent(This,value) \
    ( (This)->lpVtbl->ProcessUpEvent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_Tapped(This,handler,token) \
    ( (This)->lpVtbl->add_Tapped(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_Tapped(This,token) \
    ( (This)->lpVtbl->remove_Tapped(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_RightTapped(This,handler,token) \
    ( (This)->lpVtbl->add_RightTapped(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_RightTapped(This,token) \
    ( (This)->lpVtbl->remove_RightTapped(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_Holding(This,handler,token) \
    ( (This)->lpVtbl->add_Holding(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_Holding(This,token) \
    ( (This)->lpVtbl->remove_Holding(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_Dragging(This,handler,token) \
    ( (This)->lpVtbl->add_Dragging(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_Dragging(This,token) \
    ( (This)->lpVtbl->remove_Dragging(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_ManipulationStarted(This,handler,token) \
    ( (This)->lpVtbl->add_ManipulationStarted(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_ManipulationStarted(This,token) \
    ( (This)->lpVtbl->remove_ManipulationStarted(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_ManipulationUpdated(This,handler,token) \
    ( (This)->lpVtbl->add_ManipulationUpdated(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_ManipulationUpdated(This,token) \
    ( (This)->lpVtbl->remove_ManipulationUpdated(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_ManipulationInertiaStarting(This,handler,token) \
    ( (This)->lpVtbl->add_ManipulationInertiaStarting(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_ManipulationInertiaStarting(This,token) \
    ( (This)->lpVtbl->remove_ManipulationInertiaStarting(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_ManipulationCompleted(This,handler,token) \
    ( (This)->lpVtbl->add_ManipulationCompleted(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_ManipulationCompleted(This,token) \
    ( (This)->lpVtbl->remove_ManipulationCompleted(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_add_CrossSliding(This,handler,token) \
    ( (This)->lpVtbl->add_CrossSliding(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_remove_CrossSliding(This,token) \
    ( (This)->lpVtbl->remove_CrossSliding(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIGestureRecognizer_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IHoldingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.HoldingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IHoldingEventArgs[] = L"Microsoft.UI.Input.IHoldingEventArgs";
/* [object, uuid("8E449E85-D223-533C-B0B2-BF7C6D10C2DB"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HoldingState )(
        __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CHoldingState * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_get_HoldingState(This,value) \
    ( (This)->lpVtbl->get_HoldingState(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIHoldingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputActivationListener
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputActivationListener
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputActivationListener[] = L"Microsoft.UI.Input.IInputActivationListener";
/* [object, uuid("3B818627-6CE7-5E0D-A0F5-6684FD1AEC78"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_State )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CInputActivationState * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_InputActivationChanged )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputActivationListener_Microsoft__CUI__CInput__CInputActivationListenerActivationChangedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_InputActivationChanged )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_get_State(This,value) \
    ( (This)->lpVtbl->get_State(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_add_InputActivationChanged(This,handler,token) \
    ( (This)->lpVtbl->add_InputActivationChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_remove_InputActivationChanged(This,token) \
    ( (This)->lpVtbl->remove_InputActivationChanged(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputActivationListenerActivationChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputActivationListenerActivationChangedEventArgs[] = L"Microsoft.UI.Input.IInputActivationListenerActivationChangedEventArgs";
/* [object, uuid("7978526B-00B6-5303-8F7D-55BEF36DA786"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerActivationChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputActivationListenerStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputActivationListener
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputActivationListenerStatics[] = L"Microsoft.UI.Input.IInputActivationListenerStatics";
/* [object, uuid("C4249843-F053-5C99-9D51-720ADE94224D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetForWindowId )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListener * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_GetForWindowId(This,windowId,result) \
    ( (This)->lpVtbl->GetForWindowId(This,windowId,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputActivationListenerStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCursor[] = L"Microsoft.UI.Input.IInputCursor";
/* [object, uuid("359B15F9-19C2-5714-8432-75176826406B"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputCursorFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCursorFactory[] = L"Microsoft.UI.Input.IInputCursorFactory";
/* [object, uuid("2F47647B-4BE0-53E9-BE7E-C38D5459DB6B"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCursorStatics[] = L"Microsoft.UI.Input.IInputCursorStatics";
/* [object, uuid("92F6A552-099F-55FB-8C31-E450284C9643"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateFromCoreCursor )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics * This,
        /* [in] */__x_ABI_CWindows_CUI_CCore_CICoreCursor * cursor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_CreateFromCoreCursor(This,cursor,result) \
    ( (This)->lpVtbl->CreateFromCoreCursor(This,cursor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputCustomCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCustomCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCustomCursor[] = L"Microsoft.UI.Input.IInputCustomCursor";
/* [object, uuid("5486F042-7E1A-5DC8-8041-E47B609A5BA1"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputCustomCursorFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputCustomCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputCustomCursorFactory[] = L"Microsoft.UI.Input.IInputCustomCursorFactory";
/* [object, uuid("6F402882-66E0-57D3-89D0-AA5E2FF917BC"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputCustomCursorFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopNamedResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopNamedResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopNamedResourceCursor[] = L"Microsoft.UI.Input.IInputDesktopNamedResourceCursor";
/* [object, uuid("F40EA93B-0ED7-5B3A-BFE2-14E2B5AD88A3"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ModuleName )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ResourceName )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * This,
        /* [retval, out] */HSTRING * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_get_ModuleName(This,value) \
    ( (This)->lpVtbl->get_ModuleName(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_get_ResourceName(This,value) \
    ( (This)->lpVtbl->get_ResourceName(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopNamedResourceCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopNamedResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopNamedResourceCursorStatics[] = L"Microsoft.UI.Input.IInputDesktopNamedResourceCursorStatics";
/* [object, uuid("E8B6D5AA-898B-5E69-B01F-383A0943E3E4"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This,
        /* [in] */HSTRING resourceName,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateFromModule )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics * This,
        /* [in] */HSTRING moduleName,
        /* [in] */HSTRING resourceName,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursor * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_Create(This,resourceName,result) \
    ( (This)->lpVtbl->Create(This,resourceName,result) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_CreateFromModule(This,moduleName,resourceName,result) \
    ( (This)->lpVtbl->CreateFromModule(This,moduleName,resourceName,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopNamedResourceCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopResourceCursor[] = L"Microsoft.UI.Input.IInputDesktopResourceCursor";
/* [object, uuid("1DF2777F-7C90-58FC-A7A3-D5736C6510FD"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ModuleName )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ResourceId )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * This,
        /* [retval, out] */UINT32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_get_ModuleName(This,value) \
    ( (This)->lpVtbl->get_ModuleName(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_get_ResourceId(This,value) \
    ( (This)->lpVtbl->get_ResourceId(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputDesktopResourceCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputDesktopResourceCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputDesktopResourceCursorStatics[] = L"Microsoft.UI.Input.IInputDesktopResourceCursorStatics";
/* [object, uuid("F440DC37-A0B6-56EB-BCEC-B024F2233D47"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This,
        /* [in] */UINT32 resourceId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateFromModule )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics * This,
        /* [in] */HSTRING moduleName,
        /* [in] */UINT32 resourceId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursor * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_Create(This,resourceId,result) \
    ( (This)->lpVtbl->Create(This,resourceId,result) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_CreateFromModule(This,moduleName,resourceId,result) \
    ( (This)->lpVtbl->CreateFromModule(This,moduleName,resourceId,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputDesktopResourceCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputKeyboardSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputKeyboardSource
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputKeyboardSource[] = L"Microsoft.UI.Input.IInputKeyboardSource";
/* [object, uuid("ED61B906-16AD-5DF7-A550-5E6F7D2229F7"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputKeyboardSourceStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputKeyboardSource
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputKeyboardSourceStatics[] = L"Microsoft.UI.Input.IInputKeyboardSourceStatics";
/* [object, uuid("F4E1563D-8C2E-5BCD-B784-47ADEAA3CD7E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetKeyStateForCurrentThread )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics * This,
        /* [in] */__x_ABI_CWindows_CSystem_CVirtualKey virtualKey,
        /* [retval, out] */__x_ABI_CWindows_CUI_CCore_CCoreVirtualKeyStates * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_GetKeyStateForCurrentThread(This,virtualKey,result) \
    ( (This)->lpVtbl->GetKeyStateForCurrentThread(This,virtualKey,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputKeyboardSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputLightDismissAction
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputLightDismissAction
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputLightDismissAction[] = L"Microsoft.UI.Input.IInputLightDismissAction";
/* [object, uuid("E8A39502-A860-502F-8C10-3646D43AECF1"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Dismissed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputLightDismissAction_Microsoft__CUI__CInput__CInputLightDismissEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Dismissed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_add_Dismissed(This,handler,token) \
    ( (This)->lpVtbl->add_Dismissed(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_remove_Dismissed(This,token) \
    ( (This)->lpVtbl->remove_Dismissed(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputLightDismissActionStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputLightDismissAction
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputLightDismissActionStatics[] = L"Microsoft.UI.Input.IInputLightDismissActionStatics";
/* [object, uuid("ED9B8DEF-6496-5169-984D-D44B4E690623"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetForWindowId )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissAction * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_GetForWindowId(This,windowId,result) \
    ( (This)->lpVtbl->GetForWindowId(This,windowId,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissActionStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputLightDismissEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputLightDismissEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputLightDismissEventArgs[] = L"Microsoft.UI.Input.IInputLightDismissEventArgs";
/* [object, uuid("078660EE-07CA-5808-B982-E6E899CF098C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputLightDismissEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputObject[] = L"Microsoft.UI.Input.IInputObject";
/* [object, uuid("42EDBC88-D386-544D-B1B8-68617FE68282"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DispatcherQueue )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputObject * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputObject
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObject_get_DispatcherQueue(This,value) \
    ( (This)->lpVtbl->get_DispatcherQueue(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputObject;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObject_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputObjectFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputObjectFactory[] = L"Microsoft.UI.Input.IInputObjectFactory";
/* [object, uuid("F7786BC2-B0B8-5961-9A57-AE199D452106"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputObjectFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputPointerSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputPointerSource
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputPointerSource[] = L"Microsoft.UI.Input.IInputPointerSource";
/* [object, uuid("6A6C2764-C3F4-5BE5-8447-C9A98766C240"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cursor )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Cursor )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIInputCursor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DeviceKinds )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CInputPointerSourceDeviceKinds * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerCaptureLost )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerCaptureLost )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerEntered )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerEntered )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerExited )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerExited )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerMoved )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerMoved )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerPressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerPressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerReleased )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerReleased )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerRoutedAway )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerRoutedAway )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerRoutedReleased )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerRoutedReleased )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerRoutedTo )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerRoutedTo )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PointerWheelChanged )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CInput__CInputPointerSource_Microsoft__CUI__CInput__CPointerEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PointerWheelChanged )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_get_Cursor(This,value) \
    ( (This)->lpVtbl->get_Cursor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_put_Cursor(This,value) \
    ( (This)->lpVtbl->put_Cursor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_get_DeviceKinds(This,value) \
    ( (This)->lpVtbl->get_DeviceKinds(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerCaptureLost(This,handler,token) \
    ( (This)->lpVtbl->add_PointerCaptureLost(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerCaptureLost(This,token) \
    ( (This)->lpVtbl->remove_PointerCaptureLost(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerEntered(This,handler,token) \
    ( (This)->lpVtbl->add_PointerEntered(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerEntered(This,token) \
    ( (This)->lpVtbl->remove_PointerEntered(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerExited(This,handler,token) \
    ( (This)->lpVtbl->add_PointerExited(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerExited(This,token) \
    ( (This)->lpVtbl->remove_PointerExited(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerMoved(This,handler,token) \
    ( (This)->lpVtbl->add_PointerMoved(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerMoved(This,token) \
    ( (This)->lpVtbl->remove_PointerMoved(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerPressed(This,handler,token) \
    ( (This)->lpVtbl->add_PointerPressed(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerPressed(This,token) \
    ( (This)->lpVtbl->remove_PointerPressed(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerReleased(This,handler,token) \
    ( (This)->lpVtbl->add_PointerReleased(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerReleased(This,token) \
    ( (This)->lpVtbl->remove_PointerReleased(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerRoutedAway(This,handler,token) \
    ( (This)->lpVtbl->add_PointerRoutedAway(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerRoutedAway(This,token) \
    ( (This)->lpVtbl->remove_PointerRoutedAway(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerRoutedReleased(This,handler,token) \
    ( (This)->lpVtbl->add_PointerRoutedReleased(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerRoutedReleased(This,token) \
    ( (This)->lpVtbl->remove_PointerRoutedReleased(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerRoutedTo(This,handler,token) \
    ( (This)->lpVtbl->add_PointerRoutedTo(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerRoutedTo(This,token) \
    ( (This)->lpVtbl->remove_PointerRoutedTo(This,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_add_PointerWheelChanged(This,handler,token) \
    ( (This)->lpVtbl->add_PointerWheelChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_remove_PointerWheelChanged(This,token) \
    ( (This)->lpVtbl->remove_PointerWheelChanged(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputSystemCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputSystemCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputSystemCursor[] = L"Microsoft.UI.Input.IInputSystemCursor";
/* [object, uuid("59F538E7-C500-59AB-8B54-0BC6100FD49E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CursorShape )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CInputSystemCursorShape * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_get_CursorShape(This,value) \
    ( (This)->lpVtbl->get_CursorShape(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IInputSystemCursorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.InputSystemCursor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IInputSystemCursorStatics[] = L"Microsoft.UI.Input.IInputSystemCursorStatics";
/* [object, uuid("D3860BB6-698A-5814-AEDD-C2FA8BBA5A02"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CInputSystemCursorShape type,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursor * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_Create(This,type,result) \
    ( (This)->lpVtbl->Create(This,type,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIInputSystemCursorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationCompletedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationCompletedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationCompletedEventArgs[] = L"Microsoft.UI.Input.IManipulationCompletedEventArgs";
/* [object, uuid("0E0249D4-46E4-5559-AEE3-FA45CE2A7F56"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cumulative )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Velocities )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationVelocities * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_get_Cumulative(This,value) \
    ( (This)->lpVtbl->get_Cumulative(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_get_Velocities(This,value) \
    ( (This)->lpVtbl->get_Velocities(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationCompletedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationInertiaStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationInertiaStartingEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationInertiaStartingEventArgs[] = L"Microsoft.UI.Input.IManipulationInertiaStartingEventArgs";
/* [object, uuid("ACF9EF71-6E15-56AB-9260-F0D3CE5F66E8"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cumulative )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Delta )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Velocities )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationVelocities * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_get_Cumulative(This,value) \
    ( (This)->lpVtbl->get_Cumulative(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_get_Delta(This,value) \
    ( (This)->lpVtbl->get_Delta(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_get_Velocities(This,value) \
    ( (This)->lpVtbl->get_Velocities(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationInertiaStartingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationStartedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationStartedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationStartedEventArgs[] = L"Microsoft.UI.Input.IManipulationStartedEventArgs";
/* [object, uuid("4A616613-EEF1-5F1B-A768-0775478D49D4"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cumulative )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_get_Cumulative(This,value) \
    ( (This)->lpVtbl->get_Cumulative(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationStartedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IManipulationUpdatedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.ManipulationUpdatedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IManipulationUpdatedEventArgs[] = L"Microsoft.UI.Input.IManipulationUpdatedEventArgs";
/* [object, uuid("406E1961-0C98-5FC0-B3D8-116492EF0053"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cumulative )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Delta )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationDelta * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Velocities )(
        __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CManipulationVelocities * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_get_Cumulative(This,value) \
    ( (This)->lpVtbl->get_Cumulative(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_get_Delta(This,value) \
    ( (This)->lpVtbl->get_Delta(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_get_Velocities(This,value) \
    ( (This)->lpVtbl->get_Velocities(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIManipulationUpdatedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IMouseWheelParameters
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.MouseWheelParameters
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IMouseWheelParameters[] = L"Microsoft.UI.Input.IMouseWheelParameters";
/* [object, uuid("6D98BE40-1D56-51D1-AA0D-F325439CD009"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParametersVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CharTranslation )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CharTranslation )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DeltaScale )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DeltaScale )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DeltaRotationAngle )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DeltaRotationAngle )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PageTranslation )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PageTranslation )(
        __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParametersVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParametersVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_get_CharTranslation(This,value) \
    ( (This)->lpVtbl->get_CharTranslation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_put_CharTranslation(This,value) \
    ( (This)->lpVtbl->put_CharTranslation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_get_DeltaScale(This,value) \
    ( (This)->lpVtbl->get_DeltaScale(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_put_DeltaScale(This,value) \
    ( (This)->lpVtbl->put_DeltaScale(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_get_DeltaRotationAngle(This,value) \
    ( (This)->lpVtbl->get_DeltaRotationAngle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_put_DeltaRotationAngle(This,value) \
    ( (This)->lpVtbl->put_DeltaRotationAngle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_get_PageTranslation(This,value) \
    ( (This)->lpVtbl->get_PageTranslation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_put_PageTranslation(This,value) \
    ( (This)->lpVtbl->put_PageTranslation(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIMouseWheelParameters_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerEventArgs[] = L"Microsoft.UI.Input.IPointerEventArgs";
/* [object, uuid("865B188C-2ED5-5DF8-829F-AC0701D5C51A"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CurrentPoint )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Handled )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Handled )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_KeyModifiers )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CSystem_CVirtualKeyModifiers * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetIntermediatePoints )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
        /* [retval, out] */__FIVector_1_Microsoft__CUI__CInput__CPointerPoint * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetIntermediateTransformedPoints )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * transform,
        /* [retval, out] */__FIVector_1_Microsoft__CUI__CInput__CPointerPoint * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_get_CurrentPoint(This,value) \
    ( (This)->lpVtbl->get_CurrentPoint(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_get_Handled(This,value) \
    ( (This)->lpVtbl->get_Handled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_put_Handled(This,value) \
    ( (This)->lpVtbl->put_Handled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_get_KeyModifiers(This,value) \
    ( (This)->lpVtbl->get_KeyModifiers(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_GetIntermediatePoints(This,result) \
    ( (This)->lpVtbl->GetIntermediatePoints(This,result) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_GetIntermediateTransformedPoints(This,transform,result) \
    ( (This)->lpVtbl->GetIntermediateTransformedPoints(This,transform,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPoint
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPoint
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPoint[] = L"Microsoft.UI.Input.IPointerPoint";
/* [object, uuid("0D430EE6-252C-59A4-B2A2-D44264DC6A40"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FrameId )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */UINT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsInContact )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerId )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */UINT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Properties )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Timestamp )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [retval, out] */UINT64 * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetTransformedPoint )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * transform,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_FrameId(This,value) \
    ( (This)->lpVtbl->get_FrameId(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_IsInContact(This,value) \
    ( (This)->lpVtbl->get_IsInContact(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_PointerId(This,value) \
    ( (This)->lpVtbl->get_PointerId(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_Properties(This,value) \
    ( (This)->lpVtbl->get_Properties(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_get_Timestamp(This,value) \
    ( (This)->lpVtbl->get_Timestamp(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_GetTransformedPoint(This,transform,result) \
    ( (This)->lpVtbl->GetTransformedPoint(This,transform,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPointProperties
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPointProperties
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPointProperties[] = L"Microsoft.UI.Input.IPointerPointProperties";
/* [object, uuid("D760ED77-4B10-57A5-B3CC-D9BF3413E996"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointPropertiesVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ContactRect )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CRect * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsBarrelButtonPressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsCanceled )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsEraser )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsHorizontalMouseWheel )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsInRange )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsInverted )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsLeftButtonPressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsMiddleButtonPressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsPrimary )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsRightButtonPressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsXButton1Pressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsXButton2Pressed )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MouseWheelDelta )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Orientation )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerUpdateKind )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerUpdateKind * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Pressure )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TouchConfidence )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Twist )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_XTilt )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_YTilt )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties * This,
        /* [retval, out] */FLOAT * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointPropertiesVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointPropertiesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_ContactRect(This,value) \
    ( (This)->lpVtbl->get_ContactRect(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsBarrelButtonPressed(This,value) \
    ( (This)->lpVtbl->get_IsBarrelButtonPressed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsCanceled(This,value) \
    ( (This)->lpVtbl->get_IsCanceled(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsEraser(This,value) \
    ( (This)->lpVtbl->get_IsEraser(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsHorizontalMouseWheel(This,value) \
    ( (This)->lpVtbl->get_IsHorizontalMouseWheel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsInRange(This,value) \
    ( (This)->lpVtbl->get_IsInRange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsInverted(This,value) \
    ( (This)->lpVtbl->get_IsInverted(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsLeftButtonPressed(This,value) \
    ( (This)->lpVtbl->get_IsLeftButtonPressed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsMiddleButtonPressed(This,value) \
    ( (This)->lpVtbl->get_IsMiddleButtonPressed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsPrimary(This,value) \
    ( (This)->lpVtbl->get_IsPrimary(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsRightButtonPressed(This,value) \
    ( (This)->lpVtbl->get_IsRightButtonPressed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsXButton1Pressed(This,value) \
    ( (This)->lpVtbl->get_IsXButton1Pressed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_IsXButton2Pressed(This,value) \
    ( (This)->lpVtbl->get_IsXButton2Pressed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_MouseWheelDelta(This,value) \
    ( (This)->lpVtbl->get_MouseWheelDelta(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_Orientation(This,value) \
    ( (This)->lpVtbl->get_Orientation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_PointerUpdateKind(This,value) \
    ( (This)->lpVtbl->get_PointerUpdateKind(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_Pressure(This,value) \
    ( (This)->lpVtbl->get_Pressure(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_TouchConfidence(This,value) \
    ( (This)->lpVtbl->get_TouchConfidence(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_Twist(This,value) \
    ( (This)->lpVtbl->get_Twist(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_XTilt(This,value) \
    ( (This)->lpVtbl->get_XTilt(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_get_YTilt(This,value) \
    ( (This)->lpVtbl->get_YTilt(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointProperties_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPointTransform
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPointTransform[] = L"Microsoft.UI.Input.IPointerPointTransform";
/* [object, uuid("DB4791BC-994D-54C7-92EF-66EA1DE9B43C"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransformVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Inverse )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * * value
        );
    HRESULT ( STDMETHODCALLTYPE *TryTransform )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint inPoint,
        /* [out] */__x_ABI_CWindows_CFoundation_CPoint * outPoint,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *TryTransformBounds )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CRect inRect,
        /* [out] */__x_ABI_CWindows_CFoundation_CRect * outRect,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransformVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransformVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_get_Inverse(This,value) \
    ( (This)->lpVtbl->get_Inverse(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_TryTransform(This,inPoint,outPoint,result) \
    ( (This)->lpVtbl->TryTransform(This,inPoint,outPoint,result) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_TryTransformBounds(This,inRect,outRect,result) \
    ( (This)->lpVtbl->TryTransformBounds(This,inRect,outRect,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPointTransform_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPredictor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPredictor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPredictor[] = L"Microsoft.UI.Input.IPointerPredictor";
/* [object, uuid("12C100EC-2100-565F-A60C-F1187F438828"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PredictionTime )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CTimeSpan * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PredictionTime )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CTimeSpan value
        );
    HRESULT ( STDMETHODCALLTYPE *GetPredictedPoints )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * point,
        /* [out] */UINT32 * __resultSize,
        /* [size_is(, *(__resultSize)), retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPoint * * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_get_PredictionTime(This,value) \
    ( (This)->lpVtbl->get_PredictionTime(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_put_PredictionTime(This,value) \
    ( (This)->lpVtbl->put_PredictionTime(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_GetPredictedPoints(This,point,__resultSize,result) \
    ( (This)->lpVtbl->GetPredictedPoints(This,point,__resultSize,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IPointerPredictorStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.PointerPredictor
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IPointerPredictorStatics[] = L"Microsoft.UI.Input.IPointerPredictorStatics";
/* [object, uuid("78A8EF30-3E5C-55CD-8F85-65AC09B1A987"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateForInputPointerSource )(
        __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CInput_CIInputPointerSource * inputPointerSource,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictor * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_CreateForInputPointerSource(This,inputPointerSource,result) \
    ( (This)->lpVtbl->CreateForInputPointerSource(This,inputPointerSource,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIPointerPredictorStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.IRightTappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.RightTappedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_IRightTappedEventArgs[] = L"Microsoft.UI.Input.IRightTappedEventArgs";
/* [object, uuid("8FF73B39-887E-50A4-8500-77953039DCB4"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CIRightTappedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Input.ITappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Input.TappedEventArgs
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Input_ITappedEventArgs[] = L"Microsoft.UI.Input.ITappedEventArgs";
/* [object, uuid("C3A01BB5-6076-5E0F-871A-9D94A6A8F82B"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointerDeviceType )(
        __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CInput_CPointerDeviceType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TapCount )(
        __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs * This,
        /* [retval, out] */UINT32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_get_PointerDeviceType(This,value) \
    ( (This)->lpVtbl->get_PointerDeviceType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_get_TapCount(This,value) \
    ( (This)->lpVtbl->get_TapCount(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CInput_CITappedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.CrossSlidingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.ICrossSlidingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_CrossSlidingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_CrossSlidingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_CrossSlidingEventArgs[] = L"Microsoft.UI.Input.CrossSlidingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.DraggingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IDraggingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_DraggingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_DraggingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_DraggingEventArgs[] = L"Microsoft.UI.Input.DraggingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.GestureRecognizer
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IGestureRecognizer ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_GestureRecognizer_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_GestureRecognizer_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_GestureRecognizer[] = L"Microsoft.UI.Input.GestureRecognizer";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.HoldingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IHoldingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_HoldingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_HoldingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_HoldingEventArgs[] = L"Microsoft.UI.Input.HoldingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputActivationListener
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputActivationListenerStatics interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputActivationListener ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputActivationListener_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputActivationListener_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputActivationListener[] = L"Microsoft.UI.Input.InputActivationListener";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputActivationListenerActivationChangedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputActivationListenerActivationChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputActivationListenerActivationChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputActivationListenerActivationChangedEventArgs[] = L"Microsoft.UI.Input.InputActivationListenerActivationChangedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputCursorStatics interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputCursor ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputCursor[] = L"Microsoft.UI.Input.InputCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputCustomCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputCustomCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputCustomCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputCustomCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputCustomCursor[] = L"Microsoft.UI.Input.InputCustomCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputDesktopNamedResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.1
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputDesktopNamedResourceCursorStatics interface starting with version 1.1 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputDesktopNamedResourceCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputDesktopNamedResourceCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputDesktopNamedResourceCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputDesktopNamedResourceCursor[] = L"Microsoft.UI.Input.InputDesktopNamedResourceCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10001


/*
 *
 * Class Microsoft.UI.Input.InputDesktopResourceCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputDesktopResourceCursorStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputDesktopResourceCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputDesktopResourceCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputDesktopResourceCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputDesktopResourceCursor[] = L"Microsoft.UI.Input.InputDesktopResourceCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputKeyboardSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputKeyboardSourceStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputKeyboardSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputKeyboardSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputKeyboardSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputKeyboardSource[] = L"Microsoft.UI.Input.InputKeyboardSource";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputLightDismissAction
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputLightDismissActionStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputLightDismissAction ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissAction_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissAction_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputLightDismissAction[] = L"Microsoft.UI.Input.InputLightDismissAction";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputLightDismissEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputLightDismissEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputLightDismissEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputLightDismissEventArgs[] = L"Microsoft.UI.Input.InputLightDismissEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputObject ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputObject_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputObject_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputObject[] = L"Microsoft.UI.Input.InputObject";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputPointerSource
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputPointerSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputPointerSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputPointerSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputPointerSource[] = L"Microsoft.UI.Input.InputPointerSource";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.InputSystemCursor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IInputSystemCursorStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IInputSystemCursor ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_InputSystemCursor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_InputSystemCursor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_InputSystemCursor[] = L"Microsoft.UI.Input.InputSystemCursor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationCompletedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationCompletedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationCompletedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationCompletedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationCompletedEventArgs[] = L"Microsoft.UI.Input.ManipulationCompletedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationInertiaStartingEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationInertiaStartingEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationInertiaStartingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationInertiaStartingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationInertiaStartingEventArgs[] = L"Microsoft.UI.Input.ManipulationInertiaStartingEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationStartedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationStartedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationStartedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationStartedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationStartedEventArgs[] = L"Microsoft.UI.Input.ManipulationStartedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.ManipulationUpdatedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IManipulationUpdatedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_ManipulationUpdatedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_ManipulationUpdatedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_ManipulationUpdatedEventArgs[] = L"Microsoft.UI.Input.ManipulationUpdatedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.MouseWheelParameters
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IMouseWheelParameters ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_MouseWheelParameters_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_MouseWheelParameters_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_MouseWheelParameters[] = L"Microsoft.UI.Input.MouseWheelParameters";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerEventArgs[] = L"Microsoft.UI.Input.PointerEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerPoint
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerPoint ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerPoint_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerPoint_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerPoint[] = L"Microsoft.UI.Input.PointerPoint";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerPointProperties
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerPointProperties ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerPointProperties_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerPointProperties_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerPointProperties[] = L"Microsoft.UI.Input.PointerPointProperties";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.PointerPredictor
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Input.IPointerPredictorStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IPointerPredictor ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_PointerPredictor_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_PointerPredictor_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_PointerPredictor[] = L"Microsoft.UI.Input.PointerPredictor";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.RightTappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.IRightTappedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_RightTappedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_RightTappedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_RightTappedEventArgs[] = L"Microsoft.UI.Input.RightTappedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Input.TappedEventArgs
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Input.ITappedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Input_TappedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Input_TappedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Input_TappedEventArgs[] = L"Microsoft.UI.Input.TappedEventArgs";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000




#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EInput_p_h__

#endif // __Microsoft2EUI2EInput_h__
