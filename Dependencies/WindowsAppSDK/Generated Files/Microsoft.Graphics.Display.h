/* Header file automatically generated from Microsoft.Graphics.Display.idl */
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
#ifndef __Microsoft2EGraphics2EDisplay_h__
#define __Microsoft2EGraphics2EDisplay_h__
#ifndef __Microsoft2EGraphics2EDisplay_p_h__
#define __Microsoft2EGraphics2EDisplay_p_h__


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
#include "Windows.Storage.Streams.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                interface IDisplayAdvancedColorInfo;
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo ABI::Microsoft::Graphics::Display::IDisplayAdvancedColorInfo

#endif // ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                interface IDisplayInformation;
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation ABI::Microsoft::Graphics::Display::IDisplayInformation

#endif // ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                interface IDisplayInformationStatics;
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics ABI::Microsoft::Graphics::Display::IDisplayInformationStatics

#endif // ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                class DisplayInformation;
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

#ifndef DEF___FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_USE
#define DEF___FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("a8d63481-b7e5-551a-a5ea-8632c543dcc9"))
ITypedEventHandler<ABI::Microsoft::Graphics::Display::DisplayInformation*,IInspectable*> : ITypedEventHandler_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::Graphics::Display::DisplayInformation*, ABI::Microsoft::Graphics::Display::IDisplayInformation*>,IInspectable*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.TypedEventHandler`2<Microsoft.Graphics.Display.DisplayInformation, Object>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef ITypedEventHandler<ABI::Microsoft::Graphics::Display::DisplayInformation*,IInspectable*> __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_t;
#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable ABI::Windows::Foundation::__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::Graphics::Display::IDisplayInformation*,IInspectable*>
//#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_t ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::Graphics::Display::IDisplayInformation*,IInspectable*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

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

#ifndef DEF___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#define DEF___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("398c4183-793d-5b00-819b-4aef92485e94"))
IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IRandomAccessStream*> : IAsyncOperationCompletedHandler_impl<ABI::Windows::Storage::Streams::IRandomAccessStream*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Storage.Streams.IRandomAccessStream>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IRandomAccessStream*> __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_t;
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IRandomAccessStream*>
//#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_t ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Storage::Streams::IRandomAccessStream*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000

#ifndef DEF___FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#define DEF___FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation {
template <>
struct __declspec(uuid("430ecece-1418-5d19-81b2-5ddb381603cc"))
IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*> : IAsyncOperation_impl<ABI::Windows::Storage::Streams::IRandomAccessStream*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.Streams.IRandomAccessStream>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*> __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_t;
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::__FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_t
/* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*>
//#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_t ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_USE */


#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000






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
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                
                typedef enum DisplayAdvancedColorKind : int DisplayAdvancedColorKind;
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                
                typedef enum DisplayHdrMetadataFormat : int DisplayHdrMetadataFormat;
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                class DisplayAdvancedColorInfo;
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */











/*
 *
 * Struct Microsoft.Graphics.Display.DisplayAdvancedColorKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                /* [v1_enum, contract] */
                enum DisplayAdvancedColorKind : int
                {
                    DisplayAdvancedColorKind_StandardDynamicRange = 0,
                    DisplayAdvancedColorKind_WideColorGamut = 1,
                    DisplayAdvancedColorKind_HighDynamicRange = 2,
                };
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Struct Microsoft.Graphics.Display.DisplayHdrMetadataFormat
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                /* [v1_enum, contract] */
                enum DisplayHdrMetadataFormat : int
                {
                    DisplayHdrMetadataFormat_Hdr10 = 0,
                    DisplayHdrMetadataFormat_Hdr10Plus = 1,
                };
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.Graphics.Display.IDisplayAdvancedColorInfo
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.Graphics.Display.DisplayAdvancedColorInfo
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_Graphics_Display_IDisplayAdvancedColorInfo[] = L"Microsoft.Graphics.Display.IDisplayAdvancedColorInfo";
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                /* [object, uuid("B44F0F47-7065-5175-BA3E-714489C85A3E"), exclusiveto, contract] */
                MIDL_INTERFACE("B44F0F47-7065-5175-BA3E-714489C85A3E")
                IDisplayAdvancedColorInfo : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CurrentAdvancedColorKind(
                        /* [retval, out] */ABI::Microsoft::Graphics::Display::DisplayAdvancedColorKind * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RedPrimary(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_GreenPrimary(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BluePrimary(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_WhitePoint(
                        /* [retval, out] */ABI::Windows::Foundation::Point * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MaxLuminanceInNits(
                        /* [retval, out] */DOUBLE * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MinLuminanceInNits(
                        /* [retval, out] */DOUBLE * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MaxAverageFullFrameLuminanceInNits(
                        /* [retval, out] */DOUBLE * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SdrWhiteLevelInNits(
                        /* [retval, out] */DOUBLE * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE IsHdrMetadataFormatCurrentlySupported(
                        /* [in] */ABI::Microsoft::Graphics::Display::DisplayHdrMetadataFormat format,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE IsAdvancedColorKindAvailable(
                        /* [in] */ABI::Microsoft::Graphics::Display::DisplayAdvancedColorKind kind,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayAdvancedColorInfo=_uuidof(IDisplayAdvancedColorInfo);
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.Graphics.Display.IDisplayInformation
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.Graphics.Display.DisplayInformation
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_Graphics_Display_IDisplayInformation[] = L"Microsoft.Graphics.Display.IDisplayInformation";
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                /* [object, uuid("F0D58D4F-84CE-5B27-B222-4F8F7DC0AAEB"), exclusiveto, contract] */
                MIDL_INTERFACE("F0D58D4F-84CE-5B27-B222-4F8F7DC0AAEB")
                IDisplayInformation : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DispatcherQueue(
                        /* [retval, out] */ABI::Microsoft::UI::Dispatching::IDispatcherQueue * * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsStereoEnabled(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_IsStereoEnabledChanged(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_IsStereoEnabledChanged(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetColorProfileAsync(
                        /* [retval, out] */__FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * * operation
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetColorProfile(
                        /* [retval, out] */ABI::Windows::Storage::Streams::IRandomAccessStream * * result
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_ColorProfileChanged(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_ColorProfileChanged(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetAdvancedColorInfo(
                        /* [retval, out] */ABI::Microsoft::Graphics::Display::IDisplayAdvancedColorInfo * * result
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_AdvancedColorInfoChanged(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_AdvancedColorInfoChanged(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    /* [eventadd] */virtual HRESULT STDMETHODCALLTYPE add_Destroyed(
                        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
                        /* [retval, out] */EventRegistrationToken * token
                        ) = 0;
                    /* [eventremove] */virtual HRESULT STDMETHODCALLTYPE remove_Destroyed(
                        /* [in] */EventRegistrationToken token
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayInformation=_uuidof(IDisplayInformation);
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation;
#endif /* !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.Graphics.Display.IDisplayInformationStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.Graphics.Display.DisplayInformation
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_Graphics_Display_IDisplayInformationStatics[] = L"Microsoft.Graphics.Display.IDisplayInformationStatics";
namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace Display {
                /* [object, uuid("2DE85048-37FA-56C0-AC30-47E2044D7EA8"), exclusiveto, contract] */
                MIDL_INTERFACE("2DE85048-37FA-56C0-AC30-47E2044D7EA8")
                IDisplayInformationStatics : public IInspectable
                {
                public:
                    virtual HRESULT STDMETHODCALLTYPE CreateForWindowId(
                        /* [in] */ABI::Microsoft::UI::WindowId windowId,
                        /* [retval, out] */ABI::Microsoft::Graphics::Display::IDisplayInformation * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CreateForDisplayId(
                        /* [in] */ABI::Microsoft::UI::DisplayId displayId,
                        /* [retval, out] */ABI::Microsoft::Graphics::Display::IDisplayInformation * * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IDisplayInformationStatics=_uuidof(IDisplayInformationStatics);
                
            } /* Display */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Class Microsoft.Graphics.Display.DisplayAdvancedColorInfo
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.Graphics.Display.IDisplayAdvancedColorInfo ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

#ifndef RUNTIMECLASS_Microsoft_Graphics_Display_DisplayAdvancedColorInfo_DEFINED
#define RUNTIMECLASS_Microsoft_Graphics_Display_DisplayAdvancedColorInfo_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_Graphics_Display_DisplayAdvancedColorInfo[] = L"Microsoft.Graphics.Display.DisplayAdvancedColorInfo";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Class Microsoft.Graphics.Display.DisplayInformation
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.Graphics.Display.IDisplayInformationStatics interface starting with version 1.2 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.Graphics.Display.IDisplayInformation ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

#ifndef RUNTIMECLASS_Microsoft_Graphics_Display_DisplayInformation_DEFINED
#define RUNTIMECLASS_Microsoft_Graphics_Display_DisplayInformation_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_Graphics_Display_DisplayInformation[] = L"Microsoft.Graphics.Display.DisplayInformation";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002




#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo;

#endif // ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation;

#endif // ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics;

#endif // ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_INTERFACE_DEFINED__)
#define ____FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_INTERFACE_DEFINED__

typedef interface __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable;

typedef struct __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectableVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * This,/* [in] */ __RPC__in_opt __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * sender,/* [in] */ __RPC__in_opt IInspectable * e);
    END_INTERFACE
} __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectableVtbl;

interface __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable
{
    CONST_VTBL struct __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_Invoke(This,sender,e)	\
    ( (This)->lpVtbl -> Invoke(This,sender,e) ) 
#endif /* COBJMACROS */



#endif // ____FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream;

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__)
#define ____FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

typedef interface __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream;

// Forward declare the async operation.
typedef interface __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream;

typedef struct __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);

    HRESULT ( STDMETHODCALLTYPE *Invoke )(__RPC__in __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,/* [in] */ __RPC__in_opt __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *asyncInfo, /* [in] */ AsyncStatus status);
    END_INTERFACE
} __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl;

interface __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream
{
    CONST_VTBL struct __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_Invoke(This,asyncInfo,status)	\
    ( (This)->lpVtbl -> Invoke(This,asyncInfo,status) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000


#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000
#if !defined(____FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__)
#define ____FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

typedef interface __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream;

typedef struct __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Completed )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [in] */ __RPC__in_opt __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream *handler);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Completed )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__deref_out_opt __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream **handler);
    HRESULT ( STDMETHODCALLTYPE *GetResults )(__RPC__in __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * This, /* [retval][out] */ __RPC__out __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * *results);
    END_INTERFACE
} __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl;

interface __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream
{
    CONST_VTBL struct __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_put_Completed(This,handler)	\
    ( (This)->lpVtbl -> put_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_get_Completed(This,handler)	\
    ( (This)->lpVtbl -> get_Completed(This,handler) ) 
#define __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_GetResults(This,results)	\
    ( (This)->lpVtbl -> GetResults(This,results) ) 
#endif /* COBJMACROS */


#endif // ____FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream_INTERFACE_DEFINED__

#endif // WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >= 0x10000





#ifndef ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIClosable __x_ABI_CWindows_CFoundation_CIClosable;

#endif // ____x_ABI_CWindows_CFoundation_CIClosable_FWD_DEFINED__



typedef struct __x_ABI_CWindows_CFoundation_CPoint __x_ABI_CWindows_CFoundation_CPoint;








typedef enum __x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayAdvancedColorKind __x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayAdvancedColorKind;


typedef enum __x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayHdrMetadataFormat __x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayHdrMetadataFormat;














/*
 *
 * Struct Microsoft.Graphics.Display.DisplayAdvancedColorKind
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayAdvancedColorKind
{
    DisplayAdvancedColorKind_StandardDynamicRange = 0,
    DisplayAdvancedColorKind_WideColorGamut = 1,
    DisplayAdvancedColorKind_HighDynamicRange = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Struct Microsoft.Graphics.Display.DisplayHdrMetadataFormat
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayHdrMetadataFormat
{
    DisplayHdrMetadataFormat_Hdr10 = 0,
    DisplayHdrMetadataFormat_Hdr10Plus = 1,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.Graphics.Display.IDisplayAdvancedColorInfo
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.Graphics.Display.DisplayAdvancedColorInfo
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_Graphics_Display_IDisplayAdvancedColorInfo[] = L"Microsoft.Graphics.Display.IDisplayAdvancedColorInfo";
/* [object, uuid("B44F0F47-7065-5175-BA3E-714489C85A3E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CurrentAdvancedColorKind )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayAdvancedColorKind * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RedPrimary )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_GreenPrimary )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BluePrimary )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_WhitePoint )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CPoint * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MaxLuminanceInNits )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MinLuminanceInNits )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MaxAverageFullFrameLuminanceInNits )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SdrWhiteLevelInNits )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [retval, out] */DOUBLE * value
        );
    HRESULT ( STDMETHODCALLTYPE *IsHdrMetadataFormatCurrentlySupported )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [in] */__x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayHdrMetadataFormat format,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *IsAdvancedColorKindAvailable )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * This,
        /* [in] */__x_ABI_CMicrosoft_CGraphics_CDisplay_CDisplayAdvancedColorKind kind,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfoVtbl;

interface __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_CurrentAdvancedColorKind(This,value) \
    ( (This)->lpVtbl->get_CurrentAdvancedColorKind(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_RedPrimary(This,value) \
    ( (This)->lpVtbl->get_RedPrimary(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_GreenPrimary(This,value) \
    ( (This)->lpVtbl->get_GreenPrimary(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_BluePrimary(This,value) \
    ( (This)->lpVtbl->get_BluePrimary(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_WhitePoint(This,value) \
    ( (This)->lpVtbl->get_WhitePoint(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_MaxLuminanceInNits(This,value) \
    ( (This)->lpVtbl->get_MaxLuminanceInNits(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_MinLuminanceInNits(This,value) \
    ( (This)->lpVtbl->get_MinLuminanceInNits(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_MaxAverageFullFrameLuminanceInNits(This,value) \
    ( (This)->lpVtbl->get_MaxAverageFullFrameLuminanceInNits(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_get_SdrWhiteLevelInNits(This,value) \
    ( (This)->lpVtbl->get_SdrWhiteLevelInNits(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_IsHdrMetadataFormatCurrentlySupported(This,format,result) \
    ( (This)->lpVtbl->IsHdrMetadataFormatCurrentlySupported(This,format,result) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_IsAdvancedColorKindAvailable(This,kind,result) \
    ( (This)->lpVtbl->IsAdvancedColorKindAvailable(This,kind,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo;
#endif /* !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.Graphics.Display.IDisplayInformation
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.Graphics.Display.DisplayInformation
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_Graphics_Display_IDisplayInformation[] = L"Microsoft.Graphics.Display.IDisplayInformation";
/* [object, uuid("F0D58D4F-84CE-5B27-B222-4F8F7DC0AAEB"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DispatcherQueue )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CDispatching_CIDispatcherQueue * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsStereoEnabled )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [retval, out] */boolean * value
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_IsStereoEnabledChanged )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_IsStereoEnabledChanged )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *GetColorProfileAsync )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [retval, out] */__FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream * * operation
        );
    HRESULT ( STDMETHODCALLTYPE *GetColorProfile )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [retval, out] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * * result
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_ColorProfileChanged )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_ColorProfileChanged )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */EventRegistrationToken token
        );
    HRESULT ( STDMETHODCALLTYPE *GetAdvancedColorInfo )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayAdvancedColorInfo * * result
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_AdvancedColorInfoChanged )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_AdvancedColorInfoChanged )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */EventRegistrationToken token
        );
    /* [eventadd] */HRESULT ( STDMETHODCALLTYPE *add_Destroyed )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */__FITypedEventHandler_2_Microsoft__CGraphics__CDisplay__CDisplayInformation_IInspectable * handler,
        /* [retval, out] */EventRegistrationToken * token
        );
    /* [eventremove] */HRESULT ( STDMETHODCALLTYPE *remove_Destroyed )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * This,
        /* [in] */EventRegistrationToken token
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationVtbl;

interface __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_get_DispatcherQueue(This,value) \
    ( (This)->lpVtbl->get_DispatcherQueue(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_get_IsStereoEnabled(This,value) \
    ( (This)->lpVtbl->get_IsStereoEnabled(This,value) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_add_IsStereoEnabledChanged(This,handler,token) \
    ( (This)->lpVtbl->add_IsStereoEnabledChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_remove_IsStereoEnabledChanged(This,token) \
    ( (This)->lpVtbl->remove_IsStereoEnabledChanged(This,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_GetColorProfileAsync(This,operation) \
    ( (This)->lpVtbl->GetColorProfileAsync(This,operation) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_GetColorProfile(This,result) \
    ( (This)->lpVtbl->GetColorProfile(This,result) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_add_ColorProfileChanged(This,handler,token) \
    ( (This)->lpVtbl->add_ColorProfileChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_remove_ColorProfileChanged(This,token) \
    ( (This)->lpVtbl->remove_ColorProfileChanged(This,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_GetAdvancedColorInfo(This,result) \
    ( (This)->lpVtbl->GetAdvancedColorInfo(This,result) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_add_AdvancedColorInfoChanged(This,handler,token) \
    ( (This)->lpVtbl->add_AdvancedColorInfoChanged(This,handler,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_remove_AdvancedColorInfoChanged(This,token) \
    ( (This)->lpVtbl->remove_AdvancedColorInfoChanged(This,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_add_Destroyed(This,handler,token) \
    ( (This)->lpVtbl->add_Destroyed(This,handler,token) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_remove_Destroyed(This,token) \
    ( (This)->lpVtbl->remove_Destroyed(This,token) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation;
#endif /* !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Interface Microsoft.Graphics.Display.IDisplayInformationStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Interface is a part of the implementation of type Microsoft.Graphics.Display.DisplayInformation
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002
#if !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_Graphics_Display_IDisplayInformationStatics[] = L"Microsoft.Graphics.Display.IDisplayInformationStatics";
/* [object, uuid("2DE85048-37FA-56C0-AC30-47E2044D7EA8"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateForWindowId )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CWindowId windowId,
        /* [retval, out] */__x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * * result
        );
    HRESULT ( STDMETHODCALLTYPE *CreateForDisplayId )(
        __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CDisplayId displayId,
        /* [retval, out] */__x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformation * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStaticsVtbl;

interface __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_CreateForWindowId(This,windowId,result) \
    ( (This)->lpVtbl->CreateForWindowId(This,windowId,result) )

#define __x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_CreateForDisplayId(This,displayId,result) \
    ( (This)->lpVtbl->CreateForDisplayId(This,displayId,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CGraphics_CDisplay_CIDisplayInformationStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Class Microsoft.Graphics.Display.DisplayAdvancedColorInfo
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.Graphics.Display.IDisplayAdvancedColorInfo ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

#ifndef RUNTIMECLASS_Microsoft_Graphics_Display_DisplayAdvancedColorInfo_DEFINED
#define RUNTIMECLASS_Microsoft_Graphics_Display_DisplayAdvancedColorInfo_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_Graphics_Display_DisplayAdvancedColorInfo[] = L"Microsoft.Graphics.Display.DisplayAdvancedColorInfo";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002


/*
 *
 * Class Microsoft.Graphics.Display.DisplayInformation
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.2
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.Graphics.Display.IDisplayInformationStatics interface starting with version 1.2 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.Graphics.Display.IDisplayInformation ** Default Interface **
 *    Windows.Foundation.IClosable
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002

#ifndef RUNTIMECLASS_Microsoft_Graphics_Display_DisplayInformation_DEFINED
#define RUNTIMECLASS_Microsoft_Graphics_Display_DisplayInformation_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_Graphics_Display_DisplayInformation[] = L"Microsoft.Graphics.Display.DisplayInformation";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10002




#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EGraphics2EDisplay_p_h__

#endif // __Microsoft2EGraphics2EDisplay_h__
