/* Header file automatically generated from Microsoft.UI.Xaml.Data.idl */
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
#ifndef __Microsoft2EUI2EXaml2EData_h__
#define __Microsoft2EUI2EXaml2EData_h__
#ifndef __Microsoft2EUI2EXaml2EData_p_h__
#define __Microsoft2EUI2EXaml2EData_p_h__


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
#include "Windows.UI.Xaml.Interop.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICurrentChangingEventHandler;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IPropertyChangedEventHandler;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler ABI::Microsoft::UI::Xaml::Data::IPropertyChangedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBinding;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding ABI::Microsoft::UI::Xaml::Data::IBinding

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingBase;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase ABI::Microsoft::UI::Xaml::Data::IBindingBase

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingBaseFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory ABI::Microsoft::UI::Xaml::Data::IBindingBaseFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingExpression;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression ABI::Microsoft::UI::Xaml::Data::IBindingExpression

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingExpressionBase;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase ABI::Microsoft::UI::Xaml::Data::IBindingExpressionBase

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingExpressionBaseFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory ABI::Microsoft::UI::Xaml::Data::IBindingExpressionBaseFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingExpressionFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory ABI::Microsoft::UI::Xaml::Data::IBindingExpressionFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory ABI::Microsoft::UI::Xaml::Data::IBindingFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingOperations;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations ABI::Microsoft::UI::Xaml::Data::IBindingOperations

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IBindingOperationsStatics;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics ABI::Microsoft::UI::Xaml::Data::IBindingOperationsStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICollectionView;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView ABI::Microsoft::UI::Xaml::Data::ICollectionView

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICollectionViewFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory ABI::Microsoft::UI::Xaml::Data::ICollectionViewFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICollectionViewGroup;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup ABI::Microsoft::UI::Xaml::Data::ICollectionViewGroup

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICollectionViewSource;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource ABI::Microsoft::UI::Xaml::Data::ICollectionViewSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICollectionViewSourceStatics;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics ABI::Microsoft::UI::Xaml::Data::ICollectionViewSourceStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICurrentChangingEventArgs;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICurrentChangingEventArgsFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventArgsFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICustomProperty;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty ABI::Microsoft::UI::Xaml::Data::ICustomProperty

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ICustomPropertyProvider;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider ABI::Microsoft::UI::Xaml::Data::ICustomPropertyProvider

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IDataErrorsChangedEventArgs;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs ABI::Microsoft::UI::Xaml::Data::IDataErrorsChangedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IDataErrorsChangedEventArgsFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory ABI::Microsoft::UI::Xaml::Data::IDataErrorsChangedEventArgsFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IItemIndexRange;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange ABI::Microsoft::UI::Xaml::Data::IItemIndexRange

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IItemIndexRangeFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory ABI::Microsoft::UI::Xaml::Data::IItemIndexRangeFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IItemsRangeInfo;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo ABI::Microsoft::UI::Xaml::Data::IItemsRangeInfo

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface INotifyDataErrorInfo;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo ABI::Microsoft::UI::Xaml::Data::INotifyDataErrorInfo

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface INotifyPropertyChanged;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged ABI::Microsoft::UI::Xaml::Data::INotifyPropertyChanged

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IPropertyChangedEventArgs;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs ABI::Microsoft::UI::Xaml::Data::IPropertyChangedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IPropertyChangedEventArgsFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory ABI::Microsoft::UI::Xaml::Data::IPropertyChangedEventArgsFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IRelativeSource;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource ABI::Microsoft::UI::Xaml::Data::IRelativeSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IRelativeSourceFactory;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory ABI::Microsoft::UI::Xaml::Data::IRelativeSourceFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ISelectionInfo;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo ABI::Microsoft::UI::Xaml::Data::ISelectionInfo

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface ISupportIncrementalLoading;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading ABI::Microsoft::UI::Xaml::Data::ISupportIncrementalLoading

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    interface IValueConverter;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter ABI::Microsoft::UI::Xaml::Data::IValueConverter

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class ItemIndexRange;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE
#define DEF___FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("8cbc4e1b-9d17-5fec-bbb0-0b25601a4e2d"))
IIterator<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*, ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Microsoft.UI.Xaml.Data.ItemIndexRange>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*> __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t;
#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange ABI::Windows::Foundation::Collections::__FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>
//#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE
#define DEF___FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("6f328b92-1eb5-54df-b756-e8d9fd9abea5"))
IIterable<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*, ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Microsoft.UI.Xaml.Data.ItemIndexRange>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*> __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t;
#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange ABI::Windows::Foundation::Collections::__FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>
//#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE
#define DEF___FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("465af292-d97c-5407-9cbe-6d35d67a030f"))
IVectorView<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*, ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Microsoft.UI.Xaml.Data.ItemIndexRange>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Microsoft::UI::Xaml::Data::ItemIndexRange*> __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t;
#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange ABI::Windows::Foundation::Collections::__FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>
//#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Xaml::Data::IItemIndexRange*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class DataErrorsChangedEventArgs;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_USE
#define DEF___FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("aba7f52e-2991-5bcd-bcb8-24145b8f2649"))
IEventHandler<ABI::Microsoft::UI::Xaml::Data::DataErrorsChangedEventArgs*> : IEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Data::DataErrorsChangedEventArgs*, ABI::Microsoft::UI::Xaml::Data::IDataErrorsChangedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.EventHandler`1<Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IEventHandler<ABI::Microsoft::UI::Xaml::Data::DataErrorsChangedEventArgs*> __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_t;
#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs ABI::Windows::Foundation::__FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs ABI::Windows::Foundation::IEventHandler<ABI::Microsoft::UI::Xaml::Data::IDataErrorsChangedEventArgs*>
//#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_t ABI::Windows::Foundation::IEventHandler<ABI::Microsoft::UI::Xaml::Data::IDataErrorsChangedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    struct LoadMoreItemsResult;
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#ifndef DEF___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_USE
#define DEF___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("8806a4d7-81d6-50f6-9128-52a9534febe1"))
IAsyncOperationCompletedHandler<struct ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> : IAsyncOperationCompletedHandler_impl<struct ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Microsoft.UI.Xaml.Data.LoadMoreItemsResult>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperationCompletedHandler<struct ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_t;
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult>
//#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_t ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_USE */





#ifndef DEF___FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_USE
#define DEF___FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("87c6d0a7-9748-5f9c-b359-1e12759cf3ce"))
IAsyncOperation<struct ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> : IAsyncOperation_impl<struct ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.IAsyncOperation`1<Microsoft.UI.Xaml.Data.LoadMoreItemsResult>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperation<struct ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_t;
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult ABI::Windows::Foundation::__FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult ABI::Windows::Foundation::IAsyncOperation<ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult>
//#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_t ABI::Windows::Foundation::IAsyncOperation<ABI::Microsoft::UI::Xaml::Data::LoadMoreItemsResult>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_USE */





#ifndef DEF___FIIterator_1_IInspectable_USE
#define DEF___FIIterator_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("44a94f2d-04f8-5091-b336-be7892dd10be"))
IIterator<IInspectable*> : IIterator_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<IInspectable*> __FIIterator_1_IInspectable_t;
#define __FIIterator_1_IInspectable ABI::Windows::Foundation::Collections::__FIIterator_1_IInspectable_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_IInspectable ABI::Windows::Foundation::Collections::IIterator<IInspectable*>
//#define __FIIterator_1_IInspectable_t ABI::Windows::Foundation::Collections::IIterator<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_IInspectable_USE */





#ifndef DEF___FIIterable_1_IInspectable_USE
#define DEF___FIIterable_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("092b849b-60b1-52be-a44a-6fe8e933cbe4"))
IIterable<IInspectable*> : IIterable_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<IInspectable*> __FIIterable_1_IInspectable_t;
#define __FIIterable_1_IInspectable ABI::Windows::Foundation::Collections::__FIIterable_1_IInspectable_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_IInspectable ABI::Windows::Foundation::Collections::IIterable<IInspectable*>
//#define __FIIterable_1_IInspectable_t ABI::Windows::Foundation::Collections::IIterable<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_IInspectable_USE */





#ifndef DEF___FIVectorView_1_IInspectable_USE
#define DEF___FIVectorView_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("a6487363-b074-5c60-ab16-866dce4ee54d"))
IVectorView<IInspectable*> : IVectorView_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<IInspectable*> __FIVectorView_1_IInspectable_t;
#define __FIVectorView_1_IInspectable ABI::Windows::Foundation::Collections::__FIVectorView_1_IInspectable_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_IInspectable ABI::Windows::Foundation::Collections::IVectorView<IInspectable*>
//#define __FIVectorView_1_IInspectable_t ABI::Windows::Foundation::Collections::IVectorView<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_IInspectable_USE */





#ifndef DEF___FIVector_1_IInspectable_USE
#define DEF___FIVector_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("b32bdca4-5e52-5b27-bc5d-d66a1a268c2a"))
IVector<IInspectable*> : IVector_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<IInspectable*> __FIVector_1_IInspectable_t;
#define __FIVector_1_IInspectable ABI::Windows::Foundation::Collections::__FIVector_1_IInspectable_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_IInspectable ABI::Windows::Foundation::Collections::IVector<IInspectable*>
//#define __FIVector_1_IInspectable_t ABI::Windows::Foundation::Collections::IVector<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_IInspectable_USE */





