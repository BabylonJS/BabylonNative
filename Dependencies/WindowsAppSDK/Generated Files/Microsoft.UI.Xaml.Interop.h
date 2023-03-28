/* Header file automatically generated from Microsoft.UI.Xaml.Interop.idl */
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
#ifndef __Microsoft2EUI2EXaml2EInterop_h__
#define __Microsoft2EUI2EXaml2EInterop_h__
#ifndef __Microsoft2EUI2EXaml2EInterop_p_h__
#define __Microsoft2EUI2EXaml2EInterop_p_h__


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

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface IBindableVectorChangedEventHandler;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler ABI::Microsoft::UI::Xaml::Interop::IBindableVectorChangedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface INotifyCollectionChangedEventHandler;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChangedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface IBindableIterable;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable ABI::Microsoft::UI::Xaml::Interop::IBindableIterable

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface IBindableIterator;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator ABI::Microsoft::UI::Xaml::Interop::IBindableIterator

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface IBindableObservableVector;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector ABI::Microsoft::UI::Xaml::Interop::IBindableObservableVector

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface IBindableVector;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector ABI::Microsoft::UI::Xaml::Interop::IBindableVector

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface IBindableVectorView;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView ABI::Microsoft::UI::Xaml::Interop::IBindableVectorView

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface INotifyCollectionChanged;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChanged

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface INotifyCollectionChangedEventArgs;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChangedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    interface INotifyCollectionChangedEventArgsFactory;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChangedEventArgsFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_FWD_DEFINED__






namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    
                    typedef enum NotifyCollectionChangedAction : int NotifyCollectionChangedAction;
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */











namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    class NotifyCollectionChangedEventArgs;
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */







