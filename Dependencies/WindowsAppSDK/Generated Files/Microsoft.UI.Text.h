/* Header file automatically generated from Microsoft.UI.Text.idl */
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
#ifndef __Microsoft2EUI2EText_h__
#define __Microsoft2EUI2EText_h__
#ifndef __Microsoft2EUI2EText_p_h__
#define __Microsoft2EUI2EText_p_h__


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
#include "Windows.Foundation.h"
#include "Windows.Storage.Streams.h"
#include "Windows.UI.h"
#include "Windows.UI.Text.h"

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface IFontWeights;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights ABI::Microsoft::UI::Text::IFontWeights

#endif // ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface IFontWeightsStatics;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics ABI::Microsoft::UI::Text::IFontWeightsStatics

#endif // ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface ITextCharacterFormat;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat ABI::Microsoft::UI::Text::ITextCharacterFormat

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface ITextConstantsStatics;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics ABI::Microsoft::UI::Text::ITextConstantsStatics

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface ITextDocument;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument ABI::Microsoft::UI::Text::ITextDocument

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface ITextParagraphFormat;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat ABI::Microsoft::UI::Text::ITextParagraphFormat

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextRange_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextRange_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface ITextRange;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CITextRange ABI::Microsoft::UI::Text::ITextRange

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextRange_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                interface ITextSelection;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection ABI::Microsoft::UI::Text::ITextSelection

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_FWD_DEFINED__


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
    namespace Windows {
        namespace UI {
            
            typedef struct Color Color;
            
        } /* UI */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Text {
                
                typedef enum FontStretch : int FontStretch;
                
            } /* Text */
        } /* UI */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Text {
                
                typedef enum FontStyle : int FontStyle;
                
            } /* Text */
        } /* UI */
    } /* Windows */
} /* ABI */

namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Text {
                
                typedef struct FontWeight FontWeight;
                
            } /* Text */
        } /* UI */
    } /* Windows */
} /* ABI */




namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum CaretType : int CaretType;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum FindOptions : unsigned int FindOptions;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum FormatEffect : int FormatEffect;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum HorizontalCharacterAlignment : int HorizontalCharacterAlignment;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum LetterCase : int LetterCase;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum LineSpacingRule : int LineSpacingRule;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum LinkType : int LinkType;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum MarkerAlignment : int MarkerAlignment;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum MarkerStyle : int MarkerStyle;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum MarkerType : int MarkerType;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum ParagraphAlignment : int ParagraphAlignment;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum ParagraphStyle : int ParagraphStyle;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum PointOptions : unsigned int PointOptions;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum RangeGravity : int RangeGravity;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum RichEditMathMode : int RichEditMathMode;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum SelectionOptions : unsigned int SelectionOptions;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum SelectionType : int SelectionType;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum TabAlignment : int TabAlignment;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum TabLeader : int TabLeader;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum TextGetOptions : unsigned int TextGetOptions;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum TextRangeUnit : int TextRangeUnit;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum TextScript : int TextScript;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum TextSetOptions : unsigned int TextSetOptions;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum UnderlineType : int UnderlineType;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                
                typedef enum VerticalCharacterAlignment : int VerticalCharacterAlignment;
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */









namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                class FontWeights;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                class RichEditTextDocument;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                class RichEditTextRange;
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */







