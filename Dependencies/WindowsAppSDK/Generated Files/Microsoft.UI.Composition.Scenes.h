/* Header file automatically generated from Microsoft.UI.Composition.Scenes.idl */
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
#ifndef __Microsoft2EUI2EComposition2EScenes_h__
#define __Microsoft2EUI2EComposition2EScenes_h__
#ifndef __Microsoft2EUI2EComposition2EScenes_p_h__
#define __Microsoft2EUI2EComposition2EScenes_p_h__


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
#include "Microsoft.Graphics.DirectX.h"
#include "Microsoft.UI.Composition.h"
#include "Windows.Foundation.Numerics.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneBoundingBox;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox ABI::Microsoft::UI::Composition::Scenes::ISceneBoundingBox

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneComponent;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent ABI::Microsoft::UI::Composition::Scenes::ISceneComponent

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneComponentCollection;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection ABI::Microsoft::UI::Composition::Scenes::ISceneComponentCollection

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneComponentFactory;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory ABI::Microsoft::UI::Composition::Scenes::ISceneComponentFactory

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMaterial;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial ABI::Microsoft::UI::Composition::Scenes::ISceneMaterial

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMaterialFactory;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialFactory

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMaterialInput;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMaterialInputFactory;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInputFactory

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMesh;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh ABI::Microsoft::UI::Composition::Scenes::ISceneMesh

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMeshMaterialAttributeMap;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap ABI::Microsoft::UI::Composition::Scenes::ISceneMeshMaterialAttributeMap

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMeshRendererComponent;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent ABI::Microsoft::UI::Composition::Scenes::ISceneMeshRendererComponent

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMeshRendererComponentStatics;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics ABI::Microsoft::UI::Composition::Scenes::ISceneMeshRendererComponentStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMeshStatics;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics ABI::Microsoft::UI::Composition::Scenes::ISceneMeshStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMetallicRoughnessMaterial;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial ABI::Microsoft::UI::Composition::Scenes::ISceneMetallicRoughnessMaterial

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneMetallicRoughnessMaterialStatics;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics ABI::Microsoft::UI::Composition::Scenes::ISceneMetallicRoughnessMaterialStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneModelTransform;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform ABI::Microsoft::UI::Composition::Scenes::ISceneModelTransform

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneNode;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode ABI::Microsoft::UI::Composition::Scenes::ISceneNode

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneNodeCollection;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection ABI::Microsoft::UI::Composition::Scenes::ISceneNodeCollection

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneNodeStatics;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics ABI::Microsoft::UI::Composition::Scenes::ISceneNodeStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneObject;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject ABI::Microsoft::UI::Composition::Scenes::ISceneObject

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneObjectFactory;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory ABI::Microsoft::UI::Composition::Scenes::ISceneObjectFactory

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface IScenePbrMaterial;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial ABI::Microsoft::UI::Composition::Scenes::IScenePbrMaterial

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface IScenePbrMaterialFactory;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory ABI::Microsoft::UI::Composition::Scenes::IScenePbrMaterialFactory

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneRendererComponent;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent ABI::Microsoft::UI::Composition::Scenes::ISceneRendererComponent

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneRendererComponentFactory;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory ABI::Microsoft::UI::Composition::Scenes::ISceneRendererComponentFactory

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneSurfaceMaterialInput;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput ABI::Microsoft::UI::Composition::Scenes::ISceneSurfaceMaterialInput

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneSurfaceMaterialInputStatics;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics ABI::Microsoft::UI::Composition::Scenes::ISceneSurfaceMaterialInputStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneVisual;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual ABI::Microsoft::UI::Composition::Scenes::ISceneVisual

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    interface ISceneVisualStatics;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics ABI::Microsoft::UI::Composition::Scenes::ISceneVisualStatics

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneComponent;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#define DEF___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("5bae5c3e-e6cc-5583-b768-b577456babe8"))
IIterator<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*, ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Microsoft.UI.Composition.Scenes.SceneComponent>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t;
#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::__FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
//#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#define DEF___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("2685e472-2678-505f-943f-b18b1689891e"))
IIterable<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*, ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Microsoft.UI.Composition.Scenes.SceneComponent>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t;
#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::__FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
//#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneNode;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#define DEF___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("9f0ca077-4147-5aed-8819-a9149d2b234d"))
IIterator<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneNode*, ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Microsoft.UI.Composition.Scenes.SceneNode>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t;
#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::__FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
//#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t ABI::Windows::Foundation::Collections::IIterator<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#define DEF___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("9cc8ee93-c366-5810-ab51-3590047515f2"))
IIterable<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneNode*, ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Microsoft.UI.Composition.Scenes.SceneNode>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t;
#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::__FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
//#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t ABI::Windows::Foundation::Collections::IIterable<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    enum SceneAttributeSemantic : int;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


#ifndef DEF___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#define DEF___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("109a063e-257f-5701-b780-e50bd9912b50"))
IKeyValuePair<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> : IKeyValuePair_impl<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IKeyValuePair`2<String, Microsoft.UI.Composition.Scenes.SceneAttributeSemantic>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IKeyValuePair<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t;
#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>
//#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE */





#ifndef DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#define DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("eb061e61-364d-546d-8b49-82c585464c00"))
IIterator<__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic*> : IIterator_impl<__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Windows.Foundation.Collections.IKeyValuePair`2<String, Microsoft.UI.Composition.Scenes.SceneAttributeSemantic>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic*> __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t;
#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::__FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>*>
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE */





#ifndef DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#define DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("d2e146d1-5e16-5c59-b07a-e253e65cc7e7"))
IIterable<__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic*> : IIterable_impl<__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Windows.Foundation.Collections.IKeyValuePair`2<String, Microsoft.UI.Composition.Scenes.SceneAttributeSemantic>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<__FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic*> __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t;
#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::__FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>*>
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE */





#ifndef DEF___FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#define DEF___FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("a0015c14-a54e-5da8-bd55-ac76ea838517"))
IMapView<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> : IMapView_impl<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IMapView`2<String, Microsoft.UI.Composition.Scenes.SceneAttributeSemantic>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IMapView<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t;
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::__FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::IMapView<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>
//#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t ABI::Windows::Foundation::Collections::IMapView<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE */





#ifndef DEF___FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#define DEF___FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("e024c948-c902-5dd9-9768-ae332ae0be14"))
IMap<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> : IMap_impl<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IMap`2<String, Microsoft.UI.Composition.Scenes.SceneAttributeSemantic>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IMap<HSTRING,enum ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic> __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t;
#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::__FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic ABI::Windows::Foundation::Collections::IMap<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>
//#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_t ABI::Windows::Foundation::Collections::IMap<HSTRING,ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_USE */




#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#define DEF___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("6817b7fd-1182-58b7-aa8a-110337639e4b"))
IVectorView<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*, ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Microsoft.UI.Composition.Scenes.SceneComponent>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t;
#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::__FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
//#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#define DEF___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("be91d459-5f24-5044-bb9b-79787c1b3dfd"))
IVectorView<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneNode*, ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<Microsoft.UI.Composition.Scenes.SceneNode>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t;
#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::__FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
//#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t ABI::Windows::Foundation::Collections::IVectorView<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#define DEF___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("9c16c63a-f909-56c7-8778-d7ebc94808d9"))
IVector<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*, ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Microsoft.UI.Composition.Scenes.SceneComponent>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<ABI::Microsoft::UI::Composition::Scenes::SceneComponent*> __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t;
#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::__FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
//#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_t ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Composition::Scenes::ISceneComponent*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef DEF___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#define DEF___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("15c6abb0-bf50-5a9b-8190-16b631f12705"))
IVector<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<ABI::Microsoft::UI::Composition::Scenes::SceneNode*, ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVector`1<Microsoft.UI.Composition.Scenes.SceneNode>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVector<ABI::Microsoft::UI::Composition::Scenes::SceneNode*> __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t;
#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::__FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t
/* Collections */ } /* Foundation */ } /* Windows */ } /* ABI */ } 

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
//#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_t ABI::Windows::Foundation::Collections::IVector<ABI::Microsoft::UI::Composition::Scenes::ISceneNode*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_USE */


