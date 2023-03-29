/* Header file automatically generated from Microsoft.UI.Xaml.Media.Media3D.idl */
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
#ifndef __Microsoft2EUI2EXaml2EMedia2EMedia3D_h__
#define __Microsoft2EUI2EXaml2EMedia2EMedia3D_h__
#ifndef __Microsoft2EUI2EXaml2EMedia2EMedia3D_p_h__
#define __Microsoft2EUI2EXaml2EMedia2EMedia3D_p_h__


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
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface ICompositeTransform3D;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D ABI::Microsoft::UI::Xaml::Media::Media3D::ICompositeTransform3D

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface ICompositeTransform3DStatics;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics ABI::Microsoft::UI::Xaml::Media::Media3D::ICompositeTransform3DStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface IMatrix3DHelper;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper ABI::Microsoft::UI::Xaml::Media::Media3D::IMatrix3DHelper

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface IMatrix3DHelperStatics;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics ABI::Microsoft::UI::Xaml::Media::Media3D::IMatrix3DHelperStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface IPerspectiveTransform3D;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D ABI::Microsoft::UI::Xaml::Media::Media3D::IPerspectiveTransform3D

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface IPerspectiveTransform3DStatics;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics ABI::Microsoft::UI::Xaml::Media::Media3D::IPerspectiveTransform3DStatics

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface ITransform3D;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D ABI::Microsoft::UI::Xaml::Media::Media3D::ITransform3D

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        interface ITransform3DFactory;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory ABI::Microsoft::UI::Xaml::Media::Media3D::ITransform3DFactory

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_FWD_DEFINED__


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
                    namespace Media3D {
                        
                        typedef struct Matrix3D Matrix3D;
                        
                    } /* Media3D */
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
                    namespace Media3D {
                        class CompositeTransform3D;
                    } /* Media3D */
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
                    namespace Media3D {
                        class Matrix3DHelper;
                    } /* Media3D */
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
                    namespace Media3D {
                        class PerspectiveTransform3D;
                    } /* Media3D */
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
                    namespace Media3D {
                        class Transform3D;
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */








/*
 *
 * Struct Microsoft.UI.Xaml.Media.Media3D.Matrix3D
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
                    namespace Media3D {
                        /* [contract] */
                        struct Matrix3D
                        {
                            DOUBLE M11;
                            DOUBLE M12;
                            DOUBLE M13;
                            DOUBLE M14;
                            DOUBLE M21;
                            DOUBLE M22;
                            DOUBLE M23;
                            DOUBLE M24;
                            DOUBLE M31;
                            DOUBLE M32;
                            DOUBLE M33;
                            DOUBLE M34;
                            DOUBLE OffsetX;
                            DOUBLE OffsetY;
                            DOUBLE OffsetZ;
                            DOUBLE M44;
                        };
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ICompositeTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3D";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("CBAF163F-C254-5DCF-8AE4-40E21CE1B4CA"), exclusiveto, contract] */
                        MIDL_INTERFACE("CBAF163F-C254-5DCF-8AE4-40E21CE1B4CA")
                        ICompositeTransform3D : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CenterX(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CenterX(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CenterY(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CenterY(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CenterZ(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CenterZ(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationX(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RotationX(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationY(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RotationY(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationZ(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RotationZ(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ScaleX(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ScaleX(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ScaleY(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ScaleY(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ScaleZ(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ScaleZ(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TranslateX(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TranslateX(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TranslateY(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TranslateY(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TranslateZ(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TranslateZ(
                                /* [in] */DOUBLE value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ICompositeTransform3D=_uuidof(ICompositeTransform3D);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3DStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ICompositeTransform3DStatics[] = L"Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3DStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("B64D4181-6988-5D46-858A-224DB7089DC4"), exclusiveto, contract] */
                        MIDL_INTERFACE("B64D4181-6988-5D46-858A-224DB7089DC4")
                        ICompositeTransform3DStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CenterXProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CenterYProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CenterZProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationXProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationYProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationZProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ScaleXProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ScaleYProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ScaleZProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TranslateXProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TranslateYProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TranslateZProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ICompositeTransform3DStatics=_uuidof(ICompositeTransform3DStatics);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IMatrix3DHelper[] = L"Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelper";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("D2909BE1-9C28-5B38-B63C-88E838644533"), exclusiveto, contract] */
                        MIDL_INTERFACE("D2909BE1-9C28-5B38-B63C-88E838644533")
                        IMatrix3DHelper : public IInspectable
                        {
                        public:
                            
                        };

                        MIDL_CONST_ID IID & IID_IMatrix3DHelper=_uuidof(IMatrix3DHelper);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelperStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IMatrix3DHelperStatics[] = L"Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelperStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("930E447B-265C-5DED-9E64-57B8933C55C3"), exclusiveto, contract] */
                        MIDL_INTERFACE("930E447B-265C-5DED-9E64-57B8933C55C3")
                        IMatrix3DHelperStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Identity(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D * value
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE Multiply(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D matrix1,
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D matrix2,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D * result
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE FromElements(
                                /* [in] */DOUBLE m11,
                                /* [in] */DOUBLE m12,
                                /* [in] */DOUBLE m13,
                                /* [in] */DOUBLE m14,
                                /* [in] */DOUBLE m21,
                                /* [in] */DOUBLE m22,
                                /* [in] */DOUBLE m23,
                                /* [in] */DOUBLE m24,
                                /* [in] */DOUBLE m31,
                                /* [in] */DOUBLE m32,
                                /* [in] */DOUBLE m33,
                                /* [in] */DOUBLE m34,
                                /* [in] */DOUBLE offsetX,
                                /* [in] */DOUBLE offsetY,
                                /* [in] */DOUBLE offsetZ,
                                /* [in] */DOUBLE m44,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D * result
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE GetHasInverse(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D target,
                                /* [retval, out] */::boolean * result
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE GetIsIdentity(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D target,
                                /* [retval, out] */::boolean * result
                                ) = 0;
                            virtual HRESULT STDMETHODCALLTYPE Invert(
                                /* [in] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D target,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Media3D::Matrix3D * result
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IMatrix3DHelperStatics=_uuidof(IMatrix3DHelperStatics);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IPerspectiveTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3D";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("4006CC46-684E-54EA-A421-DAE5B0556B85"), exclusiveto, contract] */
                        MIDL_INTERFACE("4006CC46-684E-54EA-A421-DAE5B0556B85")
                        IPerspectiveTransform3D : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Depth(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Depth(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OffsetX(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_OffsetX(
                                /* [in] */DOUBLE value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OffsetY(
                                /* [retval, out] */DOUBLE * value
                                ) = 0;
                            /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_OffsetY(
                                /* [in] */DOUBLE value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IPerspectiveTransform3D=_uuidof(IPerspectiveTransform3D);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3DStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IPerspectiveTransform3DStatics[] = L"Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3DStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("3B16AA8D-0EE2-5D46-A723-DC8E5C1C0B19"), exclusiveto, contract] */
                        MIDL_INTERFACE("3B16AA8D-0EE2-5D46-A723-DC8E5C1C0B19")
                        IPerspectiveTransform3DStatics : public IInspectable
                        {
                        public:
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DepthProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OffsetXProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OffsetYProperty(
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::IDependencyProperty * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_IPerspectiveTransform3DStatics=_uuidof(IPerspectiveTransform3DStatics);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ITransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Transform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ITransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.ITransform3D";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("AFEA4941-2E49-533C-9F8F-2C126EF9893A"), exclusiveto, contract] */
                        MIDL_INTERFACE("AFEA4941-2E49-533C-9F8F-2C126EF9893A")
                        ITransform3D : public IInspectable
                        {
                        public:
                            
                        };

                        MIDL_CONST_ID IID & IID_ITransform3D=_uuidof(ITransform3D);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ITransform3DFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Transform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ITransform3DFactory[] = L"Microsoft.UI.Xaml.Media.Media3D.ITransform3DFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Xaml {
                namespace Media {
                    namespace Media3D {
                        /* [object, uuid("9BCCE0A1-10AC-5319-BDF1-548D2E5AE504"), exclusiveto, contract] */
                        MIDL_INTERFACE("9BCCE0A1-10AC-5319-BDF1-548D2E5AE504")
                        ITransform3DFactory : public IInspectable
                        {
                        public:
                            virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                                /* [in] */IInspectable * baseInterface,
                                /* [out] */IInspectable * * innerInterface,
                                /* [retval, out] */ABI::Microsoft::UI::Xaml::Media::Media3D::ITransform3D * * value
                                ) = 0;
                            
                        };

                        MIDL_CONST_ID IID & IID_ITransform3DFactory=_uuidof(ITransform3DFactory);
                        
                    } /* Media3D */
                } /* Media */
            } /* Xaml */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3DStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3D ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_CompositeTransform3D_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_CompositeTransform3D_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_CompositeTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelperStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Matrix3DHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Matrix3DHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_Matrix3DHelper[] = L"Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3DStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3D ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_PerspectiveTransform3D_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_PerspectiveTransform3D_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_PerspectiveTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.Transform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.ITransform3D ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Transform3D_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Transform3D_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_Transform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.Transform3D";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000






#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty __x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty;

#endif // ____x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty_FWD_DEFINED__







typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D;



















/*
 *
 * Struct Microsoft.UI.Xaml.Media.Media3D.Matrix3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D
{
    DOUBLE M11;
    DOUBLE M12;
    DOUBLE M13;
    DOUBLE M14;
    DOUBLE M21;
    DOUBLE M22;
    DOUBLE M23;
    DOUBLE M24;
    DOUBLE M31;
    DOUBLE M32;
    DOUBLE M33;
    DOUBLE M34;
    DOUBLE OffsetX;
    DOUBLE OffsetY;
    DOUBLE OffsetZ;
    DOUBLE M44;
};
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ICompositeTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3D";
/* [object, uuid("CBAF163F-C254-5DCF-8AE4-40E21CE1B4CA"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CenterX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CenterX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CenterY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CenterY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CenterZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CenterZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RotationX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RotationY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RotationZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ScaleX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ScaleX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ScaleY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ScaleY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ScaleZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ScaleZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TranslateX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TranslateX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TranslateY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TranslateY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TranslateZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TranslateZ )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D * This,
        /* [in] */DOUBLE value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_CenterX(This,value) \
    ( (This)->lpVtbl->get_CenterX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_CenterX(This,value) \
    ( (This)->lpVtbl->put_CenterX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_CenterY(This,value) \
    ( (This)->lpVtbl->get_CenterY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_CenterY(This,value) \
    ( (This)->lpVtbl->put_CenterY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_CenterZ(This,value) \
    ( (This)->lpVtbl->get_CenterZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_CenterZ(This,value) \
    ( (This)->lpVtbl->put_CenterZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_RotationX(This,value) \
    ( (This)->lpVtbl->get_RotationX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_RotationX(This,value) \
    ( (This)->lpVtbl->put_RotationX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_RotationY(This,value) \
    ( (This)->lpVtbl->get_RotationY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_RotationY(This,value) \
    ( (This)->lpVtbl->put_RotationY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_RotationZ(This,value) \
    ( (This)->lpVtbl->get_RotationZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_RotationZ(This,value) \
    ( (This)->lpVtbl->put_RotationZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_ScaleX(This,value) \
    ( (This)->lpVtbl->get_ScaleX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_ScaleX(This,value) \
    ( (This)->lpVtbl->put_ScaleX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_ScaleY(This,value) \
    ( (This)->lpVtbl->get_ScaleY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_ScaleY(This,value) \
    ( (This)->lpVtbl->put_ScaleY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_ScaleZ(This,value) \
    ( (This)->lpVtbl->get_ScaleZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_ScaleZ(This,value) \
    ( (This)->lpVtbl->put_ScaleZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_TranslateX(This,value) \
    ( (This)->lpVtbl->get_TranslateX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_TranslateX(This,value) \
    ( (This)->lpVtbl->put_TranslateX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_TranslateY(This,value) \
    ( (This)->lpVtbl->get_TranslateY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_TranslateY(This,value) \
    ( (This)->lpVtbl->put_TranslateY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_get_TranslateZ(This,value) \
    ( (This)->lpVtbl->get_TranslateZ(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_put_TranslateZ(This,value) \
    ( (This)->lpVtbl->put_TranslateZ(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3D_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3DStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ICompositeTransform3DStatics[] = L"Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3DStatics";
/* [object, uuid("B64D4181-6988-5D46-858A-224DB7089DC4"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CenterXProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CenterYProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CenterZProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationXProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationYProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationZProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ScaleXProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ScaleYProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ScaleZProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TranslateXProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TranslateYProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TranslateZProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_CenterXProperty(This,value) \
    ( (This)->lpVtbl->get_CenterXProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_CenterYProperty(This,value) \
    ( (This)->lpVtbl->get_CenterYProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_CenterZProperty(This,value) \
    ( (This)->lpVtbl->get_CenterZProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_RotationXProperty(This,value) \
    ( (This)->lpVtbl->get_RotationXProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_RotationYProperty(This,value) \
    ( (This)->lpVtbl->get_RotationYProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_RotationZProperty(This,value) \
    ( (This)->lpVtbl->get_RotationZProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_ScaleXProperty(This,value) \
    ( (This)->lpVtbl->get_ScaleXProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_ScaleYProperty(This,value) \
    ( (This)->lpVtbl->get_ScaleYProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_ScaleZProperty(This,value) \
    ( (This)->lpVtbl->get_ScaleZProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_TranslateXProperty(This,value) \
    ( (This)->lpVtbl->get_TranslateXProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_TranslateYProperty(This,value) \
    ( (This)->lpVtbl->get_TranslateYProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_get_TranslateZProperty(This,value) \
    ( (This)->lpVtbl->get_TranslateZProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CICompositeTransform3DStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IMatrix3DHelper[] = L"Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelper";
/* [object, uuid("D2909BE1-9C28-5B38-B63C-88E838644533"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelperStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IMatrix3DHelperStatics[] = L"Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelperStatics";
/* [object, uuid("930E447B-265C-5DED-9E64-57B8933C55C3"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Identity )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D * value
        );
    HRESULT ( STDMETHODCALLTYPE *Multiply )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D matrix1,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D matrix2,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D * result
        );
    HRESULT ( STDMETHODCALLTYPE *FromElements )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
        /* [in] */DOUBLE m11,
        /* [in] */DOUBLE m12,
        /* [in] */DOUBLE m13,
        /* [in] */DOUBLE m14,
        /* [in] */DOUBLE m21,
        /* [in] */DOUBLE m22,
        /* [in] */DOUBLE m23,
        /* [in] */DOUBLE m24,
        /* [in] */DOUBLE m31,
        /* [in] */DOUBLE m32,
        /* [in] */DOUBLE m33,
        /* [in] */DOUBLE m34,
        /* [in] */DOUBLE offsetX,
        /* [in] */DOUBLE offsetY,
        /* [in] */DOUBLE offsetZ,
        /* [in] */DOUBLE m44,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetHasInverse )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D target,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetIsIdentity )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D target,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *Invert )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D target,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CMatrix3D * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_get_Identity(This,value) \
    ( (This)->lpVtbl->get_Identity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_Multiply(This,matrix1,matrix2,result) \
    ( (This)->lpVtbl->Multiply(This,matrix1,matrix2,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_FromElements(This,m11,m12,m13,m14,m21,m22,m23,m24,m31,m32,m33,m34,offsetX,offsetY,offsetZ,m44,result) \
    ( (This)->lpVtbl->FromElements(This,m11,m12,m13,m14,m21,m22,m23,m24,m31,m32,m33,m34,offsetX,offsetY,offsetZ,m44,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_GetHasInverse(This,target,result) \
    ( (This)->lpVtbl->GetHasInverse(This,target,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_GetIsIdentity(This,target,result) \
    ( (This)->lpVtbl->GetIsIdentity(This,target,result) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_Invert(This,target,result) \
    ( (This)->lpVtbl->Invert(This,target,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIMatrix3DHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IPerspectiveTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3D";
/* [object, uuid("4006CC46-684E-54EA-A421-DAE5B0556B85"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Depth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Depth )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OffsetX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_OffsetX )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
        /* [in] */DOUBLE value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OffsetY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
        /* [retval, out] */DOUBLE * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_OffsetY )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D * This,
        /* [in] */DOUBLE value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_get_Depth(This,value) \
    ( (This)->lpVtbl->get_Depth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_put_Depth(This,value) \
    ( (This)->lpVtbl->put_Depth(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_get_OffsetX(This,value) \
    ( (This)->lpVtbl->get_OffsetX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_put_OffsetX(This,value) \
    ( (This)->lpVtbl->put_OffsetX(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_get_OffsetY(This,value) \
    ( (This)->lpVtbl->get_OffsetY(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_put_OffsetY(This,value) \
    ( (This)->lpVtbl->put_OffsetY(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3D_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3DStatics
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_IPerspectiveTransform3DStatics[] = L"Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3DStatics";
/* [object, uuid("3B16AA8D-0EE2-5D46-A723-DC8E5C1C0B19"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DepthProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OffsetXProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OffsetYProperty )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CIDependencyProperty * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_get_DepthProperty(This,value) \
    ( (This)->lpVtbl->get_DepthProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_get_OffsetXProperty(This,value) \
    ( (This)->lpVtbl->get_OffsetXProperty(This,value) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_get_OffsetYProperty(This,value) \
    ( (This)->lpVtbl->get_OffsetYProperty(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CIPerspectiveTransform3DStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ITransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Transform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ITransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.ITransform3D";
/* [object, uuid("AFEA4941-2E49-533C-9F8F-2C126EF9893A"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Xaml.Media.Media3D.ITransform3DFactory
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Xaml.Media.Media3D.Transform3D
 *
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Xaml_Media_Media3D_ITransform3DFactory[] = L"Microsoft.UI.Xaml.Media.Media3D.ITransform3DFactory";
/* [object, uuid("9BCCE0A1-10AC-5319-BDF1-548D2E5AE504"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory * This,
        /* [in] */IInspectable * baseInterface,
        /* [out] */IInspectable * * innerInterface,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3D * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_CreateInstance(This,baseInterface,innerInterface,value) \
    ( (This)->lpVtbl->CreateInstance(This,baseInterface,innerInterface,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CXaml_CMedia_CMedia3D_CITransform3DFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3DStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.ICompositeTransform3D ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_CompositeTransform3D_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_CompositeTransform3D_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_CompositeTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.CompositeTransform3D";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelperStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.IMatrix3DHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Matrix3DHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Matrix3DHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_Matrix3DHelper[] = L"Microsoft.UI.Xaml.Media.Media3D.Matrix3DHelper";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3DStatics interface starting with version 1.0 of the Microsoft.UI.Xaml.WinUIContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.IPerspectiveTransform3D ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_PerspectiveTransform3D_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_PerspectiveTransform3D_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_PerspectiveTransform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.PerspectiveTransform3D";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Xaml.Media.Media3D.Transform3D
 *
 * Introduced to Microsoft.UI.Xaml.WinUIContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Xaml.Media.Media3D.ITransform3D ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Transform3D_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Xaml_Media_Media3D_Transform3D_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Xaml_Media_Media3D_Transform3D[] = L"Microsoft.UI.Xaml.Media.Media3D.Transform3D";
#endif
#endif // MICROSOFT_UI_XAML_WINUICONTRACT_VERSION >= 0x10000






#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EXaml2EMedia2EMedia3D_p_h__

#endif // __Microsoft2EUI2EXaml2EMedia2EMedia3D_h__
