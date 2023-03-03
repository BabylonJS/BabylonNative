/* Header file automatically generated from Microsoft.UI.Xaml.Shapes.idl */
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
#ifndef __Microsoft2EUI2EXaml2EShapes_h__
#define __Microsoft2EUI2EXaml2EShapes_h__
#ifndef __Microsoft2EUI2EXaml2EShapes_p_h__
#define __Microsoft2EUI2EXaml2EShapes_p_h__


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
#include "Microsoft.UI.Composition.h"
#include "Microsoft.UI.Xaml.h"
#include "Microsoft.UI.Xaml.Media.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IEllipse;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse ABI::Microsoft::UI::Xaml::Shapes::IEllipse

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface ILine;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine ABI::Microsoft::UI::Xaml::Shapes::ILine

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface ILineStatics;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics ABI::Microsoft::UI::Xaml::Shapes::ILineStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPath;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath ABI::Microsoft::UI::Xaml::Shapes::IPath

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPathFactory;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory ABI::Microsoft::UI::Xaml::Shapes::IPathFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPathStatics;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics ABI::Microsoft::UI::Xaml::Shapes::IPathStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPolygon;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon ABI::Microsoft::UI::Xaml::Shapes::IPolygon

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPolygonStatics;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics ABI::Microsoft::UI::Xaml::Shapes::IPolygonStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPolyline;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline ABI::Microsoft::UI::Xaml::Shapes::IPolyline

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IPolylineStatics;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics ABI::Microsoft::UI::Xaml::Shapes::IPolylineStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IRectangle;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle ABI::Microsoft::UI::Xaml::Shapes::IRectangle

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IRectangleStatics;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics ABI::Microsoft::UI::Xaml::Shapes::IRectangleStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IShape;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape ABI::Microsoft::UI::Xaml::Shapes::IShape

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IShapeFactory;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory ABI::Microsoft::UI::Xaml::Shapes::IShapeFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    interface IShapeStatics;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics ABI::Microsoft::UI::Xaml::Shapes::IShapeStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Windows {
        namespace Foundation {
            struct Point;
            
        } /* Foundation */
    } /* Windows */
} /* ABI */


#ifndef DEF___FIIterator_1_Windows__CFoundation__CPoint_USE
#define DEF___FIIterator_1_Windows__CFoundation__CPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c602b59e-0a8e-5e99-b478-2b564585278d"))
IIterator<struct ABI::Windows::Foundation::Point> : IIterator_impl<struct ABI::Windows::Foundation::Point> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Windows.Foundation.Point>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<struct ABI::Windows::Foundation::Point> __FIIterator_1_Windows__CFoundation__CPoint_t;
#define __FIIterator_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::__FIIterator_1_Windows__CFoundation__CPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Point>
//#define __FIIterator_1_Windows__CFoundation__CPoint_t ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Point>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Windows__CFoundation__CPoint_USE */





#ifndef DEF___FIIterable_1_Windows__CFoundation__CPoint_USE
#define DEF___FIIterable_1_Windows__CFoundation__CPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c192280d-3a09-5423-9dc5-67b83ebde41d"))
IIterable<struct ABI::Windows::Foundation::Point> : IIterable_impl<struct ABI::Windows::Foundation::Point> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Windows.Foundation.Point>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<struct ABI::Windows::Foundation::Point> __FIIterable_1_Windows__CFoundation__CPoint_t;
#define __FIIterable_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::__FIIterable_1_Windows__CFoundation__CPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Point>
//#define __FIIterable_1_Windows__CFoundation__CPoint_t ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Point>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Windows__CFoundation__CPoint_USE */





#ifndef DEF___FIVectorView_1_Windows__CFoundation__CPoint_USE
#define DEF___FIVectorView_1_Windows__CFoundation__CPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("0b7b4c9d-182f-582a-bddb-42b1aac30cad"))
IVectorView<struct ABI::Windows::Foundation::Point> : IVectorView_impl<struct ABI::Windows::Foundation::Point> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Windows.Foundation.Point>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<struct ABI::Windows::Foundation::Point> __FIVectorView_1_Windows__CFoundation__CPoint_t;
#define __FIVectorView_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::__FIVectorView_1_Windows__CFoundation__CPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Foundation::Point>
//#define __FIVectorView_1_Windows__CFoundation__CPoint_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Foundation::Point>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Windows__CFoundation__CPoint_USE */





#ifndef DEF___FIVector_1_Windows__CFoundation__CPoint_USE
#define DEF___FIVector_1_Windows__CFoundation__CPoint_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c0d513a9-ec4a-5a5d-b6d5-b707defdb9f7"))
IVector<struct ABI::Windows::Foundation::Point> : IVector_impl<struct ABI::Windows::Foundation::Point> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Windows.Foundation.Point>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<struct ABI::Windows::Foundation::Point> __FIVector_1_Windows__CFoundation__CPoint_t;
#define __FIVector_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CFoundation__CPoint_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_Windows__CFoundation__CPoint ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Foundation::Point>
//#define __FIVector_1_Windows__CFoundation__CPoint_t ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Foundation::Point>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_Windows__CFoundation__CPoint_USE */