#ifndef DEF___FVectorChangedEventHandler_1_IInspectable_USE
#define DEF___FVectorChangedEventHandler_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("b423a801-d35e-56b9-813b-00889536cb98"))
VectorChangedEventHandler<IInspectable*> : VectorChangedEventHandler_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.VectorChangedEventHandler`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef VectorChangedEventHandler<IInspectable*> __FVectorChangedEventHandler_1_IInspectable_t;
#define __FVectorChangedEventHandler_1_IInspectable ABI::Windows::Foundation::Collections::__FVectorChangedEventHandler_1_IInspectable_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FVectorChangedEventHandler_1_IInspectable ABI::Windows::Foundation::Collections::VectorChangedEventHandler<IInspectable*>
//#define __FVectorChangedEventHandler_1_IInspectable_t ABI::Windows::Foundation::Collections::VectorChangedEventHandler<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FVectorChangedEventHandler_1_IInspectable_USE */





#ifndef DEF___FIObservableVector_1_IInspectable_USE
#define DEF___FIObservableVector_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("7b81c56a-0985-518d-baa9-0da9ae009f65"))
IObservableVector<IInspectable*> : IObservableVector_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IObservableVector`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IObservableVector<IInspectable*> __FIObservableVector_1_IInspectable_t;
#define __FIObservableVector_1_IInspectable ABI::Windows::Foundation::Collections::__FIObservableVector_1_IInspectable_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIObservableVector_1_IInspectable ABI::Windows::Foundation::Collections::IObservableVector<IInspectable*>
//#define __FIObservableVector_1_IInspectable_t ABI::Windows::Foundation::Collections::IObservableVector<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIObservableVector_1_IInspectable_USE */