/*
 *
 * Struct Microsoft.UI.Text.CaretType
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum CaretType : int
                {
                    CaretType_Normal = 0,
                    CaretType_Null = 1,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.FindOptions
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version, flags] */
                enum FindOptions : unsigned int
                {
                    FindOptions_None = 0,
                    FindOptions_Word = 0x2,
                    FindOptions_Case = 0x4,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(FindOptions)
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.FormatEffect
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum FormatEffect : int
                {
                    FormatEffect_Off = 0,
                    FormatEffect_On = 1,
                    FormatEffect_Toggle = 2,
                    FormatEffect_Undefined = 3,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.HorizontalCharacterAlignment
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum HorizontalCharacterAlignment : int
                {
                    HorizontalCharacterAlignment_Left = 0,
                    HorizontalCharacterAlignment_Right = 1,
                    HorizontalCharacterAlignment_Center = 2,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.LetterCase
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum LetterCase : int
                {
                    LetterCase_Lower = 0,
                    LetterCase_Upper = 1,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.LineSpacingRule
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum LineSpacingRule : int
                {
                    LineSpacingRule_Undefined = 0,
                    LineSpacingRule_Single = 1,
                    LineSpacingRule_OneAndHalf = 2,
                    LineSpacingRule_Double = 3,
                    LineSpacingRule_AtLeast = 4,
                    LineSpacingRule_Exactly = 5,
                    LineSpacingRule_Multiple = 6,
                    LineSpacingRule_Percent = 7,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.LinkType
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum LinkType : int
                {
                    LinkType_Undefined = 0,
                    LinkType_NotALink = 1,
                    LinkType_ClientLink = 2,
                    LinkType_FriendlyLinkName = 3,
                    LinkType_FriendlyLinkAddress = 4,
                    LinkType_AutoLink = 5,
                    LinkType_AutoLinkEmail = 6,
                    LinkType_AutoLinkPhone = 7,
                    LinkType_AutoLinkPath = 8,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.MarkerAlignment
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum MarkerAlignment : int
                {
                    MarkerAlignment_Undefined = 0,
                    MarkerAlignment_Left = 1,
                    MarkerAlignment_Center = 2,
                    MarkerAlignment_Right = 3,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.MarkerStyle
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum MarkerStyle : int
                {
                    MarkerStyle_Undefined = 0,
                    MarkerStyle_Parenthesis = 1,
                    MarkerStyle_Parentheses = 2,
                    MarkerStyle_Period = 3,
                    MarkerStyle_Plain = 4,
                    MarkerStyle_Minus = 5,
                    MarkerStyle_NoNumber = 6,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.MarkerType
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum MarkerType : int
                {
                    MarkerType_Undefined = 0,
                    MarkerType_None = 1,
                    MarkerType_Bullet = 2,
                    MarkerType_Arabic = 3,
                    MarkerType_LowercaseEnglishLetter = 4,
                    MarkerType_UppercaseEnglishLetter = 5,
                    MarkerType_LowercaseRoman = 6,
                    MarkerType_UppercaseRoman = 7,
                    MarkerType_UnicodeSequence = 8,
                    MarkerType_CircledNumber = 9,
                    MarkerType_BlackCircleWingding = 10,
                    MarkerType_WhiteCircleWingding = 11,
                    MarkerType_ArabicWide = 12,
                    MarkerType_SimplifiedChinese = 13,
                    MarkerType_TraditionalChinese = 14,
                    MarkerType_JapanSimplifiedChinese = 15,
                    MarkerType_JapanKorea = 16,
                    MarkerType_ArabicDictionary = 17,
                    MarkerType_ArabicAbjad = 18,
                    MarkerType_Hebrew = 19,
                    MarkerType_ThaiAlphabetic = 20,
                    MarkerType_ThaiNumeric = 21,
                    MarkerType_DevanagariVowel = 22,
                    MarkerType_DevanagariConsonant = 23,
                    MarkerType_DevanagariNumeric = 24,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.ParagraphAlignment
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum ParagraphAlignment : int
                {
                    ParagraphAlignment_Undefined = 0,
                    ParagraphAlignment_Left = 1,
                    ParagraphAlignment_Center = 2,
                    ParagraphAlignment_Right = 3,
                    ParagraphAlignment_Justify = 4,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.ParagraphStyle
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum ParagraphStyle : int
                {
                    ParagraphStyle_Undefined = 0,
                    ParagraphStyle_None = 1,
                    ParagraphStyle_Normal = 2,
                    ParagraphStyle_Heading1 = 3,
                    ParagraphStyle_Heading2 = 4,
                    ParagraphStyle_Heading3 = 5,
                    ParagraphStyle_Heading4 = 6,
                    ParagraphStyle_Heading5 = 7,
                    ParagraphStyle_Heading6 = 8,
                    ParagraphStyle_Heading7 = 9,
                    ParagraphStyle_Heading8 = 10,
                    ParagraphStyle_Heading9 = 11,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.PointOptions
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version, flags] */
                enum PointOptions : unsigned int
                {
                    PointOptions_None = 0,
                    PointOptions_IncludeInset = 0x1,
                    PointOptions_Start = 0x20,
                    PointOptions_ClientCoordinates = 0x100,
                    PointOptions_AllowOffClient = 0x200,
                    PointOptions_Transform = 0x400,
                    PointOptions_NoHorizontalScroll = 0x10000,
                    PointOptions_NoVerticalScroll = 0x40000,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(PointOptions)
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.RangeGravity
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum RangeGravity : int
                {
                    RangeGravity_UIBehavior = 0,
                    RangeGravity_Backward = 1,
                    RangeGravity_Forward = 2,
                    RangeGravity_Inward = 3,
                    RangeGravity_Outward = 4,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.RichEditMathMode
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum RichEditMathMode : int
                {
                    RichEditMathMode_NoMath = 0,
                    RichEditMathMode_MathOnly = 1,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.SelectionOptions
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version, flags] */
                enum SelectionOptions : unsigned int
                {
                    SelectionOptions_StartActive = 0x1,
                    SelectionOptions_AtEndOfLine = 0x2,
                    SelectionOptions_Overtype = 0x4,
                    SelectionOptions_Active = 0x8,
                    SelectionOptions_Replace = 0x10,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(SelectionOptions)
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.SelectionType
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum SelectionType : int
                {
                    SelectionType_None = 0,
                    SelectionType_InsertionPoint = 1,
                    SelectionType_Normal = 2,
                    SelectionType_InlineShape = 7,
                    SelectionType_Shape = 8,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.TabAlignment
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum TabAlignment : int
                {
                    TabAlignment_Left = 0,
                    TabAlignment_Center = 1,
                    TabAlignment_Right = 2,
                    TabAlignment_Decimal = 3,
                    TabAlignment_Bar = 4,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.TabLeader
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum TabLeader : int
                {
                    TabLeader_Spaces = 0,
                    TabLeader_Dots = 1,
                    TabLeader_Dashes = 2,
                    TabLeader_Lines = 3,
                    TabLeader_ThickLines = 4,
                    TabLeader_Equals = 5,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.TextGetOptions
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version, flags] */
                enum TextGetOptions : unsigned int
                {
                    TextGetOptions_None = 0,
                    TextGetOptions_AdjustCrlf = 0x1,
                    TextGetOptions_UseCrlf = 0x2,
                    TextGetOptions_UseObjectText = 0x4,
                    TextGetOptions_AllowFinalEop = 0x8,
                    TextGetOptions_NoHidden = 0x20,
                    TextGetOptions_IncludeNumbering = 0x40,
                    TextGetOptions_FormatRtf = 0x2000,
                    TextGetOptions_UseLf = 0x1000000,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(TextGetOptions)
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.TextRangeUnit
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum TextRangeUnit : int
                {
                    TextRangeUnit_Character = 0,
                    TextRangeUnit_Word = 1,
                    TextRangeUnit_Sentence = 2,
                    TextRangeUnit_Paragraph = 3,
                    TextRangeUnit_Line = 4,
                    TextRangeUnit_Story = 5,
                    TextRangeUnit_Screen = 6,
                    TextRangeUnit_Section = 7,
                    TextRangeUnit_Window = 8,
                    TextRangeUnit_CharacterFormat = 9,
                    TextRangeUnit_ParagraphFormat = 10,
                    TextRangeUnit_Object = 11,
                    TextRangeUnit_HardParagraph = 12,
                    TextRangeUnit_Cluster = 13,
                    TextRangeUnit_Bold = 14,
                    TextRangeUnit_Italic = 15,
                    TextRangeUnit_Underline = 16,
                    TextRangeUnit_Strikethrough = 17,
                    TextRangeUnit_ProtectedText = 18,
                    TextRangeUnit_Link = 19,
                    TextRangeUnit_SmallCaps = 20,
                    TextRangeUnit_AllCaps = 21,
                    TextRangeUnit_Hidden = 22,
                    TextRangeUnit_Outline = 23,
                    TextRangeUnit_Shadow = 24,
                    TextRangeUnit_Imprint = 25,
                    TextRangeUnit_Disabled = 26,
                    TextRangeUnit_Revised = 27,
                    TextRangeUnit_Subscript = 28,
                    TextRangeUnit_Superscript = 29,
                    TextRangeUnit_FontBound = 30,
                    TextRangeUnit_LinkProtected = 31,
                    TextRangeUnit_ContentLink = 32,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.TextScript
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum TextScript : int
                {
                    TextScript_Undefined = 0,
                    TextScript_Ansi = 1,
                    TextScript_EastEurope = 2,
                    TextScript_Cyrillic = 3,
                    TextScript_Greek = 4,
                    TextScript_Turkish = 5,
                    TextScript_Hebrew = 6,
                    TextScript_Arabic = 7,
                    TextScript_Baltic = 8,
                    TextScript_Vietnamese = 9,
                    TextScript_Default = 10,
                    TextScript_Symbol = 11,
                    TextScript_Thai = 12,
                    TextScript_ShiftJis = 13,
                    TextScript_GB2312 = 14,
                    TextScript_Hangul = 15,
                    TextScript_Big5 = 16,
                    TextScript_PC437 = 17,
                    TextScript_Oem = 18,
                    TextScript_Mac = 19,
                    TextScript_Armenian = 20,
                    TextScript_Syriac = 21,
                    TextScript_Thaana = 22,
                    TextScript_Devanagari = 23,
                    TextScript_Bengali = 24,
                    TextScript_Gurmukhi = 25,
                    TextScript_Gujarati = 26,
                    TextScript_Oriya = 27,
                    TextScript_Tamil = 28,
                    TextScript_Telugu = 29,
                    TextScript_Kannada = 30,
                    TextScript_Malayalam = 31,
                    TextScript_Sinhala = 32,
                    TextScript_Lao = 33,
                    TextScript_Tibetan = 34,
                    TextScript_Myanmar = 35,
                    TextScript_Georgian = 36,
                    TextScript_Jamo = 37,
                    TextScript_Ethiopic = 38,
                    TextScript_Cherokee = 39,
                    TextScript_Aboriginal = 40,
                    TextScript_Ogham = 41,
                    TextScript_Runic = 42,
                    TextScript_Khmer = 43,
                    TextScript_Mongolian = 44,
                    TextScript_Braille = 45,
                    TextScript_Yi = 46,
                    TextScript_Limbu = 47,
                    TextScript_TaiLe = 48,
                    TextScript_NewTaiLue = 49,
                    TextScript_SylotiNagri = 50,
                    TextScript_Kharoshthi = 51,
                    TextScript_Kayahli = 52,
                    TextScript_UnicodeSymbol = 53,
                    TextScript_Emoji = 54,
                    TextScript_Glagolitic = 55,
                    TextScript_Lisu = 56,
                    TextScript_Vai = 57,
                    TextScript_NKo = 58,
                    TextScript_Osmanya = 59,
                    TextScript_PhagsPa = 60,
                    TextScript_Gothic = 61,
                    TextScript_Deseret = 62,
                    TextScript_Tifinagh = 63,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.TextSetOptions
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version, flags] */
                enum TextSetOptions : unsigned int
                {
                    TextSetOptions_None = 0,
                    TextSetOptions_UnicodeBidi = 0x1,
                    TextSetOptions_Unlink = 0x8,
                    TextSetOptions_Unhide = 0x10,
                    TextSetOptions_CheckTextLimit = 0x20,
                    TextSetOptions_FormatRtf = 0x2000,
                    TextSetOptions_ApplyRtfDocumentDefaults = 0x4000,
                };
                
                DEFINE_ENUM_FLAG_OPERATORS(TextSetOptions)
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.UnderlineType
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum UnderlineType : int
                {
                    UnderlineType_Undefined = 0,
                    UnderlineType_None = 1,
                    UnderlineType_Single = 2,
                    UnderlineType_Words = 3,
                    UnderlineType_Double = 4,
                    UnderlineType_Dotted = 5,
                    UnderlineType_Dash = 6,
                    UnderlineType_DashDot = 7,
                    UnderlineType_DashDotDot = 8,
                    UnderlineType_Wave = 9,
                    UnderlineType_Thick = 10,
                    UnderlineType_Thin = 11,
                    UnderlineType_DoubleWave = 12,
                    UnderlineType_HeavyWave = 13,
                    UnderlineType_LongDash = 14,
                    UnderlineType_ThickDash = 15,
                    UnderlineType_ThickDashDot = 16,
                    UnderlineType_ThickDashDotDot = 17,
                    UnderlineType_ThickDotted = 18,
                    UnderlineType_ThickLongDash = 19,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Struct Microsoft.UI.Text.VerticalCharacterAlignment
 *
 */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [v1_enum, version] */
                enum VerticalCharacterAlignment : int
                {
                    VerticalCharacterAlignment_Top = 0,
                    VerticalCharacterAlignment_Baseline = 1,
                    VerticalCharacterAlignment_Bottom = 2,
                };
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */


/*
 *
 * Interface Microsoft.UI.Text.IFontWeights
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.FontWeights
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_IFontWeights[] = L"Microsoft.UI.Text.IFontWeights";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("386CD040-5404-5A8D-8BC7-2CA989F5C065"), exclusiveto] */
                MIDL_INTERFACE("386CD040-5404-5A8D-8BC7-2CA989F5C065")
                IFontWeights : public IInspectable
                {
                public:
                    
                };

                MIDL_CONST_ID IID & IID_IFontWeights=_uuidof(IFontWeights);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CIFontWeights;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.IFontWeightsStatics
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.FontWeights
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_IFontWeightsStatics[] = L"Microsoft.UI.Text.IFontWeightsStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("CC390DF6-76B0-5807-8B9D-E949A4E623AE"), exclusiveto] */
                MIDL_INTERFACE("CC390DF6-76B0-5807-8B9D-E949A4E623AE")
                IFontWeightsStatics : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Black(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Bold(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ExtraBlack(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ExtraBold(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ExtraLight(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Light(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Medium(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Normal(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SemiBold(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SemiLight(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Thin(
                        /* [retval, out] */ABI::Windows::UI::Text::FontWeight * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_IFontWeightsStatics=_uuidof(IFontWeightsStatics);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextCharacterFormat
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextCharacterFormat[] = L"Microsoft.UI.Text.ITextCharacterFormat";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("F5710050-98E5-5788-B1E3-32191EEBF94D")] */
                MIDL_INTERFACE("F5710050-98E5-5788-B1E3-32191EEBF94D")
                ITextCharacterFormat : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AllCaps(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AllCaps(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BackgroundColor(
                        /* [retval, out] */ABI::Windows::UI::Color * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_BackgroundColor(
                        /* [in] */ABI::Windows::UI::Color value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Bold(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Bold(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FontStretch(
                        /* [retval, out] */ABI::Windows::UI::Text::FontStretch * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FontStretch(
                        /* [in] */ABI::Windows::UI::Text::FontStretch value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FontStyle(
                        /* [retval, out] */ABI::Windows::UI::Text::FontStyle * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FontStyle(
                        /* [in] */ABI::Windows::UI::Text::FontStyle value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ForegroundColor(
                        /* [retval, out] */ABI::Windows::UI::Color * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ForegroundColor(
                        /* [in] */ABI::Windows::UI::Color value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Hidden(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Hidden(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Italic(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Italic(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Kerning(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Kerning(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LanguageTag(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_LanguageTag(
                        /* [in] */HSTRING value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LinkType(
                        /* [retval, out] */ABI::Microsoft::UI::Text::LinkType * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Name(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Name(
                        /* [in] */HSTRING value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Outline(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Outline(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Position(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Position(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ProtectedText(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ProtectedText(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Size(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Size(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SmallCaps(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_SmallCaps(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Spacing(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Spacing(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Strikethrough(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Strikethrough(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Subscript(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Subscript(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Superscript(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Superscript(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TextScript(
                        /* [retval, out] */ABI::Microsoft::UI::Text::TextScript * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_TextScript(
                        /* [in] */ABI::Microsoft::UI::Text::TextScript value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Underline(
                        /* [retval, out] */ABI::Microsoft::UI::Text::UnderlineType * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Underline(
                        /* [in] */ABI::Microsoft::UI::Text::UnderlineType value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Weight(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Weight(
                        /* [in] */INT32 value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetClone(
                        /* [in] */ABI::Microsoft::UI::Text::ITextCharacterFormat * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetClone(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextCharacterFormat * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE IsEqual(
                        /* [in] */ABI::Microsoft::UI::Text::ITextCharacterFormat * format,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITextCharacterFormat=_uuidof(ITextCharacterFormat);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextConstantsStatics
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.TextConstants
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextConstantsStatics[] = L"Microsoft.UI.Text.ITextConstantsStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("CD353B3C-AF63-5CFB-918C-0F9C8931A161"), exclusiveto] */
                MIDL_INTERFACE("CD353B3C-AF63-5CFB-918C-0F9C8931A161")
                ITextConstantsStatics : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AutoColor(
                        /* [retval, out] */ABI::Windows::UI::Color * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MinUnitCount(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MaxUnitCount(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UndefinedColor(
                        /* [retval, out] */ABI::Windows::UI::Color * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UndefinedFloatValue(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UndefinedInt32Value(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UndefinedFontStretch(
                        /* [retval, out] */ABI::Windows::UI::Text::FontStretch * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UndefinedFontStyle(
                        /* [retval, out] */ABI::Windows::UI::Text::FontStyle * value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITextConstantsStatics=_uuidof(ITextConstantsStatics);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextDocument
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.RichEditTextDocument
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextDocument_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextDocument[] = L"Microsoft.UI.Text.ITextDocument";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("1149D57D-86A6-59DD-88D9-196F27BC5C85"), exclusiveto] */
                MIDL_INTERFACE("1149D57D-86A6-59DD-88D9-196F27BC5C85")
                ITextDocument : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CaretType(
                        /* [retval, out] */ABI::Microsoft::UI::Text::CaretType * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CaretType(
                        /* [in] */ABI::Microsoft::UI::Text::CaretType value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DefaultTabStop(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DefaultTabStop(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Selection(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextSelection * * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_UndoLimit(
                        /* [retval, out] */UINT32 * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_UndoLimit(
                        /* [in] */UINT32 value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CanCopy(
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CanPaste(
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CanRedo(
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CanUndo(
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ApplyDisplayUpdates(
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE BatchDisplayUpdates(
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE BeginUndoGroup(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE EndUndoGroup(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetDefaultCharacterFormat(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextCharacterFormat * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetDefaultParagraphFormat(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextParagraphFormat * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetRange(
                        /* [in] */INT32 startPosition,
                        /* [in] */INT32 endPosition,
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextRange * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetRangeFromPoint(
                        /* [in] */ABI::Windows::Foundation::Point point,
                        /* [in] */ABI::Microsoft::UI::Text::PointOptions options,
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextRange * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetText(
                        /* [in] */ABI::Microsoft::UI::Text::TextGetOptions options,
                        /* [out] */HSTRING * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE LoadFromStream(
                        /* [in] */ABI::Microsoft::UI::Text::TextSetOptions options,
                        /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Redo(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SaveToStream(
                        /* [in] */ABI::Microsoft::UI::Text::TextGetOptions options,
                        /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetDefaultCharacterFormat(
                        /* [in] */ABI::Microsoft::UI::Text::ITextCharacterFormat * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetDefaultParagraphFormat(
                        /* [in] */ABI::Microsoft::UI::Text::ITextParagraphFormat * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetText(
                        /* [in] */ABI::Microsoft::UI::Text::TextSetOptions options,
                        /* [in] */HSTRING value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Undo(void) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AlignmentIncludesTrailingWhitespace(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AlignmentIncludesTrailingWhitespace(
                        /* [in] */::boolean value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IgnoreTrailingCharacterSpacing(
                        /* [retval, out] */::boolean * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_IgnoreTrailingCharacterSpacing(
                        /* [in] */::boolean value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ClearUndoRedoHistory(void) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITextDocument=_uuidof(ITextDocument);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextDocument;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextDocument_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextParagraphFormat
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextParagraphFormat[] = L"Microsoft.UI.Text.ITextParagraphFormat";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("219B6CDF-0D0B-5701-B8A1-6C906B3EBBE1")] */
                MIDL_INTERFACE("219B6CDF-0D0B-5701-B8A1-6C906B3EBBE1")
                ITextParagraphFormat : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Alignment(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ParagraphAlignment * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Alignment(
                        /* [in] */ABI::Microsoft::UI::Text::ParagraphAlignment value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FirstLineIndent(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_KeepTogether(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_KeepTogether(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_KeepWithNext(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_KeepWithNext(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LeftIndent(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LineSpacing(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LineSpacingRule(
                        /* [retval, out] */ABI::Microsoft::UI::Text::LineSpacingRule * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ListAlignment(
                        /* [retval, out] */ABI::Microsoft::UI::Text::MarkerAlignment * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ListAlignment(
                        /* [in] */ABI::Microsoft::UI::Text::MarkerAlignment value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ListLevelIndex(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ListLevelIndex(
                        /* [in] */INT32 value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ListStart(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ListStart(
                        /* [in] */INT32 value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ListStyle(
                        /* [retval, out] */ABI::Microsoft::UI::Text::MarkerStyle * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ListStyle(
                        /* [in] */ABI::Microsoft::UI::Text::MarkerStyle value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ListTab(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ListTab(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ListType(
                        /* [retval, out] */ABI::Microsoft::UI::Text::MarkerType * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ListType(
                        /* [in] */ABI::Microsoft::UI::Text::MarkerType value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NoLineNumber(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_NoLineNumber(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PageBreakBefore(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_PageBreakBefore(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RightIndent(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RightIndent(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RightToLeft(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_RightToLeft(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Style(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ParagraphStyle * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Style(
                        /* [in] */ABI::Microsoft::UI::Text::ParagraphStyle value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SpaceAfter(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_SpaceAfter(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SpaceBefore(
                        /* [retval, out] */FLOAT * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_SpaceBefore(
                        /* [in] */FLOAT value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_WidowControl(
                        /* [retval, out] */ABI::Microsoft::UI::Text::FormatEffect * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_WidowControl(
                        /* [in] */ABI::Microsoft::UI::Text::FormatEffect value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_TabCount(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE AddTab(
                        /* [in] */FLOAT position,
                        /* [in] */ABI::Microsoft::UI::Text::TabAlignment align,
                        /* [in] */ABI::Microsoft::UI::Text::TabLeader leader
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ClearAllTabs(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE DeleteTab(
                        /* [in] */FLOAT position
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetClone(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextParagraphFormat * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetTab(
                        /* [in] */INT32 index,
                        /* [out] */FLOAT * position,
                        /* [out] */ABI::Microsoft::UI::Text::TabAlignment * align,
                        /* [out] */ABI::Microsoft::UI::Text::TabLeader * leader
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE IsEqual(
                        /* [in] */ABI::Microsoft::UI::Text::ITextParagraphFormat * format,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetClone(
                        /* [in] */ABI::Microsoft::UI::Text::ITextParagraphFormat * format
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetIndents(
                        /* [in] */FLOAT start,
                        /* [in] */FLOAT left,
                        /* [in] */FLOAT right
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetLineSpacing(
                        /* [in] */ABI::Microsoft::UI::Text::LineSpacingRule rule,
                        /* [in] */FLOAT spacing
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITextParagraphFormat=_uuidof(ITextParagraphFormat);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextRange
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextRange_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextRange_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextRange[] = L"Microsoft.UI.Text.ITextRange";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("06D4ABCF-0C06-5D12-A743-85537EFD09EA")] */
                MIDL_INTERFACE("06D4ABCF-0C06-5D12-A743-85537EFD09EA")
                ITextRange : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Character(
                        /* [retval, out] */WCHAR * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Character(
                        /* [in] */WCHAR value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CharacterFormat(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextCharacterFormat * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_CharacterFormat(
                        /* [in] */ABI::Microsoft::UI::Text::ITextCharacterFormat * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FormattedText(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextRange * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_FormattedText(
                        /* [in] */ABI::Microsoft::UI::Text::ITextRange * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_EndPosition(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_EndPosition(
                        /* [in] */INT32 value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Gravity(
                        /* [retval, out] */ABI::Microsoft::UI::Text::RangeGravity * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Gravity(
                        /* [in] */ABI::Microsoft::UI::Text::RangeGravity value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Length(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Link(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Link(
                        /* [in] */HSTRING value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ParagraphFormat(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextParagraphFormat * * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ParagraphFormat(
                        /* [in] */ABI::Microsoft::UI::Text::ITextParagraphFormat * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StartPosition(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_StartPosition(
                        /* [in] */INT32 value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_StoryLength(
                        /* [retval, out] */INT32 * value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Text(
                        /* [retval, out] */HSTRING * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Text(
                        /* [in] */HSTRING value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE CanPaste(
                        /* [in] */INT32 format,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ChangeCase(
                        /* [in] */ABI::Microsoft::UI::Text::LetterCase value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Collapse(
                        /* [in] */::boolean value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Copy(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Cut(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Delete(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE EndOf(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Expand(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE FindText(
                        /* [in] */HSTRING value,
                        /* [in] */INT32 scanLength,
                        /* [in] */ABI::Microsoft::UI::Text::FindOptions options,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetCharacterUtf32(
                        /* [out] */UINT32 * value,
                        /* [in] */INT32 offset
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetClone(
                        /* [retval, out] */ABI::Microsoft::UI::Text::ITextRange * * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetIndex(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetPoint(
                        /* [in] */ABI::Microsoft::UI::Text::HorizontalCharacterAlignment horizontalAlign,
                        /* [in] */ABI::Microsoft::UI::Text::VerticalCharacterAlignment verticalAlign,
                        /* [in] */ABI::Microsoft::UI::Text::PointOptions options,
                        /* [out] */ABI::Windows::Foundation::Point * point
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetRect(
                        /* [in] */ABI::Microsoft::UI::Text::PointOptions options,
                        /* [out] */ABI::Windows::Foundation::Rect * rect,
                        /* [out] */INT32 * hit
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetText(
                        /* [in] */ABI::Microsoft::UI::Text::TextGetOptions options,
                        /* [out] */HSTRING * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE GetTextViaStream(
                        /* [in] */ABI::Microsoft::UI::Text::TextGetOptions options,
                        /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE InRange(
                        /* [in] */ABI::Microsoft::UI::Text::ITextRange * range,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE InsertImage(
                        /* [in] */INT32 width,
                        /* [in] */INT32 height,
                        /* [in] */INT32 ascent,
                        /* [in] */ABI::Microsoft::UI::Text::VerticalCharacterAlignment verticalAlign,
                        /* [in] */HSTRING alternateText,
                        /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE InStory(
                        /* [in] */ABI::Microsoft::UI::Text::ITextRange * range,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE IsEqual(
                        /* [in] */ABI::Microsoft::UI::Text::ITextRange * range,
                        /* [retval, out] */::boolean * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Move(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveEnd(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveStart(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE Paste(
                        /* [in] */INT32 format
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE ScrollIntoView(
                        /* [in] */ABI::Microsoft::UI::Text::PointOptions value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MatchSelection(void) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetIndex(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 index,
                        /* [in] */::boolean extend
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetPoint(
                        /* [in] */ABI::Windows::Foundation::Point point,
                        /* [in] */ABI::Microsoft::UI::Text::PointOptions options,
                        /* [in] */::boolean extend
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetRange(
                        /* [in] */INT32 startPosition,
                        /* [in] */INT32 endPosition
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetText(
                        /* [in] */ABI::Microsoft::UI::Text::TextSetOptions options,
                        /* [in] */HSTRING value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE SetTextViaStream(
                        /* [in] */ABI::Microsoft::UI::Text::TextSetOptions options,
                        /* [in] */ABI::Windows::Storage::Streams::IRandomAccessStream * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE StartOf(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITextRange=_uuidof(ITextRange);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextRange;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextRange_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextSelection
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Text.ITextRange
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextSelection_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextSelection[] = L"Microsoft.UI.Text.ITextSelection";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Text {
                /* [object, version, uuid("8F5E6CB1-2B04-589F-BD24-54E5CD8DD399")] */
                MIDL_INTERFACE("8F5E6CB1-2B04-589F-BD24-54E5CD8DD399")
                ITextSelection : public IInspectable
                {
                public:
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Options(
                        /* [retval, out] */ABI::Microsoft::UI::Text::SelectionOptions * value
                        ) = 0;
                    /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Options(
                        /* [in] */ABI::Microsoft::UI::Text::SelectionOptions value
                        ) = 0;
                    /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Type(
                        /* [retval, out] */ABI::Microsoft::UI::Text::SelectionType * value
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE EndKey(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE HomeKey(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveDown(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveLeft(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveRight(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE MoveUp(
                        /* [in] */ABI::Microsoft::UI::Text::TextRangeUnit unit,
                        /* [in] */INT32 count,
                        /* [in] */::boolean extend,
                        /* [retval, out] */INT32 * result
                        ) = 0;
                    virtual HRESULT STDMETHODCALLTYPE TypeText(
                        /* [in] */HSTRING value
                        ) = 0;
                    
                };

                MIDL_CONST_ID IID & IID_ITextSelection=_uuidof(ITextSelection);
                
            } /* Text */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextSelection;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextSelection_INTERFACE_DEFINED__) */


/*
 *
 * Class Microsoft.UI.Text.FontWeights
 *
 * RuntimeClass contains static methods.
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Text.IFontWeights ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_Microsoft_UI_Text_FontWeights_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_FontWeights_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_FontWeights[] = L"Microsoft.UI.Text.FontWeights";
#endif


/*
 *
 * Class Microsoft.UI.Text.RichEditTextDocument
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Text.ITextDocument ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_Microsoft_UI_Text_RichEditTextDocument_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_RichEditTextDocument_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_RichEditTextDocument[] = L"Microsoft.UI.Text.RichEditTextDocument";
#endif


/*
 *
 * Class Microsoft.UI.Text.RichEditTextRange
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Text.ITextRange ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_Microsoft_UI_Text_RichEditTextRange_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_RichEditTextRange_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_RichEditTextRange[] = L"Microsoft.UI.Text.RichEditTextRange";
#endif


/*
 *
 * Class Microsoft.UI.Text.TextConstants
 *
 * RuntimeClass contains static methods.
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#ifndef RUNTIMECLASS_Microsoft_UI_Text_TextConstants_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_TextConstants_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_TextConstants[] = L"Microsoft.UI.Text.TextConstants";
#endif




#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CIFontWeights __x_ABI_CMicrosoft_CUI_CText_CIFontWeights;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CITextDocument __x_ABI_CMicrosoft_CUI_CText_CITextDocument;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextRange_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextRange_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CITextRange __x_ABI_CMicrosoft_CUI_CText_CITextRange;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextRange_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CText_CITextSelection __x_ABI_CMicrosoft_CUI_CText_CITextSelection;

#endif // ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_FWD_DEFINED__


typedef struct __x_ABI_CWindows_CFoundation_CPoint __x_ABI_CWindows_CFoundation_CPoint;


typedef struct __x_ABI_CWindows_CFoundation_CRect __x_ABI_CWindows_CFoundation_CRect;



#ifndef ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream;

#endif // ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream_FWD_DEFINED__






typedef struct __x_ABI_CWindows_CUI_CColor __x_ABI_CWindows_CUI_CColor;




typedef enum __x_ABI_CWindows_CUI_CText_CFontStretch __x_ABI_CWindows_CUI_CText_CFontStretch;


typedef enum __x_ABI_CWindows_CUI_CText_CFontStyle __x_ABI_CWindows_CUI_CText_CFontStyle;


typedef struct __x_ABI_CWindows_CUI_CText_CFontWeight __x_ABI_CWindows_CUI_CText_CFontWeight;





typedef enum __x_ABI_CMicrosoft_CUI_CText_CCaretType __x_ABI_CMicrosoft_CUI_CText_CCaretType;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CFindOptions __x_ABI_CMicrosoft_CUI_CText_CFindOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CFormatEffect __x_ABI_CMicrosoft_CUI_CText_CFormatEffect;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CHorizontalCharacterAlignment __x_ABI_CMicrosoft_CUI_CText_CHorizontalCharacterAlignment;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CLetterCase __x_ABI_CMicrosoft_CUI_CText_CLetterCase;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CLineSpacingRule __x_ABI_CMicrosoft_CUI_CText_CLineSpacingRule;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CLinkType __x_ABI_CMicrosoft_CUI_CText_CLinkType;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CMarkerAlignment __x_ABI_CMicrosoft_CUI_CText_CMarkerAlignment;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CMarkerStyle __x_ABI_CMicrosoft_CUI_CText_CMarkerStyle;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CMarkerType __x_ABI_CMicrosoft_CUI_CText_CMarkerType;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CParagraphAlignment __x_ABI_CMicrosoft_CUI_CText_CParagraphAlignment;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CParagraphStyle __x_ABI_CMicrosoft_CUI_CText_CParagraphStyle;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CPointOptions __x_ABI_CMicrosoft_CUI_CText_CPointOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CRangeGravity __x_ABI_CMicrosoft_CUI_CText_CRangeGravity;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CRichEditMathMode __x_ABI_CMicrosoft_CUI_CText_CRichEditMathMode;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CSelectionOptions __x_ABI_CMicrosoft_CUI_CText_CSelectionOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CSelectionType __x_ABI_CMicrosoft_CUI_CText_CSelectionType;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CTabAlignment __x_ABI_CMicrosoft_CUI_CText_CTabAlignment;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CTabLeader __x_ABI_CMicrosoft_CUI_CText_CTabLeader;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CTextGetOptions __x_ABI_CMicrosoft_CUI_CText_CTextGetOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit __x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CTextScript __x_ABI_CMicrosoft_CUI_CText_CTextScript;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CTextSetOptions __x_ABI_CMicrosoft_CUI_CText_CTextSetOptions;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CUnderlineType __x_ABI_CMicrosoft_CUI_CText_CUnderlineType;


typedef enum __x_ABI_CMicrosoft_CUI_CText_CVerticalCharacterAlignment __x_ABI_CMicrosoft_CUI_CText_CVerticalCharacterAlignment;

















/*
 *
 * Struct Microsoft.UI.Text.CaretType
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CCaretType
{
    CaretType_Normal = 0,
    CaretType_Null = 1,
};


/*
 *
 * Struct Microsoft.UI.Text.FindOptions
 *
 */

/* [v1_enum, version, flags] */
enum __x_ABI_CMicrosoft_CUI_CText_CFindOptions
{
    FindOptions_None = 0,
    FindOptions_Word = 0x2,
    FindOptions_Case = 0x4,
};


/*
 *
 * Struct Microsoft.UI.Text.FormatEffect
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CFormatEffect
{
    FormatEffect_Off = 0,
    FormatEffect_On = 1,
    FormatEffect_Toggle = 2,
    FormatEffect_Undefined = 3,
};


/*
 *
 * Struct Microsoft.UI.Text.HorizontalCharacterAlignment
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CHorizontalCharacterAlignment
{
    HorizontalCharacterAlignment_Left = 0,
    HorizontalCharacterAlignment_Right = 1,
    HorizontalCharacterAlignment_Center = 2,
};


/*
 *
 * Struct Microsoft.UI.Text.LetterCase
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CLetterCase
{
    LetterCase_Lower = 0,
    LetterCase_Upper = 1,
};


/*
 *
 * Struct Microsoft.UI.Text.LineSpacingRule
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CLineSpacingRule
{
    LineSpacingRule_Undefined = 0,
    LineSpacingRule_Single = 1,
    LineSpacingRule_OneAndHalf = 2,
    LineSpacingRule_Double = 3,
    LineSpacingRule_AtLeast = 4,
    LineSpacingRule_Exactly = 5,
    LineSpacingRule_Multiple = 6,
    LineSpacingRule_Percent = 7,
};


/*
 *
 * Struct Microsoft.UI.Text.LinkType
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CLinkType
{
    LinkType_Undefined = 0,
    LinkType_NotALink = 1,
    LinkType_ClientLink = 2,
    LinkType_FriendlyLinkName = 3,
    LinkType_FriendlyLinkAddress = 4,
    LinkType_AutoLink = 5,
    LinkType_AutoLinkEmail = 6,
    LinkType_AutoLinkPhone = 7,
    LinkType_AutoLinkPath = 8,
};


/*
 *
 * Struct Microsoft.UI.Text.MarkerAlignment
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CMarkerAlignment
{
    MarkerAlignment_Undefined = 0,
    MarkerAlignment_Left = 1,
    MarkerAlignment_Center = 2,
    MarkerAlignment_Right = 3,
};


/*
 *
 * Struct Microsoft.UI.Text.MarkerStyle
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CMarkerStyle
{
    MarkerStyle_Undefined = 0,
    MarkerStyle_Parenthesis = 1,
    MarkerStyle_Parentheses = 2,
    MarkerStyle_Period = 3,
    MarkerStyle_Plain = 4,
    MarkerStyle_Minus = 5,
    MarkerStyle_NoNumber = 6,
};


/*
 *
 * Struct Microsoft.UI.Text.MarkerType
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CMarkerType
{
    MarkerType_Undefined = 0,
    MarkerType_None = 1,
    MarkerType_Bullet = 2,
    MarkerType_Arabic = 3,
    MarkerType_LowercaseEnglishLetter = 4,
    MarkerType_UppercaseEnglishLetter = 5,
    MarkerType_LowercaseRoman = 6,
    MarkerType_UppercaseRoman = 7,
    MarkerType_UnicodeSequence = 8,
    MarkerType_CircledNumber = 9,
    MarkerType_BlackCircleWingding = 10,
    MarkerType_WhiteCircleWingding = 11,
    MarkerType_ArabicWide = 12,
    MarkerType_SimplifiedChinese = 13,
    MarkerType_TraditionalChinese = 14,
    MarkerType_JapanSimplifiedChinese = 15,
    MarkerType_JapanKorea = 16,
    MarkerType_ArabicDictionary = 17,
    MarkerType_ArabicAbjad = 18,
    MarkerType_Hebrew = 19,
    MarkerType_ThaiAlphabetic = 20,
    MarkerType_ThaiNumeric = 21,
    MarkerType_DevanagariVowel = 22,
    MarkerType_DevanagariConsonant = 23,
    MarkerType_DevanagariNumeric = 24,
};


/*
 *
 * Struct Microsoft.UI.Text.ParagraphAlignment
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CParagraphAlignment
{
    ParagraphAlignment_Undefined = 0,
    ParagraphAlignment_Left = 1,
    ParagraphAlignment_Center = 2,
    ParagraphAlignment_Right = 3,
    ParagraphAlignment_Justify = 4,
};


/*
 *
 * Struct Microsoft.UI.Text.ParagraphStyle
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CParagraphStyle
{
    ParagraphStyle_Undefined = 0,
    ParagraphStyle_None = 1,
    ParagraphStyle_Normal = 2,
    ParagraphStyle_Heading1 = 3,
    ParagraphStyle_Heading2 = 4,
    ParagraphStyle_Heading3 = 5,
    ParagraphStyle_Heading4 = 6,
    ParagraphStyle_Heading5 = 7,
    ParagraphStyle_Heading6 = 8,
    ParagraphStyle_Heading7 = 9,
    ParagraphStyle_Heading8 = 10,
    ParagraphStyle_Heading9 = 11,
};


/*
 *
 * Struct Microsoft.UI.Text.PointOptions
 *
 */

/* [v1_enum, version, flags] */
enum __x_ABI_CMicrosoft_CUI_CText_CPointOptions
{
    PointOptions_None = 0,
    PointOptions_IncludeInset = 0x1,
    PointOptions_Start = 0x20,
    PointOptions_ClientCoordinates = 0x100,
    PointOptions_AllowOffClient = 0x200,
    PointOptions_Transform = 0x400,
    PointOptions_NoHorizontalScroll = 0x10000,
    PointOptions_NoVerticalScroll = 0x40000,
};


/*
 *
 * Struct Microsoft.UI.Text.RangeGravity
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CRangeGravity
{
    RangeGravity_UIBehavior = 0,
    RangeGravity_Backward = 1,
    RangeGravity_Forward = 2,
    RangeGravity_Inward = 3,
    RangeGravity_Outward = 4,
};


/*
 *
 * Struct Microsoft.UI.Text.RichEditMathMode
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CRichEditMathMode
{
    RichEditMathMode_NoMath = 0,
    RichEditMathMode_MathOnly = 1,
};


/*
 *
 * Struct Microsoft.UI.Text.SelectionOptions
 *
 */

/* [v1_enum, version, flags] */
enum __x_ABI_CMicrosoft_CUI_CText_CSelectionOptions
{
    SelectionOptions_StartActive = 0x1,
    SelectionOptions_AtEndOfLine = 0x2,
    SelectionOptions_Overtype = 0x4,
    SelectionOptions_Active = 0x8,
    SelectionOptions_Replace = 0x10,
};


/*
 *
 * Struct Microsoft.UI.Text.SelectionType
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CSelectionType
{
    SelectionType_None = 0,
    SelectionType_InsertionPoint = 1,
    SelectionType_Normal = 2,
    SelectionType_InlineShape = 7,
    SelectionType_Shape = 8,
};


/*
 *
 * Struct Microsoft.UI.Text.TabAlignment
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CTabAlignment
{
    TabAlignment_Left = 0,
    TabAlignment_Center = 1,
    TabAlignment_Right = 2,
    TabAlignment_Decimal = 3,
    TabAlignment_Bar = 4,
};


/*
 *
 * Struct Microsoft.UI.Text.TabLeader
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CTabLeader
{
    TabLeader_Spaces = 0,
    TabLeader_Dots = 1,
    TabLeader_Dashes = 2,
    TabLeader_Lines = 3,
    TabLeader_ThickLines = 4,
    TabLeader_Equals = 5,
};


/*
 *
 * Struct Microsoft.UI.Text.TextGetOptions
 *
 */

/* [v1_enum, version, flags] */
enum __x_ABI_CMicrosoft_CUI_CText_CTextGetOptions
{
    TextGetOptions_None = 0,
    TextGetOptions_AdjustCrlf = 0x1,
    TextGetOptions_UseCrlf = 0x2,
    TextGetOptions_UseObjectText = 0x4,
    TextGetOptions_AllowFinalEop = 0x8,
    TextGetOptions_NoHidden = 0x20,
    TextGetOptions_IncludeNumbering = 0x40,
    TextGetOptions_FormatRtf = 0x2000,
    TextGetOptions_UseLf = 0x1000000,
};


/*
 *
 * Struct Microsoft.UI.Text.TextRangeUnit
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit
{
    TextRangeUnit_Character = 0,
    TextRangeUnit_Word = 1,
    TextRangeUnit_Sentence = 2,
    TextRangeUnit_Paragraph = 3,
    TextRangeUnit_Line = 4,
    TextRangeUnit_Story = 5,
    TextRangeUnit_Screen = 6,
    TextRangeUnit_Section = 7,
    TextRangeUnit_Window = 8,
    TextRangeUnit_CharacterFormat = 9,
    TextRangeUnit_ParagraphFormat = 10,
    TextRangeUnit_Object = 11,
    TextRangeUnit_HardParagraph = 12,
    TextRangeUnit_Cluster = 13,
    TextRangeUnit_Bold = 14,
    TextRangeUnit_Italic = 15,
    TextRangeUnit_Underline = 16,
    TextRangeUnit_Strikethrough = 17,
    TextRangeUnit_ProtectedText = 18,
    TextRangeUnit_Link = 19,
    TextRangeUnit_SmallCaps = 20,
    TextRangeUnit_AllCaps = 21,
    TextRangeUnit_Hidden = 22,
    TextRangeUnit_Outline = 23,
    TextRangeUnit_Shadow = 24,
    TextRangeUnit_Imprint = 25,
    TextRangeUnit_Disabled = 26,
    TextRangeUnit_Revised = 27,
    TextRangeUnit_Subscript = 28,
    TextRangeUnit_Superscript = 29,
    TextRangeUnit_FontBound = 30,
    TextRangeUnit_LinkProtected = 31,
    TextRangeUnit_ContentLink = 32,
};


/*
 *
 * Struct Microsoft.UI.Text.TextScript
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CTextScript
{
    TextScript_Undefined = 0,
    TextScript_Ansi = 1,
    TextScript_EastEurope = 2,
    TextScript_Cyrillic = 3,
    TextScript_Greek = 4,
    TextScript_Turkish = 5,
    TextScript_Hebrew = 6,
    TextScript_Arabic = 7,
    TextScript_Baltic = 8,
    TextScript_Vietnamese = 9,
    TextScript_Default = 10,
    TextScript_Symbol = 11,
    TextScript_Thai = 12,
    TextScript_ShiftJis = 13,
    TextScript_GB2312 = 14,
    TextScript_Hangul = 15,
    TextScript_Big5 = 16,
    TextScript_PC437 = 17,
    TextScript_Oem = 18,
    TextScript_Mac = 19,
    TextScript_Armenian = 20,
    TextScript_Syriac = 21,
    TextScript_Thaana = 22,
    TextScript_Devanagari = 23,
    TextScript_Bengali = 24,
    TextScript_Gurmukhi = 25,
    TextScript_Gujarati = 26,
    TextScript_Oriya = 27,
    TextScript_Tamil = 28,
    TextScript_Telugu = 29,
    TextScript_Kannada = 30,
    TextScript_Malayalam = 31,
    TextScript_Sinhala = 32,
    TextScript_Lao = 33,
    TextScript_Tibetan = 34,
    TextScript_Myanmar = 35,
    TextScript_Georgian = 36,
    TextScript_Jamo = 37,
    TextScript_Ethiopic = 38,
    TextScript_Cherokee = 39,
    TextScript_Aboriginal = 40,
    TextScript_Ogham = 41,
    TextScript_Runic = 42,
    TextScript_Khmer = 43,
    TextScript_Mongolian = 44,
    TextScript_Braille = 45,
    TextScript_Yi = 46,
    TextScript_Limbu = 47,
    TextScript_TaiLe = 48,
    TextScript_NewTaiLue = 49,
    TextScript_SylotiNagri = 50,
    TextScript_Kharoshthi = 51,
    TextScript_Kayahli = 52,
    TextScript_UnicodeSymbol = 53,
    TextScript_Emoji = 54,
    TextScript_Glagolitic = 55,
    TextScript_Lisu = 56,
    TextScript_Vai = 57,
    TextScript_NKo = 58,
    TextScript_Osmanya = 59,
    TextScript_PhagsPa = 60,
    TextScript_Gothic = 61,
    TextScript_Deseret = 62,
    TextScript_Tifinagh = 63,
};


/*
 *
 * Struct Microsoft.UI.Text.TextSetOptions
 *
 */

/* [v1_enum, version, flags] */
enum __x_ABI_CMicrosoft_CUI_CText_CTextSetOptions
{
    TextSetOptions_None = 0,
    TextSetOptions_UnicodeBidi = 0x1,
    TextSetOptions_Unlink = 0x8,
    TextSetOptions_Unhide = 0x10,
    TextSetOptions_CheckTextLimit = 0x20,
    TextSetOptions_FormatRtf = 0x2000,
    TextSetOptions_ApplyRtfDocumentDefaults = 0x4000,
};


/*
 *
 * Struct Microsoft.UI.Text.UnderlineType
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CUnderlineType
{
    UnderlineType_Undefined = 0,
    UnderlineType_None = 1,
    UnderlineType_Single = 2,
    UnderlineType_Words = 3,
    UnderlineType_Double = 4,
    UnderlineType_Dotted = 5,
    UnderlineType_Dash = 6,
    UnderlineType_DashDot = 7,
    UnderlineType_DashDotDot = 8,
    UnderlineType_Wave = 9,
    UnderlineType_Thick = 10,
    UnderlineType_Thin = 11,
    UnderlineType_DoubleWave = 12,
    UnderlineType_HeavyWave = 13,
    UnderlineType_LongDash = 14,
    UnderlineType_ThickDash = 15,
    UnderlineType_ThickDashDot = 16,
    UnderlineType_ThickDashDotDot = 17,
    UnderlineType_ThickDotted = 18,
    UnderlineType_ThickLongDash = 19,
};


/*
 *
 * Struct Microsoft.UI.Text.VerticalCharacterAlignment
 *
 */

/* [v1_enum, version] */
enum __x_ABI_CMicrosoft_CUI_CText_CVerticalCharacterAlignment
{
    VerticalCharacterAlignment_Top = 0,
    VerticalCharacterAlignment_Baseline = 1,
    VerticalCharacterAlignment_Bottom = 2,
};


/*
 *
 * Interface Microsoft.UI.Text.IFontWeights
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.FontWeights
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_IFontWeights[] = L"Microsoft.UI.Text.IFontWeights";
/* [object, version, uuid("386CD040-5404-5A8D-8BC7-2CA989F5C065"), exclusiveto] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeights * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeights * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeights * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeights * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeights * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeights * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CIFontWeights
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeights_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CIFontWeights;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeights_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.IFontWeightsStatics
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.FontWeights
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_IFontWeightsStatics[] = L"Microsoft.UI.Text.IFontWeightsStatics";
/* [object, version, uuid("CC390DF6-76B0-5807-8B9D-E949A4E623AE"), exclusiveto] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Black )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Bold )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ExtraBlack )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ExtraBold )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ExtraLight )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Light )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Medium )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Normal )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SemiBold )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SemiLight )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Thin )(
        __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontWeight * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_Black(This,value) \
    ( (This)->lpVtbl->get_Black(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_Bold(This,value) \
    ( (This)->lpVtbl->get_Bold(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_ExtraBlack(This,value) \
    ( (This)->lpVtbl->get_ExtraBlack(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_ExtraBold(This,value) \
    ( (This)->lpVtbl->get_ExtraBold(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_ExtraLight(This,value) \
    ( (This)->lpVtbl->get_ExtraLight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_Light(This,value) \
    ( (This)->lpVtbl->get_Light(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_Medium(This,value) \
    ( (This)->lpVtbl->get_Medium(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_Normal(This,value) \
    ( (This)->lpVtbl->get_Normal(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_SemiBold(This,value) \
    ( (This)->lpVtbl->get_SemiBold(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_SemiLight(This,value) \
    ( (This)->lpVtbl->get_SemiLight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_get_Thin(This,value) \
    ( (This)->lpVtbl->get_Thin(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CIFontWeightsStatics_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextCharacterFormat
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextCharacterFormat[] = L"Microsoft.UI.Text.ITextCharacterFormat";
/* [object, version, uuid("F5710050-98E5-5788-B1E3-32191EEBF94D")] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormatVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AllCaps )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AllCaps )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_BackgroundColor )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CWindows_CUI_CColor value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Bold )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Bold )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FontStretch )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontStretch * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FontStretch )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CWindows_CUI_CText_CFontStretch value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FontStyle )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontStyle * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FontStyle )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CWindows_CUI_CText_CFontStyle value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ForegroundColor )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CWindows_CUI_CColor value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Hidden )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Hidden )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Italic )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Italic )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Kerning )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Kerning )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LanguageTag )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_LanguageTag )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LinkType )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CLinkType * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Name )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Name )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Outline )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Outline )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Position )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Position )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ProtectedText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ProtectedText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Size )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Size )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SmallCaps )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_SmallCaps )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Spacing )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Spacing )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Strikethrough )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Strikethrough )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Subscript )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Subscript )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Superscript )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Superscript )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TextScript )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CTextScript * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_TextScript )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextScript value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Underline )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CUnderlineType * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Underline )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CUnderlineType value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Weight )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Weight )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */INT32 value
        );
    HRESULT ( STDMETHODCALLTYPE *SetClone )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetClone )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * * result
        );
    HRESULT ( STDMETHODCALLTYPE *IsEqual )(
        __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * format,
        /* [retval, out] */boolean * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormatVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormatVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_AllCaps(This,value) \
    ( (This)->lpVtbl->get_AllCaps(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_AllCaps(This,value) \
    ( (This)->lpVtbl->put_AllCaps(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_BackgroundColor(This,value) \
    ( (This)->lpVtbl->get_BackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_BackgroundColor(This,value) \
    ( (This)->lpVtbl->put_BackgroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Bold(This,value) \
    ( (This)->lpVtbl->get_Bold(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Bold(This,value) \
    ( (This)->lpVtbl->put_Bold(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_FontStretch(This,value) \
    ( (This)->lpVtbl->get_FontStretch(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_FontStretch(This,value) \
    ( (This)->lpVtbl->put_FontStretch(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_FontStyle(This,value) \
    ( (This)->lpVtbl->get_FontStyle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_FontStyle(This,value) \
    ( (This)->lpVtbl->put_FontStyle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_ForegroundColor(This,value) \
    ( (This)->lpVtbl->get_ForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_ForegroundColor(This,value) \
    ( (This)->lpVtbl->put_ForegroundColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Hidden(This,value) \
    ( (This)->lpVtbl->get_Hidden(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Hidden(This,value) \
    ( (This)->lpVtbl->put_Hidden(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Italic(This,value) \
    ( (This)->lpVtbl->get_Italic(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Italic(This,value) \
    ( (This)->lpVtbl->put_Italic(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Kerning(This,value) \
    ( (This)->lpVtbl->get_Kerning(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Kerning(This,value) \
    ( (This)->lpVtbl->put_Kerning(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_LanguageTag(This,value) \
    ( (This)->lpVtbl->get_LanguageTag(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_LanguageTag(This,value) \
    ( (This)->lpVtbl->put_LanguageTag(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_LinkType(This,value) \
    ( (This)->lpVtbl->get_LinkType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Name(This,value) \
    ( (This)->lpVtbl->get_Name(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Name(This,value) \
    ( (This)->lpVtbl->put_Name(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Outline(This,value) \
    ( (This)->lpVtbl->get_Outline(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Outline(This,value) \
    ( (This)->lpVtbl->put_Outline(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Position(This,value) \
    ( (This)->lpVtbl->get_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Position(This,value) \
    ( (This)->lpVtbl->put_Position(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_ProtectedText(This,value) \
    ( (This)->lpVtbl->get_ProtectedText(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_ProtectedText(This,value) \
    ( (This)->lpVtbl->put_ProtectedText(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Size(This,value) \
    ( (This)->lpVtbl->get_Size(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Size(This,value) \
    ( (This)->lpVtbl->put_Size(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_SmallCaps(This,value) \
    ( (This)->lpVtbl->get_SmallCaps(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_SmallCaps(This,value) \
    ( (This)->lpVtbl->put_SmallCaps(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Spacing(This,value) \
    ( (This)->lpVtbl->get_Spacing(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Spacing(This,value) \
    ( (This)->lpVtbl->put_Spacing(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Strikethrough(This,value) \
    ( (This)->lpVtbl->get_Strikethrough(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Strikethrough(This,value) \
    ( (This)->lpVtbl->put_Strikethrough(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Subscript(This,value) \
    ( (This)->lpVtbl->get_Subscript(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Subscript(This,value) \
    ( (This)->lpVtbl->put_Subscript(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Superscript(This,value) \
    ( (This)->lpVtbl->get_Superscript(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Superscript(This,value) \
    ( (This)->lpVtbl->put_Superscript(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_TextScript(This,value) \
    ( (This)->lpVtbl->get_TextScript(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_TextScript(This,value) \
    ( (This)->lpVtbl->put_TextScript(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Underline(This,value) \
    ( (This)->lpVtbl->get_Underline(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Underline(This,value) \
    ( (This)->lpVtbl->put_Underline(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_get_Weight(This,value) \
    ( (This)->lpVtbl->get_Weight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_put_Weight(This,value) \
    ( (This)->lpVtbl->put_Weight(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_SetClone(This,value) \
    ( (This)->lpVtbl->SetClone(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_GetClone(This,result) \
    ( (This)->lpVtbl->GetClone(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_IsEqual(This,format,result) \
    ( (This)->lpVtbl->IsEqual(This,format,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextConstantsStatics
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.TextConstants
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextConstantsStatics[] = L"Microsoft.UI.Text.ITextConstantsStatics";
/* [object, version, uuid("CD353B3C-AF63-5CFB-918C-0F9C8931A161"), exclusiveto] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AutoColor )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MinUnitCount )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MaxUnitCount )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UndefinedColor )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UndefinedFloatValue )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UndefinedInt32Value )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UndefinedFontStretch )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontStretch * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UndefinedFontStyle )(
        __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CText_CFontStyle * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_AutoColor(This,value) \
    ( (This)->lpVtbl->get_AutoColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_MinUnitCount(This,value) \
    ( (This)->lpVtbl->get_MinUnitCount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_MaxUnitCount(This,value) \
    ( (This)->lpVtbl->get_MaxUnitCount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_UndefinedColor(This,value) \
    ( (This)->lpVtbl->get_UndefinedColor(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_UndefinedFloatValue(This,value) \
    ( (This)->lpVtbl->get_UndefinedFloatValue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_UndefinedInt32Value(This,value) \
    ( (This)->lpVtbl->get_UndefinedInt32Value(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_UndefinedFontStretch(This,value) \
    ( (This)->lpVtbl->get_UndefinedFontStretch(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_get_UndefinedFontStyle(This,value) \
    ( (This)->lpVtbl->get_UndefinedFontStyle(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextConstantsStatics_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextDocument
 *
 * Interface is a part of the implementation of type Microsoft.UI.Text.RichEditTextDocument
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextDocument_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextDocument_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextDocument[] = L"Microsoft.UI.Text.ITextDocument";
/* [object, version, uuid("1149D57D-86A6-59DD-88D9-196F27BC5C85"), exclusiveto] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CITextDocumentVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CaretType )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CCaretType * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CaretType )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CCaretType value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DefaultTabStop )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DefaultTabStop )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Selection )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextSelection * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_UndoLimit )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */UINT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_UndoLimit )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */UINT32 value
        );
    HRESULT ( STDMETHODCALLTYPE *CanCopy )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *CanPaste )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *CanRedo )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *CanUndo )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *ApplyDisplayUpdates )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *BatchDisplayUpdates )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *BeginUndoGroup )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
        );
    HRESULT ( STDMETHODCALLTYPE *EndUndoGroup )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
        );
    HRESULT ( STDMETHODCALLTYPE *GetDefaultCharacterFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetDefaultParagraphFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetRange )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */INT32 startPosition,
        /* [in] */INT32 endPosition,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetRangeFromPoint )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint point,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CPointOptions options,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextGetOptions options,
        /* [out] */HSTRING * value
        );
    HRESULT ( STDMETHODCALLTYPE *LoadFromStream )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextSetOptions options,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * value
        );
    HRESULT ( STDMETHODCALLTYPE *Redo )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
        );
    HRESULT ( STDMETHODCALLTYPE *SaveToStream )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextGetOptions options,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * value
        );
    HRESULT ( STDMETHODCALLTYPE *SetDefaultCharacterFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * value
        );
    HRESULT ( STDMETHODCALLTYPE *SetDefaultParagraphFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * value
        );
    HRESULT ( STDMETHODCALLTYPE *SetText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextSetOptions options,
        /* [in] */HSTRING value
        );
    HRESULT ( STDMETHODCALLTYPE *Undo )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AlignmentIncludesTrailingWhitespace )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AlignmentIncludesTrailingWhitespace )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */boolean value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IgnoreTrailingCharacterSpacing )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [retval, out] */boolean * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_IgnoreTrailingCharacterSpacing )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This,
        /* [in] */boolean value
        );
    HRESULT ( STDMETHODCALLTYPE *ClearUndoRedoHistory )(
        __x_ABI_CMicrosoft_CUI_CText_CITextDocument * This
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CITextDocumentVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CITextDocument
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CITextDocumentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_get_CaretType(This,value) \
    ( (This)->lpVtbl->get_CaretType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_put_CaretType(This,value) \
    ( (This)->lpVtbl->put_CaretType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_get_DefaultTabStop(This,value) \
    ( (This)->lpVtbl->get_DefaultTabStop(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_put_DefaultTabStop(This,value) \
    ( (This)->lpVtbl->put_DefaultTabStop(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_get_Selection(This,value) \
    ( (This)->lpVtbl->get_Selection(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_get_UndoLimit(This,value) \
    ( (This)->lpVtbl->get_UndoLimit(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_put_UndoLimit(This,value) \
    ( (This)->lpVtbl->put_UndoLimit(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_CanCopy(This,result) \
    ( (This)->lpVtbl->CanCopy(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_CanPaste(This,result) \
    ( (This)->lpVtbl->CanPaste(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_CanRedo(This,result) \
    ( (This)->lpVtbl->CanRedo(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_CanUndo(This,result) \
    ( (This)->lpVtbl->CanUndo(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_ApplyDisplayUpdates(This,result) \
    ( (This)->lpVtbl->ApplyDisplayUpdates(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_BatchDisplayUpdates(This,result) \
    ( (This)->lpVtbl->BatchDisplayUpdates(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_BeginUndoGroup(This) \
    ( (This)->lpVtbl->BeginUndoGroup(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_EndUndoGroup(This) \
    ( (This)->lpVtbl->EndUndoGroup(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetDefaultCharacterFormat(This,result) \
    ( (This)->lpVtbl->GetDefaultCharacterFormat(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetDefaultParagraphFormat(This,result) \
    ( (This)->lpVtbl->GetDefaultParagraphFormat(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetRange(This,startPosition,endPosition,result) \
    ( (This)->lpVtbl->GetRange(This,startPosition,endPosition,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetRangeFromPoint(This,point,options,result) \
    ( (This)->lpVtbl->GetRangeFromPoint(This,point,options,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_GetText(This,options,value) \
    ( (This)->lpVtbl->GetText(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_LoadFromStream(This,options,value) \
    ( (This)->lpVtbl->LoadFromStream(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_Redo(This) \
    ( (This)->lpVtbl->Redo(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_SaveToStream(This,options,value) \
    ( (This)->lpVtbl->SaveToStream(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_SetDefaultCharacterFormat(This,value) \
    ( (This)->lpVtbl->SetDefaultCharacterFormat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_SetDefaultParagraphFormat(This,value) \
    ( (This)->lpVtbl->SetDefaultParagraphFormat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_SetText(This,options,value) \
    ( (This)->lpVtbl->SetText(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_Undo(This) \
    ( (This)->lpVtbl->Undo(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_get_AlignmentIncludesTrailingWhitespace(This,value) \
    ( (This)->lpVtbl->get_AlignmentIncludesTrailingWhitespace(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_put_AlignmentIncludesTrailingWhitespace(This,value) \
    ( (This)->lpVtbl->put_AlignmentIncludesTrailingWhitespace(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_get_IgnoreTrailingCharacterSpacing(This,value) \
    ( (This)->lpVtbl->get_IgnoreTrailingCharacterSpacing(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_put_IgnoreTrailingCharacterSpacing(This,value) \
    ( (This)->lpVtbl->put_IgnoreTrailingCharacterSpacing(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextDocument_ClearUndoRedoHistory(This) \
    ( (This)->lpVtbl->ClearUndoRedoHistory(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextDocument;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextDocument_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextParagraphFormat
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextParagraphFormat[] = L"Microsoft.UI.Text.ITextParagraphFormat";
/* [object, version, uuid("219B6CDF-0D0B-5701-B8A1-6C906B3EBBE1")] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormatVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Alignment )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CParagraphAlignment * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Alignment )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CParagraphAlignment value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FirstLineIndent )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_KeepTogether )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_KeepTogether )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_KeepWithNext )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_KeepWithNext )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LeftIndent )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LineSpacing )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LineSpacingRule )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CLineSpacingRule * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ListAlignment )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CMarkerAlignment * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ListAlignment )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CMarkerAlignment value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ListLevelIndex )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ListLevelIndex )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */INT32 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ListStart )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ListStart )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */INT32 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ListStyle )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CMarkerStyle * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ListStyle )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CMarkerStyle value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ListTab )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ListTab )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ListType )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CMarkerType * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ListType )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CMarkerType value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NoLineNumber )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_NoLineNumber )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PageBreakBefore )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_PageBreakBefore )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RightIndent )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RightIndent )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RightToLeft )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_RightToLeft )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Style )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CParagraphStyle * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Style )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CParagraphStyle value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SpaceAfter )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_SpaceAfter )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SpaceBefore )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_SpaceBefore )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_WidowControl )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_WidowControl )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFormatEffect value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_TabCount )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */INT32 * value
        );
    HRESULT ( STDMETHODCALLTYPE *AddTab )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT position,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTabAlignment align,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTabLeader leader
        );
    HRESULT ( STDMETHODCALLTYPE *ClearAllTabs )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This
        );
    HRESULT ( STDMETHODCALLTYPE *DeleteTab )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT position
        );
    HRESULT ( STDMETHODCALLTYPE *GetClone )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetTab )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */INT32 index,
        /* [out] */FLOAT * position,
        /* [out] */__x_ABI_CMicrosoft_CUI_CText_CTabAlignment * align,
        /* [out] */__x_ABI_CMicrosoft_CUI_CText_CTabLeader * leader
        );
    HRESULT ( STDMETHODCALLTYPE *IsEqual )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * format,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *SetClone )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * format
        );
    HRESULT ( STDMETHODCALLTYPE *SetIndents )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */FLOAT start,
        /* [in] */FLOAT left,
        /* [in] */FLOAT right
        );
    HRESULT ( STDMETHODCALLTYPE *SetLineSpacing )(
        __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CLineSpacingRule rule,
        /* [in] */FLOAT spacing
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormatVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormatVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_Alignment(This,value) \
    ( (This)->lpVtbl->get_Alignment(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_Alignment(This,value) \
    ( (This)->lpVtbl->put_Alignment(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_FirstLineIndent(This,value) \
    ( (This)->lpVtbl->get_FirstLineIndent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_KeepTogether(This,value) \
    ( (This)->lpVtbl->get_KeepTogether(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_KeepTogether(This,value) \
    ( (This)->lpVtbl->put_KeepTogether(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_KeepWithNext(This,value) \
    ( (This)->lpVtbl->get_KeepWithNext(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_KeepWithNext(This,value) \
    ( (This)->lpVtbl->put_KeepWithNext(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_LeftIndent(This,value) \
    ( (This)->lpVtbl->get_LeftIndent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_LineSpacing(This,value) \
    ( (This)->lpVtbl->get_LineSpacing(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_LineSpacingRule(This,value) \
    ( (This)->lpVtbl->get_LineSpacingRule(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_ListAlignment(This,value) \
    ( (This)->lpVtbl->get_ListAlignment(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_ListAlignment(This,value) \
    ( (This)->lpVtbl->put_ListAlignment(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_ListLevelIndex(This,value) \
    ( (This)->lpVtbl->get_ListLevelIndex(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_ListLevelIndex(This,value) \
    ( (This)->lpVtbl->put_ListLevelIndex(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_ListStart(This,value) \
    ( (This)->lpVtbl->get_ListStart(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_ListStart(This,value) \
    ( (This)->lpVtbl->put_ListStart(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_ListStyle(This,value) \
    ( (This)->lpVtbl->get_ListStyle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_ListStyle(This,value) \
    ( (This)->lpVtbl->put_ListStyle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_ListTab(This,value) \
    ( (This)->lpVtbl->get_ListTab(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_ListTab(This,value) \
    ( (This)->lpVtbl->put_ListTab(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_ListType(This,value) \
    ( (This)->lpVtbl->get_ListType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_ListType(This,value) \
    ( (This)->lpVtbl->put_ListType(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_NoLineNumber(This,value) \
    ( (This)->lpVtbl->get_NoLineNumber(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_NoLineNumber(This,value) \
    ( (This)->lpVtbl->put_NoLineNumber(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_PageBreakBefore(This,value) \
    ( (This)->lpVtbl->get_PageBreakBefore(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_PageBreakBefore(This,value) \
    ( (This)->lpVtbl->put_PageBreakBefore(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_RightIndent(This,value) \
    ( (This)->lpVtbl->get_RightIndent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_RightIndent(This,value) \
    ( (This)->lpVtbl->put_RightIndent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_RightToLeft(This,value) \
    ( (This)->lpVtbl->get_RightToLeft(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_RightToLeft(This,value) \
    ( (This)->lpVtbl->put_RightToLeft(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_Style(This,value) \
    ( (This)->lpVtbl->get_Style(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_Style(This,value) \
    ( (This)->lpVtbl->put_Style(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_SpaceAfter(This,value) \
    ( (This)->lpVtbl->get_SpaceAfter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_SpaceAfter(This,value) \
    ( (This)->lpVtbl->put_SpaceAfter(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_SpaceBefore(This,value) \
    ( (This)->lpVtbl->get_SpaceBefore(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_SpaceBefore(This,value) \
    ( (This)->lpVtbl->put_SpaceBefore(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_WidowControl(This,value) \
    ( (This)->lpVtbl->get_WidowControl(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_put_WidowControl(This,value) \
    ( (This)->lpVtbl->put_WidowControl(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_get_TabCount(This,value) \
    ( (This)->lpVtbl->get_TabCount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_AddTab(This,position,align,leader) \
    ( (This)->lpVtbl->AddTab(This,position,align,leader) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_ClearAllTabs(This) \
    ( (This)->lpVtbl->ClearAllTabs(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_DeleteTab(This,position) \
    ( (This)->lpVtbl->DeleteTab(This,position) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_GetClone(This,result) \
    ( (This)->lpVtbl->GetClone(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_GetTab(This,index,position,align,leader) \
    ( (This)->lpVtbl->GetTab(This,index,position,align,leader) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_IsEqual(This,format,result) \
    ( (This)->lpVtbl->IsEqual(This,format,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_SetClone(This,format) \
    ( (This)->lpVtbl->SetClone(This,format) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_SetIndents(This,start,left,right) \
    ( (This)->lpVtbl->SetIndents(This,start,left,right) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_SetLineSpacing(This,rule,spacing) \
    ( (This)->lpVtbl->SetLineSpacing(This,rule,spacing) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextRange
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextRange_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextRange_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextRange[] = L"Microsoft.UI.Text.ITextRange";
/* [object, version, uuid("06D4ABCF-0C06-5D12-A743-85537EFD09EA")] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CITextRangeVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextRange * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextRange * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Character )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */WCHAR * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Character )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */WCHAR value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CharacterFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_CharacterFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextCharacterFormat * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FormattedText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_FormattedText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_EndPosition )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_EndPosition )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */INT32 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Gravity )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CRangeGravity * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Gravity )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CRangeGravity value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Length )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Link )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Link )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ParagraphFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ParagraphFormat )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextParagraphFormat * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StartPosition )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */INT32 * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_StartPosition )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */INT32 value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_StoryLength )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */INT32 * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Text )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Text )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */HSTRING value
        );
    HRESULT ( STDMETHODCALLTYPE *CanPaste )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */INT32 format,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *ChangeCase )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CLetterCase value
        );
    HRESULT ( STDMETHODCALLTYPE *Collapse )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */boolean value
        );
    HRESULT ( STDMETHODCALLTYPE *Copy )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This
        );
    HRESULT ( STDMETHODCALLTYPE *Cut )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This
        );
    HRESULT ( STDMETHODCALLTYPE *Delete )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *EndOf )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *Expand )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *FindText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */HSTRING value,
        /* [in] */INT32 scanLength,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CFindOptions options,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetCharacterUtf32 )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [out] */UINT32 * value,
        /* [in] */INT32 offset
        );
    HRESULT ( STDMETHODCALLTYPE *GetClone )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetIndex )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *GetPoint )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CHorizontalCharacterAlignment horizontalAlign,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CVerticalCharacterAlignment verticalAlign,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CPointOptions options,
        /* [out] */__x_ABI_CWindows_CFoundation_CPoint * point
        );
    HRESULT ( STDMETHODCALLTYPE *GetRect )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CPointOptions options,
        /* [out] */__x_ABI_CWindows_CFoundation_CRect * rect,
        /* [out] */INT32 * hit
        );
    HRESULT ( STDMETHODCALLTYPE *GetText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextGetOptions options,
        /* [out] */HSTRING * value
        );
    HRESULT ( STDMETHODCALLTYPE *GetTextViaStream )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextGetOptions options,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * value
        );
    HRESULT ( STDMETHODCALLTYPE *InRange )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * range,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *InsertImage )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */INT32 width,
        /* [in] */INT32 height,
        /* [in] */INT32 ascent,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CVerticalCharacterAlignment verticalAlign,
        /* [in] */HSTRING alternateText,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * value
        );
    HRESULT ( STDMETHODCALLTYPE *InStory )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * range,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *IsEqual )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CITextRange * range,
        /* [retval, out] */boolean * result
        );
    HRESULT ( STDMETHODCALLTYPE *Move )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveEnd )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveStart )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *Paste )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */INT32 format
        );
    HRESULT ( STDMETHODCALLTYPE *ScrollIntoView )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CPointOptions value
        );
    HRESULT ( STDMETHODCALLTYPE *MatchSelection )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This
        );
    HRESULT ( STDMETHODCALLTYPE *SetIndex )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 index,
        /* [in] */boolean extend
        );
    HRESULT ( STDMETHODCALLTYPE *SetPoint )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CWindows_CFoundation_CPoint point,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CPointOptions options,
        /* [in] */boolean extend
        );
    HRESULT ( STDMETHODCALLTYPE *SetRange )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */INT32 startPosition,
        /* [in] */INT32 endPosition
        );
    HRESULT ( STDMETHODCALLTYPE *SetText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextSetOptions options,
        /* [in] */HSTRING value
        );
    HRESULT ( STDMETHODCALLTYPE *SetTextViaStream )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextSetOptions options,
        /* [in] */__x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStream * value
        );
    HRESULT ( STDMETHODCALLTYPE *StartOf )(
        __x_ABI_CMicrosoft_CUI_CText_CITextRange * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CITextRangeVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CITextRange
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CITextRangeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_Character(This,value) \
    ( (This)->lpVtbl->get_Character(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_Character(This,value) \
    ( (This)->lpVtbl->put_Character(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_CharacterFormat(This,value) \
    ( (This)->lpVtbl->get_CharacterFormat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_CharacterFormat(This,value) \
    ( (This)->lpVtbl->put_CharacterFormat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_FormattedText(This,value) \
    ( (This)->lpVtbl->get_FormattedText(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_FormattedText(This,value) \
    ( (This)->lpVtbl->put_FormattedText(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_EndPosition(This,value) \
    ( (This)->lpVtbl->get_EndPosition(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_EndPosition(This,value) \
    ( (This)->lpVtbl->put_EndPosition(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_Gravity(This,value) \
    ( (This)->lpVtbl->get_Gravity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_Gravity(This,value) \
    ( (This)->lpVtbl->put_Gravity(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_Length(This,value) \
    ( (This)->lpVtbl->get_Length(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_Link(This,value) \
    ( (This)->lpVtbl->get_Link(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_Link(This,value) \
    ( (This)->lpVtbl->put_Link(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_ParagraphFormat(This,value) \
    ( (This)->lpVtbl->get_ParagraphFormat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_ParagraphFormat(This,value) \
    ( (This)->lpVtbl->put_ParagraphFormat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_StartPosition(This,value) \
    ( (This)->lpVtbl->get_StartPosition(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_StartPosition(This,value) \
    ( (This)->lpVtbl->put_StartPosition(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_StoryLength(This,value) \
    ( (This)->lpVtbl->get_StoryLength(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_get_Text(This,value) \
    ( (This)->lpVtbl->get_Text(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_put_Text(This,value) \
    ( (This)->lpVtbl->put_Text(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_CanPaste(This,format,result) \
    ( (This)->lpVtbl->CanPaste(This,format,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_ChangeCase(This,value) \
    ( (This)->lpVtbl->ChangeCase(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Collapse(This,value) \
    ( (This)->lpVtbl->Collapse(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Copy(This) \
    ( (This)->lpVtbl->Copy(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Cut(This) \
    ( (This)->lpVtbl->Cut(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Delete(This,unit,count,result) \
    ( (This)->lpVtbl->Delete(This,unit,count,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_EndOf(This,unit,extend,result) \
    ( (This)->lpVtbl->EndOf(This,unit,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Expand(This,unit,result) \
    ( (This)->lpVtbl->Expand(This,unit,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_FindText(This,value,scanLength,options,result) \
    ( (This)->lpVtbl->FindText(This,value,scanLength,options,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetCharacterUtf32(This,value,offset) \
    ( (This)->lpVtbl->GetCharacterUtf32(This,value,offset) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetClone(This,result) \
    ( (This)->lpVtbl->GetClone(This,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetIndex(This,unit,result) \
    ( (This)->lpVtbl->GetIndex(This,unit,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetPoint(This,horizontalAlign,verticalAlign,options,point) \
    ( (This)->lpVtbl->GetPoint(This,horizontalAlign,verticalAlign,options,point) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetRect(This,options,rect,hit) \
    ( (This)->lpVtbl->GetRect(This,options,rect,hit) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetText(This,options,value) \
    ( (This)->lpVtbl->GetText(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_GetTextViaStream(This,options,value) \
    ( (This)->lpVtbl->GetTextViaStream(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_InRange(This,range,result) \
    ( (This)->lpVtbl->InRange(This,range,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_InsertImage(This,width,height,ascent,verticalAlign,alternateText,value) \
    ( (This)->lpVtbl->InsertImage(This,width,height,ascent,verticalAlign,alternateText,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_InStory(This,range,result) \
    ( (This)->lpVtbl->InStory(This,range,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_IsEqual(This,range,result) \
    ( (This)->lpVtbl->IsEqual(This,range,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Move(This,unit,count,result) \
    ( (This)->lpVtbl->Move(This,unit,count,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_MoveEnd(This,unit,count,result) \
    ( (This)->lpVtbl->MoveEnd(This,unit,count,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_MoveStart(This,unit,count,result) \
    ( (This)->lpVtbl->MoveStart(This,unit,count,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_Paste(This,format) \
    ( (This)->lpVtbl->Paste(This,format) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_ScrollIntoView(This,value) \
    ( (This)->lpVtbl->ScrollIntoView(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_MatchSelection(This) \
    ( (This)->lpVtbl->MatchSelection(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_SetIndex(This,unit,index,extend) \
    ( (This)->lpVtbl->SetIndex(This,unit,index,extend) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_SetPoint(This,point,options,extend) \
    ( (This)->lpVtbl->SetPoint(This,point,options,extend) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_SetRange(This,startPosition,endPosition) \
    ( (This)->lpVtbl->SetRange(This,startPosition,endPosition) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_SetText(This,options,value) \
    ( (This)->lpVtbl->SetText(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_SetTextViaStream(This,options,value) \
    ( (This)->lpVtbl->SetTextViaStream(This,options,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextRange_StartOf(This,unit,extend,result) \
    ( (This)->lpVtbl->StartOf(This,unit,extend,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextRange;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextRange_INTERFACE_DEFINED__) */


/*
 *
 * Interface Microsoft.UI.Text.ITextSelection
 *
 * Any object which implements this interface must also implement the following interfaces:
 *     Microsoft.UI.Text.ITextRange
 *
 *
 */
#if !defined(____x_ABI_CMicrosoft_CUI_CText_CITextSelection_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CText_CITextSelection_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Text_ITextSelection[] = L"Microsoft.UI.Text.ITextSelection";
/* [object, version, uuid("8F5E6CB1-2B04-589F-BD24-54E5CD8DD399")] */
typedef struct __x_ABI_CMicrosoft_CUI_CText_CITextSelectionVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Options )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CSelectionOptions * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Options )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CSelectionOptions value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Type )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CText_CSelectionType * value
        );
    HRESULT ( STDMETHODCALLTYPE *EndKey )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *HomeKey )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveDown )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveLeft )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveRight )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *MoveUp )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CText_CTextRangeUnit unit,
        /* [in] */INT32 count,
        /* [in] */boolean extend,
        /* [retval, out] */INT32 * result
        );
    HRESULT ( STDMETHODCALLTYPE *TypeText )(
        __x_ABI_CMicrosoft_CUI_CText_CITextSelection * This,
        /* [in] */HSTRING value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CText_CITextSelectionVtbl;

interface __x_ABI_CMicrosoft_CUI_CText_CITextSelection
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CText_CITextSelectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_get_Options(This,value) \
    ( (This)->lpVtbl->get_Options(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_put_Options(This,value) \
    ( (This)->lpVtbl->put_Options(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_get_Type(This,value) \
    ( (This)->lpVtbl->get_Type(This,value) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_EndKey(This,unit,extend,result) \
    ( (This)->lpVtbl->EndKey(This,unit,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_HomeKey(This,unit,extend,result) \
    ( (This)->lpVtbl->HomeKey(This,unit,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_MoveDown(This,unit,count,extend,result) \
    ( (This)->lpVtbl->MoveDown(This,unit,count,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_MoveLeft(This,unit,count,extend,result) \
    ( (This)->lpVtbl->MoveLeft(This,unit,count,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_MoveRight(This,unit,count,extend,result) \
    ( (This)->lpVtbl->MoveRight(This,unit,count,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_MoveUp(This,unit,count,extend,result) \
    ( (This)->lpVtbl->MoveUp(This,unit,count,extend,result) )

#define __x_ABI_CMicrosoft_CUI_CText_CITextSelection_TypeText(This,value) \
    ( (This)->lpVtbl->TypeText(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CText_CITextSelection;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CText_CITextSelection_INTERFACE_DEFINED__) */


/*
 *
 * Class Microsoft.UI.Text.FontWeights
 *
 * RuntimeClass contains static methods.
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Text.IFontWeights ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_Microsoft_UI_Text_FontWeights_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_FontWeights_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_FontWeights[] = L"Microsoft.UI.Text.FontWeights";
#endif


/*
 *
 * Class Microsoft.UI.Text.RichEditTextDocument
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Text.ITextDocument ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_Microsoft_UI_Text_RichEditTextDocument_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_RichEditTextDocument_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_RichEditTextDocument[] = L"Microsoft.UI.Text.RichEditTextDocument";
#endif


/*
 *
 * Class Microsoft.UI.Text.RichEditTextRange
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Text.ITextRange ** Default Interface **
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_Microsoft_UI_Text_RichEditTextRange_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_RichEditTextRange_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_RichEditTextRange[] = L"Microsoft.UI.Text.RichEditTextRange";
#endif


/*
 *
 * Class Microsoft.UI.Text.TextConstants
 *
 * RuntimeClass contains static methods.
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#ifndef RUNTIMECLASS_Microsoft_UI_Text_TextConstants_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Text_TextConstants_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Text_TextConstants[] = L"Microsoft.UI.Text.TextConstants";
#endif




#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EText_p_h__

#endif // __Microsoft2EUI2EText_h__