#ifndef DEF___FIIterator_1_double_USE
#define DEF___FIIterator_1_double_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("638a2cf4-f474-5318-9055-141cb909ac4b"))
IIterator<double> : IIterator_impl<double> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Double>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<double> __FIIterator_1_double_t;
#define __FIIterator_1_double ABI::Windows::Foundation::Collections::__FIIterator_1_double_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_double ABI::Windows::Foundation::Collections::IIterator<DOUBLE>
//#define __FIIterator_1_double_t ABI::Windows::Foundation::Collections::IIterator<DOUBLE>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_double_USE */




#ifndef DEF___FIIterable_1_double_USE
#define DEF___FIIterable_1_double_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c738964e-9c64-5bce-b5ce-61e9a282ec4a"))
IIterable<double> : IIterable_impl<double> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Double>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<double> __FIIterable_1_double_t;
#define __FIIterable_1_double ABI::Windows::Foundation::Collections::__FIIterable_1_double_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_double ABI::Windows::Foundation::Collections::IIterable<DOUBLE>
//#define __FIIterable_1_double_t ABI::Windows::Foundation::Collections::IIterable<DOUBLE>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_double_USE */




#ifndef DEF___FIVectorView_1_double_USE
#define DEF___FIVectorView_1_double_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("af7586a8-6b21-5f61-bff1-1b682293ad96"))
IVectorView<double> : IVectorView_impl<double> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Double>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<double> __FIVectorView_1_double_t;
#define __FIVectorView_1_double ABI::Windows::Foundation::Collections::__FIVectorView_1_double_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_double ABI::Windows::Foundation::Collections::IVectorView<DOUBLE>
//#define __FIVectorView_1_double_t ABI::Windows::Foundation::Collections::IVectorView<DOUBLE>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_double_USE */




#ifndef DEF___FIVector_1_double_USE
#define DEF___FIVector_1_double_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("f452d23c-bf05-5f3e-88e7-d17a6716b911"))
IVector<double> : IVector_impl<double> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Double>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<double> __FIVector_1_double_t;
#define __FIVector_1_double ABI::Windows::Foundation::Collections::__FIVector_1_double_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_double ABI::Windows::Foundation::Collections::IVector<DOUBLE>
//#define __FIVector_1_double_t ABI::Windows::Foundation::Collections::IVector<DOUBLE>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_double_USE */





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                class CompositionBrush;
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                interface ICompositionBrush;
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush ABI::Microsoft::UI::Composition::ICompositionBrush

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush_FWD_DEFINED__





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
                    class Brush;
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    interface IBrush;
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush ABI::Microsoft::UI::Xaml::Media::IBrush

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush_FWD_DEFINED__


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    class DoubleCollection;
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
                    
                    typedef enum FillRule : int FillRule;
                    
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
                    class Geometry;
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    interface IGeometry;
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry ABI::Microsoft::UI::Xaml::Media::IGeometry

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry_FWD_DEFINED__


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    
                    typedef enum PenLineCap : int PenLineCap;
                    
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
                    
                    typedef enum PenLineJoin : int PenLineJoin;
                    
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
                    class PointCollection;
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
                    
                    typedef enum Stretch : int Stretch;
                    
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
                    class Transform;
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    interface ITransform;
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform ABI::Microsoft::UI::Xaml::Media::ITransform

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform_FWD_DEFINED__

























namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Ellipse;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Line;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Path;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Polygon;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Polyline;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Rectangle;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    class Shape;
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */







