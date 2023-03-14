/* Header file automatically generated from Microsoft.UI.Xaml.Media.Imaging.idl */
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
#ifndef __Microsoft2EUI2EXaml2EMedia2EImaging_h__
#define __Microsoft2EUI2EXaml2EMedia2EImaging_h__
#ifndef __Microsoft2EUI2EXaml2EMedia2EImaging_p_h__
#define __Microsoft2EUI2EXaml2EMedia2EImaging_p_h__


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
#include "Microsoft.UI.Xaml.Media.h"
#include "Windows.ApplicationModel.Background.h"
#include "Windows.Graphics.Imaging.h"
#include "Windows.Storage.Streams.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IDownloadProgressEventHandler;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler ABI::Microsoft::UI::Xaml::Media::Imaging::IDownloadProgressEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IBitmapImage;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapImage

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IBitmapImageFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapImageFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IBitmapImageStatics;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapImageStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IBitmapSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IBitmapSourceFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapSourceFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IBitmapSourceStatics;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapSourceStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IDownloadProgressEventArgs;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs ABI::Microsoft::UI::Xaml::Media::Imaging::IDownloadProgressEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IRenderTargetBitmap;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap ABI::Microsoft::UI::Xaml::Media::Imaging::IRenderTargetBitmap

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IRenderTargetBitmapStatics;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics ABI::Microsoft::UI::Xaml::Media::Imaging::IRenderTargetBitmapStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISoftwareBitmapSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource ABI::Microsoft::UI::Xaml::Media::Imaging::ISoftwareBitmapSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISurfaceImageSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource ABI::Microsoft::UI::Xaml::Media::Imaging::ISurfaceImageSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISurfaceImageSourceFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory ABI::Microsoft::UI::Xaml::Media::Imaging::ISurfaceImageSourceFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISvgImageSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISvgImageSourceFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISvgImageSourceFailedEventArgs;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceFailedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISvgImageSourceOpenedEventArgs;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceOpenedEventArgs

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface ISvgImageSourceStatics;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IVirtualSurfaceImageSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource ABI::Microsoft::UI::Xaml::Media::Imaging::IVirtualSurfaceImageSource

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IVirtualSurfaceImageSourceFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory ABI::Microsoft::UI::Xaml::Media::Imaging::IVirtualSurfaceImageSourceFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IWriteableBitmap;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap ABI::Microsoft::UI::Xaml::Media::Imaging::IWriteableBitmap

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IWriteableBitmapFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory ABI::Microsoft::UI::Xaml::Media::Imaging::IWriteableBitmapFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IXamlRenderingBackgroundTask;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask ABI::Microsoft::UI::Xaml::Media::Imaging::IXamlRenderingBackgroundTask

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IXamlRenderingBackgroundTaskFactory;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory ABI::Microsoft::UI::Xaml::Media::Imaging::IXamlRenderingBackgroundTaskFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        interface IXamlRenderingBackgroundTaskOverrides;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides ABI::Microsoft::UI::Xaml::Media::Imaging::IXamlRenderingBackgroundTaskOverrides

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        enum SvgImageSourceLoadStatus : int;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#ifndef DEF___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_USE
#define DEF___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("cf51e7f6-29ae-510b-9607-0f86103c61b8"))
IAsyncOperationCompletedHandler<enum ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus> : IAsyncOperationCompletedHandler_impl<enum ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceLoadStatus>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperationCompletedHandler<enum ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus> __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_t;
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus>
//#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_t ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_USE */