/*
 *
 * Struct Microsoft.UI.Xaml.Interop.NotifyCollectionChangedAction
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
                namespace Interop {
                    /* [v1_enum, contract] */
                    enum NotifyCollectionChangedAction : int
                    {
                        NotifyCollectionChangedAction_Add = 0,
                        NotifyCollectionChangedAction_Remove = 1,
                        NotifyCollectionChangedAction_Replace = 2,
                        NotifyCollectionChangedAction_Move = 3,
                        NotifyCollectionChangedAction_Reset = 4,
                    };
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Interop.BindableVectorChangedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("624CD4E1-D007-43B1-9C03-AF4D3E6258C4"), contract] */
                    MIDL_INTERFACE("624CD4E1-D007-43B1-9C03-AF4D3E6258C4")
                    IBindableVectorChangedEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::IBindableObservableVector * vector,
                            /* [in] */IInspectable * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindableVectorChangedEventHandler=_uuidof(IBindableVectorChangedEventHandler);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("8B0909DC-2005-5D93-BF8A-725F017BAA8D"), contract] */
                    MIDL_INTERFACE("8B0909DC-2005-5D93-BF8A-725F017BAA8D")
                    INotifyCollectionChangedEventHandler : public IUnknown
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Invoke(
                            /* [in] */IInspectable * sender,
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChangedEventArgs * e
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INotifyCollectionChangedEventHandler=_uuidof(INotifyCollectionChangedEventHandler);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableIterable[] = L"Microsoft.UI.Xaml.Interop.IBindableIterable";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("036D2C08-DF29-41AF-8AA2-D774BE62BA6F"), contract] */
                    MIDL_INTERFACE("036D2C08-DF29-41AF-8AA2-D774BE62BA6F")
                    IBindableIterable : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE First(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Interop::IBindableIterator * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindableIterable=_uuidof(IBindableIterable);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableIterator
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableIterator[] = L"Microsoft.UI.Xaml.Interop.IBindableIterator";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("6A1D6C07-076D-49F2-8314-F52C9C9A8331"), contract] */
                    MIDL_INTERFACE("6A1D6C07-076D-49F2-8314-F52C9C9A8331")
                    IBindableIterator : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Current(
                            /* [retval, out] */IInspectable * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HasCurrent(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE MoveNext(
                            /* [retval, out] */::boolean * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindableIterator=_uuidof(IBindableIterator);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableObservableVector
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Xaml.Interop.IBindableVector
 *     Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableObservableVector[] = L"Microsoft.UI.Xaml.Interop.IBindableObservableVector";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("FE1EB536-7E7F-4F90-AC9A-474984AAE512"), contract] */
                    MIDL_INTERFACE("FE1EB536-7E7F-4F90-AC9A-474984AAE512")
                    IBindableObservableVector : public IInspectable
                    {
                    public:
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_VectorChanged(
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::IBindableVectorChangedEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_VectorChanged(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindableObservableVector=_uuidof(IBindableObservableVector);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableVector
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableVector[] = L"Microsoft.UI.Xaml.Interop.IBindableVector";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("393DE7DE-6FD0-4C0D-BB71-47244A113E93"), contract] */
                    MIDL_INTERFACE("393DE7DE-6FD0-4C0D-BB71-47244A113E93")
                    IBindableVector : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE GetAt(
                            /* [in] */UINT32 index,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Size(
                            /* [retval, out] */UINT32 * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetView(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Interop::IBindableVectorView * * result
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE IndexOf(
                            /* [in] */IInspectable * value,
                            /* [out] */UINT32 * index,
                            /* [retval, out] */::boolean * returnValue
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE SetAt(
                            /* [in] */UINT32 index,
                            /* [in] */IInspectable * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE InsertAt(
                            /* [in] */UINT32 index,
                            /* [in] */IInspectable * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE RemoveAt(
                            /* [in] */UINT32 index
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE Append(
                            /* [in] */IInspectable * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE RemoveAtEnd(void) = 0;
                        virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindableVector=_uuidof(IBindableVector);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableVectorView
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableVectorView[] = L"Microsoft.UI.Xaml.Interop.IBindableVectorView";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("346DD6E7-976E-4BC3-815D-ECE243BC0F33"), contract] */
                    MIDL_INTERFACE("346DD6E7-976E-4BC3-815D-ECE243BC0F33")
                    IBindableVectorView : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE GetAt(
                            /* [in] */UINT32 index,
                            /* [retval, out] */IInspectable * * result
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Size(
                            /* [retval, out] */UINT32 * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE IndexOf(
                            /* [in] */IInspectable * value,
                            /* [out] */UINT32 * index,
                            /* [retval, out] */::boolean * returnValue
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IBindableVectorView=_uuidof(IBindableVectorView);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.INotifyCollectionChanged
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_INotifyCollectionChanged[] = L"Microsoft.UI.Xaml.Interop.INotifyCollectionChanged";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("530155E1-28A5-5693-87CE-30724D95A06D"), contract] */
                    MIDL_INTERFACE("530155E1-28A5-5693-87CE-30724D95A06D")
                    INotifyCollectionChanged : public IInspectable
                    {
                    public:
                        /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_CollectionChanged(
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChangedEventHandler  * handler,
                            /* [retval, out] */EventRegistrationToken * token
                            ) = 0;
                        /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_CollectionChanged(
                            /* [in] */EventRegistrationToken token
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INotifyCollectionChanged=_uuidof(INotifyCollectionChanged);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_INotifyCollectionChangedEventArgs[] = L"Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("DA049FF2-D2E0-5FE8-8C7B-F87F26060B6F"), exclusiveto, contract] */
                    MIDL_INTERFACE("DA049FF2-D2E0-5FE8-8C7B-F87F26060B6F")
                    INotifyCollectionChangedEventArgs : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Action(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Interop::NotifyCollectionChangedAction * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NewItems(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Interop::IBindableVector * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OldItems(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Interop::IBindableVector * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NewStartingIndex(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OldStartingIndex(
                            /* [retval, out] */INT32 * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INotifyCollectionChangedEventArgs=_uuidof(INotifyCollectionChangedEventArgs);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_INotifyCollectionChangedEventArgsFactory[] = L"Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgsFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Interop {
                    /* [object, uuid("5108EBA4-4892-5A20-8374-A96815E0FD27"), exclusiveto, contract] */
                    MIDL_INTERFACE("5108EBA4-4892-5A20-8374-A96815E0FD27")
                    INotifyCollectionChangedEventArgsFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithAllParameters(
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::NotifyCollectionChangedAction action,
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::IBindableVector * newItems,
                            /* [in] */ABI::Microsoft::UI::Xaml::Interop::IBindableVector * oldItems,
                            /* [in] */INT32 newIndex,
                            /* [in] */INT32 oldIndex,
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Interop::INotifyCollectionChangedEventArgs * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_INotifyCollectionChangedEventArgsFactory=_uuidof(INotifyCollectionChangedEventArgsFactory);
                    
                } /* Interop */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Interop_NotifyCollectionChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Interop_NotifyCollectionChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Interop_NotifyCollectionChangedEventArgs[] = L"Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_FWD_DEFINED__






typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CNotifyCollectionChangedAction __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CNotifyCollectionChangedAction;

















/*
 *
 * Struct Microsoft.UI.Xaml.Interop.NotifyCollectionChangedAction
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CNotifyCollectionChangedAction
{
    NotifyCollectionChangedAction_Add = 0,
    NotifyCollectionChangedAction_Remove = 1,
    NotifyCollectionChangedAction_Replace = 2,
    NotifyCollectionChangedAction_Move = 3,
    NotifyCollectionChangedAction_Reset = 4,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Interop.BindableVectorChangedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_INTERFACE_DEFINED__
/* [object, uuid("624CD4E1-D007-43B1-9C03-AF4D3E6258C4"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * vector,
        /* [in] */IInspectable * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_Invoke(This,vector,e) \
    ( (This)->lpVtbl->Invoke(This,vector,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_INTERFACE_DEFINED__
/* [object, uuid("8B0909DC-2005-5D93-BF8A-725F017BAA8D"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableIterable[] = L"Microsoft.UI.Xaml.Interop.IBindableIterable";
/* [object, uuid("036D2C08-DF29-41AF-8AA2-D774BE62BA6F"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *First )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterableVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_First(This,result) \
    ( (This)->lpVtbl->First(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterable_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableIterator
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableIterator[] = L"Microsoft.UI.Xaml.Interop.IBindableIterator";
/* [object, uuid("6A1D6C07-076D-49F2-8314-F52C9C9A8331"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIteratorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Current )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
        /* [retval, out] */IInspectable * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
        /* [retval, out] */boolean * value
        );
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator * This,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIteratorVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIteratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_get_Current(This,value) \
    ( (This)->lpVtbl->get_Current(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_get_HasCurrent(This,value) \
    ( (This)->lpVtbl->get_HasCurrent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_MoveNext(This,result) \
    ( (This)->lpVtbl->MoveNext(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableIterator_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableObservableVector
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Xaml.Interop.IBindableVector
 *     Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableObservableVector[] = L"Microsoft.UI.Xaml.Interop.IBindableObservableVector";
/* [object, uuid("FE1EB536-7E7F-4F90-AC9A-474984AAE512"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVectorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_VectorChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorChangedEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_VectorChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVectorVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVectorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_add_VectorChanged(This,handler,token) \
    ( (This)->lpVtbl->add_VectorChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_remove_VectorChanged(This,token) \
    ( (This)->lpVtbl->remove_VectorChanged(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableObservableVector_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableVector
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableVector[] = L"Microsoft.UI.Xaml.Interop.IBindableVector";
/* [object, uuid("393DE7DE-6FD0-4C0D-BB71-47244A113E93"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetAt )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [in] */UINT32 index,
        /* [retval, out] */IInspectable * * result
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Size )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [retval, out] */UINT32 * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetView )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * * result
        );
    HRESULT ( STDMETHODCALLTYPE *IndexOf )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [in] */IInspectable * value,
        /* [out] */UINT32 * index,
        /* [retval, out] */boolean * returnValue
        );
    HRESULT ( STDMETHODCALLTYPE *SetAt )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [in] */UINT32 index,
        /* [in] */IInspectable * value
        );
    HRESULT ( STDMETHODCALLTYPE *InsertAt )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [in] */UINT32 index,
        /* [in] */IInspectable * value
        );
    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [in] */UINT32 index
        );
    HRESULT ( STDMETHODCALLTYPE *Append )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This,
        /* [in] */IInspectable * value
        );
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This
        );
    HRESULT ( STDMETHODCALLTYPE *Clear )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_GetAt(This,index,result) \
    ( (This)->lpVtbl->GetAt(This,index,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_get_Size(This,value) \
    ( (This)->lpVtbl->get_Size(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_GetView(This,result) \
    ( (This)->lpVtbl->GetView(This,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_IndexOf(This,value,index,returnValue) \
    ( (This)->lpVtbl->IndexOf(This,value,index,returnValue) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_SetAt(This,index,value) \
    ( (This)->lpVtbl->SetAt(This,index,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_InsertAt(This,index,value) \
    ( (This)->lpVtbl->InsertAt(This,index,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_RemoveAt(This,index) \
    ( (This)->lpVtbl->RemoveAt(This,index) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_Append(This,value) \
    ( (This)->lpVtbl->Append(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_RemoveAtEnd(This) \
    ( (This)->lpVtbl->RemoveAtEnd(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_Clear(This) \
    ( (This)->lpVtbl->Clear(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.IBindableVectorView
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Xaml.Interop.IBindableIterable
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_IBindableVectorView[] = L"Microsoft.UI.Xaml.Interop.IBindableVectorView";
/* [object, uuid("346DD6E7-976E-4BC3-815D-ECE243BC0F33"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorViewVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *GetAt )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
        /* [in] */UINT32 index,
        /* [retval, out] */IInspectable * * result
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Size )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
        /* [retval, out] */UINT32 * value
        );
    HRESULT ( STDMETHODCALLTYPE *IndexOf )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView * This,
        /* [in] */IInspectable * value,
        /* [out] */UINT32 * index,
        /* [retval, out] */boolean * returnValue
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorViewVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorViewVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_GetAt(This,index,result) \
    ( (This)->lpVtbl->GetAt(This,index,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_get_Size(This,value) \
    ( (This)->lpVtbl->get_Size(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_IndexOf(This,value,index,returnValue) \
    ( (This)->lpVtbl->IndexOf(This,value,index,returnValue) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVectorView_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.INotifyCollectionChanged
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_INotifyCollectionChanged[] = L"Microsoft.UI.Xaml.Interop.INotifyCollectionChanged";
/* [object, uuid("530155E1-28A5-5693-87CE-30724D95A06D"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_CollectionChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_CollectionChanged )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_add_CollectionChanged(This,handler,token) \
    ( (This)->lpVtbl->add_CollectionChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_remove_CollectionChanged(This,token) \
    ( (This)->lpVtbl->remove_CollectionChanged(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChanged_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_INotifyCollectionChangedEventArgs[] = L"Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgs";
/* [object, uuid("DA049FF2-D2E0-5FE8-8C7B-F87F26060B6F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Action )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CNotifyCollectionChangedAction * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NewItems )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OldItems )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NewStartingIndex )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OldStartingIndex )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * This,
        /* [retval, out] */INT32 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_get_Action(This,value) \
    ( (This)->lpVtbl->get_Action(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_get_NewItems(This,value) \
    ( (This)->lpVtbl->get_NewItems(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_get_OldItems(This,value) \
    ( (This)->lpVtbl->get_OldItems(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_get_NewStartingIndex(This,value) \
    ( (This)->lpVtbl->get_NewStartingIndex(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_get_OldStartingIndex(This,value) \
    ( (This)->lpVtbl->get_OldStartingIndex(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgsFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Interop_INotifyCollectionChangedEventArgsFactory[] = L"Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgsFactory";
/* [object, uuid("5108EBA4-4892-5A20-8374-A96815E0FD27"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithAllParameters )(
        __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CNotifyCollectionChangedAction action,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * newItems,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CIBindableVector * oldItems,
        /* [in] */INT32 newIndex,
        /* [in] */INT32 oldIndex,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgs * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_CreateInstanceWithAllParameters(This,action,newItems,oldItems,newIndex,oldIndex,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstanceWithAllParameters(This,action,newItems,oldItems,newIndex,oldIndex,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CInterop_CINotifyCollectionChangedEventArgsFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Interop.INotifyCollectionChangedEventArgs ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Interop_NotifyCollectionChangedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Interop_NotifyCollectionChangedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Interop_NotifyCollectionChangedEventArgs[] = L"Microsoft.UI.Xaml.Interop.NotifyCollectionChangedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EInterop_p_h__

#endif // __Microsoft2EUI2EXaml2EInterop_h__