/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IEllipse
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Ellipse
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IEllipse[] = L"Microsoft.UI.Xaml.Shapes.IEllipse";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("805C39AA-FA8A-5E0B-9847-4AB81B42A3DF"), exclusiveto, contract] */
                    MIDL_INTERFACE("805C39AA-FA8A-5E0B-9847-4AB81B42A3DF")
                    IEllipse : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IEllipse=_uuidof(IEllipse);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.ILine
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Line
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_ILine[] = L"Microsoft.UI.Xaml.Shapes.ILine";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("507B3858-AF7E-559B-A87E-4CC6A5D8BA96"), exclusiveto, contract] */
                    MIDL_INTERFACE("507B3858-AF7E-559B-A87E-4CC6A5D8BA96")
                    ILine : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_X1(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_X1(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Y1(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Y1(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_X2(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_X2(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Y2(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Y2(
                            /* [in] */DOUBLE value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ILine=_uuidof(ILine);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.ILineStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Line
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_ILineStatics[] = L"Microsoft.UI.Xaml.Shapes.ILineStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("A425BF93-F1F3-5DCB-997E-B6A26F7AE8C0"), exclusiveto, contract] */
                    MIDL_INTERFACE("A425BF93-F1F3-5DCB-997E-B6A26F7AE8C0")
                    ILineStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_X1Property(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Y1Property(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_X2Property(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Y2Property(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ILineStatics=_uuidof(ILineStatics);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPath
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Path
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPath[] = L"Microsoft.UI.Xaml.Shapes.IPath";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("757D1CD8-0EC0-55C5-B400-66657E493E78"), exclusiveto, contract] */
                    MIDL_INTERFACE("757D1CD8-0EC0-55C5-B400-66657E493E78")
                    IPath : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Data(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::IGeometry * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Data(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::IGeometry * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPath=_uuidof(IPath);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPathFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Path
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPathFactory[] = L"Microsoft.UI.Xaml.Shapes.IPathFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("5E82E4C9-7502-5B1F-B940-C3346A71362A"), exclusiveto, contract] */
                    MIDL_INTERFACE("5E82E4C9-7502-5B1F-B940-C3346A71362A")
                    IPathFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Shapes::IPath * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPathFactory=_uuidof(IPathFactory);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPathStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Path
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPathStatics[] = L"Microsoft.UI.Xaml.Shapes.IPathStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("2146D36F-721C-5B54-AF7D-60F3ADC4FBCA"), exclusiveto, contract] */
                    MIDL_INTERFACE("2146D36F-721C-5B54-AF7D-60F3ADC4FBCA")
                    IPathStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DataProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPathStatics=_uuidof(IPathStatics);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolygon
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polygon
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolygon[] = L"Microsoft.UI.Xaml.Shapes.IPolygon";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("FA126347-D1D4-54DD-B1A4-C35019397944"), exclusiveto, contract] */
                    MIDL_INTERFACE("FA126347-D1D4-54DD-B1A4-C35019397944")
                    IPolygon : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FillRule(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::FillRule * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FillRule(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::FillRule value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Points(
                            /* [retval, out] */__FIVector_1_Windows__CFoundation__CPoint * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Points(
                            /* [in] */__FIVector_1_Windows__CFoundation__CPoint * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPolygon=_uuidof(IPolygon);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolygonStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polygon
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolygonStatics[] = L"Microsoft.UI.Xaml.Shapes.IPolygonStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("85DDBADA-9E37-5971-A9AA-DCE31F9CF67A"), exclusiveto, contract] */
                    MIDL_INTERFACE("85DDBADA-9E37-5971-A9AA-DCE31F9CF67A")
                    IPolygonStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FillRuleProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointsProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPolygonStatics=_uuidof(IPolygonStatics);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolyline
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polyline
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolyline[] = L"Microsoft.UI.Xaml.Shapes.IPolyline";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("C7F0BEC6-184C-5D96-8102-04DD211E100C"), exclusiveto, contract] */
                    MIDL_INTERFACE("C7F0BEC6-184C-5D96-8102-04DD211E100C")
                    IPolyline : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FillRule(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::FillRule * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FillRule(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::FillRule value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Points(
                            /* [retval, out] */__FIVector_1_Windows__CFoundation__CPoint * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Points(
                            /* [in] */__FIVector_1_Windows__CFoundation__CPoint * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPolyline=_uuidof(IPolyline);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolylineStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polyline
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolylineStatics[] = L"Microsoft.UI.Xaml.Shapes.IPolylineStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("48840FE7-D735-5080-9C6D-2862665CDDA0"), exclusiveto, contract] */
                    MIDL_INTERFACE("48840FE7-D735-5080-9C6D-2862665CDDA0")
                    IPolylineStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FillRuleProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PointsProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IPolylineStatics=_uuidof(IPolylineStatics);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IRectangle
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Rectangle
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IRectangle[] = L"Microsoft.UI.Xaml.Shapes.IRectangle";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("BF7D30B9-152C-556E-9F10-D0B7EBA4E52F"), exclusiveto, contract] */
                    MIDL_INTERFACE("BF7D30B9-152C-556E-9F10-D0B7EBA4E52F")
                    IRectangle : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RadiusX(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RadiusX(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RadiusY(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RadiusY(
                            /* [in] */DOUBLE value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IRectangle=_uuidof(IRectangle);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IRectangleStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Rectangle
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IRectangleStatics[] = L"Microsoft.UI.Xaml.Shapes.IRectangleStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("3CC3CC79-C332-5AD0-8743-1F1B1E670A86"), exclusiveto, contract] */
                    MIDL_INTERFACE("3CC3CC79-C332-5AD0-8743-1F1B1E670A86")
                    IRectangleStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RadiusXProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RadiusYProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IRectangleStatics=_uuidof(IRectangleStatics);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IShape
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Shape
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IShape[] = L"Microsoft.UI.Xaml.Shapes.IShape";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("9941AAD3-6AF2-5BA2-9085-8506D5F2485E"), exclusiveto, contract] */
                    MIDL_INTERFACE("9941AAD3-6AF2-5BA2-9085-8506D5F2485E")
                    IShape : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Fill(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::IBrush * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Fill(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::IBrush * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Stroke(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::IBrush * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Stroke(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::IBrush * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeMiterLimit(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeMiterLimit(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeThickness(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeThickness(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeStartLineCap(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::PenLineCap * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeStartLineCap(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::PenLineCap value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeEndLineCap(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::PenLineCap * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeEndLineCap(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::PenLineCap value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeLineJoin(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::PenLineJoin * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeLineJoin(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::PenLineJoin value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeDashOffset(
                            /* [retval, out] */DOUBLE * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeDashOffset(
                            /* [in] */DOUBLE value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeDashCap(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::PenLineCap * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeDashCap(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::PenLineCap value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeDashArray(
                            /* [retval, out] */__FIVector_1_double * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StrokeDashArray(
                            /* [in] */__FIVector_1_double * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Stretch(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Stretch * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Stretch(
                            /* [in] */ABI::Microsoft::UI::Xaml::Media::Stretch value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_GeometryTransform(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::ITransform * * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE GetAlphaMask(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::ICompositionBrush * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IShape=_uuidof(IShape);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IShapeFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Shape
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IShapeFactory[] = L"Microsoft.UI.Xaml.Shapes.IShapeFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("4FECAFAF-8265-5252-BA5C-F43639F974A5"), exclusiveto, contract] */
                    MIDL_INTERFACE("4FECAFAF-8265-5252-BA5C-F43639F974A5")
                    IShapeFactory : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                            /* [in] */IInspectable * baseInterface,
                            /* [out] */IInspectable * * innerInterface,
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::Shapes::IShape * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IShapeFactory=_uuidof(IShapeFactory);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IShapeStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Shape
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IShapeStatics[] = L"Microsoft.UI.Xaml.Shapes.IShapeStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Shapes {
                    /* [object, uuid("EA407C43-8A09-587A-958A-4DD17D217CE1"), exclusiveto, contract] */
                    MIDL_INTERFACE("EA407C43-8A09-587A-958A-4DD17D217CE1")
                    IShapeStatics : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FillProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeMiterLimitProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeThicknessProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeStartLineCapProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeEndLineCapProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeLineJoinProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeDashOffsetProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeDashCapProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StrokeDashArrayProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StretchProperty(
                            /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IShapeStatics=_uuidof(IShapeStatics);
                    
                } /* Shapes */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Ellipse
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IEllipse ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Ellipse_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Ellipse_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Ellipse[] = L"Microsoft.UI.Xaml.Shapes.Ellipse";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Line
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.ILineStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.ILine ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Line_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Line_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Line[] = L"Microsoft.UI.Xaml.Shapes.Line";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Path
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IPathStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IPath ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Path_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Path_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Path[] = L"Microsoft.UI.Xaml.Shapes.Path";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Polygon
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IPolygonStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IPolygon ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polygon_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polygon_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Polygon[] = L"Microsoft.UI.Xaml.Shapes.Polygon";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Polyline
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IPolylineStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IPolyline ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polyline_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polyline_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Polyline[] = L"Microsoft.UI.Xaml.Shapes.Polyline";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Rectangle
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IRectangleStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IRectangle ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Rectangle_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Rectangle_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Rectangle[] = L"Microsoft.UI.Xaml.Shapes.Rectangle";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Shape
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IShapeStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IShape ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Shape_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Shape_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Shape[] = L"Microsoft.UI.Xaml.Shapes.Shape";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions
struct __x_ABI_CWindows_CFoundation_CPoint;

#if !defined(____FIIterator_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__)
#define ____FIIterator_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Windows__CFoundation__CPoint __FIIterator_1_Windows__CFoundation__CPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Windows__CFoundation__CPoint;

typedef struct __FIIterator_1_Windows__CFoundation__CPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This, /* [retval][out] */ __RPC__out struct __x_ABI_CWindows_CFoundation_CPoint *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Windows__CFoundation__CPoint * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) struct __x_ABI_CWindows_CFoundation_CPoint *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Windows__CFoundation__CPointVtbl;

interface __FIIterator_1_Windows__CFoundation__CPoint
{
    CONST_VTBL struct __FIIterator_1_Windows__CFoundation__CPointVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Windows__CFoundation__CPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Windows__CFoundation__CPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Windows__CFoundation__CPoint_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Windows__CFoundation__CPoint_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__



#if !defined(____FIIterable_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__)
#define ____FIIterable_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Windows__CFoundation__CPoint __FIIterable_1_Windows__CFoundation__CPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Windows__CFoundation__CPoint;

typedef  struct __FIIterable_1_Windows__CFoundation__CPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Windows__CFoundation__CPoint * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Windows__CFoundation__CPoint **first);

    END_INTERFACE
} __FIIterable_1_Windows__CFoundation__CPointVtbl;

interface __FIIterable_1_Windows__CFoundation__CPoint
{
    CONST_VTBL struct __FIIterable_1_Windows__CFoundation__CPointVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Windows__CFoundation__CPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Windows__CFoundation__CPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Windows__CFoundation__CPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Windows__CFoundation__CPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Windows__CFoundation__CPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Windows__CFoundation__CPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Windows__CFoundation__CPoint_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__



#if !defined(____FIVectorView_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Windows__CFoundation__CPoint __FIVectorView_1_Windows__CFoundation__CPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Windows__CFoundation__CPoint;

typedef struct __FIVectorView_1_Windows__CFoundation__CPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out struct __x_ABI_CWindows_CFoundation_CPoint *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
            /* [in] */ struct __x_ABI_CWindows_CFoundation_CPoint item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Windows__CFoundation__CPoint * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) struct __x_ABI_CWindows_CFoundation_CPoint *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Windows__CFoundation__CPointVtbl;

interface __FIVectorView_1_Windows__CFoundation__CPoint
{
    CONST_VTBL struct __FIVectorView_1_Windows__CFoundation__CPointVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Windows__CFoundation__CPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Windows__CFoundation__CPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Windows__CFoundation__CPoint_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Windows__CFoundation__CPoint_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__



#if !defined(____FIVector_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__)
#define ____FIVector_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__

typedef interface __FIVector_1_Windows__CFoundation__CPoint __FIVector_1_Windows__CFoundation__CPoint;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_Windows__CFoundation__CPoint;

typedef struct __FIVector_1_Windows__CFoundation__CPointVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This, /* [out] */ __RPC__deref_out_opt struct __x_ABI_CWindows_CFoundation_CPoint *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt struct __x_ABI_CWindows_CFoundation_CPoint *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_Windows__CFoundation__CPoint **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [in] */ __RPC__in struct __x_ABI_CWindows_CFoundation_CPoint item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in struct __x_ABI_CWindows_CFoundation_CPoint item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in struct __x_ABI_CWindows_CFoundation_CPoint item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This, /* [in] */ __RPC__in struct __x_ABI_CWindows_CFoundation_CPoint item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) struct __x_ABI_CWindows_CFoundation_CPoint *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_Windows__CFoundation__CPoint * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) struct __x_ABI_CWindows_CFoundation_CPoint *value);

    END_INTERFACE
} __FIVector_1_Windows__CFoundation__CPointVtbl;

interface __FIVector_1_Windows__CFoundation__CPoint
{
    CONST_VTBL struct __FIVector_1_Windows__CFoundation__CPointVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_Windows__CFoundation__CPoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_Windows__CFoundation__CPoint_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_Windows__CFoundation__CPoint_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_Windows__CFoundation__CPoint_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_Windows__CFoundation__CPoint_INTERFACE_DEFINED__


#if !defined(____FIIterator_1_double_INTERFACE_DEFINED__)
#define ____FIIterator_1_double_INTERFACE_DEFINED__

typedef interface __FIIterator_1_double __FIIterator_1_double;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_double;

typedef struct __FIIterator_1_doubleVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_double * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_double * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_double * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_double * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_double * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_double * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_double * This, /* [retval][out] */ __RPC__out double *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_double * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_double * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_double * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) double *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_doubleVtbl;

interface __FIIterator_1_double
{
    CONST_VTBL struct __FIIterator_1_doubleVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_double_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_double_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_double_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_double_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_double_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_double_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_double_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_double_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_double_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_double_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_double_INTERFACE_DEFINED__


#if !defined(____FIIterable_1_double_INTERFACE_DEFINED__)
#define ____FIIterable_1_double_INTERFACE_DEFINED__

typedef interface __FIIterable_1_double __FIIterable_1_double;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_double;

typedef  struct __FIIterable_1_doubleVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_double * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_double * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_double * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_double * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_double * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_double * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_double * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_double **first);

    END_INTERFACE
} __FIIterable_1_doubleVtbl;

interface __FIIterable_1_double
{
    CONST_VTBL struct __FIIterable_1_doubleVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_double_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_double_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_double_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_double_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_double_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_double_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_double_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_double_INTERFACE_DEFINED__


#if !defined(____FIVectorView_1_double_INTERFACE_DEFINED__)
#define ____FIVectorView_1_double_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_double __FIVectorView_1_double;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_double;

typedef struct __FIVectorView_1_doubleVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_double * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_double * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_double * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_double * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_double * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_double * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_double * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out double *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_double * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_double * This,
            /* [in] */ double item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_double * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) double *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_doubleVtbl;

interface __FIVectorView_1_double
{
    CONST_VTBL struct __FIVectorView_1_doubleVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_double_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_double_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_double_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_double_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_double_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_double_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_double_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_double_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_double_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_double_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_double_INTERFACE_DEFINED__


#if !defined(____FIVector_1_double_INTERFACE_DEFINED__)
#define ____FIVector_1_double_INTERFACE_DEFINED__

typedef interface __FIVector_1_double __FIVector_1_double;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_double;

typedef struct __FIVector_1_doubleVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_double * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_double * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_double * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_double * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_double * This, /* [out] */ __RPC__deref_out_opt double *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_double * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_double * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt double *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_double * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_double * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_double **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_double * This,
        /* [in] */ __RPC__in double item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_double * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in double item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_double * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in double item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_double * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_double * This, /* [in] */ __RPC__in double item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_double * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_double * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_double * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) double *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_double * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) double *value);

    END_INTERFACE
} __FIVector_1_doubleVtbl;

interface __FIVector_1_double
{
    CONST_VTBL struct __FIVector_1_doubleVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_double_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_double_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_double_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_double_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_double_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_double_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_double_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_double_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_double_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_double_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_double_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_double_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_double_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_double_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_double_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_double_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_double_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_double_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_double_INTERFACE_DEFINED__



#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush __x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush_FWD_DEFINED__





#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__





#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush_FWD_DEFINED__




typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CFillRule __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CFillRule;

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry_FWD_DEFINED__



typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap;


typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineJoin __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineJoin;



typedef enum __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CStretch __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CStretch;

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform_FWD_DEFINED__





































/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IEllipse
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Ellipse
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IEllipse[] = L"Microsoft.UI.Xaml.Shapes.IEllipse";
/* [object, uuid("805C39AA-FA8A-5E0B-9847-4AB81B42A3DF"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipseVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipseVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipseVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIEllipse_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.ILine
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Line
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_ILine[] = L"Microsoft.UI.Xaml.Shapes.ILine";
/* [object, uuid("507B3858-AF7E-559B-A87E-4CC6A5D8BA96"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_X1 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_X1 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Y1 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Y1 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_X2 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_X2 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Y2 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Y2 )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine * This,
        /* [in] */DOUBLE value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_get_X1(This,value) \
    ( (This)->lpVtbl->get_X1(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_put_X1(This,value) \
    ( (This)->lpVtbl->put_X1(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_get_Y1(This,value) \
    ( (This)->lpVtbl->get_Y1(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_put_Y1(This,value) \
    ( (This)->lpVtbl->put_Y1(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_get_X2(This,value) \
    ( (This)->lpVtbl->get_X2(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_put_X2(This,value) \
    ( (This)->lpVtbl->put_X2(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_get_Y2(This,value) \
    ( (This)->lpVtbl->get_Y2(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_put_Y2(This,value) \
    ( (This)->lpVtbl->put_Y2(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILine_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.ILineStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Line
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_ILineStatics[] = L"Microsoft.UI.Xaml.Shapes.ILineStatics";
/* [object, uuid("A425BF93-F1F3-5DCB-997E-B6A26F7AE8C0"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_X1Property )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Y1Property )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_X2Property )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Y2Property )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_get_X1Property(This,value) \
    ( (This)->lpVtbl->get_X1Property(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_get_Y1Property(This,value) \
    ( (This)->lpVtbl->get_Y1Property(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_get_X2Property(This,value) \
    ( (This)->lpVtbl->get_X2Property(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_get_Y2Property(This,value) \
    ( (This)->lpVtbl->get_Y2Property(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CILineStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPath
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Path
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPath[] = L"Microsoft.UI.Xaml.Shapes.IPath";
/* [object, uuid("757D1CD8-0EC0-55C5-B400-66657E493E78"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Data )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Data )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIGeometry * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_get_Data(This,value) \
    ( (This)->lpVtbl->get_Data(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_put_Data(This,value) \
    ( (This)->lpVtbl->put_Data(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPathFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Path
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPathFactory[] = L"Microsoft.UI.Xaml.Shapes.IPathFactory";
/* [object, uuid("5E82E4C9-7502-5B1F-B940-C3346A71362A"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPath * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPathStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Path
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPathStatics[] = L"Microsoft.UI.Xaml.Shapes.IPathStatics";
/* [object, uuid("2146D36F-721C-5B54-AF7D-60F3ADC4FBCA"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DataProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_get_DataProperty(This,value) \
    ( (This)->lpVtbl->get_DataProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPathStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolygon
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polygon
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolygon[] = L"Microsoft.UI.Xaml.Shapes.IPolygon";
/* [object, uuid("FA126347-D1D4-54DD-B1A4-C35019397944"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FillRule )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CFillRule * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FillRule )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CFillRule value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Points )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
        /* [retval, out] */__FIVector_1_Windows__CFoundation__CPoint * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Points )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon * This,
        /* [in] */__FIVector_1_Windows__CFoundation__CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_get_FillRule(This,value) \
    ( (This)->lpVtbl->get_FillRule(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_put_FillRule(This,value) \
    ( (This)->lpVtbl->put_FillRule(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_get_Points(This,value) \
    ( (This)->lpVtbl->get_Points(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_put_Points(This,value) \
    ( (This)->lpVtbl->put_Points(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygon_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolygonStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polygon
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolygonStatics[] = L"Microsoft.UI.Xaml.Shapes.IPolygonStatics";
/* [object, uuid("85DDBADA-9E37-5971-A9AA-DCE31F9CF67A"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FillRuleProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointsProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_get_FillRuleProperty(This,value) \
    ( (This)->lpVtbl->get_FillRuleProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_get_PointsProperty(This,value) \
    ( (This)->lpVtbl->get_PointsProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolygonStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolyline
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polyline
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolyline[] = L"Microsoft.UI.Xaml.Shapes.IPolyline";
/* [object, uuid("C7F0BEC6-184C-5D96-8102-04DD211E100C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FillRule )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CFillRule * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FillRule )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CFillRule value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Points )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
        /* [retval, out] */__FIVector_1_Windows__CFoundation__CPoint * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Points )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline * This,
        /* [in] */__FIVector_1_Windows__CFoundation__CPoint * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_get_FillRule(This,value) \
    ( (This)->lpVtbl->get_FillRule(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_put_FillRule(This,value) \
    ( (This)->lpVtbl->put_FillRule(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_get_Points(This,value) \
    ( (This)->lpVtbl->get_Points(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_put_Points(This,value) \
    ( (This)->lpVtbl->put_Points(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolyline_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IPolylineStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Polyline
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IPolylineStatics[] = L"Microsoft.UI.Xaml.Shapes.IPolylineStatics";
/* [object, uuid("48840FE7-D735-5080-9C6D-2862665CDDA0"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FillRuleProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PointsProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_get_FillRuleProperty(This,value) \
    ( (This)->lpVtbl->get_FillRuleProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_get_PointsProperty(This,value) \
    ( (This)->lpVtbl->get_PointsProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIPolylineStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IRectangle
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Rectangle
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IRectangle[] = L"Microsoft.UI.Xaml.Shapes.IRectangle";
/* [object, uuid("BF7D30B9-152C-556E-9F10-D0B7EBA4E52F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RadiusX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RadiusX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RadiusY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RadiusY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle * This,
        /* [in] */DOUBLE value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_get_RadiusX(This,value) \
    ( (This)->lpVtbl->get_RadiusX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_put_RadiusX(This,value) \
    ( (This)->lpVtbl->put_RadiusX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_get_RadiusY(This,value) \
    ( (This)->lpVtbl->get_RadiusY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_put_RadiusY(This,value) \
    ( (This)->lpVtbl->put_RadiusY(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangle_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IRectangleStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Rectangle
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IRectangleStatics[] = L"Microsoft.UI.Xaml.Shapes.IRectangleStatics";
/* [object, uuid("3CC3CC79-C332-5AD0-8743-1F1B1E670A86"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RadiusXProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RadiusYProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_get_RadiusXProperty(This,value) \
    ( (This)->lpVtbl->get_RadiusXProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_get_RadiusYProperty(This,value) \
    ( (This)->lpVtbl->get_RadiusYProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIRectangleStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IShape
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Shape
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IShape[] = L"Microsoft.UI.Xaml.Shapes.IShape";
/* [object, uuid("9941AAD3-6AF2-5BA2-9085-8506D5F2485E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Fill )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Fill )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Stroke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Stroke )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CIBrush * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeMiterLimit )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeMiterLimit )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeThickness )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeThickness )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeStartLineCap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeStartLineCap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeEndLineCap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeEndLineCap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeLineJoin )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineJoin * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeLineJoin )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineJoin value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeDashOffset )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeDashOffset )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeDashCap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeDashCap )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CPenLineCap value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeDashArray )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__FIVector_1_double * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StrokeDashArray )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__FIVector_1_double * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Stretch )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CStretch * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Stretch )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CStretch value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_GeometryTransform )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CITransform * * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetAlphaMask )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositionBrush * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_Fill(This,value) \
    ( (This)->lpVtbl->get_Fill(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_Fill(This,value) \
    ( (This)->lpVtbl->put_Fill(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_Stroke(This,value) \
    ( (This)->lpVtbl->get_Stroke(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_Stroke(This,value) \
    ( (This)->lpVtbl->put_Stroke(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeMiterLimit(This,value) \
    ( (This)->lpVtbl->get_StrokeMiterLimit(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeMiterLimit(This,value) \
    ( (This)->lpVtbl->put_StrokeMiterLimit(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeThickness(This,value) \
    ( (This)->lpVtbl->get_StrokeThickness(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeThickness(This,value) \
    ( (This)->lpVtbl->put_StrokeThickness(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeStartLineCap(This,value) \
    ( (This)->lpVtbl->get_StrokeStartLineCap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeStartLineCap(This,value) \
    ( (This)->lpVtbl->put_StrokeStartLineCap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeEndLineCap(This,value) \
    ( (This)->lpVtbl->get_StrokeEndLineCap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeEndLineCap(This,value) \
    ( (This)->lpVtbl->put_StrokeEndLineCap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeLineJoin(This,value) \
    ( (This)->lpVtbl->get_StrokeLineJoin(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeLineJoin(This,value) \
    ( (This)->lpVtbl->put_StrokeLineJoin(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeDashOffset(This,value) \
    ( (This)->lpVtbl->get_StrokeDashOffset(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeDashOffset(This,value) \
    ( (This)->lpVtbl->put_StrokeDashOffset(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeDashCap(This,value) \
    ( (This)->lpVtbl->get_StrokeDashCap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeDashCap(This,value) \
    ( (This)->lpVtbl->put_StrokeDashCap(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_StrokeDashArray(This,value) \
    ( (This)->lpVtbl->get_StrokeDashArray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_StrokeDashArray(This,value) \
    ( (This)->lpVtbl->put_StrokeDashArray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_Stretch(This,value) \
    ( (This)->lpVtbl->get_Stretch(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_put_Stretch(This,value) \
    ( (This)->lpVtbl->put_Stretch(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_get_GeometryTransform(This,value) \
    ( (This)->lpVtbl->get_GeometryTransform(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_GetAlphaMask(This,result) \
    ( (This)->lpVtbl->GetAlphaMask(This,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IShapeFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Shape
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IShapeFactory[] = L"Microsoft.UI.Xaml.Shapes.IShapeFactory";
/* [object, uuid("4FECAFAF-8265-5252-BA5C-F43639F974A5"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShape * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Shapes.IShapeStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Shapes.Shape
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Shapes_IShapeStatics[] = L"Microsoft.UI.Xaml.Shapes.IShapeStatics";
/* [object, uuid("EA407C43-8A09-587A-958A-4DD17D217CE1"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FillProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeMiterLimitProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeThicknessProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeStartLineCapProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeEndLineCapProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeLineJoinProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeDashOffsetProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeDashCapProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StrokeDashArrayProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StretchProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_FillProperty(This,value) \
    ( (This)->lpVtbl->get_FillProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeMiterLimitProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeMiterLimitProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeThicknessProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeThicknessProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeStartLineCapProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeStartLineCapProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeEndLineCapProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeEndLineCapProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeLineJoinProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeLineJoinProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeDashOffsetProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeDashOffsetProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeDashCapProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeDashCapProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StrokeDashArrayProperty(This,value) \
    ( (This)->lpVtbl->get_StrokeDashArrayProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_get_StretchProperty(This,value) \
    ( (This)->lpVtbl->get_StretchProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CShapes_CIShapeStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Ellipse
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IEllipse ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Ellipse_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Ellipse_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Ellipse[] = L"Microsoft.UI.Xaml.Shapes.Ellipse";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Line
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.ILineStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.ILine ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Line_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Line_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Line[] = L"Microsoft.UI.Xaml.Shapes.Line";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Path
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IPathStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IPath ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Path_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Path_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Path[] = L"Microsoft.UI.Xaml.Shapes.Path";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Polygon
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IPolygonStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IPolygon ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polygon_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polygon_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Polygon[] = L"Microsoft.UI.Xaml.Shapes.Polygon";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Polyline
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IPolylineStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IPolyline ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polyline_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Polyline_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Polyline[] = L"Microsoft.UI.Xaml.Shapes.Polyline";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Rectangle
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IRectangleStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IRectangle ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Rectangle_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Rectangle_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Rectangle[] = L"Microsoft.UI.Xaml.Shapes.Rectangle";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Shapes.Shape
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Shapes.IShapeStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Shapes.IShape ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Shape_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Shapes_Shape_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Shapes_Shape[] = L"Microsoft.UI.Xaml.Shapes.Shape";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EShapes_p_h__

#endif // __Microsoft2EUI2EXaml2EShapes_h__