#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000






namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace DirectX {
                
                typedef enum DirectXPixelFormat : int DirectXPixelFormat;
                
            } /* DirectX */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace Graphics {
            namespace DirectX {
                
                typedef enum DirectXPrimitiveTopology : int DirectXPrimitiveTopology;
                
            } /* DirectX */
        } /* Graphics */
    } /* Microsoft */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                
                typedef enum CompositionBitmapInterpolationMode : int CompositionBitmapInterpolationMode;
                
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                class Compositor;
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositor_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                interface ICompositor;
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CICompositor ABI::Microsoft::UI::Composition::ICompositor

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositor_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                interface ICompositionSurface;
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface ABI::Microsoft::UI::Composition::ICompositionSurface

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface_FWD_DEFINED__





namespace ABI {
    namespace Windows {
        namespace Foundation {
            class MemoryBuffer;
        } /* Foundation */
    } /* Windows */
} /* ABI */

#ifndef ____x_ABI_CWindows_CFoundation_CIMemoryBuffer_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIMemoryBuffer_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Foundation {
            interface IMemoryBuffer;
        } /* Foundation */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CFoundation_CIMemoryBuffer ABI::Windows::Foundation::IMemoryBuffer

#endif // ____x_ABI_CWindows_CFoundation_CIMemoryBuffer_FWD_DEFINED__




namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Numerics {
                
                typedef struct Quaternion Quaternion;
                
            } /* Numerics */
        } /* Foundation */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Numerics {
                
                typedef struct Vector3 Vector3;
                
            } /* Numerics */
        } /* Foundation */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Numerics {
                
                typedef struct Vector4 Vector4;
                
            } /* Numerics */
        } /* Foundation */
    } /* Windows */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    
                    typedef enum SceneAlphaMode : int SceneAlphaMode;
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    
                    typedef enum SceneAttributeSemantic : int SceneAttributeSemantic;
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    
                    typedef enum SceneComponentType : int SceneComponentType;
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    
                    typedef enum SceneWrappingMode : int SceneWrappingMode;
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */






























namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneBoundingBox;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneComponentCollection;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneMaterial;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneMaterialInput;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneMesh;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneMeshMaterialAttributeMap;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneMeshRendererComponent;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneMetallicRoughnessMaterial;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneModelTransform;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneNodeCollection;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneObject;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class ScenePbrMaterial;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneRendererComponent;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneSurfaceMaterialInput;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    class SceneVisual;
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */












