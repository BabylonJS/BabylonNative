/* Header file automatically generated from Microsoft.UI.Xaml.Printing.idl */
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
#ifndef __Microsoft2EUI2EXaml2EPrinting_h__
#define __Microsoft2EUI2EXaml2EPrinting_h__
#ifndef __Microsoft2EUI2EXaml2EPrinting_p_h__
#define __Microsoft2EUI2EXaml2EPrinting_p_h__


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
#include "Windows.Graphics.Printing.h"

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IAddPagesEventHandler;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler ABI::Microsoft::UI::Xaml::Printing::IAddPagesEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IGetPreviewPageEventHandler;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler ABI::Microsoft::UI::Xaml::Printing::IGetPreviewPageEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IPaginateEventHandler;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler ABI::Microsoft::UI::Xaml::Printing::IPaginateEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IAddPagesEventArgs;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs ABI::Microsoft::UI::Xaml::Printing::IAddPagesEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IGetPreviewPageEventArgs;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs ABI::Microsoft::UI::Xaml::Printing::IGetPreviewPageEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IPaginateEventArgs;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs ABI::Microsoft::UI::Xaml::Printing::IPaginateEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IPrintDocument;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument ABI::Microsoft::UI::Xaml::Printing::IPrintDocument

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IPrintDocumentFactory;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory ABI::Microsoft::UI::Xaml::Printing::IPrintDocumentFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    interface IPrintDocumentStatics;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics ABI::Microsoft::UI::Xaml::Printing::IPrintDocumentStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_FWD_DEFINED__


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






#ifndef ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Graphics {
            namespace Printing {
                interface IPrintDocumentSource;
            } /* Printing */
        } /* Graphics */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource ABI::Windows::Graphics::Printing::IPrintDocumentSource

#endif // ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource_FWD_DEFINED__