#ifndef DEF___FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_USE
#define DEF___FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("04fe49a9-ed47-56ca-93ea-eaa7032271c5"))
IAsyncOperation<enum ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus> : IAsyncOperation_impl<enum ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.IAsyncOperation`1<Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceLoadStatus>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperation<enum ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus> __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_t;
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus ABI::Windows::Foundation::__FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus ABI::Windows::Foundation::IAsyncOperation<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus>
//#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_t ABI::Windows::Foundation::IAsyncOperation<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_USE */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class SvgImageSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class SvgImageSourceFailedEventArgs;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("c5f3237b-4599-5ef5-932f-4b8e723dd67e"))
ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceFailedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource*, ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceFailedEventArgs*, ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceFailedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Xaml.Media.Imaging.SvgImageSource, Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceFailedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceFailedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceFailedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class SvgImageSourceOpenedEventArgs;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef DEF___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_USE
#define DEF___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("e81e0783-eb64-5e84-b57a-ac6164814f85"))
ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceOpenedEventArgs*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource*, ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource*>,ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceOpenedEventArgs*, ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceOpenedEventArgs*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.UI.Xaml.Media.Imaging.SvgImageSource, Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceOpenedEventArgs*> __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_t;
#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceOpenedEventArgs*>
//#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource*,ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSourceOpenedEventArgs*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_USE */


#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIBuffer_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIBuffer_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Storage {
            namespace Streams {
                interface IBuffer;
            } /* Streams */
        } /* Storage */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CStorage_CStreams_CIBuffer ABI::Windows::Storage::Streams::IBuffer

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIBuffer_FWD_DEFINED__


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_USE
#define DEF___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("51c3d2fd-b8a1-5620-b746-7ee6d533aca3"))
IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IBuffer*> : IAsyncOperationCompletedHandler_impl<ABI::Windows::Storage::Streams::IBuffer*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Storage.Streams.IBuffer>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IBuffer*> __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_t;
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IBuffer*>
//#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_t ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IBuffer*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_USE
#define DEF___FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("3bee8834-b9a7-5a80-a746-5ef097227878"))
IAsyncOperation<ABI::Windows::Storage::Streams::IBuffer*> : IAsyncOperation_impl<ABI::Windows::Storage::Streams::IBuffer*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.Streams.IBuffer>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperation<ABI::Windows::Storage::Streams::IBuffer*> __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_t;
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer ABI::Windows::Foundation::__FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IBuffer*>
//#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_t ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IBuffer*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000



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


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IExceptionRoutedEventHandler;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler ABI::Microsoft::UI::Xaml::IExceptionRoutedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                interface IRoutedEventHandler;
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler ABI::Microsoft::UI::Xaml::IRoutedEventHandler

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler_FWD_DEFINED__


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






#ifndef ____x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance_FWD_DEFINED__
#define ____x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace ApplicationModel {
            namespace Background {
                interface IBackgroundTaskInstance;
            } /* Background */
        } /* ApplicationModel */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance ABI::Windows::ApplicationModel::Background::IBackgroundTaskInstance

#endif // ____x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance_FWD_DEFINED__





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
        namespace Graphics {
            namespace Imaging {
                class SoftwareBitmap;
            } /* Imaging */
        } /* Graphics */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Graphics {
            namespace Imaging {
                interface ISoftwareBitmap;
            } /* Imaging */
        } /* Graphics */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap ABI::Windows::Graphics::Imaging::ISoftwareBitmap

#endif // ____x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap_FWD_DEFINED__






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





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        
                        typedef enum BitmapCreateOptions : unsigned int BitmapCreateOptions;
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        
                        typedef enum DecodePixelType : int DecodePixelType;
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        
                        typedef enum SvgImageSourceLoadStatus : int SvgImageSourceLoadStatus;
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


























namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class BitmapImage;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class BitmapSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class DownloadProgressEventArgs;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class RenderTargetBitmap;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class SoftwareBitmapSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class SurfaceImageSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class VirtualSurfaceImageSource;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class WriteableBitmap;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        class XamlRenderingBackgroundTask;
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */














/*
 *
 * Struct Microsoft.UI.Xaml.Media.Imaging.BitmapCreateOptions
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
                namespace Media {
                    namespace Imaging {
                        /* [v1_enum, flags, contract] */
                        enum BitmapCreateOptions : unsigned int
                        {
                            BitmapCreateOptions_None = 0,
                            BitmapCreateOptions_IgnoreImageCache = 0x8,
                        };
                        
                        DEFINE_ENUM_FLAG_OPERATORS(BitmapCreateOptions)
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Media.Imaging.DecodePixelType
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
                namespace Media {
                    namespace Imaging {
                        /* [v1_enum, contract] */
                        enum DecodePixelType : int
                        {
                            DecodePixelType_Physical = 0,
                            DecodePixelType_Logical = 1,
                        };
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceLoadStatus
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
                namespace Media {
                    namespace Imaging {
                        /* [v1_enum, contract] */
                        enum SvgImageSourceLoadStatus : int
                        {
                            SvgImageSourceLoadStatus_Success = 0,
                            SvgImageSourceLoadStatus_NetworkError = 1,
                            SvgImageSourceLoadStatus_InvalidFormat = 2,
                            SvgImageSourceLoadStatus_Other = 3,
                        };
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_INTERFACE_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("9A8E4AF5-B124-5205-8AE9-3496E063C569"), contract] */
                        MIDL_INTERFACE("9A8E4AF5-B124-5205-8AE9-3496E063C569")
                        IDownloadProgressEventHandler : public IUnknown
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE Invoke(
                                /* [in] */IInspectable * sender,
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Imaging::IDownloadProgressEventArgs * e
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IDownloadProgressEventHandler=_uuidof(IDownloadProgressEventHandler);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapImage
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapImage[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapImage";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("5CC29916-A411-5BC2-A3C5-A00D99A59DA8"), exclusiveto, contract] */
                        MIDL_INTERFACE("5CC29916-A411-5BC2-A3C5-A00D99A59DA8")
                        IBitmapImage : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CreateOptions(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::BitmapCreateOptions * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CreateOptions(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Imaging::BitmapCreateOptions value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UriSource(
                                /* [retval, out] */ABI::Windows::Foundation::IUriRuntimeClass * * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_UriSource(
                                /* [in] */ABI::Windows::Foundation::IUriRuntimeClass * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DecodePixelWidth(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DecodePixelWidth(
                                /* [in] */INT32 value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DecodePixelHeight(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DecodePixelHeight(
                                /* [in] */INT32 value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DecodePixelType(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::DecodePixelType * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DecodePixelType(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Imaging::DecodePixelType value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsAnimatedBitmap(
                                /* [retval, out] */::boolean * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsPlaying(
                                /* [retval, out] */::boolean * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AutoPlay(
                                /* [retval, out] */::boolean * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AutoPlay(
                                /* [in] */::boolean value
                                ) = 0;
                            /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_DownloadProgress(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Imaging::IDownloadProgressEventHandler  * handler,
                                /* [retval, out] */EventRegistrationToken * token
                                ) = 0;
                            /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_DownloadProgress(
                                /* [in] */EventRegistrationToken token
                                ) = 0;
                            /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ImageOpened(
                                /* [in] */ABI::Microsoft::UI::Xaml::IRoutedEventHandler  * handler,
                                /* [retval, out] */EventRegistrationToken * token
                                ) = 0;
                            /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ImageOpened(
                                /* [in] */EventRegistrationToken token
                                ) = 0;
                            /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ImageFailed(
                                /* [in] */ABI::Microsoft::UI::Xaml::IExceptionRoutedEventHandler  * handler,
                                /* [retval, out] */EventRegistrationToken * token
                                ) = 0;
                            /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ImageFailed(
                                /* [in] */EventRegistrationToken token
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE Play(void) = 0;
                            virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IBitmapImage=_uuidof(IBitmapImage);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapImageFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapImageFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapImageFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("F037E0E9-F229-522E-95C9-DA2211A14B05"), exclusiveto, contract] */
                        MIDL_INTERFACE("F037E0E9-F229-522E-95C9-DA2211A14B05")
                        IBitmapImageFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithUriSource(
                                /* [in] */ABI::Windows::Foundation::IUriRuntimeClass * uriSource,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapImage * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IBitmapImageFactory=_uuidof(IBitmapImageFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapImageStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapImageStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapImageStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("4BCF71A9-1897-51DC-8E3F-2C5C796D1CD9"), exclusiveto, contract] */
                        MIDL_INTERFACE("4BCF71A9-1897-51DC-8E3F-2C5C796D1CD9")
                        IBitmapImageStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CreateOptionsProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UriSourceProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DecodePixelWidthProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DecodePixelHeightProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DecodePixelTypeProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsAnimatedBitmapProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsPlayingProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AutoPlayProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IBitmapImageStatics=_uuidof(IBitmapImageStatics);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("8424269D-9B82-534F-8FEA-AF5B5EF96BF2"), exclusiveto, contract] */
                        MIDL_INTERFACE("8424269D-9B82-534F-8FEA-AF5B5EF96BF2")
                        IBitmapSource : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelWidth(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelHeight(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE SetSource(
                                /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * streamSource
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE SetSourceAsync(
                                /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * streamSource,
                                /* [retval, out] */ABI::Windows::Foundation::IAsyncAction * * operation
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IBitmapSource=_uuidof(IBitmapSource);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("0392F025-1868-5876-AD67-12E94A8DA5BF"), exclusiveto, contract] */
                        MIDL_INTERFACE("0392F025-1868-5876-AD67-12E94A8DA5BF")
                        IBitmapSourceFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::IBitmapSource * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IBitmapSourceFactory=_uuidof(IBitmapSourceFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapSourceStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("EFA3745E-4400-5F0B-BDC7-3F2911A3D719"), exclusiveto, contract] */
                        MIDL_INTERFACE("EFA3745E-4400-5F0B-BDC7-3F2911A3D719")
                        IBitmapSourceStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelWidthProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelHeightProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IBitmapSourceStatics=_uuidof(IBitmapSourceStatics);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IDownloadProgressEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IDownloadProgressEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.IDownloadProgressEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("9A0EA80B-1A17-50D5-83F3-377738212619"), exclusiveto, contract] */
                        MIDL_INTERFACE("9A0EA80B-1A17-50D5-83F3-377738212619")
                        IDownloadProgressEventArgs : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Progress(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Progress(
                                /* [in] */INT32 value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IDownloadProgressEventArgs=_uuidof(IDownloadProgressEventArgs);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IRenderTargetBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmap";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("CF10407D-FA8B-57A3-9574-710529AE0B04"), exclusiveto, contract] */
                        MIDL_INTERFACE("CF10407D-FA8B-57A3-9574-710529AE0B04")
                        IRenderTargetBitmap : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelWidth(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelHeight(
                                /* [retval, out] */INT32 * value
                                ) = 0;
                            /* [overload] */virtual HRESULT STDMETHODCALLTYPE RenderAsync(
                                /* [in] */ABI::Microsoft::UI::Xaml::IUIElement * element,
                                /* [retval, out] */ABI::Windows::Foundation::IAsyncAction * * operation
                                ) = 0;
                            /* [overload] */virtual HRESULT STDMETHODCALLTYPE RenderToSizeAsync(
                                /* [in] */ABI::Microsoft::UI::Xaml::IUIElement * element,
                                /* [in] */INT32 scaledWidth,
                                /* [in] */INT32 scaledHeight,
                                /* [retval, out] */ABI::Windows::Foundation::IAsyncAction * * operation
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE GetPixelsAsync(
                                /* [retval, out] */__FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * * operation
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IRenderTargetBitmap=_uuidof(IRenderTargetBitmap);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmapStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IRenderTargetBitmapStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmapStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("83E822E4-9F84-5986-93B0-E4F7019C367D"), exclusiveto, contract] */
                        MIDL_INTERFACE("83E822E4-9F84-5986-93B0-E4F7019C367D")
                        IRenderTargetBitmapStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelWidthProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelHeightProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IRenderTargetBitmapStatics=_uuidof(IRenderTargetBitmapStatics);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISoftwareBitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SoftwareBitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISoftwareBitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.ISoftwareBitmapSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("A6ACA802-1F24-5A1E-BF08-781A85ED5511"), exclusiveto, contract] */
                        MIDL_INTERFACE("A6ACA802-1F24-5A1E-BF08-781A85ED5511")
                        ISoftwareBitmapSource : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE SetBitmapAsync(
                                /* [in] */ABI::Windows::Graphics::Imaging::ISoftwareBitmap * softwareBitmap,
                                /* [retval, out] */ABI::Windows::Foundation::IAsyncAction * * operation
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ISoftwareBitmapSource=_uuidof(ISoftwareBitmapSource);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("AC078D9C-D0E0-5FF9-B73E-98E82E4C8D36"), exclusiveto, contract] */
                        MIDL_INTERFACE("AC078D9C-D0E0-5FF9-B73E-98E82E4C8D36")
                        ISurfaceImageSource : public IInspectable
                        {
                        public:
                            
                        };

                        MIDL_CONST_ID IID & IID_ISurfaceImageSource=_uuidof(ISurfaceImageSource);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISurfaceImageSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSourceFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("09A26ED2-11B3-5EF1-AC56-20D064CCCA34"), exclusiveto, contract] */
                        MIDL_INTERFACE("09A26ED2-11B3-5EF1-AC56-20D064CCCA34")
                        ISurfaceImageSourceFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithDimensions(
                                /* [in] */INT32 pixelWidth,
                                /* [in] */INT32 pixelHeight,
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::ISurfaceImageSource * * value
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithDimensionsAndOpacity(
                                /* [in] */INT32 pixelWidth,
                                /* [in] */INT32 pixelHeight,
                                /* [in] */::boolean isOpaque,
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::ISurfaceImageSource * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ISurfaceImageSourceFactory=_uuidof(ISurfaceImageSourceFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("D5B61D3C-B68D-53A2-B07B-BA6ADFDD5887"), exclusiveto, contract] */
                        MIDL_INTERFACE("D5B61D3C-B68D-53A2-B07B-BA6ADFDD5887")
                        ISvgImageSource : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UriSource(
                                /* [retval, out] */ABI::Windows::Foundation::IUriRuntimeClass * * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_UriSource(
                                /* [in] */ABI::Windows::Foundation::IUriRuntimeClass * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RasterizePixelWidth(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RasterizePixelWidth(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RasterizePixelHeight(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RasterizePixelHeight(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Opened(
                                /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs * handler,
                                /* [retval, out] */EventRegistrationToken * token
                                ) = 0;
                            /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Opened(
                                /* [in] */EventRegistrationToken token
                                ) = 0;
                            /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_OpenFailed(
                                /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs * handler,
                                /* [retval, out] */EventRegistrationToken * token
                                ) = 0;
                            /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_OpenFailed(
                                /* [in] */EventRegistrationToken token
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE SetSourceAsync(
                                /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * streamSource,
                                /* [retval, out] */__FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * * operation
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ISvgImageSource=_uuidof(ISvgImageSource);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("2F85673F-AC64-570D-9BDA-94FA082EEAD9"), exclusiveto, contract] */
                        MIDL_INTERFACE("2F85673F-AC64-570D-9BDA-94FA082EEAD9")
                        ISvgImageSourceFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource * * value
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithUriSource(
                                /* [in] */ABI::Windows::Foundation::IUriRuntimeClass * uriSource,
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::ISvgImageSource * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ISvgImageSourceFactory=_uuidof(ISvgImageSourceFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceFailedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFailedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("76E66278-7804-5439-A50D-14C5BA896714"), exclusiveto, contract] */
                        MIDL_INTERFACE("76E66278-7804-5439-A50D-14C5BA896714")
                        ISvgImageSourceFailedEventArgs : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Status(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::SvgImageSourceLoadStatus * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ISvgImageSourceFailedEventArgs=_uuidof(ISvgImageSourceFailedEventArgs);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceOpenedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceOpenedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceOpenedEventArgs";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("1C9860D5-38D0-5B21-8D48-072F1E254E39"), exclusiveto, contract] */
                        MIDL_INTERFACE("1C9860D5-38D0-5B21-8D48-072F1E254E39")
                        ISvgImageSourceOpenedEventArgs : public IInspectable
                        {
                        public:
                            
                        };

                        MIDL_CONST_ID IID & IID_ISvgImageSourceOpenedEventArgs=_uuidof(ISvgImageSourceOpenedEventArgs);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("E3AD1068-F4C6-5513-A777-2980F0BA41BD"), exclusiveto, contract] */
                        MIDL_INTERFACE("E3AD1068-F4C6-5513-A777-2980F0BA41BD")
                        ISvgImageSourceStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UriSourceProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RasterizePixelWidthProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RasterizePixelHeightProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ISvgImageSourceStatics=_uuidof(ISvgImageSourceStatics);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IVirtualSurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSource";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("E4FF96A6-FEDE-589C-A007-4178B53B6739"), exclusiveto, contract] */
                        MIDL_INTERFACE("E4FF96A6-FEDE-589C-A007-4178B53B6739")
                        IVirtualSurfaceImageSource : public IInspectable
                        {
                        public:
                            
                        };

                        MIDL_CONST_ID IID & IID_IVirtualSurfaceImageSource=_uuidof(IVirtualSurfaceImageSource);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IVirtualSurfaceImageSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSourceFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("08490F2C-04A8-5031-B9C7-707060D7CD48"), exclusiveto, contract] */
                        MIDL_INTERFACE("08490F2C-04A8-5031-B9C7-707060D7CD48")
                        IVirtualSurfaceImageSourceFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithDimensions(
                                /* [in] */INT32 pixelWidth,
                                /* [in] */INT32 pixelHeight,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::IVirtualSurfaceImageSource * * value
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithDimensionsAndOpacity(
                                /* [in] */INT32 pixelWidth,
                                /* [in] */INT32 pixelHeight,
                                /* [in] */::boolean isOpaque,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::IVirtualSurfaceImageSource * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IVirtualSurfaceImageSourceFactory=_uuidof(IVirtualSurfaceImageSourceFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IWriteableBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmap";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("78C824A9-0E43-5F1E-93BC-D046CCA82B7E"), exclusiveto, contract] */
                        MIDL_INTERFACE("78C824A9-0E43-5F1E-93BC-D046CCA82B7E")
                        IWriteableBitmap : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PixelBuffer(
                                /* [retval, out] */ABI::Windows::Storage::Streams::IBuffer * * value
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE Invalidate(void) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IWriteableBitmap=_uuidof(IWriteableBitmap);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmapFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IWriteableBitmapFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmapFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("26E861D9-B080-512B-96C4-80050E7E08D1"), exclusiveto, contract] */
                        MIDL_INTERFACE("26E861D9-B080-512B-96C4-80050E7E08D1")
                        IWriteableBitmapFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithDimensions(
                                /* [in] */INT32 pixelWidth,
                                /* [in] */INT32 pixelHeight,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::IWriteableBitmap * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IWriteableBitmapFactory=_uuidof(IWriteableBitmapFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTask
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IXamlRenderingBackgroundTask[] = L"Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTask";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("7807000C-A050-5121-AC74-3322D5358E39"), exclusiveto, contract] */
                        MIDL_INTERFACE("7807000C-A050-5121-AC74-3322D5358E39")
                        IXamlRenderingBackgroundTask : public IInspectable
                        {
                        public:
                            
                        };

                        MIDL_CONST_ID IID & IID_IXamlRenderingBackgroundTask=_uuidof(IXamlRenderingBackgroundTask);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IXamlRenderingBackgroundTaskFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("205247A3-9FFE-599A-A21A-7181442A9D75"), exclusiveto, contract] */
                        MIDL_INTERFACE("205247A3-9FFE-599A-A21A-7181442A9D75")
                        IXamlRenderingBackgroundTaskFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Imaging::IXamlRenderingBackgroundTask * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IXamlRenderingBackgroundTaskFactory=_uuidof(IXamlRenderingBackgroundTaskFactory);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskOverrides
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IXamlRenderingBackgroundTaskOverrides[] = L"Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskOverrides";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Imaging {
                        /* [object, uuid("18733237-324B-57C0-89B2-5875472ACC80"), exclusiveto, contract] */
                        MIDL_INTERFACE("18733237-324B-57C0-89B2-5875472ACC80")
                        IXamlRenderingBackgroundTaskOverrides : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE OnRun(
                                /* [in] */ABI::Windows::ApplicationModel::Background::IBackgroundTaskInstance * taskInstance
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IXamlRenderingBackgroundTaskOverrides=_uuidof(IXamlRenderingBackgroundTaskOverrides);
                        
                    } /* Imaging */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *   Type can be activated via the Microsoft.UI.Xaml.Media.Imaging.IBitmapImageFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.IBitmapImageStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IBitmapImage ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapImage_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapImage_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_BitmapImage[] = L"Microsoft.UI.Xaml.Media.Imaging.BitmapImage";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IBitmapSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_BitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.BitmapSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IDownloadProgressEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_DownloadProgressEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_DownloadProgressEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_DownloadProgressEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmapStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmap ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_RenderTargetBitmap_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_RenderTargetBitmap_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_RenderTargetBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SoftwareBitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISoftwareBitmapSource ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SoftwareBitmapSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SoftwareBitmapSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SoftwareBitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.SoftwareBitmapSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SurfaceImageSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SurfaceImageSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISvgImageSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SvgImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.SvgImageSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFailedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceFailedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceFailedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceFailedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceOpenedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceOpenedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceOpenedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceOpenedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSourceFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmapFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmap ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_WriteableBitmap_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_WriteableBitmap_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_WriteableBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTask ** Default Interface **
 *    Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskOverrides
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_XamlRenderingBackgroundTask_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_XamlRenderingBackgroundTask_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_XamlRenderingBackgroundTask[] = L"Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000






#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions
enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CSvgImageSourceLoadStatus;
#if !defined(____FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_INTERFACE_DEFINED__)
#define ____FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_INTERFACE_DEFINED__

typedef interface __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus;

// Forward declare the async operation.
typedef interface __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus;

typedef struct __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatusVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This,/* [in] */ __RPC__in_opt __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus *asyncInfo, /* [in] */ AsyncStatus status);
    END_INTERFACE
} __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatusVtbl;

interface __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus
{
    CONST_VTBL struct __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatusVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_Invoke(This,asyncInfo,status)	\
    ( (This)->lpVtbl -> Invoke(This,asyncInfo,status) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_INTERFACE_DEFINED__



#if !defined(____FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_INTERFACE_DEFINED__)
#define ____FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_INTERFACE_DEFINED__

typedef interface __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus;

typedef struct __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatusVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Completed )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This, /* [in] */ __RPC__in_opt __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus *handler);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Completed )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This, /* [retval][out] */ __RPC__deref_out_opt __FIAsyncOperationCompletedHandler_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus **handler);
    HRESULT ( STDMETHODCALLTYPE *GetResults )(__RPC__in __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * This, /* [retval][out] */ __RPC__out enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CSvgImageSourceLoadStatus *results);
    END_INTERFACE
} __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatusVtbl;

interface __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus
{
    CONST_VTBL struct __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatusVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_put_Completed(This,handler)	\
    ( (This)->lpVtbl -> put_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_get_Completed(This,handler)	\
    ( (This)->lpVtbl -> get_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_GetResults(This,results)	\
    ( (This)->lpVtbl -> GetResults(This,results) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus_INTERFACE_DEFINED__




#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000



#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs;

typedef struct __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * sender,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgsVtbl;

interface __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__

#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIBuffer_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIBuffer_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CStorage_CStreams_CIBuffer __x_ABI_CWindows_CStorage_CStreams_CIBuffer;

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIBuffer_FWD_DEFINED__


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_INTERFACE_DEFINED__)
#define ____FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_INTERFACE_DEFINED__

typedef interface __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer;

// Forward declare the async operation.
typedef interface __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer;

typedef struct __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBufferVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer * This,/* [in] */ __RPC__in_opt __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer *asyncInfo, /* [in] */ AsyncStatus status);
    END_INTERFACE
} __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBufferVtbl;

interface __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer
{
    CONST_VTBL struct __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBufferVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_Invoke(This,asyncInfo,status)	\
    ( (This)->lpVtbl -> Invoke(This,asyncInfo,status) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_INTERFACE_DEFINED__)
#define ____FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_INTERFACE_DEFINED__

typedef interface __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer;

typedef struct __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBufferVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Completed )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This, /* [in] */ __RPC__in_opt __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer *handler);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Completed )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This, /* [retval][out] */ __RPC__deref_out_opt __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIBuffer **handler);
    HRESULT ( STDMETHODCALLTYPE *GetResults )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * This, /* [retval][out] */ __RPC__out __x_ABI_CWindows_CStorage_CStreams_CIBuffer * *results);
    END_INTERFACE
} __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBufferVtbl;

interface __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer
{
    CONST_VTBL struct __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBufferVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_put_Completed(This,handler)	\
    ( (This)->lpVtbl -> put_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_get_Completed(This,handler)	\
    ( (This)->lpVtbl -> get_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_GetResults(This,results)	\
    ( (This)->lpVtbl -> GetResults(This,results) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler __x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement __x_ABI_CMicrosoft_CUI_CXaml_CIUIElement;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIUIElement_FWD_DEFINED__






#ifndef ____x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance_FWD_DEFINED__
#define ____x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance __x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance;

#endif // ____x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance_FWD_DEFINED__





#ifndef ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIAsyncAction __x_ABI_CWindows_CFoundation_CIAsyncAction;

#endif // ____x_ABI_CWindows_CFoundation_CIAsyncAction_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIClosable __x_ABI_CWindows_CFoundation_CIClosable;

#endif // ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIUriRuntimeClass __x_ABI_CWindows_CFoundation_CIUriRuntimeClass;

#endif // ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_FWD_DEFINED__




#ifndef ____x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap __x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap;

#endif // ____x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap_FWD_DEFINED__






#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream;

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__






typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CBitmapCreateOptions __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CBitmapCreateOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CDecodePixelType __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CDecodePixelType;


typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CSvgImageSourceLoadStatus __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CSvgImageSourceLoadStatus;


















































/*
 *
 * Struct Microsoft.UI.Xaml.Media.Imaging.BitmapCreateOptions
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, flags, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CBitmapCreateOptions
{
    BitmapCreateOptions_None = 0,
    BitmapCreateOptions_IgnoreImageCache = 0x8,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Media.Imaging.DecodePixelType
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CDecodePixelType
{
    DecodePixelType_Physical = 0,
    DecodePixelType_Logical = 1,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceLoadStatus
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */

#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CSvgImageSourceLoadStatus
{
    SvgImageSourceLoadStatus_Success = 0,
    SvgImageSourceLoadStatus_NetworkError = 1,
    SvgImageSourceLoadStatus_InvalidFormat = 2,
    SvgImageSourceLoadStatus_Other = 3,
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Delegate Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventHandler
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_INTERFACE_DEFINED__
/* [object, uuid("9A8E4AF5-B124-5205-8AE9-3496E063C569"), contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandlerVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler * This,
        /* [in] */IInspectable * sender,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * e
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandlerVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_Invoke(This,sender,e) \
    ( (This)->lpVtbl->Invoke(This,sender,e) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapImage
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapImage[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapImage";
/* [object, uuid("5CC29916-A411-5BC2-A3C5-A00D99A59DA8"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CreateOptions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CBitmapCreateOptions * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CreateOptions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CBitmapCreateOptions value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UriSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_UriSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DecodePixelWidth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DecodePixelWidth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */INT32 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DecodePixelHeight )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DecodePixelHeight )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */INT32 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DecodePixelType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CDecodePixelType * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DecodePixelType )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CDecodePixelType value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsAnimatedBitmap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsPlaying )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */boolean * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AutoPlay )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AutoPlay )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */boolean value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_DownloadProgress )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_DownloadProgress )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ImageOpened )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIRoutedEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ImageOpened )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ImageFailed )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIExceptionRoutedEventHandler  * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ImageFailed )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *Play )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This
        );
    HRESULT ( STDMETHODCALLTYPE *Stop )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_CreateOptions(This,value) \
    ( (This)->lpVtbl->get_CreateOptions(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_put_CreateOptions(This,value) \
    ( (This)->lpVtbl->put_CreateOptions(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_UriSource(This,value) \
    ( (This)->lpVtbl->get_UriSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_put_UriSource(This,value) \
    ( (This)->lpVtbl->put_UriSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_DecodePixelWidth(This,value) \
    ( (This)->lpVtbl->get_DecodePixelWidth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_put_DecodePixelWidth(This,value) \
    ( (This)->lpVtbl->put_DecodePixelWidth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_DecodePixelHeight(This,value) \
    ( (This)->lpVtbl->get_DecodePixelHeight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_put_DecodePixelHeight(This,value) \
    ( (This)->lpVtbl->put_DecodePixelHeight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_DecodePixelType(This,value) \
    ( (This)->lpVtbl->get_DecodePixelType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_put_DecodePixelType(This,value) \
    ( (This)->lpVtbl->put_DecodePixelType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_IsAnimatedBitmap(This,value) \
    ( (This)->lpVtbl->get_IsAnimatedBitmap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_IsPlaying(This,value) \
    ( (This)->lpVtbl->get_IsPlaying(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_get_AutoPlay(This,value) \
    ( (This)->lpVtbl->get_AutoPlay(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_put_AutoPlay(This,value) \
    ( (This)->lpVtbl->put_AutoPlay(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_add_DownloadProgress(This,handler,token) \
    ( (This)->lpVtbl->add_DownloadProgress(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_remove_DownloadProgress(This,token) \
    ( (This)->lpVtbl->remove_DownloadProgress(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_add_ImageOpened(This,handler,token) \
    ( (This)->lpVtbl->add_ImageOpened(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_remove_ImageOpened(This,token) \
    ( (This)->lpVtbl->remove_ImageOpened(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_add_ImageFailed(This,handler,token) \
    ( (This)->lpVtbl->add_ImageFailed(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_remove_ImageFailed(This,token) \
    ( (This)->lpVtbl->remove_ImageFailed(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_Play(This) \
    ( (This)->lpVtbl->Play(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_Stop(This) \
    ( (This)->lpVtbl->Stop(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapImageFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapImageFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapImageFactory";
/* [object, uuid("F037E0E9-F229-522E-95C9-DA2211A14B05"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithUriSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * uriSource,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImage * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_CreateInstanceWithUriSource(This,uriSource,value) \
    ( (This)->lpVtbl->CreateInstanceWithUriSource(This,uriSource,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapImageStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapImageStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapImageStatics";
/* [object, uuid("4BCF71A9-1897-51DC-8E3F-2C5C796D1CD9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CreateOptionsProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UriSourceProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DecodePixelWidthProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DecodePixelHeightProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DecodePixelTypeProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsAnimatedBitmapProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsPlayingProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AutoPlayProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_CreateOptionsProperty(This,value) \
    ( (This)->lpVtbl->get_CreateOptionsProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_UriSourceProperty(This,value) \
    ( (This)->lpVtbl->get_UriSourceProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_DecodePixelWidthProperty(This,value) \
    ( (This)->lpVtbl->get_DecodePixelWidthProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_DecodePixelHeightProperty(This,value) \
    ( (This)->lpVtbl->get_DecodePixelHeightProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_DecodePixelTypeProperty(This,value) \
    ( (This)->lpVtbl->get_DecodePixelTypeProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_IsAnimatedBitmapProperty(This,value) \
    ( (This)->lpVtbl->get_IsAnimatedBitmapProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_IsPlayingProperty(This,value) \
    ( (This)->lpVtbl->get_IsPlayingProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_get_AutoPlayProperty(This,value) \
    ( (This)->lpVtbl->get_AutoPlayProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapImageStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapSource";
/* [object, uuid("8424269D-9B82-534F-8FEA-AF5B5EF96BF2"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelWidth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelHeight )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
        /* [retval, out] */INT32 * value
        );
    HRESULT ( STDMETHODCALLTYPE *SetSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * streamSource
        );
    HRESULT ( STDMETHODCALLTYPE *SetSourceAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * This,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * streamSource,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIAsyncAction * * operation
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_get_PixelWidth(This,value) \
    ( (This)->lpVtbl->get_PixelWidth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_get_PixelHeight(This,value) \
    ( (This)->lpVtbl->get_PixelHeight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_SetSource(This,streamSource) \
    ( (This)->lpVtbl->SetSource(This,streamSource) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_SetSourceAsync(This,streamSource,operation) \
    ( (This)->lpVtbl->SetSourceAsync(This,streamSource,operation) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceFactory";
/* [object, uuid("0392F025-1868-5876-AD67-12E94A8DA5BF"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSource * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IBitmapSourceStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceStatics";
/* [object, uuid("EFA3745E-4400-5F0B-BDC7-3F2911A3D719"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelWidthProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelHeightProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_get_PixelWidthProperty(This,value) \
    ( (This)->lpVtbl->get_PixelWidthProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_get_PixelHeightProperty(This,value) \
    ( (This)->lpVtbl->get_PixelHeightProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIBitmapSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IDownloadProgressEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IDownloadProgressEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.IDownloadProgressEventArgs";
/* [object, uuid("9A0EA80B-1A17-50D5-83F3-377738212619"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Progress )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Progress )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs * This,
        /* [in] */INT32 value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_get_Progress(This,value) \
    ( (This)->lpVtbl->get_Progress(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_put_Progress(This,value) \
    ( (This)->lpVtbl->put_Progress(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIDownloadProgressEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IRenderTargetBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmap";
/* [object, uuid("CF10407D-FA8B-57A3-9574-710529AE0B04"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelWidth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelHeight )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
        /* [retval, out] */INT32 * value
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *RenderAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIUIElement * element,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIAsyncAction * * operation
        );
    /* [overload] */HRESULT ( STDMETHODCALLTYPE *RenderToSizeAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CIUIElement * element,
        /* [in] */INT32 scaledWidth,
        /* [in] */INT32 scaledHeight,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIAsyncAction * * operation
        );
    HRESULT ( STDMETHODCALLTYPE *GetPixelsAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap * This,
        /* [retval, out] */__FIAsyncOperation_1_Windows__CStorage__CStreams__CIBuffer * * operation
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_get_PixelWidth(This,value) \
    ( (This)->lpVtbl->get_PixelWidth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_get_PixelHeight(This,value) \
    ( (This)->lpVtbl->get_PixelHeight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_RenderAsync(This,element,operation) \
    ( (This)->lpVtbl->RenderAsync(This,element,operation) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_RenderToSizeAsync(This,element,scaledWidth,scaledHeight,operation) \
    ( (This)->lpVtbl->RenderToSizeAsync(This,element,scaledWidth,scaledHeight,operation) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_GetPixelsAsync(This,operation) \
    ( (This)->lpVtbl->GetPixelsAsync(This,operation) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmap_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmapStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IRenderTargetBitmapStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmapStatics";
/* [object, uuid("83E822E4-9F84-5986-93B0-E4F7019C367D"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelWidthProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelHeightProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_get_PixelWidthProperty(This,value) \
    ( (This)->lpVtbl->get_PixelWidthProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_get_PixelHeightProperty(This,value) \
    ( (This)->lpVtbl->get_PixelHeightProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIRenderTargetBitmapStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISoftwareBitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SoftwareBitmapSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISoftwareBitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.ISoftwareBitmapSource";
/* [object, uuid("A6ACA802-1F24-5A1E-BF08-781A85ED5511"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *SetBitmapAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CImaging_CISoftwareBitmap * softwareBitmap,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIAsyncAction * * operation
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_SetBitmapAsync(This,softwareBitmap,operation) \
    ( (This)->lpVtbl->SetBitmapAsync(This,softwareBitmap,operation) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISoftwareBitmapSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSource";
/* [object, uuid("AC078D9C-D0E0-5FF9-B73E-98E82E4C8D36"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISurfaceImageSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSourceFactory";
/* [object, uuid("09A26ED2-11B3-5EF1-AC56-20D064CCCA34"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithDimensions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This,
        /* [in] */INT32 pixelWidth,
        /* [in] */INT32 pixelHeight,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * * value
        );
    HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithDimensionsAndOpacity )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory * This,
        /* [in] */INT32 pixelWidth,
        /* [in] */INT32 pixelHeight,
        /* [in] */boolean isOpaque,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSource * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_CreateInstanceWithDimensions(This,pixelWidth,pixelHeight,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstanceWithDimensions(This,pixelWidth,pixelHeight,baseInterface,innerInterface,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_CreateInstanceWithDimensionsAndOpacity(This,pixelWidth,pixelHeight,isOpaque,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstanceWithDimensionsAndOpacity(This,pixelWidth,pixelHeight,isOpaque,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISurfaceImageSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSource";
/* [object, uuid("D5B61D3C-B68D-53A2-B07B-BA6ADFDD5887"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UriSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_UriSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RasterizePixelWidth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RasterizePixelWidth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RasterizePixelHeight )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RasterizePixelHeight )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */DOUBLE value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Opened )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceOpenedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Opened )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_OpenFailed )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSource_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceFailedEventArgs * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_OpenFailed )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *SetSourceAsync )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * This,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * streamSource,
        /* [retval, out] */__FIAsyncOperation_1_Microsoft__CUI__CXaml__CMedia__CImaging__CSvgImageSourceLoadStatus * * operation
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_get_UriSource(This,value) \
    ( (This)->lpVtbl->get_UriSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_put_UriSource(This,value) \
    ( (This)->lpVtbl->put_UriSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_get_RasterizePixelWidth(This,value) \
    ( (This)->lpVtbl->get_RasterizePixelWidth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_put_RasterizePixelWidth(This,value) \
    ( (This)->lpVtbl->put_RasterizePixelWidth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_get_RasterizePixelHeight(This,value) \
    ( (This)->lpVtbl->get_RasterizePixelHeight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_put_RasterizePixelHeight(This,value) \
    ( (This)->lpVtbl->put_RasterizePixelHeight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_add_Opened(This,handler,token) \
    ( (This)->lpVtbl->add_Opened(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_remove_Opened(This,token) \
    ( (This)->lpVtbl->remove_Opened(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_add_OpenFailed(This,handler,token) \
    ( (This)->lpVtbl->add_OpenFailed(This,handler,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_remove_OpenFailed(This,token) \
    ( (This)->lpVtbl->remove_OpenFailed(This,token) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_SetSourceAsync(This,streamSource,operation) \
    ( (This)->lpVtbl->SetSourceAsync(This,streamSource,operation) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFactory";
/* [object, uuid("2F85673F-AC64-570D-9BDA-94FA082EEAD9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * * value
        );
    HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithUriSource )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CIUriRuntimeClass * uriSource,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSource * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_CreateInstanceWithUriSource(This,uriSource,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstanceWithUriSource(This,uriSource,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceFailedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFailedEventArgs";
/* [object, uuid("76E66278-7804-5439-A50D-14C5BA896714"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Status )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CSvgImageSourceLoadStatus * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_get_Status(This,value) \
    ( (This)->lpVtbl->get_Status(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceFailedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceOpenedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceOpenedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceOpenedEventArgs";
/* [object, uuid("1C9860D5-38D0-5B21-8D48-072F1E254E39"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceOpenedEventArgs_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_ISvgImageSourceStatics[] = L"Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceStatics";
/* [object, uuid("E3AD1068-F4C6-5513-A777-2980F0BA41BD"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UriSourceProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RasterizePixelWidthProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RasterizePixelHeightProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_get_UriSourceProperty(This,value) \
    ( (This)->lpVtbl->get_UriSourceProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_get_RasterizePixelWidthProperty(This,value) \
    ( (This)->lpVtbl->get_RasterizePixelWidthProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_get_RasterizePixelHeightProperty(This,value) \
    ( (This)->lpVtbl->get_RasterizePixelHeightProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CISvgImageSourceStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IVirtualSurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSource";
/* [object, uuid("E4FF96A6-FEDE-589C-A007-4178B53B6739"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSourceFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IVirtualSurfaceImageSourceFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSourceFactory";
/* [object, uuid("08490F2C-04A8-5031-B9C7-707060D7CD48"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithDimensions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This,
        /* [in] */INT32 pixelWidth,
        /* [in] */INT32 pixelHeight,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * * value
        );
    HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithDimensionsAndOpacity )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory * This,
        /* [in] */INT32 pixelWidth,
        /* [in] */INT32 pixelHeight,
        /* [in] */boolean isOpaque,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSource * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_CreateInstanceWithDimensions(This,pixelWidth,pixelHeight,value) \
    ( (This)->lpVtbl->CreateInstanceWithDimensions(This,pixelWidth,pixelHeight,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_CreateInstanceWithDimensionsAndOpacity(This,pixelWidth,pixelHeight,isOpaque,value) \
    ( (This)->lpVtbl->CreateInstanceWithDimensionsAndOpacity(This,pixelWidth,pixelHeight,isOpaque,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIVirtualSurfaceImageSourceFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IWriteableBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmap";
/* [object, uuid("78C824A9-0E43-5F1E-93BC-D046CCA82B7E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PixelBuffer )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This,
        /* [retval, out] */__x_ABI_CWindows_CStorage_CStreams_CIBuffer * * value
        );
    HRESULT ( STDMETHODCALLTYPE *Invalidate )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_get_PixelBuffer(This,value) \
    ( (This)->lpVtbl->get_PixelBuffer(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_Invalidate(This) \
    ( (This)->lpVtbl->Invalidate(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmapFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IWriteableBitmapFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmapFactory";
/* [object, uuid("26E861D9-B080-512B-96C4-80050E7E08D1"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstanceWithDimensions )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory * This,
        /* [in] */INT32 pixelWidth,
        /* [in] */INT32 pixelHeight,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmap * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_CreateInstanceWithDimensions(This,pixelWidth,pixelHeight,value) \
    ( (This)->lpVtbl->CreateInstanceWithDimensions(This,pixelWidth,pixelHeight,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIWriteableBitmapFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTask
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IXamlRenderingBackgroundTask[] = L"Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTask";
/* [object, uuid("7807000C-A050-5121-AC74-3322D5358E39"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IXamlRenderingBackgroundTaskFactory[] = L"Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskFactory";
/* [object, uuid("205247A3-9FFE-599A-A21A-7181442A9D75"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTask * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskOverrides
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Imaging_IXamlRenderingBackgroundTaskOverrides[] = L"Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskOverrides";
/* [object, uuid("18733237-324B-57C0-89B2-5875472ACC80"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverridesVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *OnRun )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides * This,
        /* [in] */__x_ABI_CWindows_CApplicationModel_CBackground_CIBackgroundTaskInstance * taskInstance
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverridesVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverridesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_OnRun(This,taskInstance) \
    ( (This)->lpVtbl->OnRun(This,taskInstance) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CImaging_CIXamlRenderingBackgroundTaskOverrides_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.BitmapImage
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *   Type can be activated via the Microsoft.UI.Xaml.Media.Imaging.IBitmapImageFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.IBitmapImageStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IBitmapImage ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapImage_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapImage_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_BitmapImage[] = L"Microsoft.UI.Xaml.Media.Imaging.BitmapImage";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.BitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.IBitmapSourceStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IBitmapSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_BitmapSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_BitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.BitmapSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IDownloadProgressEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_DownloadProgressEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_DownloadProgressEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_DownloadProgressEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.DownloadProgressEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmapStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IRenderTargetBitmap ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_RenderTargetBitmap_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_RenderTargetBitmap_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_RenderTargetBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.RenderTargetBitmap";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SoftwareBitmapSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISoftwareBitmapSource ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SoftwareBitmapSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SoftwareBitmapSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SoftwareBitmapSource[] = L"Microsoft.UI.Xaml.Media.Imaging.SoftwareBitmapSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISurfaceImageSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SurfaceImageSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SurfaceImageSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.SurfaceImageSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SvgImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISvgImageSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SvgImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.SvgImageSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceFailedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceFailedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceFailedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceFailedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceFailedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.ISvgImageSourceOpenedEventArgs ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceOpenedEventArgs_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceOpenedEventArgs_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_SvgImageSourceOpenedEventArgs[] = L"Microsoft.UI.Xaml.Media.Imaging.SvgImageSourceOpenedEventArgs";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSourceFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IVirtualSurfaceImageSource ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource[] = L"Microsoft.UI.Xaml.Media.Imaging.VirtualSurfaceImageSource";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via the Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmapFactory interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IWriteableBitmap ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_WriteableBitmap_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_WriteableBitmap_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_WriteableBitmap[] = L"Microsoft.UI.Xaml.Media.Imaging.WriteableBitmap";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTask ** Default Interface **
 *    Microsoft.UI.Xaml.Media.Imaging.IXamlRenderingBackgroundTaskOverrides
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_XamlRenderingBackgroundTask_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Imaging_XamlRenderingBackgroundTask_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Imaging_XamlRenderingBackgroundTask[] = L"Microsoft.UI.Xaml.Media.Imaging.XamlRenderingBackgroundTask";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000






#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EMedia2EImaging_p_h__

#endif // __Microsoft2EUI2EXaml2EMedia2EImaging_h__
