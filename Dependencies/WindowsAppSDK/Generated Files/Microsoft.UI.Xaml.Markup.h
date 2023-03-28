/* Header file automatically generated from Microsoft.UI.Xaml.Markup.idl */
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
#ifndef __Microsoft2EUI2EXaml2EMarkup_h__
#define __Microsoft2EUI2EXaml2EMarkup_h__
#ifndef __Microsoft2EUI2EXaml2EMarkup_p_h__
#define __Microsoft2EUI2EXaml2EMarkup_p_h__


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
#include "Windows.Storage.Streams.h"
#include "Windows.UI.Xaml.Interop.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IComponentConnector;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector ABI::Microsoft::UI::Xaml::Markup::IComponentConnector

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IDataTemplateComponent;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent ABI::Microsoft::UI::Xaml::Markup::IDataTemplateComponent

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IMarkupExtension;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension ABI::Microsoft::UI::Xaml::Markup::IMarkupExtension

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IMarkupExtensionFactory;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory ABI::Microsoft::UI::Xaml::Markup::IMarkupExtensionFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IMarkupExtensionOverrides;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides ABI::Microsoft::UI::Xaml::Markup::IMarkupExtensionOverrides

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IProvideValueTarget;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget ABI::Microsoft::UI::Xaml::Markup::IProvideValueTarget

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IProvideValueTargetProperty;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty ABI::Microsoft::UI::Xaml::Markup::IProvideValueTargetProperty

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IRootObjectProvider;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider ABI::Microsoft::UI::Xaml::Markup::IRootObjectProvider

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IUriContext;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext ABI::Microsoft::UI::Xaml::Markup::IUriContext

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlBinaryWriter;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter ABI::Microsoft::UI::Xaml::Markup::IXamlBinaryWriter

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlBinaryWriterStatics;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics ABI::Microsoft::UI::Xaml::Markup::IXamlBinaryWriterStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlBindScopeDiagnostics;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics ABI::Microsoft::UI::Xaml::Markup::IXamlBindScopeDiagnostics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlBindingHelper;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper ABI::Microsoft::UI::Xaml::Markup::IXamlBindingHelper

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlBindingHelperStatics;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics ABI::Microsoft::UI::Xaml::Markup::IXamlBindingHelperStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlMarkupHelper;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper ABI::Microsoft::UI::Xaml::Markup::IXamlMarkupHelper

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlMarkupHelperStatics;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics ABI::Microsoft::UI::Xaml::Markup::IXamlMarkupHelperStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlMember;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember ABI::Microsoft::UI::Xaml::Markup::IXamlMember

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlMetadataProvider;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider ABI::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlReader;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader ABI::Microsoft::UI::Xaml::Markup::IXamlReader

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlReaderStatics;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics ABI::Microsoft::UI::Xaml::Markup::IXamlReaderStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlType;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType ABI::Microsoft::UI::Xaml::Markup::IXamlType

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    interface IXamlTypeResolver;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver ABI::Microsoft::UI::Xaml::Markup::IXamlTypeResolver

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Storage {
            namespace Streams {
                interface IRandomAccessStream;
            } /* Streams */
        } /* Storage */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream ABI::Windows::Storage::Streams::IRandomAccessStream

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#define DEF___FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c875446a-587f-58da-897e-3bbe5ec7c30b"))
IIterator<ABI::Windows::Storage::Streams::IRandomAccessStream*> : IIterator_impl<ABI::Windows::Storage::Streams::IRandomAccessStream*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Windows.Storage.Streams.IRandomAccessStream>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Windows::Storage::Streams::IRandomAccessStream*> __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_t;
#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::__FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Storage::Streams::IRandomAccessStream*>
//#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_t ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Storage::Streams::IRandomAccessStream*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#define DEF___FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("ba666a00-1555-5df4-81a5-07d23f7ffceb"))
IIterable<ABI::Windows::Storage::Streams::IRandomAccessStream*> : IIterable_impl<ABI::Windows::Storage::Streams::IRandomAccessStream*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Windows.Storage.Streams.IRandomAccessStream>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Windows::Storage::Streams::IRandomAccessStream*> __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_t;
#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::__FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Storage::Streams::IRandomAccessStream*>
//#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_t ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Storage::Streams::IRandomAccessStream*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#define DEF___FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("92cd0a46-2266-5cd6-9293-e111299f2793"))
IVectorView<ABI::Windows::Storage::Streams::IRandomAccessStream*> : IVectorView_impl<ABI::Windows::Storage::Streams::IRandomAccessStream*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Windows.Storage.Streams.IRandomAccessStream>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Windows::Storage::Streams::IRandomAccessStream*> __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_t;
#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::__FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Storage::Streams::IRandomAccessStream*>
//#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Storage::Streams::IRandomAccessStream*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#define DEF___FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("2736b66b-daa3-5e0c-9842-6a0f44b5440b"))
IVector<ABI::Windows::Storage::Streams::IRandomAccessStream*> : IVector_impl<ABI::Windows::Storage::Streams::IRandomAccessStream*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Windows.Storage.Streams.IRandomAccessStream>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<ABI::Windows::Storage::Streams::IRandomAccessStream*> __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_t;
#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Storage::Streams::IRandomAccessStream*>
//#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_t ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Storage::Streams::IRandomAccessStream*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                class DependencyObject;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IDependencyObject;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject ABI::Microsoft::UI::Xaml::IDependencyObject

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__


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


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IXamlServiceProvider;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider ABI::Microsoft::UI::Xaml::IXamlServiceProvider

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider_FWD_DEFINED__


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                class UIElement;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IUIElement;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement ABI::Microsoft::UI::Xaml::IUIElement

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__