#ifndef DEF___FIEventHandler_1_IInspectable_USE
#define DEF___FIEventHandler_1_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("c50898f6-c536-5f47-8583-8b2c2438a13b"))
IEventHandler<IInspectable*> : IEventHandler_impl<IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.EventHandler`1<Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IEventHandler<IInspectable*> __FIEventHandler_1_IInspectable_t;
#define __FIEventHandler_1_IInspectable ABI::Windows::Foundation::__FIEventHandler_1_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIEventHandler_1_IInspectable ABI::Windows::Foundation::IEventHandler<IInspectable*>
//#define __FIEventHandler_1_IInspectable_t ABI::Windows::Foundation::IEventHandler<IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIEventHandler_1_IInspectable_USE */





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


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                class PropertyPath;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IPropertyPath;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath ABI::Microsoft::UI::Xaml::IPropertyPath

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath_FWD_DEFINED__






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
                namespace Data {
                    
                    typedef enum BindingMode : int BindingMode;
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    
                    typedef enum RelativeSourceMode : int RelativeSourceMode;
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    
                    typedef enum UpdateSourceTrigger : int UpdateSourceTrigger;
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    
                    typedef struct LoadMoreItemsResult LoadMoreItemsResult;
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */




































namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class Binding;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class BindingBase;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class BindingExpression;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class BindingExpressionBase;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class BindingOperations;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class CollectionViewSource;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class CurrentChangingEventArgs;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class PropertyChangedEventArgs;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    class RelativeSource;
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */












/*
 *
 * Struct Microsoft.UI.Xaml.Data.BindingMode
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
                namespace Data {
                    /* [v1_enum, contract] */
                    enum BindingMode : int
                    {
                        BindingMode_OneWay = 1,
                        BindingMode_OneTime = 2,
                        BindingMode_TwoWay = 3,
                    };
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Data.RelativeSourceMode
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
                namespace Data {
                    /* [v1_enum, contract] */
                    enum RelativeSourceMode : int
                    {
                        RelativeSourceMode_None = 0,
                        RelativeSourceMode_TemplatedParent = 1,
                        RelativeSourceMode_Self = 2,
                    };
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Data.UpdateSourceTrigger
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
                namespace Data {
                    /* [v1_enum, contract] */
                    enum UpdateSourceTrigger : int
                    {
                        UpdateSourceTrigger_Default = 0,
                        UpdateSourceTrigger_PropertyChanged = 1,
                        UpdateSourceTrigger_Explicit = 2,
                        UpdateSourceTrigger_LostFocus = 3,
                    };
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Data.LoadMoreItemsResult
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
                namespace Data {
                    /* [contract] */
                    struct LoadMoreItemsResult
                    {
                        UINT32 Count;
                    };
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Data.CurrentChangingEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("3D2A98DD-95B3-5FD5-93B4-A1A2599F225C"), contract] */
                    MIDL_INTERFACE("3D2A98DD-95B3-5FD5-93B4-A1A2599F225C")
                    ICurrentChangingEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICurrentChangingEventHandler=_uuidof(ICurrentChangingEventHandler);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Data.PropertyChangedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("E3DE52F6-1E32-5DA6-BB2D-B5B6096C962D"), contract] */
                    MIDL_INTERFACE("E3DE52F6-1E32-5DA6-BB2D-B5B6096C962D")
                    IPropertyChangedEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IPropertyChangedEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPropertyChangedEventHandler=_uuidof(IPropertyChangedEventHandler);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBinding
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.Binding
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBinding[] = L"Microsoft.UI.Xaml.Data.IBinding";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("501EA0E8-EDD4-59DE-8845-76AF2EABBE00"), exclusiveto, contract] */
                    MIDL_INTERFACE("501EA0E8-EDD4-59DE-8845-76AF2EABBE00")
                    IBinding : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Path(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IPropertyPath * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Path(
                            /* [in] */ABI::Microsoft::UI::Xaml::IPropertyPath * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Mode(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::BindingMode * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Mode(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::BindingMode value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Source(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Source(
                            /* [in] */IInspectable * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RelativeSource(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IRelativeSource * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RelativeSource(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IRelativeSource * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ElementName(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ElementName(
                            /* [in] */HSTRING value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Converter(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IValueConverter * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Converter(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IValueConverter * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ConverterParameter(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ConverterParameter(
                            /* [in] */IInspectable * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ConverterLanguage(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ConverterLanguage(
                            /* [in] */HSTRING value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FallbackValue(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FallbackValue(
                            /* [in] */IInspectable * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TargetNullValue(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TargetNullValue(
                            /* [in] */IInspectable * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UpdateSourceTrigger(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::UpdateSourceTrigger * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_UpdateSourceTrigger(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::UpdateSourceTrigger value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBinding=_uuidof(IBinding);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingBase[] = L"Microsoft.UI.Xaml.Data.IBindingBase";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("91DDD141-5944-50EF-B85E-218E463F7A73"), exclusiveto, contract] */
                    MIDL_INTERFACE("91DDD141-5944-50EF-B85E-218E463F7A73")
                    IBindingBase : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingBase=_uuidof(IBindingBase);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingBaseFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingBaseFactory[] = L"Microsoft.UI.Xaml.Data.IBindingBaseFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("C8A866C5-F6F3-5F7A-9592-D385AF48BD8F"), exclusiveto, contract] */
                    MIDL_INTERFACE("C8A866C5-F6F3-5F7A-9592-D385AF48BD8F")
                    IBindingBaseFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IBindingBase * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingBaseFactory=_uuidof(IBindingBaseFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpression
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpression
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpression[] = L"Microsoft.UI.Xaml.Data.IBindingExpression";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("4C023916-37BC-5B07-BC9D-15C547BD9B26"), exclusiveto, contract] */
                    MIDL_INTERFACE("4C023916-37BC-5B07-BC9D-15C547BD9B26")
                    IBindingExpression : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DataItem(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ParentBinding(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IBinding * * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE UpdateSource(void) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingExpression=_uuidof(IBindingExpression);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpressionBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpressionBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpressionBase[] = L"Microsoft.UI.Xaml.Data.IBindingExpressionBase";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("8825E5A9-D9A3-5E87-BCD8-C63133D29029"), exclusiveto, contract] */
                    MIDL_INTERFACE("8825E5A9-D9A3-5E87-BCD8-C63133D29029")
                    IBindingExpressionBase : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingExpressionBase=_uuidof(IBindingExpressionBase);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpressionBaseFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpressionBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpressionBaseFactory[] = L"Microsoft.UI.Xaml.Data.IBindingExpressionBaseFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("41D643B9-2629-5451-A716-596C0848B5DC"), exclusiveto, contract] */
                    MIDL_INTERFACE("41D643B9-2629-5451-A716-596C0848B5DC")
                    IBindingExpressionBaseFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingExpressionBaseFactory=_uuidof(IBindingExpressionBaseFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpressionFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpression
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpressionFactory[] = L"Microsoft.UI.Xaml.Data.IBindingExpressionFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("086CAE14-81A1-588B-B619-05EE84C0F089"), exclusiveto, contract] */
                    MIDL_INTERFACE("086CAE14-81A1-588B-B619-05EE84C0F089")
                    IBindingExpressionFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingExpressionFactory=_uuidof(IBindingExpressionFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.Binding
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingFactory[] = L"Microsoft.UI.Xaml.Data.IBindingFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("CB2DE749-B115-5F67-B64A-797D54885D5C"), exclusiveto, contract] */
                    MIDL_INTERFACE("CB2DE749-B115-5F67-B64A-797D54885D5C")
                    IBindingFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IBinding * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingFactory=_uuidof(IBindingFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingOperations
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingOperations
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingOperations[] = L"Microsoft.UI.Xaml.Data.IBindingOperations";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("9A319B95-AABE-5075-B227-8EB07E443D8B"), exclusiveto, contract] */
                    MIDL_INTERFACE("9A319B95-AABE-5075-B227-8EB07E443D8B")
                    IBindingOperations : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingOperations=_uuidof(IBindingOperations);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingOperationsStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingOperations
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingOperationsStatics[] = L"Microsoft.UI.Xaml.Data.IBindingOperationsStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("1E1BDBD3-FCA5-5C85-B87D-B504CD8FA8AC"), exclusiveto, contract] */
                    MIDL_INTERFACE("1E1BDBD3-FCA5-5C85-B87D-B504CD8FA8AC")
                    IBindingOperationsStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE SetBinding(
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyObject * target,
                            /* [in] */ABI::Microsoft::UI::Xaml::IDependencyProperty * dp,
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IBindingBase * binding
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindingOperationsStatics=_uuidof(IBindingOperationsStatics);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionView
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Windows.Foundation.Collections.IObservableVector_1_IInspectable
 *     Windows.Foundation.Collections.IVector_1_IInspectable
 *     Windows.Foundation.Collections.IIterable_1_IInspectable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionView[] = L"Microsoft.UI.Xaml.Data.ICollectionView";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("F8BB90D8-E008-5D65-8C97-7BB790A4230C"), contract] */
                    MIDL_INTERFACE("F8BB90D8-E008-5D65-8C97-7BB790A4230C")
                    ICollectionView : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CurrentItem(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CurrentPosition(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsCurrentAfterLast(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsCurrentBeforeFirst(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CollectionGroups(
                            /* [retval, out] */__FIObservableVector_1_IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasMoreItems(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_CurrentChanged(
                            /* [in] */__FIEventHandler_1_IInspectable * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_CurrentChanged(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_CurrentChanging(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_CurrentChanging(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveCurrentTo(
                            /* [in] */IInspectable * item,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveCurrentToPosition(
                            /* [in] */INT32 index,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveCurrentToFirst(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveCurrentToLast(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveCurrentToNext(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveCurrentToPrevious(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE LoadMoreItemsAsync(
                            /* [in] */UINT32 count,
                            /* [retval, out] */__FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * * operation
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICollectionView=_uuidof(ICollectionView);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewFactory[] = L"Microsoft.UI.Xaml.Data.ICollectionViewFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("D971F795-5728-5BEF-9602-43F2C4250E56"), contract] */
                    MIDL_INTERFACE("D971F795-5728-5BEF-9602-43F2C4250E56")
                    ICollectionViewFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateView(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::ICollectionView * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICollectionViewFactory=_uuidof(ICollectionViewFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewGroup
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewGroup[] = L"Microsoft.UI.Xaml.Data.ICollectionViewGroup";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("96A08DA8-BE38-5AE0-903D-6FB6111E61F5"), contract] */
                    MIDL_INTERFACE("96A08DA8-BE38-5AE0-903D-6FB6111E61F5")
                    ICollectionViewGroup : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Group(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_GroupItems(
                            /* [retval, out] */__FIObservableVector_1_IInspectable * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICollectionViewGroup=_uuidof(ICollectionViewGroup);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CollectionViewSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewSource[] = L"Microsoft.UI.Xaml.Data.ICollectionViewSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("A45E3B3A-F31E-5BBB-8A7C-70CF5C64BC3F"), exclusiveto, contract] */
                    MIDL_INTERFACE("A45E3B3A-F31E-5BBB-8A7C-70CF5C64BC3F")
                    ICollectionViewSource : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Source(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Source(
                            /* [in] */IInspectable * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_View(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::ICollectionView * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsSourceGrouped(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsSourceGrouped(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ItemsPath(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IPropertyPath * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ItemsPath(
                            /* [in] */ABI::Microsoft::UI::Xaml::IPropertyPath * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICollectionViewSource=_uuidof(ICollectionViewSource);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewSourceStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CollectionViewSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewSourceStatics[] = L"Microsoft.UI.Xaml.Data.ICollectionViewSourceStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("E282F10F-D4B1-5769-8A11-30F739E6113B"), exclusiveto, contract] */
                    MIDL_INTERFACE("E282F10F-D4B1-5769-8A11-30F739E6113B")
                    ICollectionViewSourceStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SourceProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ViewProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsSourceGroupedProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ItemsPathProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICollectionViewSourceStatics=_uuidof(ICollectionViewSourceStatics);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICurrentChangingEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CurrentChangingEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICurrentChangingEventArgs[] = L"Microsoft.UI.Xaml.Data.ICurrentChangingEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("63E42ED6-E14A-51EA-9CB1-72F9C907DC80"), exclusiveto, contract] */
                    MIDL_INTERFACE("63E42ED6-E14A-51EA-9CB1-72F9C907DC80")
                    ICurrentChangingEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cancel(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Cancel(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsCancelable(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICurrentChangingEventArgs=_uuidof(ICurrentChangingEventArgs);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICurrentChangingEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CurrentChangingEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICurrentChangingEventArgsFactory[] = L"Microsoft.UI.Xaml.Data.ICurrentChangingEventArgsFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("3670F48A-AC2C-5352-8A4B-6B977A08E5F8"), exclusiveto, contract] */
                    MIDL_INTERFACE("3670F48A-AC2C-5352-8A4B-6B977A08E5F8")
                    ICurrentChangingEventArgsFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventArgs * * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE CreateWithCancelableParameter(
                            /* [in] */::boolean isCancelable,
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::ICurrentChangingEventArgs * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICurrentChangingEventArgsFactory=_uuidof(ICurrentChangingEventArgsFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICustomProperty
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICustomProperty[] = L"Microsoft.UI.Xaml.Data.ICustomProperty";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("30DA92C0-23E8-42A0-AE7C-734A0E5D2782"), contract] */
                    MIDL_INTERFACE("30DA92C0-23E8-42A0-AE7C-734A0E5D2782")
                    ICustomProperty : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Type(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Name(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetValue(
                            /* [in] */IInspectable * target,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetValue(
                            /* [in] */IInspectable * target,
                            /* [in] */IInspectable * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetIndexedValue(
                            /* [in] */IInspectable * target,
                            /* [in] */IInspectable * index,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetIndexedValue(
                            /* [in] */IInspectable * target,
                            /* [in] */IInspectable * value,
                            /* [in] */IInspectable * index
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CanWrite(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CanRead(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICustomProperty=_uuidof(ICustomProperty);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICustomPropertyProvider
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICustomPropertyProvider[] = L"Microsoft.UI.Xaml.Data.ICustomPropertyProvider";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("7C925755-3E48-42B4-8677-76372267033F"), contract] */
                    MIDL_INTERFACE("7C925755-3E48-42B4-8677-76372267033F")
                    ICustomPropertyProvider : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE GetCustomProperty(
                            /* [in] */HSTRING name,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::ICustomProperty * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetIndexedProperty(
                            /* [in] */HSTRING name,
                            /* [in] */ABI::Windows::UI::Xaml::Interop::TypeName type,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::ICustomProperty * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetStringRepresentation(
                            /* [retval, out] */HSTRING * result
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Type(
                            /* [retval, out] */ABI::Windows::UI::Xaml::Interop::TypeName * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ICustomPropertyProvider=_uuidof(ICustomPropertyProvider);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IDataErrorsChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("D026DD64-5F26-5F15-A86A-0DEC8A431796"), exclusiveto, contract] */
                    MIDL_INTERFACE("D026DD64-5F26-5F15-A86A-0DEC8A431796")
                    IDataErrorsChangedEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PropertyName(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PropertyName(
                            /* [in] */HSTRING value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IDataErrorsChangedEventArgs=_uuidof(IDataErrorsChangedEventArgs);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IDataErrorsChangedEventArgsFactory[] = L"Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgsFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("62D0BD1E-B85F-5FCC-842A-7CB0DDA37FE5"), exclusiveto, contract] */
                    MIDL_INTERFACE("62D0BD1E-B85F-5FCC-842A-7CB0DDA37FE5")
                    IDataErrorsChangedEventArgsFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */HSTRING name,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IDataErrorsChangedEventArgs * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IDataErrorsChangedEventArgsFactory=_uuidof(IDataErrorsChangedEventArgsFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IItemIndexRange
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.ItemIndexRange
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IItemIndexRange[] = L"Microsoft.UI.Xaml.Data.IItemIndexRange";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("EBA09846-2554-5B86-AC17-614F05105FA2"), exclusiveto, contract] */
                    MIDL_INTERFACE("EBA09846-2554-5B86-AC17-614F05105FA2")
                    IItemIndexRange : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FirstIndex(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Length(
                            /* [retval, out] */UINT32 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LastIndex(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IItemIndexRange=_uuidof(IItemIndexRange);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IItemIndexRangeFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.ItemIndexRange
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IItemIndexRangeFactory[] = L"Microsoft.UI.Xaml.Data.IItemIndexRangeFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("9FC73213-EDA0-5238-AA2C-401C9921F0F9"), exclusiveto, contract] */
                    MIDL_INTERFACE("9FC73213-EDA0-5238-AA2C-401C9921F0F9")
                    IItemIndexRangeFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */INT32 firstIndex,
                            /* [in] */UINT32 length,
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IItemIndexRange * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IItemIndexRangeFactory=_uuidof(IItemIndexRangeFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IItemsRangeInfo
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Windows.Foundation.IClosable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IItemsRangeInfo[] = L"Microsoft.UI.Xaml.Data.IItemsRangeInfo";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("B8376D08-85FB-563B-8273-39EF2D138256"), contract] */
                    MIDL_INTERFACE("B8376D08-85FB-563B-8273-39EF2D138256")
                    IItemsRangeInfo : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE RangesChanged(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IItemIndexRange * visibleRange,
                            /* [in] */__FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * trackedItems
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IItemsRangeInfo=_uuidof(IItemsRangeInfo);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.INotifyDataErrorInfo
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_INotifyDataErrorInfo[] = L"Microsoft.UI.Xaml.Data.INotifyDataErrorInfo";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("0EE6C2CC-273E-567D-BC0A-1DD87EE51EBA"), contract] */
                    MIDL_INTERFACE("0EE6C2CC-273E-567D-BC0A-1DD87EE51EBA")
                    INotifyDataErrorInfo : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasErrors(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ErrorsChanged(
                            /* [in] */__FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ErrorsChanged(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetErrors(
                            /* [in] */HSTRING propertyName,
                            /* [retval, out] */__FIIterable_1_IInspectable * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INotifyDataErrorInfo=_uuidof(INotifyDataErrorInfo);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.INotifyPropertyChanged
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_INotifyPropertyChanged[] = L"Microsoft.UI.Xaml.Data.INotifyPropertyChanged";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("90B17601-B065-586E-83D9-9ADC3A695284"), contract] */
                    MIDL_INTERFACE("90B17601-B065-586E-83D9-9ADC3A695284")
                    INotifyPropertyChanged : public IInspectable
                    {
                    public:
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_PropertyChanged(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IPropertyChangedEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_PropertyChanged(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INotifyPropertyChanged=_uuidof(INotifyPropertyChanged);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IPropertyChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.PropertyChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IPropertyChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.IPropertyChangedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("63D0C952-396B-54F4-AF8C-BA8724A427BF"), exclusiveto, contract] */
                    MIDL_INTERFACE("63D0C952-396B-54F4-AF8C-BA8724A427BF")
                    IPropertyChangedEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PropertyName(
                            /* [retval, out] */HSTRING * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPropertyChangedEventArgs=_uuidof(IPropertyChangedEventArgs);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IPropertyChangedEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.PropertyChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IPropertyChangedEventArgsFactory[] = L"Microsoft.UI.Xaml.Data.IPropertyChangedEventArgsFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("7C0C27A8-0B41-5070-B160-FC9AE960A36C"), exclusiveto, contract] */
                    MIDL_INTERFACE("7C0C27A8-0B41-5070-B160-FC9AE960A36C")
                    IPropertyChangedEventArgsFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */HSTRING name,
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IPropertyChangedEventArgs * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPropertyChangedEventArgsFactory=_uuidof(IPropertyChangedEventArgsFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IRelativeSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.RelativeSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IRelativeSource[] = L"Microsoft.UI.Xaml.Data.IRelativeSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("7FFC8126-5DD8-58BB-B686-C71EDDEA07B2"), exclusiveto, contract] */
                    MIDL_INTERFACE("7FFC8126-5DD8-58BB-B686-C71EDDEA07B2")
                    IRelativeSource : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Mode(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::RelativeSourceMode * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Mode(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::RelativeSourceMode value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IRelativeSource=_uuidof(IRelativeSource);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IRelativeSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.RelativeSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IRelativeSourceFactory[] = L"Microsoft.UI.Xaml.Data.IRelativeSourceFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("8518522C-85E3-5AE1-B9E9-28EA43C2051E"), exclusiveto, contract] */
                    MIDL_INTERFACE("8518522C-85E3-5AE1-B9E9-28EA43C2051E")
                    IRelativeSourceFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Data::IRelativeSource * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IRelativeSourceFactory=_uuidof(IRelativeSourceFactory);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ISelectionInfo
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ISelectionInfo[] = L"Microsoft.UI.Xaml.Data.ISelectionInfo";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("1B84C26B-9532-5803-935B-A03BF7E875DC"), contract] */
                    MIDL_INTERFACE("1B84C26B-9532-5803-935B-A03BF7E875DC")
                    ISelectionInfo : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE SelectRange(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IItemIndexRange * itemIndexRange
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE DeselectRange(
                            /* [in] */ABI::Microsoft::UI::Xaml::Data::IItemIndexRange * itemIndexRange
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE IsSelected(
                            /* [in] */INT32 index,
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetSelectedRanges(
                            /* [retval, out] */__FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISelectionInfo=_uuidof(ISelectionInfo);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ISupportIncrementalLoading
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ISupportIncrementalLoading[] = L"Microsoft.UI.Xaml.Data.ISupportIncrementalLoading";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("D8F9B586-A64A-5FF8-868E-204E144F2CF4"), contract] */
                    MIDL_INTERFACE("D8F9B586-A64A-5FF8-868E-204E144F2CF4")
                    ISupportIncrementalLoading : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE LoadMoreItemsAsync(
                            /* [in] */UINT32 count,
                            /* [retval, out] */__FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * * operation
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasMoreItems(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISupportIncrementalLoading=_uuidof(ISupportIncrementalLoading);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IValueConverter
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IValueConverter[] = L"Microsoft.UI.Xaml.Data.IValueConverter";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Data {
                    /* [object, uuid("AFDD2BFF-10F5-5173-B7C0-3590BD96CB35"), contract] */
                    MIDL_INTERFACE("AFDD2BFF-10F5-5173-B7C0-3590BD96CB35")
                    IValueConverter : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Convert(
                            /* [in] */IInspectable * value,
                            /* [in] */ABI::Windows::UI::Xaml::Interop::TypeName targetType,
                            /* [in] */IInspectable * parameter,
                            /* [in] */HSTRING language,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE ConvertBack(
                            /* [in] */IInspectable * value,
                            /* [in] */ABI::Windows::UI::Xaml::Interop::TypeName targetType,
                            /* [in] */IInspectable * parameter,
                            /* [in] */HSTRING language,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IValueConverter=_uuidof(IValueConverter);
                    
                } /* Data */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.Binding
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBinding ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_Binding_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_Binding_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_Binding[] = L"Microsoft.UI.Xaml.Data.Binding";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingBase ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingBase_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingBase_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingBase[] = L"Microsoft.UI.Xaml.Data.BindingBase";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingExpression
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingExpression ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpression_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpression_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingExpression[] = L"Microsoft.UI.Xaml.Data.BindingExpression";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingExpressionBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingExpressionBase ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpressionBase_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpressionBase_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingExpressionBase[] = L"Microsoft.UI.Xaml.Data.BindingExpressionBase";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingOperations
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Data.IBindingOperationsStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingOperations ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingOperations_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingOperations_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingOperations[] = L"Microsoft.UI.Xaml.Data.BindingOperations";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.CollectionViewSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Data.ICollectionViewSourceStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.ICollectionViewSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_CollectionViewSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_CollectionViewSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_CollectionViewSource[] = L"Microsoft.UI.Xaml.Data.CollectionViewSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.CurrentChangingEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.ICurrentChangingEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_CurrentChangingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_CurrentChangingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_CurrentChangingEventArgs[] = L"Microsoft.UI.Xaml.Data.CurrentChangingEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgsFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_DataErrorsChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_DataErrorsChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_DataErrorsChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.ItemIndexRange
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IItemIndexRange ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_ItemIndexRange_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_ItemIndexRange_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_ItemIndexRange[] = L"Microsoft.UI.Xaml.Data.ItemIndexRange";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.PropertyChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IPropertyChangedEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_PropertyChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_PropertyChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_PropertyChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.PropertyChangedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.RelativeSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IRelativeSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_RelativeSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_RelativeSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_RelativeSource[] = L"Microsoft.UI.Xaml.Data.RelativeSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__)
#define ____FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange;

typedef struct __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl;

interface __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange
{
    CONST_VTBL struct __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__)
#define ____FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange;

typedef  struct __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Microsoft__CUI__CXaml__CData__CItemIndexRange **first);

    END_INTERFACE
} __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl;

interface __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange
{
    CONST_VTBL struct __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange;

typedef struct __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
            /* [in] */ __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl;

interface __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange
{
    CONST_VTBL struct __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRangeVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_INTERFACE_DEFINED__)
#define ____FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_INTERFACE_DEFINED__

typedef interface __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs;

typedef struct __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs * This,/* [in] */ __RPC__in_opt IInspectable *sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * *e);
    END_INTERFACE
} __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgsVtbl;

interface __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs
{
    CONST_VTBL struct __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_QueryInterface(This,riid,ppvObject)	\
        ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_AddRef(This)	\
        ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_Release(This)	\
        ( (This)->lpVtbl -> Release(This) ) 

#define __FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_Invoke(This,sender,e)	\
        ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */


#endif // ____FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CLoadMoreItemsResult;

#if !defined(____FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_INTERFACE_DEFINED__)
#define ____FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_INTERFACE_DEFINED__

typedef interface __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult;

// Forward declare the async operation.
typedef interface __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult;

typedef struct __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResultVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This,/* [in] */ __RPC__in_opt __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult *asyncInfo, /* [in] */ AsyncStatus status);
    END_INTERFACE
} __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResultVtbl;

interface __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult
{
    CONST_VTBL struct __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_Invoke(This,asyncInfo,status)	\
    ( (This)->lpVtbl -> Invoke(This,asyncInfo,status) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_INTERFACE_DEFINED__



#if !defined(____FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_INTERFACE_DEFINED__)
#define ____FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_INTERFACE_DEFINED__

typedef interface __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult;

typedef struct __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResultVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Completed )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This, /* [in] */ __RPC__in_opt __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult *handler);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Completed )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This, /* [retval][out] */ __RPC__deref_out_opt __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult **handler);
    HRESULT ( STDMETHODCALLTYPE *GetResults )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * This, /* [retval][out] */ __RPC__out struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CLoadMoreItemsResult *results);
    END_INTERFACE
} __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResultVtbl;

interface __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult
{
    CONST_VTBL struct __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_put_Completed(This,handler)	\
    ( (This)->lpVtbl -> put_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_get_Completed(This,handler)	\
    ( (This)->lpVtbl -> get_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_GetResults(This,results)	\
    ( (This)->lpVtbl -> GetResults(This,results) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult_INTERFACE_DEFINED__



#if !defined(____FIIterator_1_IInspectable_INTERFACE_DEFINED__)
#define ____FIIterator_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FIIterator_1_IInspectable __FIIterator_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_IInspectable;

typedef struct __FIIterator_1_IInspectableVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_IInspectable * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_IInspectable * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_IInspectable * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_IInspectable * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_IInspectable * This, /* [retval][out] */ __RPC__out IInspectable * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_IInspectable * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_IInspectable * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_IInspectable * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) IInspectable * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_IInspectableVtbl;

interface __FIIterator_1_IInspectable
{
    CONST_VTBL struct __FIIterator_1_IInspectableVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_IInspectable_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_IInspectable_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_IInspectable_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_IInspectable_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_IInspectable_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_IInspectable_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_IInspectable_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_IInspectable_INTERFACE_DEFINED__



#if !defined(____FIIterable_1_IInspectable_INTERFACE_DEFINED__)
#define ____FIIterable_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FIIterable_1_IInspectable __FIIterable_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_IInspectable;

typedef  struct __FIIterable_1_IInspectableVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_IInspectable * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_IInspectable * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_IInspectable * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_IInspectable * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_IInspectable * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_IInspectable **first);

    END_INTERFACE
} __FIIterable_1_IInspectableVtbl;

interface __FIIterable_1_IInspectable
{
    CONST_VTBL struct __FIIterable_1_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_IInspectable_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_IInspectable_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_IInspectable_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_IInspectable_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_IInspectable_INTERFACE_DEFINED__



#if !defined(____FIVectorView_1_IInspectable_INTERFACE_DEFINED__)
#define ____FIVectorView_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_IInspectable __FIVectorView_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_IInspectable;

typedef struct __FIVectorView_1_IInspectableVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_IInspectable * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_IInspectable * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_IInspectable * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_IInspectable * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_IInspectable * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out IInspectable * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_IInspectable * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_IInspectable * This,
            /* [in] */ IInspectable * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_IInspectable * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) IInspectable * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_IInspectableVtbl;

interface __FIVectorView_1_IInspectable
{
    CONST_VTBL struct __FIVectorView_1_IInspectableVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_IInspectable_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_IInspectable_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_IInspectable_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_IInspectable_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_IInspectable_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_IInspectable_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_IInspectable_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_IInspectable_INTERFACE_DEFINED__



#if !defined(____FIVector_1_IInspectable_INTERFACE_DEFINED__)
#define ____FIVector_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FIVector_1_IInspectable __FIVector_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_IInspectable;

typedef struct __FIVector_1_IInspectableVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_IInspectable * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_IInspectable * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_IInspectable * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_IInspectable * This, /* [out] */ __RPC__deref_out_opt IInspectable * *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_IInspectable * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_IInspectable * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt IInspectable * *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_IInspectable * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_IInspectable * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_IInspectable **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_IInspectable * This,
        /* [in] */ __RPC__in IInspectable * item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_IInspectable * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in IInspectable * item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_IInspectable * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in IInspectable * item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_IInspectable * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_IInspectable * This, /* [in] */ __RPC__in IInspectable * item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_IInspectable * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_IInspectable * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_IInspectable * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) IInspectable * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_IInspectable * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) IInspectable * *value);

    END_INTERFACE
} __FIVector_1_IInspectableVtbl;

interface __FIVector_1_IInspectable
{
    CONST_VTBL struct __FIVector_1_IInspectableVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_IInspectable_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_IInspectable_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_IInspectable_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_IInspectable_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_IInspectable_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_IInspectable_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_IInspectable_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_IInspectable_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_IInspectable_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_IInspectable_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_IInspectable_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_IInspectable_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_IInspectable_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_IInspectable_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_IInspectable_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_IInspectable_INTERFACE_DEFINED__



#if !defined(____FVectorChangedEventHandler_1_IInspectable_INTERFACE_DEFINED__)
#define ____FVectorChangedEventHandler_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FVectorChangedEventHandler_1_IInspectable __FVectorChangedEventHandler_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FVectorChangedEventHandler_1_IInspectable;

typedef interface __FIObservableVector_1_IInspectable __FIObservableVector_1_IInspectable;

typedef struct __FVectorChangedEventHandler_1_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FVectorChangedEventHandler_1_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FVectorChangedEventHandler_1_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FVectorChangedEventHandler_1_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FVectorChangedEventHandler_1_IInspectable * This,
        /* [in] */ __RPC__in_opt __FIObservableVector_1_IInspectable *sender,
        /* [in] */ __RPC__in_opt __x_ABI_CWindows_CFoundation_CCollections_CIVectorChangedEventArgs *e);
    END_INTERFACE
} __FVectorChangedEventHandler_1_IInspectableVtbl;

interface __FVectorChangedEventHandler_1_IInspectable
{
    CONST_VTBL struct __FVectorChangedEventHandler_1_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FVectorChangedEventHandler_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FVectorChangedEventHandler_1_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FVectorChangedEventHandler_1_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FVectorChangedEventHandler_1_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */


#endif // ____FVectorChangedEventHandler_1_IInspectable_INTERFACE_DEFINED__



#if !defined(____FIObservableVector_1_IInspectable_INTERFACE_DEFINED__)
#define ____FIObservableVector_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FIObservableVector_1_IInspectable __FIObservableVector_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIObservableVector_1_IInspectable;

typedef struct __FIObservableVector_1_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIObservableVector_1_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIObservableVector_1_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIObservableVector_1_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIObservableVector_1_IInspectable * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIObservableVector_1_IInspectable * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIObservableVector_1_IInspectable * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *add_VectorChanged )(__RPC__in __FIObservableVector_1_IInspectable * This,
        /* [in] */ __RPC__in_opt __FVectorChangedEventHandler_1_IInspectable *handler,
        /* [retval][out] */ __RPC__out EventRegistrationToken *token);

    HRESULT ( STDMETHODCALLTYPE *remove_VectorChanged )(__RPC__in __FIObservableVector_1_IInspectable * This, /* [in] */ EventRegistrationToken token);
    END_INTERFACE
} __FIObservableVector_1_IInspectableVtbl;

interface __FIObservableVector_1_IInspectable
{
    CONST_VTBL struct __FIObservableVector_1_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIObservableVector_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIObservableVector_1_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIObservableVector_1_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIObservableVector_1_IInspectable_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIObservableVector_1_IInspectable_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIObservableVector_1_IInspectable_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIObservableVector_1_IInspectable_add_VectorChanged(This,handler,token)	\
    ( (This)->lpVtbl -> add_VectorChanged(This,handler,token) ) 
#define __FIObservableVector_1_IInspectable_remove_VectorChanged(This,token)	\
    ( (This)->lpVtbl -> remove_VectorChanged(This,token) ) 

#endif /* COBJMACROS */


#endif // ____FIObservableVector_1_IInspectable_INTERFACE_DEFINED__



#if !defined(____FIEventHandler_1_IInspectable_INTERFACE_DEFINED__)
#define ____FIEventHandler_1_IInspectable_INTERFACE_DEFINED__

typedef interface __FIEventHandler_1_IInspectable __FIEventHandler_1_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIEventHandler_1_IInspectable;

typedef struct __FIEventHandler_1_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIEventHandler_1_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIEventHandler_1_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIEventHandler_1_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FIEventHandler_1_IInspectable * This,/* [in] */ __RPC__in_opt IInspectable *sender,/* [in] */ __RPC__in_opt IInspectable * *e);
    END_INTERFACE
} __FIEventHandler_1_IInspectableVtbl;

interface __FIEventHandler_1_IInspectable
{
    CONST_VTBL struct __FIEventHandler_1_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIEventHandler_1_IInspectable_QueryInterface(This,riid,ppvObject)	\
        ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIEventHandler_1_IInspectable_AddRef(This)	\
        ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIEventHandler_1_IInspectable_Release(This)	\
        ( (This)->lpVtbl -> Release(This) ) 

#define __FIEventHandler_1_IInspectable_Invoke(This,sender,e)	\
        ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */


#endif // ____FIEventHandler_1_IInspectable_INTERFACE_DEFINED__



#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath __x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath_FWD_DEFINED__






#ifndef ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIClosable __x_ABI_CWindows_CFoundation_CIClosable;

#endif // ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__





typedef struct __x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName __x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName;






typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CData_CBindingMode __x_ABI_CMicrosoft_CUI_CXaml_CData_CBindingMode;


typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CData_CRelativeSourceMode __x_ABI_CMicrosoft_CUI_CXaml_CData_CRelativeSourceMode;


typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CData_CUpdateSourceTrigger __x_ABI_CMicrosoft_CUI_CXaml_CData_CUpdateSourceTrigger;


typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CLoadMoreItemsResult __x_ABI_CMicrosoft_CUI_CXaml_CData_CLoadMoreItemsResult;

























































/*
 *
 * Struct Microsoft.UI.Xaml.Data.BindingMode
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CData_CBindingMode
{
    BindingMode_OneWay = 1,
    BindingMode_OneTime = 2,
    BindingMode_TwoWay = 3,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Data.RelativeSourceMode
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CData_CRelativeSourceMode
{
    RelativeSourceMode_None = 0,
    RelativeSourceMode_TemplatedParent = 1,
    RelativeSourceMode_Self = 2,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Data.UpdateSourceTrigger
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CData_CUpdateSourceTrigger
{
    UpdateSourceTrigger_Default = 0,
    UpdateSourceTrigger_PropertyChanged = 1,
    UpdateSourceTrigger_Explicit = 2,
    UpdateSourceTrigger_LostFocus = 3,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Data.LoadMoreItemsResult
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CLoadMoreItemsResult
{
    UINT32 Count;
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Data.CurrentChangingEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_INTERFACE_DEFINED__
/* [object, uuid("3D2A98DD-95B3-5FD5-93B4-A1A2599F225C"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Data.PropertyChangedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_INTERFACE_DEFINED__
/* [object, uuid("E3DE52F6-1E32-5DA6-BB2D-B5B6096C962D"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBinding
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.Binding
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBinding[] = L"Microsoft.UI.Xaml.Data.IBinding";
/* [object, uuid("501EA0E8-EDD4-59DE-8845-76AF2EABBE00"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Path )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Path )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Mode )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CBindingMode * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Mode )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CBindingMode value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Source )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Source )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */IInspectable * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RelativeSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RelativeSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ElementName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ElementName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Converter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Converter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ConverterParameter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ConverterParameter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */IInspectable * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ConverterLanguage )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ConverterLanguage )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FallbackValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FallbackValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */IInspectable * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TargetNullValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TargetNullValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */IInspectable * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UpdateSourceTrigger )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CUpdateSourceTrigger * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_UpdateSourceTrigger )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CUpdateSourceTrigger value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_Path(This,value) \
    ( (This)->lpVtbl->get_Path(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_Path(This,value) \
    ( (This)->lpVtbl->put_Path(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_Mode(This,value) \
    ( (This)->lpVtbl->get_Mode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_Mode(This,value) \
    ( (This)->lpVtbl->put_Mode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_Source(This,value) \
    ( (This)->lpVtbl->get_Source(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_Source(This,value) \
    ( (This)->lpVtbl->put_Source(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_RelativeSource(This,value) \
    ( (This)->lpVtbl->get_RelativeSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_RelativeSource(This,value) \
    ( (This)->lpVtbl->put_RelativeSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_ElementName(This,value) \
    ( (This)->lpVtbl->get_ElementName(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_ElementName(This,value) \
    ( (This)->lpVtbl->put_ElementName(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_Converter(This,value) \
    ( (This)->lpVtbl->get_Converter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_Converter(This,value) \
    ( (This)->lpVtbl->put_Converter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_ConverterParameter(This,value) \
    ( (This)->lpVtbl->get_ConverterParameter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_ConverterParameter(This,value) \
    ( (This)->lpVtbl->put_ConverterParameter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_ConverterLanguage(This,value) \
    ( (This)->lpVtbl->get_ConverterLanguage(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_ConverterLanguage(This,value) \
    ( (This)->lpVtbl->put_ConverterLanguage(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_FallbackValue(This,value) \
    ( (This)->lpVtbl->get_FallbackValue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_FallbackValue(This,value) \
    ( (This)->lpVtbl->put_FallbackValue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_TargetNullValue(This,value) \
    ( (This)->lpVtbl->get_TargetNullValue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_TargetNullValue(This,value) \
    ( (This)->lpVtbl->put_TargetNullValue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_get_UpdateSourceTrigger(This,value) \
    ( (This)->lpVtbl->get_UpdateSourceTrigger(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_put_UpdateSourceTrigger(This,value) \
    ( (This)->lpVtbl->put_UpdateSourceTrigger(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingBase[] = L"Microsoft.UI.Xaml.Data.IBindingBase";
/* [object, uuid("91DDD141-5944-50EF-B85E-218E463F7A73"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingBaseFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingBaseFactory[] = L"Microsoft.UI.Xaml.Data.IBindingBaseFactory";
/* [object, uuid("C8A866C5-F6F3-5F7A-9592-D385AF48BD8F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBaseFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpression
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpression
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpression[] = L"Microsoft.UI.Xaml.Data.IBindingExpression";
/* [object, uuid("4C023916-37BC-5B07-BC9D-15C547BD9B26"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DataItem )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ParentBinding )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * * value
        );
    HRESULT ( STDMETHODCALLTYPE *UpdateSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_get_DataItem(This,value) \
    ( (This)->lpVtbl->get_DataItem(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_get_ParentBinding(This,value) \
    ( (This)->lpVtbl->get_ParentBinding(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_UpdateSource(This) \
    ( (This)->lpVtbl->UpdateSource(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpression_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpressionBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpressionBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpressionBase[] = L"Microsoft.UI.Xaml.Data.IBindingExpressionBase";
/* [object, uuid("8825E5A9-D9A3-5E87-BCD8-C63133D29029"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBase_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpressionBaseFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpressionBase
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpressionBaseFactory[] = L"Microsoft.UI.Xaml.Data.IBindingExpressionBaseFactory";
/* [object, uuid("41D643B9-2629-5451-A716-596C0848B5DC"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionBaseFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingExpressionFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingExpression
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingExpressionFactory[] = L"Microsoft.UI.Xaml.Data.IBindingExpressionFactory";
/* [object, uuid("086CAE14-81A1-588B-B619-05EE84C0F089"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingExpressionFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.Binding
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingFactory[] = L"Microsoft.UI.Xaml.Data.IBindingFactory";
/* [object, uuid("CB2DE749-B115-5F67-B64A-797D54885D5C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIBinding * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingOperations
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingOperations
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingOperations[] = L"Microsoft.UI.Xaml.Data.IBindingOperations";
/* [object, uuid("9A319B95-AABE-5075-B227-8EB07E443D8B"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperations_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IBindingOperationsStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.BindingOperations
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IBindingOperationsStatics[] = L"Microsoft.UI.Xaml.Data.IBindingOperationsStatics";
/* [object, uuid("1E1BDBD3-FCA5-5C85-B87D-B504CD8FA8AC"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *SetBinding )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyObject * target,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * dp,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingBase * binding
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_SetBinding(This,target,dp,binding) \
    ( (This)->lpVtbl->SetBinding(This,target,dp,binding) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIBindingOperationsStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionView
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Windows.Foundation.Collections.IObservableVector_1_IInspectable
 *     Windows.Foundation.Collections.IVector_1_IInspectable
 *     Windows.Foundation.Collections.IIterable_1_IInspectable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionView[] = L"Microsoft.UI.Xaml.Data.ICollectionView";
/* [object, uuid("F8BB90D8-E008-5D65-8C97-7BB790A4230C"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CurrentItem )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CurrentPosition )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsCurrentAfterLast )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsCurrentBeforeFirst )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CollectionGroups )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */__FIObservableVector_1_IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasMoreItems )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_CurrentChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */__FIEventHandler_1_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_CurrentChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_CurrentChanging )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_CurrentChanging )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *MoveCurrentTo )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */IInspectable * item,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveCurrentToPosition )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */INT32 index,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveCurrentToFirst )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveCurrentToLast )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveCurrentToNext )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveCurrentToPrevious )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *LoadMoreItemsAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * This,
        /* [in] */UINT32 count,
        /* [retval, out] */__FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * * operation
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_get_CurrentItem(This,value) \
    ( (This)->lpVtbl->get_CurrentItem(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_get_CurrentPosition(This,value) \
    ( (This)->lpVtbl->get_CurrentPosition(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_get_IsCurrentAfterLast(This,value) \
    ( (This)->lpVtbl->get_IsCurrentAfterLast(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_get_IsCurrentBeforeFirst(This,value) \
    ( (This)->lpVtbl->get_IsCurrentBeforeFirst(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_get_CollectionGroups(This,value) \
    ( (This)->lpVtbl->get_CollectionGroups(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_get_HasMoreItems(This,value) \
    ( (This)->lpVtbl->get_HasMoreItems(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_add_CurrentChanged(This,handler,token) \
    ( (This)->lpVtbl->add_CurrentChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_remove_CurrentChanged(This,token) \
    ( (This)->lpVtbl->remove_CurrentChanged(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_add_CurrentChanging(This,handler,token) \
    ( (This)->lpVtbl->add_CurrentChanging(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_remove_CurrentChanging(This,token) \
    ( (This)->lpVtbl->remove_CurrentChanging(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_MoveCurrentTo(This,item,result) \
    ( (This)->lpVtbl->MoveCurrentTo(This,item,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_MoveCurrentToPosition(This,index,result) \
    ( (This)->lpVtbl->MoveCurrentToPosition(This,index,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_MoveCurrentToFirst(This,result) \
    ( (This)->lpVtbl->MoveCurrentToFirst(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_MoveCurrentToLast(This,result) \
    ( (This)->lpVtbl->MoveCurrentToLast(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_MoveCurrentToNext(This,result) \
    ( (This)->lpVtbl->MoveCurrentToNext(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_MoveCurrentToPrevious(This,result) \
    ( (This)->lpVtbl->MoveCurrentToPrevious(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_LoadMoreItemsAsync(This,count,operation) \
    ( (This)->lpVtbl->LoadMoreItemsAsync(This,count,operation) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewFactory[] = L"Microsoft.UI.Xaml.Data.ICollectionViewFactory";
/* [object, uuid("D971F795-5728-5BEF-9602-43F2C4250E56"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateView )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_CreateView(This,result) \
    ( (This)->lpVtbl->CreateView(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewGroup
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewGroup[] = L"Microsoft.UI.Xaml.Data.ICollectionViewGroup";
/* [object, uuid("96A08DA8-BE38-5AE0-903D-6FB6111E61F5"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroupVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Group )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_GroupItems )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup * This,
        /* [retval, out] */__FIObservableVector_1_IInspectable * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroupVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroupVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_get_Group(This,value) \
    ( (This)->lpVtbl->get_Group(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_get_GroupItems(This,value) \
    ( (This)->lpVtbl->get_GroupItems(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewGroup_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CollectionViewSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewSource[] = L"Microsoft.UI.Xaml.Data.ICollectionViewSource";
/* [object, uuid("A45E3B3A-F31E-5BBB-8A7C-70CF5C64BC3F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Source )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Source )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [in] */IInspectable * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_View )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionView * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsSourceGrouped )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsSourceGrouped )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ItemsPath )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ItemsPath )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIPropertyPath * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_get_Source(This,value) \
    ( (This)->lpVtbl->get_Source(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_put_Source(This,value) \
    ( (This)->lpVtbl->put_Source(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_get_View(This,value) \
    ( (This)->lpVtbl->get_View(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_get_IsSourceGrouped(This,value) \
    ( (This)->lpVtbl->get_IsSourceGrouped(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_put_IsSourceGrouped(This,value) \
    ( (This)->lpVtbl->put_IsSourceGrouped(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_get_ItemsPath(This,value) \
    ( (This)->lpVtbl->get_ItemsPath(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_put_ItemsPath(This,value) \
    ( (This)->lpVtbl->put_ItemsPath(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICollectionViewSourceStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CollectionViewSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICollectionViewSourceStatics[] = L"Microsoft.UI.Xaml.Data.ICollectionViewSourceStatics";
/* [object, uuid("E282F10F-D4B1-5769-8A11-30F739E6113B"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SourceProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ViewProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsSourceGroupedProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ItemsPathProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_get_SourceProperty(This,value) \
    ( (This)->lpVtbl->get_SourceProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_get_ViewProperty(This,value) \
    ( (This)->lpVtbl->get_ViewProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_get_IsSourceGroupedProperty(This,value) \
    ( (This)->lpVtbl->get_IsSourceGroupedProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_get_ItemsPathProperty(This,value) \
    ( (This)->lpVtbl->get_ItemsPathProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICollectionViewSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICurrentChangingEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CurrentChangingEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICurrentChangingEventArgs[] = L"Microsoft.UI.Xaml.Data.ICurrentChangingEventArgs";
/* [object, uuid("63E42ED6-E14A-51EA-9CB1-72F9C907DC80"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cancel )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Cancel )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsCancelable )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * This,
        /* [retval, out] */boolean * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_get_Cancel(This,value) \
    ( (This)->lpVtbl->get_Cancel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_put_Cancel(This,value) \
    ( (This)->lpVtbl->put_Cancel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_get_IsCancelable(This,value) \
    ( (This)->lpVtbl->get_IsCancelable(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICurrentChangingEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.CurrentChangingEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICurrentChangingEventArgsFactory[] = L"Microsoft.UI.Xaml.Data.ICurrentChangingEventArgsFactory";
/* [object, uuid("3670F48A-AC2C-5352-8A4B-6B977A08E5F8"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * * value
        );
    HRESULT ( STDMETHODCALLTYPE *CreateWithCancelableParameter )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory * This,
        /* [in] */boolean isCancelable,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgs * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_CreateWithCancelableParameter(This,isCancelable,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateWithCancelableParameter(This,isCancelable,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICurrentChangingEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICustomProperty
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICustomProperty[] = L"Microsoft.UI.Xaml.Data.ICustomProperty";
/* [object, uuid("30DA92C0-23E8-42A0-AE7C-734A0E5D2782"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Type )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Name )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [retval, out] */HSTRING * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [in] */IInspectable * target,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [in] */IInspectable * target,
        /* [in] */IInspectable * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetIndexedValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [in] */IInspectable * target,
        /* [in] */IInspectable * index,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetIndexedValue )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [in] */IInspectable * target,
        /* [in] */IInspectable * value,
        /* [in] */IInspectable * index
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CanWrite )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CanRead )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * This,
        /* [retval, out] */boolean * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_get_Type(This,value) \
    ( (This)->lpVtbl->get_Type(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_get_Name(This,value) \
    ( (This)->lpVtbl->get_Name(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_GetValue(This,target,result) \
    ( (This)->lpVtbl->GetValue(This,target,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_SetValue(This,target,value) \
    ( (This)->lpVtbl->SetValue(This,target,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_GetIndexedValue(This,target,index,result) \
    ( (This)->lpVtbl->GetIndexedValue(This,target,index,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_SetIndexedValue(This,target,value,index) \
    ( (This)->lpVtbl->SetIndexedValue(This,target,value,index) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_get_CanWrite(This,value) \
    ( (This)->lpVtbl->get_CanWrite(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_get_CanRead(This,value) \
    ( (This)->lpVtbl->get_CanRead(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ICustomPropertyProvider
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ICustomPropertyProvider[] = L"Microsoft.UI.Xaml.Data.ICustomPropertyProvider";
/* [object, uuid("7C925755-3E48-42B4-8677-76372267033F"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProviderVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetCustomProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
        /* [in] */HSTRING name,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetIndexedProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
        /* [in] */HSTRING name,
        /* [in] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName type,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomProperty * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetStringRepresentation )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
        /* [retval, out] */HSTRING * result
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Type )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProviderVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_GetCustomProperty(This,name,result) \
    ( (This)->lpVtbl->GetCustomProperty(This,name,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_GetIndexedProperty(This,name,type,result) \
    ( (This)->lpVtbl->GetIndexedProperty(This,name,type,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_GetStringRepresentation(This,result) \
    ( (This)->lpVtbl->GetStringRepresentation(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_get_Type(This,value) \
    ( (This)->lpVtbl->get_Type(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CICustomPropertyProvider_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IDataErrorsChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgs";
/* [object, uuid("D026DD64-5F26-5F15-A86A-0DEC8A431796"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PropertyName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PropertyName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * This,
        /* [in] */HSTRING value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_get_PropertyName(This,value) \
    ( (This)->lpVtbl->get_PropertyName(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_put_PropertyName(This,value) \
    ( (This)->lpVtbl->put_PropertyName(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IDataErrorsChangedEventArgsFactory[] = L"Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgsFactory";
/* [object, uuid("62D0BD1E-B85F-5FCC-842A-7CB0DDA37FE5"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory * This,
        /* [in] */HSTRING name,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgs * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_CreateInstance(This,name,value) \
    ( (This)->lpVtbl->CreateInstance(This,name,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIDataErrorsChangedEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IItemIndexRange
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.ItemIndexRange
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IItemIndexRange[] = L"Microsoft.UI.Xaml.Data.IItemIndexRange";
/* [object, uuid("EBA09846-2554-5B86-AC17-614F05105FA2"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FirstIndex )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Length )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
        /* [retval, out] */UINT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LastIndex )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * This,
        /* [retval, out] */INT32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_get_FirstIndex(This,value) \
    ( (This)->lpVtbl->get_FirstIndex(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_get_Length(This,value) \
    ( (This)->lpVtbl->get_Length(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_get_LastIndex(This,value) \
    ( (This)->lpVtbl->get_LastIndex(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IItemIndexRangeFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.ItemIndexRange
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IItemIndexRangeFactory[] = L"Microsoft.UI.Xaml.Data.IItemIndexRangeFactory";
/* [object, uuid("9FC73213-EDA0-5238-AA2C-401C9921F0F9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory * This,
        /* [in] */INT32 firstIndex,
        /* [in] */UINT32 length,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_CreateInstance(This,firstIndex,length,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,firstIndex,length,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRangeFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IItemsRangeInfo
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Windows.Foundation.IClosable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IItemsRangeInfo[] = L"Microsoft.UI.Xaml.Data.IItemsRangeInfo";
/* [object, uuid("B8376D08-85FB-563B-8273-39EF2D138256"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *RangesChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * visibleRange,
        /* [in] */__FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * trackedItems
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfoVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_RangesChanged(This,visibleRange,trackedItems) \
    ( (This)->lpVtbl->RangesChanged(This,visibleRange,trackedItems) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemsRangeInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.INotifyDataErrorInfo
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_INotifyDataErrorInfo[] = L"Microsoft.UI.Xaml.Data.INotifyDataErrorInfo";
/* [object, uuid("0EE6C2CC-273E-567D-BC0A-1DD87EE51EBA"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasErrors )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
        /* [retval, out] */boolean * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ErrorsChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
        /* [in] */__FIEventHandler_1_Microsoft__CUI__CXaml__CData__CDataErrorsChangedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ErrorsChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *GetErrors )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo * This,
        /* [in] */HSTRING propertyName,
        /* [retval, out] */__FIIterable_1_IInspectable * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfoVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_get_HasErrors(This,value) \
    ( (This)->lpVtbl->get_HasErrors(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_add_ErrorsChanged(This,handler,token) \
    ( (This)->lpVtbl->add_ErrorsChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_remove_ErrorsChanged(This,token) \
    ( (This)->lpVtbl->remove_ErrorsChanged(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_GetErrors(This,propertyName,result) \
    ( (This)->lpVtbl->GetErrors(This,propertyName,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyDataErrorInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.INotifyPropertyChanged
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_INotifyPropertyChanged[] = L"Microsoft.UI.Xaml.Data.INotifyPropertyChanged";
/* [object, uuid("90B17601-B065-586E-83D9-9ADC3A695284"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChangedVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_PropertyChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_PropertyChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChangedVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChangedVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_add_PropertyChanged(This,handler,token) \
    ( (This)->lpVtbl->add_PropertyChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_remove_PropertyChanged(This,token) \
    ( (This)->lpVtbl->remove_PropertyChanged(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CINotifyPropertyChanged_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IPropertyChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.PropertyChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IPropertyChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.IPropertyChangedEventArgs";
/* [object, uuid("63D0C952-396B-54F4-AF8C-BA8724A427BF"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PropertyName )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * This,
        /* [retval, out] */HSTRING * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_get_PropertyName(This,value) \
    ( (This)->lpVtbl->get_PropertyName(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IPropertyChangedEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.PropertyChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IPropertyChangedEventArgsFactory[] = L"Microsoft.UI.Xaml.Data.IPropertyChangedEventArgsFactory";
/* [object, uuid("7C0C27A8-0B41-5070-B160-FC9AE960A36C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory * This,
        /* [in] */HSTRING name,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgs * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_CreateInstance(This,name,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,name,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIPropertyChangedEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IRelativeSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.RelativeSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IRelativeSource[] = L"Microsoft.UI.Xaml.Data.IRelativeSource";
/* [object, uuid("7FFC8126-5DD8-58BB-B686-C71EDDEA07B2"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Mode )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CRelativeSourceMode * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Mode )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CRelativeSourceMode value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_get_Mode(This,value) \
    ( (This)->lpVtbl->get_Mode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_put_Mode(This,value) \
    ( (This)->lpVtbl->put_Mode(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IRelativeSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Data.RelativeSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IRelativeSourceFactory[] = L"Microsoft.UI.Xaml.Data.IRelativeSourceFactory";
/* [object, uuid("8518522C-85E3-5AE1-B9E9-28EA43C2051E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSource * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIRelativeSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ISelectionInfo
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ISelectionInfo[] = L"Microsoft.UI.Xaml.Data.ISelectionInfo";
/* [object, uuid("1B84C26B-9532-5803-935B-A03BF7E875DC"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *SelectRange )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * itemIndexRange
        );
    HRESULT ( STDMETHODCALLTYPE *DeselectRange )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CData_CIItemIndexRange * itemIndexRange
        );
    HRESULT ( STDMETHODCALLTYPE *IsSelected )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
        /* [in] */INT32 index,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetSelectedRanges )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo * This,
        /* [retval, out] */__FIVectorView_1_Microsoft__CUI__CXaml__CData__CItemIndexRange * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfoVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_SelectRange(This,itemIndexRange) \
    ( (This)->lpVtbl->SelectRange(This,itemIndexRange) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_DeselectRange(This,itemIndexRange) \
    ( (This)->lpVtbl->DeselectRange(This,itemIndexRange) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_IsSelected(This,index,result) \
    ( (This)->lpVtbl->IsSelected(This,index,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_GetSelectedRanges(This,result) \
    ( (This)->lpVtbl->GetSelectedRanges(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISelectionInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.ISupportIncrementalLoading
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_ISupportIncrementalLoading[] = L"Microsoft.UI.Xaml.Data.ISupportIncrementalLoading";
/* [object, uuid("D8F9B586-A64A-5FF8-868E-204E144F2CF4"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoadingVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *LoadMoreItemsAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This,
        /* [in] */UINT32 count,
        /* [retval, out] */__FIAsyncOperation_1_Microsoft__CUI__CXaml__CData__CLoadMoreItemsResult * * operation
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasMoreItems )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading * This,
        /* [retval, out] */boolean * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoadingVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoadingVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_LoadMoreItemsAsync(This,count,operation) \
    ( (This)->lpVtbl->LoadMoreItemsAsync(This,count,operation) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_get_HasMoreItems(This,value) \
    ( (This)->lpVtbl->get_HasMoreItems(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CISupportIncrementalLoading_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Data.IValueConverter
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Data_IValueConverter[] = L"Microsoft.UI.Xaml.Data.IValueConverter";
/* [object, uuid("AFDD2BFF-10F5-5173-B7C0-3590BD96CB35"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverterVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Convert )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This,
        /* [in] */IInspectable * value,
        /* [in] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName targetType,
        /* [in] */IInspectable * parameter,
        /* [in] */HSTRING language,
        /* [retval, out] */IInspectable * * result
        );
    HRESULT ( STDMETHODCALLTYPE *ConvertBack )(
        __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter * This,
        /* [in] */IInspectable * value,
        /* [in] */__x_ABI_CWindows_CUI_CXaml_CInterop_CTypeName targetType,
        /* [in] */IInspectable * parameter,
        /* [in] */HSTRING language,
        /* [retval, out] */IInspectable * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverterVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_Convert(This,value,targetType,parameter,language,result) \
    ( (This)->lpVtbl->Convert(This,value,targetType,parameter,language,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_ConvertBack(This,value,targetType,parameter,language,result) \
    ( (This)->lpVtbl->ConvertBack(This,value,targetType,parameter,language,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CData_CIValueConverter_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.Binding
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBinding ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_Binding_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_Binding_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_Binding[] = L"Microsoft.UI.Xaml.Data.Binding";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingBase ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingBase_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingBase_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingBase[] = L"Microsoft.UI.Xaml.Data.BindingBase";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingExpression
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingExpression ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpression_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpression_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingExpression[] = L"Microsoft.UI.Xaml.Data.BindingExpression";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingExpressionBase
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingExpressionBase ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpressionBase_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingExpressionBase_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingExpressionBase[] = L"Microsoft.UI.Xaml.Data.BindingExpressionBase";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.BindingOperations
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Data.IBindingOperationsStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IBindingOperations ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingOperations_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_BindingOperations_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_BindingOperations[] = L"Microsoft.UI.Xaml.Data.BindingOperations";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.CollectionViewSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Data.ICollectionViewSourceStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.ICollectionViewSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_CollectionViewSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_CollectionViewSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_CollectionViewSource[] = L"Microsoft.UI.Xaml.Data.CollectionViewSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.CurrentChangingEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.ICurrentChangingEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_CurrentChangingEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_CurrentChangingEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_CurrentChangingEventArgs[] = L"Microsoft.UI.Xaml.Data.CurrentChangingEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgsFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IDataErrorsChangedEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_DataErrorsChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_DataErrorsChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_DataErrorsChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.DataErrorsChangedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.ItemIndexRange
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IItemIndexRange ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_ItemIndexRange_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_ItemIndexRange_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_ItemIndexRange[] = L"Microsoft.UI.Xaml.Data.ItemIndexRange";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.PropertyChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IPropertyChangedEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_PropertyChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_PropertyChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_PropertyChangedEventArgs[] = L"Microsoft.UI.Xaml.Data.PropertyChangedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Data.RelativeSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Data.IRelativeSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Data_RelativeSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Data_RelativeSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Data_RelativeSource[] = L"Microsoft.UI.Xaml.Data.RelativeSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EData_p_h__

#endif // __Microsoft2EUI2EXaml2EData_h__