namespace ABI {
    namespace Windows {
        namespace Graphics {
            namespace Printing {
                class PrintTaskOptions;
            } /* Printing */
        } /* Graphics */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Graphics {
            namespace Printing {
                interface IPrintTaskOptionsCore;
            } /* Printing */
        } /* Graphics */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore ABI::Windows::Graphics::Printing::IPrintTaskOptionsCore

#endif // ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore_FWD_DEFINED__





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    
                    typedef enum PreviewPageCountType : int PreviewPageCountType;
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */










namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    class AddPagesEventArgs;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    class GetPreviewPageEventArgs;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    class PaginateEventArgs;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    class PrintDocument;
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */







/*
 *
 * Struct Microsoft.UI.Xaml.Printing.PreviewPageCountType
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
                namespace Printing {
                    /* [v1_enum, contract] */
                    enum PreviewPageCountType : int
                    {
                        PreviewPageCountType_Final = 0,
                        PreviewPageCountType_Intermediate = 1,
                    };
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Printing.AddPagesEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("ED77566A-BD03-5118-B7C3-D9CEA64307DD"), contract] */
                    MIDL_INTERFACE("ED77566A-BD03-5118-B7C3-D9CEA64307DD")
                    IAddPagesEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::IAddPagesEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IAddPagesEventHandler=_uuidof(IAddPagesEventHandler);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Printing.GetPreviewPageEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("1C801689-A018-5574-9109-BCEF62176DA2"), contract] */
                    MIDL_INTERFACE("1C801689-A018-5574-9109-BCEF62176DA2")
                    IGetPreviewPageEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::IGetPreviewPageEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IGetPreviewPageEventHandler=_uuidof(IGetPreviewPageEventHandler);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Printing.PaginateEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("C291876C-343A-5B9B-A36C-8415BA4CD9DD"), contract] */
                    MIDL_INTERFACE("C291876C-343A-5B9B-A36C-8415BA4CD9DD")
                    IPaginateEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::IPaginateEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPaginateEventHandler=_uuidof(IPaginateEventHandler);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IAddPagesEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.AddPagesEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IAddPagesEventArgs[] = L"Microsoft.UI.Xaml.Printing.IAddPagesEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("A69F3CB3-6B74-5EE8-B034-188098A98C5D"), exclusiveto, contract] */
                    MIDL_INTERFACE("A69F3CB3-6B74-5EE8-B034-188098A98C5D")
                    IAddPagesEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PrintTaskOptions(
                            /* [retval, out] */ABI::Windows::Graphics::Printing::IPrintTaskOptionsCore * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IAddPagesEventArgs=_uuidof(IAddPagesEventArgs);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IGetPreviewPageEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.GetPreviewPageEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IGetPreviewPageEventArgs[] = L"Microsoft.UI.Xaml.Printing.IGetPreviewPageEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("A68FBE17-F577-597F-B3AB-B379447149E6"), exclusiveto, contract] */
                    MIDL_INTERFACE("A68FBE17-F577-597F-B3AB-B379447149E6")
                    IGetPreviewPageEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PageNumber(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IGetPreviewPageEventArgs=_uuidof(IGetPreviewPageEventArgs);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPaginateEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PaginateEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPaginateEventArgs[] = L"Microsoft.UI.Xaml.Printing.IPaginateEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("6499C196-11A9-5EF8-91CB-52FB963BF172"), exclusiveto, contract] */
                    MIDL_INTERFACE("6499C196-11A9-5EF8-91CB-52FB963BF172")
                    IPaginateEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PrintTaskOptions(
                            /* [retval, out] */ABI::Windows::Graphics::Printing::IPrintTaskOptionsCore * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CurrentPreviewPageNumber(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPaginateEventArgs=_uuidof(IPaginateEventArgs);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPrintDocument
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PrintDocument
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPrintDocument[] = L"Microsoft.UI.Xaml.Printing.IPrintDocument";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("1E40F1FC-5D33-5FE9-BA3E-954C0D161524"), exclusiveto, contract] */
                    MIDL_INTERFACE("1E40F1FC-5D33-5FE9-BA3E-954C0D161524")
                    IPrintDocument : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DocumentSource(
                            /* [retval, out] */ABI::Windows::Graphics::Printing::IPrintDocumentSource * * value
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Paginate(
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::IPaginateEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Paginate(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_GetPreviewPage(
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::IGetPreviewPageEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_GetPreviewPage(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_AddPages(
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::IAddPagesEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_AddPages(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE AddPage(
                            /* [in] */ABI::Microsoft::UI::Xaml::IUIElement * pageVisual
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE AddPagesComplete(void) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPreviewPageCount(
                            /* [in] */INT32 count,
                            /* [in] */ABI::Microsoft::UI::Xaml::Printing::PreviewPageCountType type
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetPreviewPage(
                            /* [in] */INT32 pageNumber,
                            /* [in] */ABI::Microsoft::UI::Xaml::IUIElement * pageVisual
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE InvalidatePreview(void) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPrintDocument=_uuidof(IPrintDocument);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPrintDocumentFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PrintDocument
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPrintDocumentFactory[] = L"Microsoft.UI.Xaml.Printing.IPrintDocumentFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("C4C1BC12-84D1-539C-B416-D7E54C45AB58"), exclusiveto, contract] */
                    MIDL_INTERFACE("C4C1BC12-84D1-539C-B416-D7E54C45AB58")
                    IPrintDocumentFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Printing::IPrintDocument * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPrintDocumentFactory=_uuidof(IPrintDocumentFactory);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPrintDocumentStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PrintDocument
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPrintDocumentStatics[] = L"Microsoft.UI.Xaml.Printing.IPrintDocumentStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Printing {
                    /* [object, uuid("8975E4BC-8FC8-5E8F-A55A-BF71B9A827B7"), exclusiveto, contract] */
                    MIDL_INTERFACE("8975E4BC-8FC8-5E8F-A55A-BF71B9A827B7")
                    IPrintDocumentStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DocumentSourceProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPrintDocumentStatics=_uuidof(IPrintDocumentStatics);
                    
                } /* Printing */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.AddPagesEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IAddPagesEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_AddPagesEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_AddPagesEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_AddPagesEventArgs[] = L"Microsoft.UI.Xaml.Printing.AddPagesEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.GetPreviewPageEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IGetPreviewPageEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_GetPreviewPageEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_GetPreviewPageEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_GetPreviewPageEventArgs[] = L"Microsoft.UI.Xaml.Printing.GetPreviewPageEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.PaginateEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IPaginateEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PaginateEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PaginateEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_PaginateEventArgs[] = L"Microsoft.UI.Xaml.Printing.PaginateEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.PrintDocument
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Printing.IPrintDocumentStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IPrintDocument ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PrintDocument_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PrintDocument_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_PrintDocument[] = L"Microsoft.UI.Xaml.Printing.PrintDocument";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__






#ifndef ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource __x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource;

#endif // ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore __x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore;

#endif // ____x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore_FWD_DEFINED__






typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CPreviewPageCountType __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CPreviewPageCountType;



















/*
 *
 * Struct Microsoft.UI.Xaml.Printing.PreviewPageCountType
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CPreviewPageCountType
{
    PreviewPageCountType_Final = 0,
    PreviewPageCountType_Intermediate = 1,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Printing.AddPagesEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_INTERFACE_DEFINED__
/* [object, uuid("ED77566A-BD03-5118-B7C3-D9CEA64307DD"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Printing.GetPreviewPageEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_INTERFACE_DEFINED__
/* [object, uuid("1C801689-A018-5574-9109-BCEF62176DA2"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Printing.PaginateEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_INTERFACE_DEFINED__
/* [object, uuid("C291876C-343A-5B9B-A36C-8415BA4CD9DD"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IAddPagesEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.AddPagesEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IAddPagesEventArgs[] = L"Microsoft.UI.Xaml.Printing.IAddPagesEventArgs";
/* [object, uuid("A69F3CB3-6B74-5EE8-B034-188098A98C5D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PrintTaskOptions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_get_PrintTaskOptions(This,value) \
    ( (This)->lpVtbl->get_PrintTaskOptions(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IGetPreviewPageEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.GetPreviewPageEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IGetPreviewPageEventArgs[] = L"Microsoft.UI.Xaml.Printing.IGetPreviewPageEventArgs";
/* [object, uuid("A68FBE17-F577-597F-B3AB-B379447149E6"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PageNumber )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs * This,
        /* [retval, out] */INT32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_get_PageNumber(This,value) \
    ( (This)->lpVtbl->get_PageNumber(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPaginateEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PaginateEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPaginateEventArgs[] = L"Microsoft.UI.Xaml.Printing.IPaginateEventArgs";
/* [object, uuid("6499C196-11A9-5EF8-91CB-52FB963BF172"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PrintTaskOptions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CPrinting_CIPrintTaskOptionsCore * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CurrentPreviewPageNumber )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs * This,
        /* [retval, out] */INT32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_get_PrintTaskOptions(This,value) \
    ( (This)->lpVtbl->get_PrintTaskOptions(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_get_CurrentPreviewPageNumber(This,value) \
    ( (This)->lpVtbl->get_CurrentPreviewPageNumber(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPrintDocument
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PrintDocument
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPrintDocument[] = L"Microsoft.UI.Xaml.Printing.IPrintDocument";
/* [object, uuid("1E40F1FC-5D33-5FE9-BA3E-954C0D161524"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DocumentSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CPrinting_CIPrintDocumentSource * * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Paginate )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPaginateEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Paginate )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_GetPreviewPage )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIGetPreviewPageEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_GetPreviewPage )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_AddPages )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIAddPagesEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_AddPages )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *AddPage )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIUIElement * pageVisual
        );
    HRESULT ( STDMETHODCALLTYPE *AddPagesComplete )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This
        );
    HRESULT ( STDMETHODCALLTYPE *SetPreviewPageCount )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */INT32 count,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CPreviewPageCountType type
        );
    HRESULT ( STDMETHODCALLTYPE *SetPreviewPage )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This,
        /* [in] */INT32 pageNumber,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIUIElement * pageVisual
        );
    HRESULT ( STDMETHODCALLTYPE *InvalidatePreview )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_get_DocumentSource(This,value) \
    ( (This)->lpVtbl->get_DocumentSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_add_Paginate(This,handler,token) \
    ( (This)->lpVtbl->add_Paginate(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_remove_Paginate(This,token) \
    ( (This)->lpVtbl->remove_Paginate(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_add_GetPreviewPage(This,handler,token) \
    ( (This)->lpVtbl->add_GetPreviewPage(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_remove_GetPreviewPage(This,token) \
    ( (This)->lpVtbl->remove_GetPreviewPage(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_add_AddPages(This,handler,token) \
    ( (This)->lpVtbl->add_AddPages(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_remove_AddPages(This,token) \
    ( (This)->lpVtbl->remove_AddPages(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_AddPage(This,pageVisual) \
    ( (This)->lpVtbl->AddPage(This,pageVisual) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_AddPagesComplete(This) \
    ( (This)->lpVtbl->AddPagesComplete(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_SetPreviewPageCount(This,count,type) \
    ( (This)->lpVtbl->SetPreviewPageCount(This,count,type) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_SetPreviewPage(This,pageNumber,pageVisual) \
    ( (This)->lpVtbl->SetPreviewPage(This,pageNumber,pageVisual) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_InvalidatePreview(This) \
    ( (This)->lpVtbl->InvalidatePreview(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPrintDocumentFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PrintDocument
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPrintDocumentFactory[] = L"Microsoft.UI.Xaml.Printing.IPrintDocumentFactory";
/* [object, uuid("C4C1BC12-84D1-539C-B416-D7E54C45AB58"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocument * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Printing.IPrintDocumentStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Printing.PrintDocument
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Printing_IPrintDocumentStatics[] = L"Microsoft.UI.Xaml.Printing.IPrintDocumentStatics";
/* [object, uuid("8975E4BC-8FC8-5E8F-A55A-BF71B9A827B7"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DocumentSourceProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_get_DocumentSourceProperty(This,value) \
    ( (This)->lpVtbl->get_DocumentSourceProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CPrinting_CIPrintDocumentStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.AddPagesEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IAddPagesEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_AddPagesEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_AddPagesEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_AddPagesEventArgs[] = L"Microsoft.UI.Xaml.Printing.AddPagesEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.GetPreviewPageEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IGetPreviewPageEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_GetPreviewPageEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_GetPreviewPageEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_GetPreviewPageEventArgs[] = L"Microsoft.UI.Xaml.Printing.GetPreviewPageEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.PaginateEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IPaginateEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PaginateEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PaginateEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_PaginateEventArgs[] = L"Microsoft.UI.Xaml.Printing.PaginateEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Printing.PrintDocument
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Printing.IPrintDocumentStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Printing.IPrintDocument ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PrintDocument_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Printing_PrintDocument_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Printing_PrintDocument[] = L"Microsoft.UI.Xaml.Printing.PrintDocument";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EPrinting_p_h__

#endif // __Microsoft2EUI2EXaml2EPrinting_h__