namespace ABI {
    namespace Windows {
        namespace Foundation {
            
            typedef struct DateTime DateTime;
            
        } /* Foundation */
    } /* Windows */
} /* ABI */

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
            
            typedef struct Size Size;
            
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
                namespace Markup {
                    
                    typedef struct XamlBinaryWriterErrorInformation XamlBinaryWriterErrorInformation;
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    
                    typedef struct XmlnsDefinition XmlnsDefinition;
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */























namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    class MarkupExtension;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    class ProvideValueTargetProperty;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    class XamlBinaryWriter;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    class XamlBindingHelper;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    class XamlMarkupHelper;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    class XamlReader;
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */







/*
 *
 * Struct Microsoft.UI.Xaml.Markup.XamlBinaryWriterErrorInformation
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
                namespace Markup {
                    /* [contract] */
                    struct XamlBinaryWriterErrorInformation
                    {
                        UINT32 InputStreamIndex;
                        UINT32 LineNumber;
                        UINT32 LinePosition;
                    };
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Markup.XmlnsDefinition
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
                namespace Markup {
                    /* [contract] */
                    struct XmlnsDefinition
                    {
                        HSTRING XmlNamespace;
                        HSTRING Namespace;
                    };
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IComponentConnector
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IComponentConnector[] = L"Microsoft.UI.Xaml.Markup.IComponentConnector";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("AD401812-B091-51D0-B915-2D682CD2AF10"), contract] */
                    MIDL_INTERFACE("AD401812-B091-51D0-B915-2D682CD2AF10")
                    IComponentConnector : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Connect(
                            /* [in] */INT32 connectionId,
                            /* [in] */IInspectable * target
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetBindingConnector(
                            /* [in] */INT32 connectionId,
                            /* [in] */IInspectable * target,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IComponentConnector * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IComponentConnector=_uuidof(IComponentConnector);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IDataTemplateComponent
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IDataTemplateComponent[] = L"Microsoft.UI.Xaml.Markup.IDataTemplateComponent";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("1743DDF7-38BA-58C9-A2A6-B0AE28713BEE"), contract] */
                    MIDL_INTERFACE("1743DDF7-38BA-58C9-A2A6-B0AE28713BEE")
                    IDataTemplateComponent : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Recycle(void) = 0;
                        virtual HRESULT STDMETHODCALLTYPE ProcessBindings(
                            /* [in] */IInspectable * item,
                            /* [in] */INT32 itemIndex,
                            /* [in] */INT32 phase,
                            /* [out] */INT32 * nextPhase
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IDataTemplateComponent=_uuidof(IDataTemplateComponent);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IMarkupExtension
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IMarkupExtension[] = L"Microsoft.UI.Xaml.Markup.IMarkupExtension";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("C355371E-091D-5136-AF4A-BAF5E00616BD"), exclusiveto, contract] */
                    MIDL_INTERFACE("C355371E-091D-5136-AF4A-BAF5E00616BD")
                    IMarkupExtension : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IMarkupExtension=_uuidof(IMarkupExtension);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IMarkupExtensionFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IMarkupExtensionFactory[] = L"Microsoft.UI.Xaml.Markup.IMarkupExtensionFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("20651AFA-5F3A-5F0C-ADB1-B6551F53A6A0"), exclusiveto, contract] */
                    MIDL_INTERFACE("20651AFA-5F3A-5F0C-ADB1-B6551F53A6A0")
                    IMarkupExtensionFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IMarkupExtension * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IMarkupExtensionFactory=_uuidof(IMarkupExtensionFactory);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IMarkupExtensionOverrides
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IMarkupExtensionOverrides[] = L"Microsoft.UI.Xaml.Markup.IMarkupExtensionOverrides";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("A12AA575-5D31-5B68-A30F-8495412A351D"), exclusiveto, contract] */
                    MIDL_INTERFACE("A12AA575-5D31-5B68-A30F-8495412A351D")
                    IMarkupExtensionOverrides : public IInspectable
                    {
                    public:
                        /* [overload] */virtual HRESULT STDMETHODCALLTYPE ProvideValue(
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        /* [overload] */virtual HRESULT STDMETHODCALLTYPE ProvideValueWithIXamlServiceProvider(
                            /* [in] */ABI::Microsoft::UI::Xaml::IXamlServiceProvider * serviceProvider,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IMarkupExtensionOverrides=_uuidof(IMarkupExtensionOverrides);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IProvideValueTarget
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IProvideValueTarget[] = L"Microsoft.UI.Xaml.Markup.IProvideValueTarget";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("3F01FF68-3EFD-591D-A506-DE13FCAABD83"), contract] */
                    MIDL_INTERFACE("3F01FF68-3EFD-591D-A506-DE13FCAABD83")
                    IProvideValueTarget : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TargetObject(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TargetProperty(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IProvideValueTarget=_uuidof(IProvideValueTarget);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IProvideValueTargetProperty
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.ProvideValueTargetProperty
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IProvideValueTargetProperty[] = L"Microsoft.UI.Xaml.Markup.IProvideValueTargetProperty";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("CE777B1F-B42E-59D1-870D-12FDF0629133"), exclusiveto, contract] */
                    MIDL_INTERFACE("CE777B1F-B42E-59D1-870D-12FDF0629133")
                    IProvideValueTargetProperty : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Name(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Type(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DeclaringType(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IProvideValueTargetProperty=_uuidof(IProvideValueTargetProperty);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IRootObjectProvider
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IRootObjectProvider[] = L"Microsoft.UI.Xaml.Markup.IRootObjectProvider";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("13D63599-352F-5EB8-81C1-BC62FB12D6DA"), contract] */
                    MIDL_INTERFACE("13D63599-352F-5EB8-81C1-BC62FB12D6DA")
                    IRootObjectProvider : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RootObject(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IRootObjectProvider=_uuidof(IRootObjectProvider);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IUriContext
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IUriContext[] = L"Microsoft.UI.Xaml.Markup.IUriContext";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("FB8605F6-8F05-52EE-A01C-3A9E118A6EA2"), contract] */
                    MIDL_INTERFACE("FB8605F6-8F05-52EE-A01C-3A9E118A6EA2")
                    IUriContext : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BaseUri(
                            /* [retval, out] */ABI::Windows::Foundation::IUriRuntimeClass * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IUriContext=_uuidof(IUriContext);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBinaryWriter
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBinaryWriter
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBinaryWriter[] = L"Microsoft.UI.Xaml.Markup.IXamlBinaryWriter";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("8FB45E3B-E689-55BF-AA11-D83B1C1CDDA1"), exclusiveto, contract] */
                    MIDL_INTERFACE("8FB45E3B-E689-55BF-AA11-D83B1C1CDDA1")
                    IXamlBinaryWriter : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlBinaryWriter=_uuidof(IXamlBinaryWriter);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBinaryWriterStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBinaryWriter
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBinaryWriterStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlBinaryWriterStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("774907FC-C846-517F-ABCC-C3F7E8C3FFC9"), exclusiveto, contract] */
                    MIDL_INTERFACE("774907FC-C846-517F-ABCC-C3F7E8C3FFC9")
                    IXamlBinaryWriterStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Write(
                            /* [in] */__FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * inputStreams,
                            /* [in] */__FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * outputStreams,
                            /* [in] */ABI::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider * xamlMetadataProvider,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::XamlBinaryWriterErrorInformation * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlBinaryWriterStatics=_uuidof(IXamlBinaryWriterStatics);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBindScopeDiagnostics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBindScopeDiagnostics[] = L"Microsoft.UI.Xaml.Markup.IXamlBindScopeDiagnostics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("3EA84E4E-FDFE-55A8-A561-EDF5697846D7"), contract] */
                    MIDL_INTERFACE("3EA84E4E-FDFE-55A8-A561-EDF5697846D7")
                    IXamlBindScopeDiagnostics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Disable(
                            /* [in] */INT32 lineNumber,
                            /* [in] */INT32 columnNumber
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlBindScopeDiagnostics=_uuidof(IXamlBindScopeDiagnostics);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBindingHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBindingHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBindingHelper[] = L"Microsoft.UI.Xaml.Markup.IXamlBindingHelper";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("607A9BF2-5A6D-5C89-A756-BB44F24F28F8"), exclusiveto, contract] */
                    MIDL_INTERFACE("607A9BF2-5A6D-5C89-A756-BB44F24F28F8")
                    IXamlBindingHelper : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlBindingHelper=_uuidof(IXamlBindingHelper);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBindingHelperStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBindingHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBindingHelperStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlBindingHelperStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("93C7DAD3-F9C2-5372-84DC-9E9C4661D083"), exclusiveto, contract] */
                    MIDL_INTERFACE("93C7DAD3-F9C2-5372-84DC-9E9C4661D083")
                    IXamlBindingHelperStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DataTemplateComponentProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetDataTemplateComponent(
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyObject * element,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IDataTemplateComponent * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetDataTemplateComponent(
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyObject * element,
                            /* [in] */ABI::Microsoft::UI::Xaml::Markup::IDataTemplateComponent * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SuspendRendering(
                            /* [in] */ABI::Microsoft::UI::Xaml::IUIElement * target
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE ResumeRendering(
                            /* [in] */ABI::Microsoft::UI::Xaml::IUIElement * target
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE ConvertValue(
                            /* [in] */ABI::Windows::UI::Xaml::Interop::TypeName type,
                            /* [in] */IInspectable * value,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromString(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */HSTRING value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromBoolean(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */::boolean value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromChar16(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */WCHAR value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromDateTime(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */ABI::Windows::Foundation::DateTime value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromDouble(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */DOUBLE value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromInt32(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */INT32 value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromUInt32(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */UINT32 value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromInt64(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */INT64 value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromUInt64(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */UINT64 value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromSingle(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */FLOAT value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromPoint(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */ABI::Windows::Foundation::Point value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromRect(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */ABI::Windows::Foundation::Rect value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromSize(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */ABI::Windows::Foundation::Size value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromTimeSpan(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */ABI::Windows::Foundation::TimeSpan value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromByte(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */BYTE value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromUri(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */ABI::Windows::Foundation::IUriRuntimeClass * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPropertyFromObject(
                            /* [in] */IInspectable * dependencyObject,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * propertyToSet,
                            /* [in] */IInspectable * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlBindingHelperStatics=_uuidof(IXamlBindingHelperStatics);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMarkupHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlMarkupHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMarkupHelper[] = L"Microsoft.UI.Xaml.Markup.IXamlMarkupHelper";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("CD677310-3B06-5A13-B31A-401849570858"), exclusiveto, contract] */
                    MIDL_INTERFACE("CD677310-3B06-5A13-B31A-401849570858")
                    IXamlMarkupHelper : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlMarkupHelper=_uuidof(IXamlMarkupHelper);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMarkupHelperStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlMarkupHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMarkupHelperStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlMarkupHelperStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("D9A0F6E3-C6CC-5CB6-8999-85788701F339"), exclusiveto, contract] */
                    MIDL_INTERFACE("D9A0F6E3-C6CC-5CB6-8999-85788701F339")
                    IXamlMarkupHelperStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE UnloadObject(
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyObject * element
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlMarkupHelperStatics=_uuidof(IXamlMarkupHelperStatics);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMember
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMember[] = L"Microsoft.UI.Xaml.Markup.IXamlMember";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("BF3A2913-5C63-50EC-8660-61809BE7B9B9"), contract] */
                    MIDL_INTERFACE("BF3A2913-5C63-50EC-8660-61809BE7B9B9")
                    IXamlMember : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsAttachable(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsDependencyProperty(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsReadOnly(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Name(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TargetType(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Type(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetValue(
                            /* [in] */IInspectable * instance,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetValue(
                            /* [in] */IInspectable * instance,
                            /* [in] */IInspectable * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlMember=_uuidof(IXamlMember);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMetadataProvider
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMetadataProvider[] = L"Microsoft.UI.Xaml.Markup.IXamlMetadataProvider";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("A96251F0-2214-5D53-8746-CE99A2593CD7"), contract] */
                    MIDL_INTERFACE("A96251F0-2214-5D53-8746-CE99A2593CD7")
                    IXamlMetadataProvider : public IInspectable
                    {
                    public:
                        /* [overload, default_overload] */virtual HRESULT STDMETHODCALLTYPE GetXamlType(
                            /* [in] */ABI::Windows::UI::Xaml::Interop::TypeName type,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * result
                            ) = 0;
                        /* [overload] */virtual HRESULT STDMETHODCALLTYPE GetXamlTypeByFullName(
                            /* [in] */HSTRING fullName,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetXmlnsDefinitions(
                            /* [out] */UINT32 * __resultSize,
                            /* [size_is(, *(__resultSize)), retval, out] */ABI::Microsoft::UI::Xaml::Markup::XmlnsDefinition * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlMetadataProvider=_uuidof(IXamlMetadataProvider);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlReader
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlReader
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlReader[] = L"Microsoft.UI.Xaml.Markup.IXamlReader";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("54CE54C8-38C6-50D9-AC98-4B03EDDBDE9F"), exclusiveto, contract] */
                    MIDL_INTERFACE("54CE54C8-38C6-50D9-AC98-4B03EDDBDE9F")
                    IXamlReader : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlReader=_uuidof(IXamlReader);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlReaderStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlReader
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlReaderStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlReaderStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("82A4CD9E-435E-5AEB-8C4F-300CECE45CAE"), exclusiveto, contract] */
                    MIDL_INTERFACE("82A4CD9E-435E-5AEB-8C4F-300CECE45CAE")
                    IXamlReaderStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Load(
                            /* [in] */HSTRING xaml,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE LoadWithInitialTemplateValidation(
                            /* [in] */HSTRING xaml,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlReaderStatics=_uuidof(IXamlReaderStatics);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlType
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlType[] = L"Microsoft.UI.Xaml.Markup.IXamlType";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("D24219DF-7EC9-57F1-A27B-6AF251D9C5BC"), contract] */
                    MIDL_INTERFACE("D24219DF-7EC9-57F1-A27B-6AF251D9C5BC")
                    IXamlType : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BaseType(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ContentProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlMember * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FullName(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsArray(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsCollection(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsConstructible(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsDictionary(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsMarkupExtension(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsBindable(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ItemType(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_KeyType(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BoxedType(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlType * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UnderlyingType(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE ActivateInstance(
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE CreateFromString(
                            /* [in] */HSTRING value,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetMember(
                            /* [in] */HSTRING name,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Markup::IXamlMember * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE AddToVector(
                            /* [in] */IInspectable * instance,
                            /* [in] */IInspectable * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE AddToMap(
                            /* [in] */IInspectable * instance,
                            /* [in] */IInspectable * key,
                            /* [in] */IInspectable * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE RunInitializer(void) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlType=_uuidof(IXamlType);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlTypeResolver
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlTypeResolver[] = L"Microsoft.UI.Xaml.Markup.IXamlTypeResolver";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Markup {
                    /* [object, uuid("3FA15615-CACF-547F-B1ED-89DAE8C67452"), contract] */
                    MIDL_INTERFACE("3FA15615-CACF-547F-B1ED-89DAE8C67452")
                    IXamlTypeResolver : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Resolve(
                            /* [in] */HSTRING qualifiedTypeName,
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IXamlTypeResolver=_uuidof(IXamlTypeResolver);
                    
                } /* Markup */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IMarkupExtension ** Default Interface **
 *    Microsoft.UI.Xaml.Markup.IMarkupExtensionOverrides
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_MarkupExtension_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_MarkupExtension_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_MarkupExtension[] = L"Microsoft.UI.Xaml.Markup.MarkupExtension";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.ProvideValueTargetProperty
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IProvideValueTargetProperty ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_ProvideValueTargetProperty_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_ProvideValueTargetProperty_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_ProvideValueTargetProperty[] = L"Microsoft.UI.Xaml.Markup.ProvideValueTargetProperty";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlBinaryWriter
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlBinaryWriterStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlBinaryWriter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBinaryWriter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBinaryWriter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlBinaryWriter[] = L"Microsoft.UI.Xaml.Markup.XamlBinaryWriter";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlBindingHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlBindingHelperStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlBindingHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBindingHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBindingHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlBindingHelper[] = L"Microsoft.UI.Xaml.Markup.XamlBindingHelper";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlMarkupHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlMarkupHelperStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlMarkupHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlMarkupHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlMarkupHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlMarkupHelper[] = L"Microsoft.UI.Xaml.Markup.XamlMarkupHelper";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlReader
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlReaderStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlReader ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlReader_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlReader_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlReader[] = L"Microsoft.UI.Xaml.Markup.XamlReader";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions
#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream;

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__)
#define ____FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream;

typedef struct __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__out __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl;

interface __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream
{
    CONST_VTBL struct __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__)
#define ____FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream;

typedef  struct __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Windows__CStorage__CStreams__CIRandomAccessStream **first);

    END_INTERFACE
} __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl;

interface __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream
{
    CONST_VTBL struct __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream;

typedef struct __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
            /* [in] */ __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl;

interface __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream
{
    CONST_VTBL struct __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__)
#define ____FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

typedef interface __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream;

typedef struct __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__deref_out_opt __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_Windows__CStorage__CStreams__CIRandomAccessStream **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ __RPC__in __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [in] */ __RPC__in __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *value);

    END_INTERFACE
} __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl;

interface __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream
{
    CONST_VTBL struct __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider __x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__







typedef struct __x_ABI_CWindows_CFoundation_CDateTime __x_ABI_CWindows_CFoundation_CDateTime;


typedef struct __x_ABI_CWindows_CFoundation_CPoint __x_ABI_CWindows_CFoundation_CPoint;


typedef struct __x_ABI_CWindows_CFoundation_CRect __x_ABI_CWindows_CFoundation_CRect;


typedef struct __x_ABI_CWindows_CFoundation_CSize __x_ABI_CWindows_CFoundation_CSize;


typedef struct __x_ABI_CWindows_CFoundation_CTimeSpan __x_ABI_CWindows_CFoundation_CTimeSpan;

#ifndef ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIUriRuntimeClass __x_ABI_CWindows_CFoundation_CIUriRuntimeClass;

#endif // ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__









typedef struct __x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName __x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName;






typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXamlBinaryWriterErrorInformation __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXamlBinaryWriterErrorInformation;


typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXmlnsDefinition __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXmlnsDefinition;


































/*
 *
 * Struct Microsoft.UI.Xaml.Markup.XamlBinaryWriterErrorInformation
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXamlBinaryWriterErrorInformation
{
    UINT32 InputStreamIndex;
    UINT32 LineNumber;
    UINT32 LinePosition;
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Markup.XmlnsDefinition
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXmlnsDefinition
{
    HSTRING XmlNamespace;
    HSTRING Namespace;
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IComponentConnector
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IComponentConnector[] = L"Microsoft.UI.Xaml.Markup.IComponentConnector";
/* [object, uuid("AD401812-B091-51D0-B915-2D682CD2AF10"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnectorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Connect )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This,
        /* [in] */INT32 connectionId,
        /* [in] */IInspectable * target
        );
    HRESULT ( STDMETHODCALLTYPE *GetBindingConnector )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * This,
        /* [in] */INT32 connectionId,
        /* [in] */IInspectable * target,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnectorVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnectorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_Connect(This,connectionId,target) \
    ( (This)->lpVtbl->Connect(This,connectionId,target) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_GetBindingConnector(This,connectionId,target,result) \
    ( (This)->lpVtbl->GetBindingConnector(This,connectionId,target,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIComponentConnector_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IDataTemplateComponent
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IDataTemplateComponent[] = L"Microsoft.UI.Xaml.Markup.IDataTemplateComponent";
/* [object, uuid("1743DDF7-38BA-58C9-A2A6-B0AE28713BEE"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponentVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Recycle )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This
        );
    HRESULT ( STDMETHODCALLTYPE *ProcessBindings )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * This,
        /* [in] */IInspectable * item,
        /* [in] */INT32 itemIndex,
        /* [in] */INT32 phase,
        /* [out] */INT32 * nextPhase
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponentVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_Recycle(This) \
    ( (This)->lpVtbl->Recycle(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_ProcessBindings(This,item,itemIndex,phase,nextPhase) \
    ( (This)->lpVtbl->ProcessBindings(This,item,itemIndex,phase,nextPhase) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IMarkupExtension
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IMarkupExtension[] = L"Microsoft.UI.Xaml.Markup.IMarkupExtension";
/* [object, uuid("C355371E-091D-5136-AF4A-BAF5E00616BD"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IMarkupExtensionFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IMarkupExtensionFactory[] = L"Microsoft.UI.Xaml.Markup.IMarkupExtensionFactory";
/* [object, uuid("20651AFA-5F3A-5F0C-ADB1-B6551F53A6A0"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtension * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IMarkupExtensionOverrides
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IMarkupExtensionOverrides[] = L"Microsoft.UI.Xaml.Markup.IMarkupExtensionOverrides";
/* [object, uuid("A12AA575-5D31-5B68-A30F-8495412A351D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverridesVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [overload] */HRESULT ( STDMETHODCALLTYPE *ProvideValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This,
        /* [retval, out] */IInspectable * * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *ProvideValueWithIXamlServiceProvider )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIXamlServiceProvider * serviceProvider,
        /* [retval, out] */IInspectable * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverridesVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverridesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_ProvideValue(This,result) \
    ( (This)->lpVtbl->ProvideValue(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_ProvideValueWithIXamlServiceProvider(This,serviceProvider,result) \
    ( (This)->lpVtbl->ProvideValueWithIXamlServiceProvider(This,serviceProvider,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIMarkupExtensionOverrides_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IProvideValueTarget
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IProvideValueTarget[] = L"Microsoft.UI.Xaml.Markup.IProvideValueTarget";
/* [object, uuid("3F01FF68-3EFD-591D-A506-DE13FCAABD83"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TargetObject )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TargetProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget * This,
        /* [retval, out] */IInspectable * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_get_TargetObject(This,value) \
    ( (This)->lpVtbl->get_TargetObject(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_get_TargetProperty(This,value) \
    ( (This)->lpVtbl->get_TargetProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTarget_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IProvideValueTargetProperty
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.ProvideValueTargetProperty
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IProvideValueTargetProperty[] = L"Microsoft.UI.Xaml.Markup.IProvideValueTargetProperty";
/* [object, uuid("CE777B1F-B42E-59D1-870D-12FDF0629133"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetPropertyVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Name )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Type )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DeclaringType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetPropertyVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetPropertyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_get_Name(This,value) \
    ( (This)->lpVtbl->get_Name(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_get_Type(This,value) \
    ( (This)->lpVtbl->get_Type(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_get_DeclaringType(This,value) \
    ( (This)->lpVtbl->get_DeclaringType(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIProvideValueTargetProperty_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IRootObjectProvider
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IRootObjectProvider[] = L"Microsoft.UI.Xaml.Markup.IRootObjectProvider";
/* [object, uuid("13D63599-352F-5EB8-81C1-BC62FB12D6DA"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProviderVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RootObject )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider * This,
        /* [retval, out] */IInspectable * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProviderVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_get_RootObject(This,value) \
    ( (This)->lpVtbl->get_RootObject(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIRootObjectProvider_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IUriContext
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IUriContext[] = L"Microsoft.UI.Xaml.Markup.IUriContext";
/* [object, uuid("FB8605F6-8F05-52EE-A01C-3A9E118A6EA2"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContextVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BaseUri )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContextVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContextVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_get_BaseUri(This,value) \
    ( (This)->lpVtbl->get_BaseUri(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIUriContext_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBinaryWriter
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBinaryWriter
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBinaryWriter[] = L"Microsoft.UI.Xaml.Markup.IXamlBinaryWriter";
/* [object, uuid("8FB45E3B-E689-55BF-AA11-D83B1C1CDDA1"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBinaryWriterStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBinaryWriter
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBinaryWriterStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlBinaryWriterStatics";
/* [object, uuid("774907FC-C846-517F-ABCC-C3F7E8C3FFC9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Write )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics * This,
        /* [in] */__FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * inputStreams,
        /* [in] */__FIVector_1_Windows__CStorage__CStreams__CIRandomAccessStream * outputStreams,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * xamlMetadataProvider,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXamlBinaryWriterErrorInformation * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_Write(This,inputStreams,outputStreams,xamlMetadataProvider,result) \
    ( (This)->lpVtbl->Write(This,inputStreams,outputStreams,xamlMetadataProvider,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBinaryWriterStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBindScopeDiagnostics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBindScopeDiagnostics[] = L"Microsoft.UI.Xaml.Markup.IXamlBindScopeDiagnostics";
/* [object, uuid("3EA84E4E-FDFE-55A8-A561-EDF5697846D7"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnosticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Disable )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics * This,
        /* [in] */INT32 lineNumber,
        /* [in] */INT32 columnNumber
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnosticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnosticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_Disable(This,lineNumber,columnNumber) \
    ( (This)->lpVtbl->Disable(This,lineNumber,columnNumber) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindScopeDiagnostics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBindingHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBindingHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBindingHelper[] = L"Microsoft.UI.Xaml.Markup.IXamlBindingHelper";
/* [object, uuid("607A9BF2-5A6D-5C89-A756-BB44F24F28F8"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlBindingHelperStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlBindingHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlBindingHelperStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlBindingHelperStatics";
/* [object, uuid("93C7DAD3-F9C2-5372-84DC-9E9C4661D083"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DataTemplateComponentProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetDataTemplateComponent )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject * element,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetDataTemplateComponent )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject * element,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIDataTemplateComponent * value
        );
    HRESULT ( STDMETHODCALLTYPE *SuspendRendering )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIUIElement * target
        );
    HRESULT ( STDMETHODCALLTYPE *ResumeRendering )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIUIElement * target
        );
    HRESULT ( STDMETHODCALLTYPE *ConvertValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName type,
        /* [in] */IInspectable * value,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromString )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */HSTRING value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromBoolean )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */boolean value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromChar16 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */WCHAR value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromDateTime )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */__x_ABI_CWindows_CFoundation_CDateTime value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromDouble )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */DOUBLE value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromInt32 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */INT32 value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromUInt32 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */UINT32 value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromInt64 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */INT64 value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromUInt64 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */UINT64 value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromSingle )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */FLOAT value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromPoint )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromRect )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */__x_ABI_CWindows_CFoundation_CRect value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromSize )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */__x_ABI_CWindows_CFoundation_CSize value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromTimeSpan )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */__x_ABI_CWindows_CFoundation_CTimeSpan value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromByte )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */BYTE value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromUri )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * value
        );
    HRESULT ( STDMETHODCALLTYPE *SetPropertyFromObject )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics * This,
        /* [in] */IInspectable * dependencyObject,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * propertyToSet,
        /* [in] */IInspectable * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_get_DataTemplateComponentProperty(This,value) \
    ( (This)->lpVtbl->get_DataTemplateComponentProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_GetDataTemplateComponent(This,element,result) \
    ( (This)->lpVtbl->GetDataTemplateComponent(This,element,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetDataTemplateComponent(This,element,value) \
    ( (This)->lpVtbl->SetDataTemplateComponent(This,element,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SuspendRendering(This,target) \
    ( (This)->lpVtbl->SuspendRendering(This,target) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_ResumeRendering(This,target) \
    ( (This)->lpVtbl->ResumeRendering(This,target) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_ConvertValue(This,type,value,result) \
    ( (This)->lpVtbl->ConvertValue(This,type,value,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromString(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromString(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromBoolean(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromBoolean(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromChar16(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromChar16(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromDateTime(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromDateTime(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromDouble(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromDouble(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromInt32(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromInt32(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromUInt32(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromUInt32(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromInt64(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromInt64(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromUInt64(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromUInt64(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromSingle(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromSingle(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromPoint(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromPoint(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromRect(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromRect(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromSize(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromSize(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromTimeSpan(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromTimeSpan(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromByte(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromByte(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromUri(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromUri(This,dependencyObject,propertyToSet,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_SetPropertyFromObject(This,dependencyObject,propertyToSet,value) \
    ( (This)->lpVtbl->SetPropertyFromObject(This,dependencyObject,propertyToSet,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlBindingHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMarkupHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlMarkupHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMarkupHelper[] = L"Microsoft.UI.Xaml.Markup.IXamlMarkupHelper";
/* [object, uuid("CD677310-3B06-5A13-B31A-401849570858"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMarkupHelperStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlMarkupHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMarkupHelperStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlMarkupHelperStatics";
/* [object, uuid("D9A0F6E3-C6CC-5CB6-8999-85788701F339"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *UnloadObject )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject * element
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_UnloadObject(This,element) \
    ( (This)->lpVtbl->UnloadObject(This,element) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMarkupHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMember
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMember[] = L"Microsoft.UI.Xaml.Markup.IXamlMember";
/* [object, uuid("BF3A2913-5C63-50EC-8660-61809BE7B9B9"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMemberVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsAttachable )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsDependencyProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsReadOnly )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Name )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TargetType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Type )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [in] */IInspectable * instance,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * This,
        /* [in] */IInspectable * instance,
        /* [in] */IInspectable * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMemberVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMemberVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_get_IsAttachable(This,value) \
    ( (This)->lpVtbl->get_IsAttachable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_get_IsDependencyProperty(This,value) \
    ( (This)->lpVtbl->get_IsDependencyProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_get_IsReadOnly(This,value) \
    ( (This)->lpVtbl->get_IsReadOnly(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_get_Name(This,value) \
    ( (This)->lpVtbl->get_Name(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_get_TargetType(This,value) \
    ( (This)->lpVtbl->get_TargetType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_get_Type(This,value) \
    ( (This)->lpVtbl->get_Type(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_GetValue(This,instance,result) \
    ( (This)->lpVtbl->GetValue(This,instance,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_SetValue(This,instance,value) \
    ( (This)->lpVtbl->SetValue(This,instance,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlMetadataProvider
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlMetadataProvider[] = L"Microsoft.UI.Xaml.Markup.IXamlMetadataProvider";
/* [object, uuid("A96251F0-2214-5D53-8746-CE99A2593CD7"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProviderVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [overload, default_overload] */HRESULT ( STDMETHODCALLTYPE *GetXamlType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
        /* [in] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName type,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * result
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *GetXamlTypeByFullName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
        /* [in] */HSTRING fullName,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetXmlnsDefinitions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider * This,
        /* [out] */UINT32 * __resultSize,
        /* [size_is(, *(__resultSize)), retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CXmlnsDefinition * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProviderVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_GetXamlType(This,type,result) \
    ( (This)->lpVtbl->GetXamlType(This,type,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_GetXamlTypeByFullName(This,fullName,result) \
    ( (This)->lpVtbl->GetXamlTypeByFullName(This,fullName,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_GetXmlnsDefinitions(This,__resultSize,result) \
    ( (This)->lpVtbl->GetXmlnsDefinitions(This,__resultSize,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMetadataProvider_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlReader
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlReader
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlReader[] = L"Microsoft.UI.Xaml.Markup.IXamlReader";
/* [object, uuid("54CE54C8-38C6-50D9-AC98-4B03EDDBDE9F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReader_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlReaderStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Markup.XamlReader
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlReaderStatics[] = L"Microsoft.UI.Xaml.Markup.IXamlReaderStatics";
/* [object, uuid("82A4CD9E-435E-5AEB-8C4F-300CECE45CAE"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Load )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This,
        /* [in] */HSTRING xaml,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *LoadWithInitialTemplateValidation )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics * This,
        /* [in] */HSTRING xaml,
        /* [retval, out] */IInspectable * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_Load(This,xaml,result) \
    ( (This)->lpVtbl->Load(This,xaml,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_LoadWithInitialTemplateValidation(This,xaml,result) \
    ( (This)->lpVtbl->LoadWithInitialTemplateValidation(This,xaml,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlReaderStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlType
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlType[] = L"Microsoft.UI.Xaml.Markup.IXamlType";
/* [object, uuid("D24219DF-7EC9-57F1-A27B-6AF251D9C5BC"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BaseType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ContentProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FullName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsArray )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsCollection )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsConstructible )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsDictionary )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsMarkupExtension )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsBindable )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ItemType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_KeyType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BoxedType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UnderlyingType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    HRESULT ( STDMETHODCALLTYPE *ActivateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateFromString )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [in] */HSTRING value,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetMember )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [in] */HSTRING name,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlMember * * result
        );
    HRESULT ( STDMETHODCALLTYPE *AddToVector )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [in] */IInspectable * instance,
        /* [in] */IInspectable * value
        );
    HRESULT ( STDMETHODCALLTYPE *AddToMap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This,
        /* [in] */IInspectable * instance,
        /* [in] */IInspectable * key,
        /* [in] */IInspectable * value
        );
    HRESULT ( STDMETHODCALLTYPE *RunInitializer )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_BaseType(This,value) \
    ( (This)->lpVtbl->get_BaseType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_ContentProperty(This,value) \
    ( (This)->lpVtbl->get_ContentProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_FullName(This,value) \
    ( (This)->lpVtbl->get_FullName(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_IsArray(This,value) \
    ( (This)->lpVtbl->get_IsArray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_IsCollection(This,value) \
    ( (This)->lpVtbl->get_IsCollection(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_IsConstructible(This,value) \
    ( (This)->lpVtbl->get_IsConstructible(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_IsDictionary(This,value) \
    ( (This)->lpVtbl->get_IsDictionary(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_IsMarkupExtension(This,value) \
    ( (This)->lpVtbl->get_IsMarkupExtension(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_IsBindable(This,value) \
    ( (This)->lpVtbl->get_IsBindable(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_ItemType(This,value) \
    ( (This)->lpVtbl->get_ItemType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_KeyType(This,value) \
    ( (This)->lpVtbl->get_KeyType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_BoxedType(This,value) \
    ( (This)->lpVtbl->get_BoxedType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_get_UnderlyingType(This,value) \
    ( (This)->lpVtbl->get_UnderlyingType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_ActivateInstance(This,result) \
    ( (This)->lpVtbl->ActivateInstance(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_CreateFromString(This,value,result) \
    ( (This)->lpVtbl->CreateFromString(This,value,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_GetMember(This,name,result) \
    ( (This)->lpVtbl->GetMember(This,name,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_AddToVector(This,instance,value) \
    ( (This)->lpVtbl->AddToVector(This,instance,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_AddToMap(This,instance,key,value) \
    ( (This)->lpVtbl->AddToMap(This,instance,key,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_RunInitializer(This) \
    ( (This)->lpVtbl->RunInitializer(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlType_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Markup.IXamlTypeResolver
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Markup_IXamlTypeResolver[] = L"Microsoft.UI.Xaml.Markup.IXamlTypeResolver";
/* [object, uuid("3FA15615-CACF-547F-B1ED-89DAE8C67452"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolverVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Resolve )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver * This,
        /* [in] */HSTRING qualifiedTypeName,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolverVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolverVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_Resolve(This,qualifiedTypeName,result) \
    ( (This)->lpVtbl->Resolve(This,qualifiedTypeName,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMarkup_CIXamlTypeResolver_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.MarkupExtension
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IMarkupExtension ** Default Interface **
 *    Microsoft.UI.Xaml.Markup.IMarkupExtensionOverrides
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_MarkupExtension_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_MarkupExtension_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_MarkupExtension[] = L"Microsoft.UI.Xaml.Markup.MarkupExtension";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.ProvideValueTargetProperty
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IProvideValueTargetProperty ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_ProvideValueTargetProperty_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_ProvideValueTargetProperty_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_ProvideValueTargetProperty[] = L"Microsoft.UI.Xaml.Markup.ProvideValueTargetProperty";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlBinaryWriter
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlBinaryWriterStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlBinaryWriter ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBinaryWriter_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBinaryWriter_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlBinaryWriter[] = L"Microsoft.UI.Xaml.Markup.XamlBinaryWriter";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlBindingHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlBindingHelperStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlBindingHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBindingHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlBindingHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlBindingHelper[] = L"Microsoft.UI.Xaml.Markup.XamlBindingHelper";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlMarkupHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlMarkupHelperStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlMarkupHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlMarkupHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlMarkupHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlMarkupHelper[] = L"Microsoft.UI.Xaml.Markup.XamlMarkupHelper";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Markup.XamlReader
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Markup.IXamlReaderStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Markup.IXamlReader ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlReader_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Markup_XamlReader_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Markup_XamlReader[] = L"Microsoft.UI.Xaml.Markup.XamlReader";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EMarkup_p_h__

#endif // __Microsoft2EUI2EXaml2EMarkup_h__