/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneAlphaMode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [v1_enum, contract] */
                    enum SceneAlphaMode : int
                    {
                        SceneAlphaMode_Opaque = 0,
                        SceneAlphaMode_AlphaTest = 1,
                        SceneAlphaMode_Blend = 2,
                    };
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneAttributeSemantic
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [v1_enum, contract] */
                    enum SceneAttributeSemantic : int
                    {
                        SceneAttributeSemantic_Index = 0,
                        SceneAttributeSemantic_Vertex = 1,
                        SceneAttributeSemantic_Normal = 2,
                        SceneAttributeSemantic_TexCoord0 = 3,
                        SceneAttributeSemantic_TexCoord1 = 4,
                        SceneAttributeSemantic_Color = 5,
                        SceneAttributeSemantic_Tangent = 6,
                    };
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneComponentType
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [v1_enum, contract] */
                    enum SceneComponentType : int
                    {
                        SceneComponentType_MeshRendererComponent = 0,
                    };
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneWrappingMode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [v1_enum, contract] */
                    enum SceneWrappingMode : int
                    {
                        SceneWrappingMode_ClampToEdge = 0,
                        SceneWrappingMode_MirroredRepeat = 1,
                        SceneWrappingMode_Repeat = 2,
                    };
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneBoundingBox
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneBoundingBox
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneBoundingBox[] = L"Microsoft.UI.Composition.Scenes.ISceneBoundingBox";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("39FB48E0-216A-5608-9186-6BA9F98B5C67"), exclusiveto, contract] */
                    MIDL_INTERFACE("39FB48E0-216A-5608-9186-6BA9F98B5C67")
                    ISceneBoundingBox : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Center(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Extents(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Max(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Min(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Size(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneBoundingBox=_uuidof(ISceneBoundingBox);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneComponent[] = L"Microsoft.UI.Composition.Scenes.ISceneComponent";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("F73361CB-8027-50E2-98EE-B2E3EA050A54"), exclusiveto, contract] */
                    MIDL_INTERFACE("F73361CB-8027-50E2-98EE-B2E3EA050A54")
                    ISceneComponent : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ComponentType(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::SceneComponentType * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneComponent=_uuidof(ISceneComponent);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneComponentCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneComponentCollection
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneComponentCollection[] = L"Microsoft.UI.Composition.Scenes.ISceneComponentCollection";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("E4B21C71-87E2-5AEB-85BE-884E8302273E"), exclusiveto, contract] */
                    MIDL_INTERFACE("E4B21C71-87E2-5AEB-85BE-884E8302273E")
                    ISceneComponentCollection : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneComponentCollection=_uuidof(ISceneComponentCollection);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneComponentFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneComponentFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneComponentFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("254088B0-BABF-503D-9A66-0D86AF5F7303"), exclusiveto, contract] */
                    MIDL_INTERFACE("254088B0-BABF-503D-9A66-0D86AF5F7303")
                    ISceneComponentFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneComponentFactory=_uuidof(ISceneComponentFactory);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterial[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterial";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("042142A7-BF6B-57AD-BADC-F581F38EDB48"), exclusiveto, contract] */
                    MIDL_INTERFACE("042142A7-BF6B-57AD-BADC-F581F38EDB48")
                    ISceneMaterial : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMaterial=_uuidof(ISceneMaterial);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterialFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterialFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterialFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("25747893-8748-5F60-969F-318FA0B735CA"), exclusiveto, contract] */
                    MIDL_INTERFACE("25747893-8748-5F60-969F-318FA0B735CA")
                    ISceneMaterialFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMaterialFactory=_uuidof(ISceneMaterialFactory);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterialInput[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterialInput";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("446BDADE-719B-5DB4-B699-F226D0062A2E"), exclusiveto, contract] */
                    MIDL_INTERFACE("446BDADE-719B-5DB4-B699-F226D0062A2E")
                    ISceneMaterialInput : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMaterialInput=_uuidof(ISceneMaterialInput);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterialInputFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterialInputFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterialInputFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("B4DABD1D-58C0-5710-928A-BC49B0735694"), exclusiveto, contract] */
                    MIDL_INTERFACE("B4DABD1D-58C0-5710-928A-BC49B0735694")
                    ISceneMaterialInputFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMaterialInputFactory=_uuidof(ISceneMaterialInputFactory);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMesh
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMesh
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMesh[] = L"Microsoft.UI.Composition.Scenes.ISceneMesh";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("5CF846AA-F53F-555E-A3AD-F5BC52CA32FB"), exclusiveto, contract] */
                    MIDL_INTERFACE("5CF846AA-F53F-555E-A3AD-F5BC52CA32FB")
                    ISceneMesh : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Bounds(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneBoundingBox * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PrimitiveTopology(
                            /* [retval, out] */ABI::Microsoft::Graphics::DirectX::DirectXPrimitiveTopology * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PrimitiveTopology(
                            /* [in] */ABI::Microsoft::Graphics::DirectX::DirectXPrimitiveTopology value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE FillMeshAttribute(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::SceneAttributeSemantic semantic,
                            /* [in] */ABI::Microsoft::Graphics::DirectX::DirectXPixelFormat format,
                            /* [in] */ABI::Windows::Foundation::IMemoryBuffer * memory
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMesh=_uuidof(ISceneMesh);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshMaterialAttributeMap
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMeshMaterialAttributeMap
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshMaterialAttributeMap[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshMaterialAttributeMap";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("2360C457-EDAE-5660-BEDC-89096582ED70"), exclusiveto, contract] */
                    MIDL_INTERFACE("2360C457-EDAE-5660-BEDC-89096582ED70")
                    ISceneMeshMaterialAttributeMap : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMeshMaterialAttributeMap=_uuidof(ISceneMeshMaterialAttributeMap);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshRendererComponent[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponent";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("D2BE85A0-70A8-5C62-84D8-8BA55E4C64A9"), exclusiveto, contract] */
                    MIDL_INTERFACE("D2BE85A0-70A8-5C62-84D8-8BA55E4C64A9")
                    ISceneMeshRendererComponent : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Material(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterial * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Material(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterial * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Mesh(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMesh * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Mesh(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMesh * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UVMappings(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMeshMaterialAttributeMap * * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMeshRendererComponent=_uuidof(ISceneMeshRendererComponent);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponentStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshRendererComponentStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponentStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("C54F8C5A-A104-5CFA-89DC-13EDAA6E3D88"), exclusiveto, contract] */
                    MIDL_INTERFACE("C54F8C5A-A104-5CFA-89DC-13EDAA6E3D88")
                    ISceneMeshRendererComponentStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Create(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositor * compositor,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMeshRendererComponent * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMeshRendererComponentStatics=_uuidof(ISceneMeshRendererComponentStatics);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMesh
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("29C52125-964B-5315-80F9-3893713290F5"), exclusiveto, contract] */
                    MIDL_INTERFACE("29C52125-964B-5315-80F9-3893713290F5")
                    ISceneMeshStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Create(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositor * compositor,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMesh * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMeshStatics=_uuidof(ISceneMeshStatics);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMetallicRoughnessMaterial[] = L"Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterial";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("0A4AFCF4-7BAE-5702-9B85-8BC849F39987"), exclusiveto, contract] */
                    MIDL_INTERFACE("0A4AFCF4-7BAE-5702-9B85-8BC849F39987")
                    ISceneMetallicRoughnessMaterial : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BaseColorInput(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_BaseColorInput(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BaseColorFactor(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector4 * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_BaseColorFactor(
                            /* [in] */ABI::Windows::Foundation::Numerics::Vector4 value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MetallicFactor(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_MetallicFactor(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MetallicRoughnessInput(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_MetallicRoughnessInput(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RoughnessFactor(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RoughnessFactor(
                            /* [in] */FLOAT value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMetallicRoughnessMaterial=_uuidof(ISceneMetallicRoughnessMaterial);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterialStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMetallicRoughnessMaterialStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterialStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("DFFD2043-AB3C-57A0-8E13-6F09725E970F"), exclusiveto, contract] */
                    MIDL_INTERFACE("DFFD2043-AB3C-57A0-8E13-6F09725E970F")
                    ISceneMetallicRoughnessMaterialStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Create(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositor * compositor,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMetallicRoughnessMaterial * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneMetallicRoughnessMaterialStatics=_uuidof(ISceneMetallicRoughnessMaterialStatics);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneModelTransform
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneModelTransform
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneModelTransform[] = L"Microsoft.UI.Composition.Scenes.ISceneModelTransform";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("3F05555F-0F67-576E-9D8A-93C1F250C29F"), exclusiveto, contract] */
                    MIDL_INTERFACE("3F05555F-0F67-576E-9D8A-93C1F250C29F")
                    ISceneModelTransform : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Orientation(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Quaternion * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Orientation(
                            /* [in] */ABI::Windows::Foundation::Numerics::Quaternion value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationAngle(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RotationAngle(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationAngleInDegrees(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RotationAngleInDegrees(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RotationAxis(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RotationAxis(
                            /* [in] */ABI::Windows::Foundation::Numerics::Vector3 value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Scale(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Scale(
                            /* [in] */ABI::Windows::Foundation::Numerics::Vector3 value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Translation(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Translation(
                            /* [in] */ABI::Windows::Foundation::Numerics::Vector3 value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneModelTransform=_uuidof(ISceneModelTransform);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneNode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneNode
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneNode[] = L"Microsoft.UI.Composition.Scenes.ISceneNode";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("A1BCE140-79C2-59E6-9B68-63B1BAB0E2A6"), exclusiveto, contract] */
                    MIDL_INTERFACE("A1BCE140-79C2-59E6-9B68-63B1BAB0E2A6")
                    ISceneNode : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Children(
                            /* [retval, out] */__FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Components(
                            /* [retval, out] */__FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Parent(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneNode * * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Transform(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneModelTransform * * value
                            ) = 0;
                        virtual HRESULT STDMETHODCALLTYPE FindFirstComponentOfType(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::SceneComponentType value,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneComponent * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneNode=_uuidof(ISceneNode);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneNodeCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneNodeCollection
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneNodeCollection[] = L"Microsoft.UI.Composition.Scenes.ISceneNodeCollection";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("F219B68E-5666-5C6C-AA4E-08DB07FD6BCF"), exclusiveto, contract] */
                    MIDL_INTERFACE("F219B68E-5666-5C6C-AA4E-08DB07FD6BCF")
                    ISceneNodeCollection : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneNodeCollection=_uuidof(ISceneNodeCollection);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneNodeStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneNode
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneNodeStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneNodeStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("801C4394-4198-5DA1-AC39-6E8A44B5CE57"), exclusiveto, contract] */
                    MIDL_INTERFACE("801C4394-4198-5DA1-AC39-6E8A44B5CE57")
                    ISceneNodeStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Create(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositor * compositor,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneNode * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneNodeStatics=_uuidof(ISceneNodeStatics);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneObject[] = L"Microsoft.UI.Composition.Scenes.ISceneObject";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("4333E514-4FC7-521E-8BCA-11C51FBCAF1E"), exclusiveto, contract] */
                    MIDL_INTERFACE("4333E514-4FC7-521E-8BCA-11C51FBCAF1E")
                    ISceneObject : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneObject=_uuidof(ISceneObject);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneObjectFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneObjectFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneObjectFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("EE797F7D-77DB-5C4C-B6F5-C1930FAD85C5"), exclusiveto, contract] */
                    MIDL_INTERFACE("EE797F7D-77DB-5C4C-B6F5-C1930FAD85C5")
                    ISceneObjectFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneObjectFactory=_uuidof(ISceneObjectFactory);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.IScenePbrMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.ScenePbrMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_IScenePbrMaterial[] = L"Microsoft.UI.Composition.Scenes.IScenePbrMaterial";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("295D0725-56FE-5954-8057-3F4CA7515B36"), exclusiveto, contract] */
                    MIDL_INTERFACE("295D0725-56FE-5954-8057-3F4CA7515B36")
                    IScenePbrMaterial : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AlphaCutoff(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AlphaCutoff(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AlphaMode(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::SceneAlphaMode * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AlphaMode(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::SceneAlphaMode value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_EmissiveInput(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_EmissiveInput(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_EmissiveFactor(
                            /* [retval, out] */ABI::Windows::Foundation::Numerics::Vector3 * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_EmissiveFactor(
                            /* [in] */ABI::Windows::Foundation::Numerics::Vector3 value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IsDoubleSided(
                            /* [retval, out] */::boolean * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IsDoubleSided(
                            /* [in] */::boolean value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NormalInput(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_NormalInput(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NormalScale(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_NormalScale(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OcclusionInput(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_OcclusionInput(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneMaterialInput * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OcclusionStrength(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_OcclusionStrength(
                            /* [in] */FLOAT value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_IScenePbrMaterial=_uuidof(IScenePbrMaterial);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.IScenePbrMaterialFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.ScenePbrMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_IScenePbrMaterialFactory[] = L"Microsoft.UI.Composition.Scenes.IScenePbrMaterialFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("9E34D32A-E30C-51F5-84AC-6467950605CA"), exclusiveto, contract] */
                    MIDL_INTERFACE("9E34D32A-E30C-51F5-84AC-6467950605CA")
                    IScenePbrMaterialFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_IScenePbrMaterialFactory=_uuidof(IScenePbrMaterialFactory);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneRendererComponent[] = L"Microsoft.UI.Composition.Scenes.ISceneRendererComponent";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("6BAB8030-89C1-5DBC-A48E-1805DDF9CDD1"), exclusiveto, contract] */
                    MIDL_INTERFACE("6BAB8030-89C1-5DBC-A48E-1805DDF9CDD1")
                    ISceneRendererComponent : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneRendererComponent=_uuidof(ISceneRendererComponent);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneRendererComponentFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneRendererComponentFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneRendererComponentFactory";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("3CCAC1D6-6A0F-582E-BB1A-10EBC1E405CA"), exclusiveto, contract] */
                    MIDL_INTERFACE("3CCAC1D6-6A0F-582E-BB1A-10EBC1E405CA")
                    ISceneRendererComponentFactory : public IInspectable
                    {
                    public:
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneRendererComponentFactory=_uuidof(ISceneRendererComponentFactory);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneSurfaceMaterialInput[] = L"Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInput";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("B9854B4F-286C-50CD-A734-491A251D5FD3"), exclusiveto, contract] */
                    MIDL_INTERFACE("B9854B4F-286C-50CD-A734-491A251D5FD3")
                    ISceneSurfaceMaterialInput : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BitmapInterpolationMode(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::CompositionBitmapInterpolationMode * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_BitmapInterpolationMode(
                            /* [in] */ABI::Microsoft::UI::Composition::CompositionBitmapInterpolationMode value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Surface(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::ICompositionSurface * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Surface(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositionSurface * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_WrappingUMode(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::SceneWrappingMode * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_WrappingUMode(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::SceneWrappingMode value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_WrappingVMode(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::SceneWrappingMode * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_WrappingVMode(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::SceneWrappingMode value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneSurfaceMaterialInput=_uuidof(ISceneSurfaceMaterialInput);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInputStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneSurfaceMaterialInputStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInputStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("8E1BA937-AD60-51BC-8256-CA62C4B2AE92"), exclusiveto, contract] */
                    MIDL_INTERFACE("8E1BA937-AD60-51BC-8256-CA62C4B2AE92")
                    ISceneSurfaceMaterialInputStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Create(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositor * compositor,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneSurfaceMaterialInput * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneSurfaceMaterialInputStatics=_uuidof(ISceneSurfaceMaterialInputStatics);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneVisual
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneVisual
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneVisual[] = L"Microsoft.UI.Composition.Scenes.ISceneVisual";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("0144D7AD-6A7D-59CB-A0F9-74A04E85352C"), exclusiveto, contract] */
                    MIDL_INTERFACE("0144D7AD-6A7D-59CB-A0F9-74A04E85352C")
                    ISceneVisual : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Root(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneNode * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Root(
                            /* [in] */ABI::Microsoft::UI::Composition::Scenes::ISceneNode * value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneVisual=_uuidof(ISceneVisual);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneVisualStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneVisual
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneVisualStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneVisualStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Scenes {
                    /* [object, uuid("7B8DA6D1-5BD8-5095-9264-E5572653EA07"), exclusiveto, contract] */
                    MIDL_INTERFACE("7B8DA6D1-5BD8-5095-9264-E5572653EA07")
                    ISceneVisualStatics : public IInspectable
                    {
                    public:
                        virtual HRESULT STDMETHODCALLTYPE Create(
                            /* [in] */ABI::Microsoft::UI::Composition::ICompositor * compositor,
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Scenes::ISceneVisual * * result
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneVisualStatics=_uuidof(ISceneVisualStatics);
                    
                } /* Scenes */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneBoundingBox
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneBoundingBox ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneBoundingBox_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneBoundingBox_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneBoundingBox[] = L"Microsoft.UI.Composition.Scenes.SceneBoundingBox";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneComponent ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponent_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponent_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneComponent[] = L"Microsoft.UI.Composition.Scenes.SceneComponent";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneComponentCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneComponentCollection
 *    Windows.Foundation.Collections.IVector_1_Microsoft.UI.Composition.Scenes.SceneComponent ** Default Interface **
 *    Windows.Foundation.Collections.IIterable_1_Microsoft.UI.Composition.Scenes.SceneComponent
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponentCollection_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponentCollection_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneComponentCollection[] = L"Microsoft.UI.Composition.Scenes.SceneComponentCollection";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMaterial ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterial_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterial_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMaterial[] = L"Microsoft.UI.Composition.Scenes.SceneMaterial";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMaterialInput ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterialInput_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterialInput_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMaterialInput[] = L"Microsoft.UI.Composition.Scenes.SceneMaterialInput";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMesh
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneMeshStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMesh ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMesh_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMesh_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMesh[] = L"Microsoft.UI.Composition.Scenes.SceneMesh";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMeshMaterialAttributeMap
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMeshMaterialAttributeMap ** Default Interface **
 *    Windows.Foundation.Collections.IMap_2_HSTRING,Microsoft.UI.Composition.Scenes.SceneAttributeSemantic
 *    Windows.Foundation.Collections.IIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshMaterialAttributeMap_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshMaterialAttributeMap_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMeshMaterialAttributeMap[] = L"Microsoft.UI.Composition.Scenes.SceneMeshMaterialAttributeMap";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponentStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponent ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshRendererComponent_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshRendererComponent_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMeshRendererComponent[] = L"Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterialStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterial ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMetallicRoughnessMaterial_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMetallicRoughnessMaterial_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMetallicRoughnessMaterial[] = L"Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneModelTransform
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneModelTransform ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneModelTransform_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneModelTransform_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneModelTransform[] = L"Microsoft.UI.Composition.Scenes.SceneModelTransform";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneNode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneNodeStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneNode ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNode_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNode_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneNode[] = L"Microsoft.UI.Composition.Scenes.SceneNode";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneNodeCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneNodeCollection
 *    Windows.Foundation.Collections.IVector_1_Microsoft.UI.Composition.Scenes.SceneNode ** Default Interface **
 *    Windows.Foundation.Collections.IIterable_1_Microsoft.UI.Composition.Scenes.SceneNode
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNodeCollection_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNodeCollection_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneNodeCollection[] = L"Microsoft.UI.Composition.Scenes.SceneNodeCollection";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneObject ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneObject_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneObject_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneObject[] = L"Microsoft.UI.Composition.Scenes.SceneObject";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.ScenePbrMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.IScenePbrMaterial ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_ScenePbrMaterial_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_ScenePbrMaterial_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_ScenePbrMaterial[] = L"Microsoft.UI.Composition.Scenes.ScenePbrMaterial";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneRendererComponent ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneRendererComponent_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneRendererComponent_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneRendererComponent[] = L"Microsoft.UI.Composition.Scenes.SceneRendererComponent";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInputStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInput ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneSurfaceMaterialInput_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneSurfaceMaterialInput_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneSurfaceMaterialInput[] = L"Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneVisual
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneVisualStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneVisual ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneVisual_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneVisual_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneVisual[] = L"Microsoft.UI.Composition.Scenes.SceneVisual";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__)
#define ____FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

typedef struct __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl;

interface __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent
{
    CONST_VTBL struct __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__)
#define ____FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

typedef  struct __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent **first);

    END_INTERFACE
} __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl;

interface __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent
{
    CONST_VTBL struct __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__)
#define ____FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

typedef interface __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

typedef struct __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl;

interface __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode
{
    CONST_VTBL struct __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__)
#define ____FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

typedef interface __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

typedef  struct __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_Microsoft__CUI__CComposition__CScenes__CSceneNode **first);

    END_INTERFACE
} __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl;

interface __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode
{
    CONST_VTBL struct __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic;
#if !defined(____FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__)
#define ____FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__

typedef interface __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

typedef struct __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
            /* [out] */ __RPC__out ULONG *iidCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__out HSTRING *key);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )(__RPC__in __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__deref_out_opt enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic *value);
    END_INTERFACE
} __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl;

interface __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
{
    CONST_VTBL struct __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_get_Key(This,key)	\
    ( (This)->lpVtbl -> get_Key(This,key) ) 

#define __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 
#endif /* COBJMACROS */


#endif // ____FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__



#if !defined(____FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__)
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__

typedef interface __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

typedef struct __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__out __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl;

interface __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
{
    CONST_VTBL struct __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__



#if !defined(____FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__)
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__

typedef interface __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

typedef  struct __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic **first);

    END_INTERFACE
} __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl;

interface __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
{
    CONST_VTBL struct __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__



#if !defined(____FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__)
#define ____FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__

typedef interface __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

typedef struct __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,/* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *Lookup )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ __RPC__in HSTRING key,
        /* [retval][out] */ __RPC__deref_out_opt enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic *value);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__out unsigned int *size);
    HRESULT ( STDMETHODCALLTYPE *HasKey )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [in] */ __RPC__in HSTRING key, /* [retval][out] */ __RPC__out boolean *found);
    HRESULT ( STDMETHODCALLTYPE *Split )(__RPC__in __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,/* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic **firstPartition,
        /* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic **secondPartition);
    END_INTERFACE
} __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl;

interface __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
{
    CONST_VTBL struct __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Lookup(This,key,value)	\
    ( (This)->lpVtbl -> Lookup(This,key,value) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_HasKey(This,key,found)	\
    ( (This)->lpVtbl -> HasKey(This,key,found) ) 
#define __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Split(This,firstPartition,secondPartition)	\
    ( (This)->lpVtbl -> Split(This,firstPartition,secondPartition) ) 
#endif /* COBJMACROS */


#endif // ____FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__



#if !defined(____FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__)
#define ____FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__

typedef interface __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic;

typedef struct __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *Lookup )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ HSTRING key,
        /* [retval][out] */ __RPC__deref_out_opt enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic **value);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__out unsigned int *size);
    HRESULT ( STDMETHODCALLTYPE *HasKey )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [in] */ HSTRING key, /* [retval][out] */ __RPC__out boolean *found);
    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This, /* [retval][out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic **view);
    HRESULT ( STDMETHODCALLTYPE *Insert )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,
        /* [in] */ HSTRING key,
        /* [in] */ __RPC__in_opt enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic *value,
        /* [retval][out] */ __RPC__out boolean *replaced);
    HRESULT ( STDMETHODCALLTYPE *Remove )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This,/* [in] */ HSTRING key);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic * This);
    END_INTERFACE
} __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl;

interface __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
{
    CONST_VTBL struct __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemanticVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Lookup(This,key,value)	\
    ( (This)->lpVtbl -> Lookup(This,key,value) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_HasKey(This,key,found)	\
    ( (This)->lpVtbl -> HasKey(This,key,found) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Insert(This,key,value,replaced)	\
    ( (This)->lpVtbl -> Insert(This,key,value,replaced) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Remove(This,key)	\
    ( (This)->lpVtbl -> Remove(This,key) ) 

#define __FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 
#endif /* COBJMACROS */



#endif // ____FIMap_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic_INTERFACE_DEFINED__



#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

typedef struct __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
            /* [in] */ __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl;

interface __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent
{
    CONST_VTBL struct __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__)
#define ____FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

typedef struct __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
            /* [in] */ __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl;

interface __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode
{
    CONST_VTBL struct __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__)
#define ____FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

typedef interface __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent;

typedef struct __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This, /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * *value);

    END_INTERFACE
} __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl;

interface __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent
{
    CONST_VTBL struct __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponentVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__)
#define ____FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

typedef interface __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode;

typedef struct __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ unsigned int index,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *item);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
        __RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [retval][out] */ __RPC__out unsigned int *size);

    HRESULT ( STDMETHODCALLTYPE *GetView )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [retval][out] */ __RPC__deref_out_opt __FIVectorView_1_Microsoft__CUI__CComposition__CScenes__CSceneNode **view);

    HRESULT ( STDMETHODCALLTYPE *IndexOf )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * item,
        /* [out] */ __RPC__out unsigned int *index,
        /* [retval][out] */ __RPC__out boolean *found);

    HRESULT ( STDMETHODCALLTYPE *SetAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * item);

    HRESULT ( STDMETHODCALLTYPE *InsertAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ unsigned int index,
        /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * item);

    HRESULT ( STDMETHODCALLTYPE *RemoveAt )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [in] */ unsigned int index);
    HRESULT ( STDMETHODCALLTYPE *Append )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This, /* [in] */ __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * item);
    HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);
    HRESULT ( STDMETHODCALLTYPE *Clear )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ unsigned int startIndex,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    HRESULT ( STDMETHODCALLTYPE *ReplaceAll )(__RPC__in __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * This,
        /* [in] */ unsigned int count,
        /* [size_is][in] */ __RPC__in_ecount_full(count) __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * *value);

    END_INTERFACE
} __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl;

interface __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode
{
    CONST_VTBL struct __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNodeVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */



#endif // ____FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode_INTERFACE_DEFINED__

#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000






typedef enum __x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPixelFormat __x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPixelFormat;


typedef enum __x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPrimitiveTopology __x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPrimitiveTopology;





typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CCompositionBitmapInterpolationMode __x_ABI_CMicrosoft_CUI_CComposition_CCompositionBitmapInterpolationMode;

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositor_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositor_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CICompositor __x_ABI_CMicrosoft_CUI_CComposition_CICompositor;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositor_FWD_DEFINED__


#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface __x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface_FWD_DEFINED__





#ifndef ____x_ABI_CWindows_CFoundation_CIMemoryBuffer_FWD_DEFINED__
#define ____x_ABI_CWindows_CFoundation_CIMemoryBuffer_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CFoundation_CIMemoryBuffer __x_ABI_CWindows_CFoundation_CIMemoryBuffer;

#endif // ____x_ABI_CWindows_CFoundation_CIMemoryBuffer_FWD_DEFINED__





typedef struct __x_ABI_CWindows_CFoundation_CNumerics_CQuaternion __x_ABI_CWindows_CFoundation_CNumerics_CQuaternion;


typedef struct __x_ABI_CWindows_CFoundation_CNumerics_CVector3 __x_ABI_CWindows_CFoundation_CNumerics_CVector3;


typedef struct __x_ABI_CWindows_CFoundation_CNumerics_CVector4 __x_ABI_CWindows_CFoundation_CNumerics_CVector4;





typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAlphaMode __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAlphaMode;


typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic;


typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneComponentType __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneComponentType;


typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode;

























































/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneAlphaMode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAlphaMode
{
    SceneAlphaMode_Opaque = 0,
    SceneAlphaMode_AlphaTest = 1,
    SceneAlphaMode_Blend = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneAttributeSemantic
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic
{
    SceneAttributeSemantic_Index = 0,
    SceneAttributeSemantic_Vertex = 1,
    SceneAttributeSemantic_Normal = 2,
    SceneAttributeSemantic_TexCoord0 = 3,
    SceneAttributeSemantic_TexCoord1 = 4,
    SceneAttributeSemantic_Color = 5,
    SceneAttributeSemantic_Tangent = 6,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneComponentType
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneComponentType
{
    SceneComponentType_MeshRendererComponent = 0,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.Composition.Scenes.SceneWrappingMode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode
{
    SceneWrappingMode_ClampToEdge = 0,
    SceneWrappingMode_MirroredRepeat = 1,
    SceneWrappingMode_Repeat = 2,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneBoundingBox
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneBoundingBox
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneBoundingBox[] = L"Microsoft.UI.Composition.Scenes.ISceneBoundingBox";
/* [object, uuid("39FB48E0-216A-5608-9186-6BA9F98B5C67"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBoxVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Center )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Extents )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Max )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Min )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Size )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBoxVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBoxVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_get_Center(This,value) \
    ( (This)->lpVtbl->get_Center(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_get_Extents(This,value) \
    ( (This)->lpVtbl->get_Extents(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_get_Max(This,value) \
    ( (This)->lpVtbl->get_Max(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_get_Min(This,value) \
    ( (This)->lpVtbl->get_Min(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_get_Size(This,value) \
    ( (This)->lpVtbl->get_Size(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneComponent[] = L"Microsoft.UI.Composition.Scenes.ISceneComponent";
/* [object, uuid("F73361CB-8027-50E2-98EE-B2E3EA050A54"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ComponentType )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneComponentType * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_get_ComponentType(This,value) \
    ( (This)->lpVtbl->get_ComponentType(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneComponentCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneComponentCollection
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneComponentCollection[] = L"Microsoft.UI.Composition.Scenes.ISceneComponentCollection";
/* [object, uuid("E4B21C71-87E2-5AEB-85BE-884E8302273E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollectionVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollectionVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentCollection_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneComponentFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneComponentFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneComponentFactory";
/* [object, uuid("254088B0-BABF-503D-9A66-0D86AF5F7303"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponentFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterial[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterial";
/* [object, uuid("042142A7-BF6B-57AD-BADC-F581F38EDB48"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterialFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterialFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterialFactory";
/* [object, uuid("25747893-8748-5F60-969F-318FA0B735CA"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterialInput[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterialInput";
/* [object, uuid("446BDADE-719B-5DB4-B699-F226D0062A2E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMaterialInputFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMaterialInputFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneMaterialInputFactory";
/* [object, uuid("B4DABD1D-58C0-5710-928A-BC49B0735694"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInputFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMesh
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMesh
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMesh[] = L"Microsoft.UI.Composition.Scenes.ISceneMesh";
/* [object, uuid("5CF846AA-F53F-555E-A3AD-F5BC52CA32FB"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Bounds )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneBoundingBox * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PrimitiveTopology )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPrimitiveTopology * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PrimitiveTopology )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
        /* [in] */__x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPrimitiveTopology value
        );
    HRESULT ( STDMETHODCALLTYPE *FillMeshAttribute )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAttributeSemantic semantic,
        /* [in] */__x_ABI_CMicrosoft_CGraphics_CDirectX_CDirectXPixelFormat format,
        /* [in] */__x_ABI_CWindows_CFoundation_CIMemoryBuffer * memory
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_get_Bounds(This,value) \
    ( (This)->lpVtbl->get_Bounds(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_get_PrimitiveTopology(This,value) \
    ( (This)->lpVtbl->get_PrimitiveTopology(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_put_PrimitiveTopology(This,value) \
    ( (This)->lpVtbl->put_PrimitiveTopology(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_FillMeshAttribute(This,semantic,format,memory) \
    ( (This)->lpVtbl->FillMeshAttribute(This,semantic,format,memory) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshMaterialAttributeMap
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMeshMaterialAttributeMap
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshMaterialAttributeMap[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshMaterialAttributeMap";
/* [object, uuid("2360C457-EDAE-5660-BEDC-89096582ED70"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMapVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMapVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMapVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshRendererComponent[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponent";
/* [object, uuid("D2BE85A0-70A8-5C62-84D8-8BA55E4C64A9"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Material )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Material )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterial * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Mesh )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Mesh )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UVMappings )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshMaterialAttributeMap * * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_get_Material(This,value) \
    ( (This)->lpVtbl->get_Material(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_put_Material(This,value) \
    ( (This)->lpVtbl->put_Material(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_get_Mesh(This,value) \
    ( (This)->lpVtbl->get_Mesh(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_put_Mesh(This,value) \
    ( (This)->lpVtbl->put_Mesh(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_get_UVMappings(This,value) \
    ( (This)->lpVtbl->get_UVMappings(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponentStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshRendererComponentStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponentStatics";
/* [object, uuid("C54F8C5A-A104-5CFA-89DC-13EDAA6E3D88"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositor * compositor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponent * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_Create(This,compositor,result) \
    ( (This)->lpVtbl->Create(This,compositor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshRendererComponentStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMeshStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMesh
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMeshStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneMeshStatics";
/* [object, uuid("29C52125-964B-5315-80F9-3893713290F5"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositor * compositor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMesh * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_Create(This,compositor,result) \
    ( (This)->lpVtbl->Create(This,compositor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMeshStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMetallicRoughnessMaterial[] = L"Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterial";
/* [object, uuid("0A4AFCF4-7BAE-5702-9B85-8BC849F39987"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BaseColorInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_BaseColorInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BaseColorFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector4 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_BaseColorFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CNumerics_CVector4 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MetallicFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_MetallicFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MetallicRoughnessInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_MetallicRoughnessInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RoughnessFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RoughnessFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * This,
        /* [in] */FLOAT value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_get_BaseColorInput(This,value) \
    ( (This)->lpVtbl->get_BaseColorInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_put_BaseColorInput(This,value) \
    ( (This)->lpVtbl->put_BaseColorInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_get_BaseColorFactor(This,value) \
    ( (This)->lpVtbl->get_BaseColorFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_put_BaseColorFactor(This,value) \
    ( (This)->lpVtbl->put_BaseColorFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_get_MetallicFactor(This,value) \
    ( (This)->lpVtbl->get_MetallicFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_put_MetallicFactor(This,value) \
    ( (This)->lpVtbl->put_MetallicFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_get_MetallicRoughnessInput(This,value) \
    ( (This)->lpVtbl->get_MetallicRoughnessInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_put_MetallicRoughnessInput(This,value) \
    ( (This)->lpVtbl->put_MetallicRoughnessInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_get_RoughnessFactor(This,value) \
    ( (This)->lpVtbl->get_RoughnessFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_put_RoughnessFactor(This,value) \
    ( (This)->lpVtbl->put_RoughnessFactor(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterialStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneMetallicRoughnessMaterialStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterialStatics";
/* [object, uuid("DFFD2043-AB3C-57A0-8E13-6F09725E970F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositor * compositor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterial * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_Create(This,compositor,result) \
    ( (This)->lpVtbl->Create(This,compositor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMetallicRoughnessMaterialStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneModelTransform
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneModelTransform
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneModelTransform[] = L"Microsoft.UI.Composition.Scenes.ISceneModelTransform";
/* [object, uuid("3F05555F-0F67-576E-9D8A-93C1F250C29F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransformVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Orientation )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CQuaternion * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Orientation )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CNumerics_CQuaternion value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationAngle )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RotationAngle )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationAngleInDegrees )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RotationAngleInDegrees )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RotationAxis )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RotationAxis )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Scale )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Scale )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Translation )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Translation )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransformVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransformVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_get_Orientation(This,value) \
    ( (This)->lpVtbl->get_Orientation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_put_Orientation(This,value) \
    ( (This)->lpVtbl->put_Orientation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_get_RotationAngle(This,value) \
    ( (This)->lpVtbl->get_RotationAngle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_put_RotationAngle(This,value) \
    ( (This)->lpVtbl->put_RotationAngle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_get_RotationAngleInDegrees(This,value) \
    ( (This)->lpVtbl->get_RotationAngleInDegrees(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_put_RotationAngleInDegrees(This,value) \
    ( (This)->lpVtbl->put_RotationAngleInDegrees(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_get_RotationAxis(This,value) \
    ( (This)->lpVtbl->get_RotationAxis(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_put_RotationAxis(This,value) \
    ( (This)->lpVtbl->put_RotationAxis(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_get_Scale(This,value) \
    ( (This)->lpVtbl->get_Scale(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_put_Scale(This,value) \
    ( (This)->lpVtbl->put_Scale(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_get_Translation(This,value) \
    ( (This)->lpVtbl->get_Translation(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_put_Translation(This,value) \
    ( (This)->lpVtbl->put_Translation(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneNode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneNode
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneNode[] = L"Microsoft.UI.Composition.Scenes.ISceneNode";
/* [object, uuid("A1BCE140-79C2-59E6-9B68-63B1BAB0E2A6"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Children )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
        /* [retval, out] */__FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneNode * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Components )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
        /* [retval, out] */__FIVector_1_Microsoft__CUI__CComposition__CScenes__CSceneComponent * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Parent )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Transform )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneModelTransform * * value
        );
    HRESULT ( STDMETHODCALLTYPE *FindFirstComponentOfType )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneComponentType value,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneComponent * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_get_Children(This,value) \
    ( (This)->lpVtbl->get_Children(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_get_Components(This,value) \
    ( (This)->lpVtbl->get_Components(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_get_Parent(This,value) \
    ( (This)->lpVtbl->get_Parent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_get_Transform(This,value) \
    ( (This)->lpVtbl->get_Transform(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_FindFirstComponentOfType(This,value,result) \
    ( (This)->lpVtbl->FindFirstComponentOfType(This,value,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneNodeCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneNodeCollection
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneNodeCollection[] = L"Microsoft.UI.Composition.Scenes.ISceneNodeCollection";
/* [object, uuid("F219B68E-5666-5C6C-AA4E-08DB07FD6BCF"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollectionVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollectionVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeCollection_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneNodeStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneNode
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneNodeStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneNodeStatics";
/* [object, uuid("801C4394-4198-5DA1-AC39-6E8A44B5CE57"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositor * compositor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_Create(This,compositor,result) \
    ( (This)->lpVtbl->Create(This,compositor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNodeStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneObject[] = L"Microsoft.UI.Composition.Scenes.ISceneObject";
/* [object, uuid("4333E514-4FC7-521E-8BCA-11C51FBCAF1E"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObject_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneObjectFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneObject
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneObjectFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneObjectFactory";
/* [object, uuid("EE797F7D-77DB-5C4C-B6F5-C1930FAD85C5"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneObjectFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.IScenePbrMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.ScenePbrMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_IScenePbrMaterial[] = L"Microsoft.UI.Composition.Scenes.IScenePbrMaterial";
/* [object, uuid("295D0725-56FE-5954-8057-3F4CA7515B36"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AlphaCutoff )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AlphaCutoff )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AlphaMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAlphaMode * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AlphaMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneAlphaMode value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_EmissiveInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_EmissiveInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_EmissiveFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_EmissiveFactor )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CNumerics_CVector3 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IsDoubleSided )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IsDoubleSided )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NormalInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_NormalInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NormalScale )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_NormalScale )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OcclusionInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_OcclusionInput )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneMaterialInput * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OcclusionStrength )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_OcclusionStrength )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial * This,
        /* [in] */FLOAT value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_AlphaCutoff(This,value) \
    ( (This)->lpVtbl->get_AlphaCutoff(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_AlphaCutoff(This,value) \
    ( (This)->lpVtbl->put_AlphaCutoff(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_AlphaMode(This,value) \
    ( (This)->lpVtbl->get_AlphaMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_AlphaMode(This,value) \
    ( (This)->lpVtbl->put_AlphaMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_EmissiveInput(This,value) \
    ( (This)->lpVtbl->get_EmissiveInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_EmissiveInput(This,value) \
    ( (This)->lpVtbl->put_EmissiveInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_EmissiveFactor(This,value) \
    ( (This)->lpVtbl->get_EmissiveFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_EmissiveFactor(This,value) \
    ( (This)->lpVtbl->put_EmissiveFactor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_IsDoubleSided(This,value) \
    ( (This)->lpVtbl->get_IsDoubleSided(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_IsDoubleSided(This,value) \
    ( (This)->lpVtbl->put_IsDoubleSided(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_NormalInput(This,value) \
    ( (This)->lpVtbl->get_NormalInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_NormalInput(This,value) \
    ( (This)->lpVtbl->put_NormalInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_NormalScale(This,value) \
    ( (This)->lpVtbl->get_NormalScale(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_NormalScale(This,value) \
    ( (This)->lpVtbl->put_NormalScale(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_OcclusionInput(This,value) \
    ( (This)->lpVtbl->get_OcclusionInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_OcclusionInput(This,value) \
    ( (This)->lpVtbl->put_OcclusionInput(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_get_OcclusionStrength(This,value) \
    ( (This)->lpVtbl->get_OcclusionStrength(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_put_OcclusionStrength(This,value) \
    ( (This)->lpVtbl->put_OcclusionStrength(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterial_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.IScenePbrMaterialFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.ScenePbrMaterial
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_IScenePbrMaterialFactory[] = L"Microsoft.UI.Composition.Scenes.IScenePbrMaterialFactory";
/* [object, uuid("9E34D32A-E30C-51F5-84AC-6467950605CA"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CIScenePbrMaterialFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneRendererComponent[] = L"Microsoft.UI.Composition.Scenes.ISceneRendererComponent";
/* [object, uuid("6BAB8030-89C1-5DBC-A48E-1805DDF9CDD1"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponent_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneRendererComponentFactory
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneRendererComponent
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneRendererComponentFactory[] = L"Microsoft.UI.Composition.Scenes.ISceneRendererComponentFactory";
/* [object, uuid("3CCAC1D6-6A0F-582E-BB1A-10EBC1E405CA"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactoryVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneRendererComponentFactory_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneSurfaceMaterialInput[] = L"Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInput";
/* [object, uuid("B9854B4F-286C-50CD-A734-491A251D5FD3"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BitmapInterpolationMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CCompositionBitmapInterpolationMode * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_BitmapInterpolationMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CCompositionBitmapInterpolationMode value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Surface )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Surface )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositionSurface * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_WrappingUMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_WrappingUMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_WrappingVMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_WrappingVMode )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CSceneWrappingMode value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_get_BitmapInterpolationMode(This,value) \
    ( (This)->lpVtbl->get_BitmapInterpolationMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_put_BitmapInterpolationMode(This,value) \
    ( (This)->lpVtbl->put_BitmapInterpolationMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_get_Surface(This,value) \
    ( (This)->lpVtbl->get_Surface(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_put_Surface(This,value) \
    ( (This)->lpVtbl->put_Surface(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_get_WrappingUMode(This,value) \
    ( (This)->lpVtbl->get_WrappingUMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_put_WrappingUMode(This,value) \
    ( (This)->lpVtbl->put_WrappingUMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_get_WrappingVMode(This,value) \
    ( (This)->lpVtbl->get_WrappingVMode(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_put_WrappingVMode(This,value) \
    ( (This)->lpVtbl->put_WrappingVMode(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInputStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneSurfaceMaterialInputStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInputStatics";
/* [object, uuid("8E1BA937-AD60-51BC-8256-CA62C4B2AE92"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositor * compositor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInput * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_Create(This,compositor,result) \
    ( (This)->lpVtbl->Create(This,compositor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneSurfaceMaterialInputStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneVisual
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneVisual
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneVisual[] = L"Microsoft.UI.Composition.Scenes.ISceneVisual";
/* [object, uuid("0144D7AD-6A7D-59CB-A0F9-74A04E85352C"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Root )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Root )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneNode * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_get_Root(This,value) \
    ( (This)->lpVtbl->get_Root(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_put_Root(This,value) \
    ( (This)->lpVtbl->put_Root(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Scenes.ISceneVisualStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Scenes.SceneVisual
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Scenes_ISceneVisualStatics[] = L"Microsoft.UI.Composition.Scenes.ISceneVisualStatics";
/* [object, uuid("7B8DA6D1-5BD8-5095-9264-E5572653EA07"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CICompositor * compositor,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisual * * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_Create(This,compositor,result) \
    ( (This)->lpVtbl->Create(This,compositor,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CScenes_CISceneVisualStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneBoundingBox
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneBoundingBox ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneBoundingBox_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneBoundingBox_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneBoundingBox[] = L"Microsoft.UI.Composition.Scenes.SceneBoundingBox";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneComponent ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponent_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponent_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneComponent[] = L"Microsoft.UI.Composition.Scenes.SceneComponent";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneComponentCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneComponentCollection
 *    Windows.Foundation.Collections.IVector_1_Microsoft.UI.Composition.Scenes.SceneComponent ** Default Interface **
 *    Windows.Foundation.Collections.IIterable_1_Microsoft.UI.Composition.Scenes.SceneComponent
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponentCollection_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneComponentCollection_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneComponentCollection[] = L"Microsoft.UI.Composition.Scenes.SceneComponentCollection";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMaterial ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterial_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterial_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMaterial[] = L"Microsoft.UI.Composition.Scenes.SceneMaterial";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMaterialInput ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterialInput_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMaterialInput_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMaterialInput[] = L"Microsoft.UI.Composition.Scenes.SceneMaterialInput";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMesh
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneMeshStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMesh ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMesh_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMesh_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMesh[] = L"Microsoft.UI.Composition.Scenes.SceneMesh";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMeshMaterialAttributeMap
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMeshMaterialAttributeMap ** Default Interface **
 *    Windows.Foundation.Collections.IMap_2_HSTRING,Microsoft.UI.Composition.Scenes.SceneAttributeSemantic
 *    Windows.Foundation.Collections.IIterable_1___FIKeyValuePair_2_HSTRING_Microsoft__CUI__CComposition__CScenes__CSceneAttributeSemantic
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshMaterialAttributeMap_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshMaterialAttributeMap_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMeshMaterialAttributeMap[] = L"Microsoft.UI.Composition.Scenes.SceneMeshMaterialAttributeMap";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponentStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMeshRendererComponent ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshRendererComponent_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMeshRendererComponent_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMeshRendererComponent[] = L"Microsoft.UI.Composition.Scenes.SceneMeshRendererComponent";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterialStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneMetallicRoughnessMaterial ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMetallicRoughnessMaterial_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneMetallicRoughnessMaterial_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneMetallicRoughnessMaterial[] = L"Microsoft.UI.Composition.Scenes.SceneMetallicRoughnessMaterial";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneModelTransform
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneModelTransform ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneModelTransform_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneModelTransform_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneModelTransform[] = L"Microsoft.UI.Composition.Scenes.SceneModelTransform";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneNode
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneNodeStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneNode ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNode_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNode_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneNode[] = L"Microsoft.UI.Composition.Scenes.SceneNode";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneNodeCollection
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneNodeCollection
 *    Windows.Foundation.Collections.IVector_1_Microsoft.UI.Composition.Scenes.SceneNode ** Default Interface **
 *    Windows.Foundation.Collections.IIterable_1_Microsoft.UI.Composition.Scenes.SceneNode
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNodeCollection_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneNodeCollection_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneNodeCollection[] = L"Microsoft.UI.Composition.Scenes.SceneNodeCollection";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneObject
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneObject ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneObject_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneObject_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneObject[] = L"Microsoft.UI.Composition.Scenes.SceneObject";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.ScenePbrMaterial
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.IScenePbrMaterial ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_ScenePbrMaterial_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_ScenePbrMaterial_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_ScenePbrMaterial[] = L"Microsoft.UI.Composition.Scenes.ScenePbrMaterial";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneRendererComponent
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneRendererComponent ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneRendererComponent_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneRendererComponent_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneRendererComponent[] = L"Microsoft.UI.Composition.Scenes.SceneRendererComponent";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInputStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneSurfaceMaterialInput ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneSurfaceMaterialInput_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneSurfaceMaterialInput_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneSurfaceMaterialInput[] = L"Microsoft.UI.Composition.Scenes.SceneSurfaceMaterialInput";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Scenes.SceneVisual
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.Composition.Scenes.ISceneVisualStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Scenes.ISceneVisual ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneVisual_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Scenes_SceneVisual_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Scenes_SceneVisual[] = L"Microsoft.UI.Composition.Scenes.SceneVisual";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EComposition2EScenes_p_h__

#endif // __Microsoft2EUI2EComposition2EScenes_h__
