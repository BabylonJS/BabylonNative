/* Header file automatically generated from Microsoft.UI.idl */
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
#ifndef __Microsoft2EUI_h__
#define __Microsoft2EUI_h__
#ifndef __Microsoft2EUI_p_h__
#define __Microsoft2EUI_p_h__


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

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)
#define WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)
#define WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION 0xe0000
#endif // defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)

#if !defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)
#define WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)
#define WINDOWS_PHONE_PHONECONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)
#define WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)

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
#include "Windows.UI.h"

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CIColorHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColorHelper_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            interface IColorHelper;
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CIColorHelper ABI::Microsoft::UI::IColorHelper

#endif // ____x_ABI_CMicrosoft_CUI_CIColorHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            interface IColorHelperStatics;
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics ABI::Microsoft::UI::IColorHelperStatics

#endif // ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CIColors_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColors_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            interface IColors;
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CIColors ABI::Microsoft::UI::IColors

#endif // ____x_ABI_CMicrosoft_CUI_CIColors_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CIColorsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColorsStatics_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            interface IColorsStatics;
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CIColorsStatics ABI::Microsoft::UI::IColorsStatics

#endif // ____x_ABI_CMicrosoft_CUI_CIColorsStatics_FWD_DEFINED__





namespace ABI {
    namespace Windows {
        namespace UI {
            
            typedef struct Color Color;
            
        } /* UI */
    } /* Windows */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct DisplayId DisplayId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct IconId IconId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

namespace ABI {
    namespace Microsoft {
        namespace UI {
            
            typedef struct WindowId WindowId;
            
        } /* UI */
    } /* Microsoft */
} /* ABI */





namespace ABI {
    namespace Microsoft {
        namespace UI {
            class ColorHelper;
        } /* UI */
    } /* Microsoft */
} /* ABI */


namespace ABI {
    namespace Microsoft {
        namespace UI {
            class Colors;
        } /* UI */
    } /* Microsoft */
} /* ABI */





/*
 *
 * Struct Microsoft.UI.DisplayId
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [contract] */
            struct DisplayId
            {
                UINT64 Value;
            };
            
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.IconId
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [contract] */
            struct IconId
            {
                UINT64 Value;
            };
            
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.WindowId
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [contract] */
            struct WindowId
            {
                UINT64 Value;
            };
            
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColorHelper
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.ColorHelper
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColorHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColorHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColorHelper[] = L"Microsoft.UI.IColorHelper";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [object, uuid("3ADDDCCD-3949-585B-A566-CCB8350DD221"), exclusiveto, contract] */
            MIDL_INTERFACE("3ADDDCCD-3949-585B-A566-CCB8350DD221")
            IColorHelper : public IInspectable
            {
            public:
                
            };

            MIDL_CONST_ID IID & IID_IColorHelper=_uuidof(IColorHelper);
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColorHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColorHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColorHelperStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.ColorHelper
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColorHelperStatics[] = L"Microsoft.UI.IColorHelperStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [object, uuid("1D1D85A1-EB63-538A-84F0-019210BC406B"), exclusiveto, contract] */
            MIDL_INTERFACE("1D1D85A1-EB63-538A-84F0-019210BC406B")
            IColorHelperStatics : public IInspectable
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE FromArgb(
                    /* [in] */BYTE a,
                    /* [in] */BYTE r,
                    /* [in] */BYTE g,
                    /* [in] */BYTE b,
                    /* [retval, out] */ABI::Windows::UI::Color * result
                    ) = 0;
                
            };

            MIDL_CONST_ID IID & IID_IColorHelperStatics=_uuidof(IColorHelperStatics);
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColorHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColors
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Colors
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColors_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColors_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColors[] = L"Microsoft.UI.IColors";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [object, uuid("8CF15863-8411-5AFD-946C-328E04DA2F2F"), exclusiveto, contract] */
            MIDL_INTERFACE("8CF15863-8411-5AFD-946C-328E04DA2F2F")
            IColors : public IInspectable
            {
            public:
                
            };

            MIDL_CONST_ID IID & IID_IColors=_uuidof(IColors);
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColors;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColors_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColorsStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Colors
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColorsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColorsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColorsStatics[] = L"Microsoft.UI.IColorsStatics";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            /* [object, uuid("8620A5B0-015A-57AC-A3F3-895D0B1269AE"), exclusiveto, contract] */
            MIDL_INTERFACE("8620A5B0-015A-57AC-A3F3-895D0B1269AE")
            IColorsStatics : public IInspectable
            {
            public:
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AliceBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AntiqueWhite(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Aqua(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Aquamarine(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Azure(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Beige(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Bisque(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Black(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BlanchedAlmond(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Blue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BlueViolet(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Brown(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_BurlyWood(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CadetBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Chartreuse(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Chocolate(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Coral(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_CornflowerBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cornsilk(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Crimson(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Cyan(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkCyan(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkGoldenrod(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkGray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkKhaki(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkMagenta(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkOliveGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkOrange(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkOrchid(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkRed(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkSalmon(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkSeaGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkSlateBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkSlateGray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkTurquoise(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DarkViolet(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DeepPink(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DeepSkyBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DimGray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DodgerBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Firebrick(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_FloralWhite(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ForestGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Fuchsia(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Gainsboro(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_GhostWhite(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Gold(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Goldenrod(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Gray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Green(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_GreenYellow(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Honeydew(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_HotPink(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_IndianRed(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Indigo(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Ivory(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Khaki(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Lavender(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LavenderBlush(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LawnGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LemonChiffon(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightCoral(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightCyan(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightGoldenrodYellow(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightGray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightPink(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightSalmon(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightSeaGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightSkyBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightSlateGray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightSteelBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LightYellow(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Lime(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_LimeGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Linen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Magenta(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Maroon(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumAquamarine(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumOrchid(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumPurple(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumSeaGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumSlateBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumSpringGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumTurquoise(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MediumVioletRed(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MidnightBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MintCream(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MistyRose(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Moccasin(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NavajoWhite(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Navy(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OldLace(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Olive(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OliveDrab(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Orange(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_OrangeRed(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Orchid(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PaleGoldenrod(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PaleGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PaleTurquoise(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PaleVioletRed(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PapayaWhip(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PeachPuff(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Peru(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Pink(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Plum(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_PowderBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Purple(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Red(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RosyBrown(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_RoyalBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SaddleBrown(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Salmon(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SandyBrown(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SeaGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SeaShell(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Sienna(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Silver(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SkyBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SlateBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SlateGray(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Snow(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SpringGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SteelBlue(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Tan(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Teal(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Thistle(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Tomato(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Transparent(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Turquoise(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Violet(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Wheat(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_White(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_WhiteSmoke(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Yellow(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_YellowGreen(
                    /* [retval, out] */ABI::Windows::UI::Color * value
                    ) = 0;
                
            };

            MIDL_CONST_ID IID & IID_IColorsStatics=_uuidof(IColorsStatics);
            
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColorsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColorsStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.ColorHelper
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.IColorHelperStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.IColorHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_ColorHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_ColorHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_ColorHelper[] = L"Microsoft.UI.ColorHelper";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Colors
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.IColorsStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.IColors ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Colors_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Colors_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Colors[] = L"Microsoft.UI.Colors";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CIColorHelper_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColorHelper_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CIColorHelper __x_ABI_CMicrosoft_CUI_CIColorHelper;

#endif // ____x_ABI_CMicrosoft_CUI_CIColorHelper_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CIColorHelperStatics __x_ABI_CMicrosoft_CUI_CIColorHelperStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CIColors_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColors_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CIColors __x_ABI_CMicrosoft_CUI_CIColors;

#endif // ____x_ABI_CMicrosoft_CUI_CIColors_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CIColorsStatics_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CIColorsStatics_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CIColorsStatics __x_ABI_CMicrosoft_CUI_CIColorsStatics;

#endif // ____x_ABI_CMicrosoft_CUI_CIColorsStatics_FWD_DEFINED__





typedef struct __x_ABI_CWindows_CUI_CColor __x_ABI_CWindows_CUI_CColor;




typedef struct __x_ABI_CMicrosoft_CUI_CDisplayId __x_ABI_CMicrosoft_CUI_CDisplayId;


typedef struct __x_ABI_CMicrosoft_CUI_CIconId __x_ABI_CMicrosoft_CUI_CIconId;


typedef struct __x_ABI_CMicrosoft_CUI_CWindowId __x_ABI_CMicrosoft_CUI_CWindowId;










/*
 *
 * Struct Microsoft.UI.DisplayId
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CDisplayId
{
    UINT64 Value;
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.IconId
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CIconId
{
    UINT64 Value;
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Struct Microsoft.UI.WindowId
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

/* [contract] */
struct __x_ABI_CMicrosoft_CUI_CWindowId
{
    UINT64 Value;
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColorHelper
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.ColorHelper
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColorHelper_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColorHelper_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColorHelper[] = L"Microsoft.UI.IColorHelper";
/* [object, uuid("3ADDDCCD-3949-585B-A566-CCB8350DD221"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CIColorHelperVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelper * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelper * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelper * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelper * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelper * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelper * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CIColorHelperVtbl;

interface __x_ABI_CMicrosoft_CUI_CIColorHelper
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CIColorHelperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CIColorHelper_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelper_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelper_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelper_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelper_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelper_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColorHelper;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColorHelper_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColorHelperStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.ColorHelper
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColorHelperStatics[] = L"Microsoft.UI.IColorHelperStatics";
/* [object, uuid("1D1D85A1-EB63-538A-84F0-019210BC406B"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CIColorHelperStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *FromArgb )(
        __x_ABI_CMicrosoft_CUI_CIColorHelperStatics * This,
        /* [in] */BYTE a,
        /* [in] */BYTE r,
        /* [in] */BYTE g,
        /* [in] */BYTE b,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * result
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CIColorHelperStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CIColorHelperStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CIColorHelperStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CIColorHelperStatics_FromArgb(This,a,r,g,b,result) \
    ( (This)->lpVtbl->FromArgb(This,a,r,g,b,result) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColorHelperStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColorHelperStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColors
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Colors
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColors_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColors_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColors[] = L"Microsoft.UI.IColors";
/* [object, uuid("8CF15863-8411-5AFD-946C-328E04DA2F2F"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CIColorsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColors * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColors * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColors * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColors * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColors * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColors * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CIColorsVtbl;

interface __x_ABI_CMicrosoft_CUI_CIColors
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CIColorsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CIColors_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CIColors_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CIColors_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CIColors_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CIColors_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CIColors_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColors;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColors_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.IColorsStatics
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Colors
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CIColorsStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CIColorsStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_IColorsStatics[] = L"Microsoft.UI.IColorsStatics";
/* [object, uuid("8620A5B0-015A-57AC-A3F3-895D0B1269AE"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CIColorsStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorsStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorsStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AliceBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AntiqueWhite )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Aqua )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Aquamarine )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Azure )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Beige )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Bisque )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Black )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BlanchedAlmond )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Blue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BlueViolet )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Brown )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_BurlyWood )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CadetBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Chartreuse )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Chocolate )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Coral )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_CornflowerBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cornsilk )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Crimson )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Cyan )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkCyan )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkGoldenrod )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkGray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkKhaki )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkMagenta )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkOliveGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkOrange )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkOrchid )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkRed )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkSalmon )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkSeaGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkSlateBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkSlateGray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkTurquoise )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DarkViolet )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DeepPink )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DeepSkyBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DimGray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DodgerBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Firebrick )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_FloralWhite )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ForestGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Fuchsia )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Gainsboro )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_GhostWhite )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Gold )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Goldenrod )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Gray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Green )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_GreenYellow )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Honeydew )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_HotPink )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_IndianRed )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Indigo )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Ivory )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Khaki )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Lavender )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LavenderBlush )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LawnGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LemonChiffon )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightCoral )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightCyan )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightGoldenrodYellow )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightGray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightPink )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightSalmon )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightSeaGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightSkyBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightSlateGray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightSteelBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LightYellow )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Lime )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_LimeGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Linen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Magenta )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Maroon )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumAquamarine )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumOrchid )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumPurple )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumSeaGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumSlateBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumSpringGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumTurquoise )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MediumVioletRed )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MidnightBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MintCream )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MistyRose )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Moccasin )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NavajoWhite )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Navy )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OldLace )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Olive )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OliveDrab )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Orange )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_OrangeRed )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Orchid )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PaleGoldenrod )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PaleGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PaleTurquoise )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PaleVioletRed )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PapayaWhip )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PeachPuff )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Peru )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Pink )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Plum )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_PowderBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Purple )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Red )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RosyBrown )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_RoyalBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SaddleBrown )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Salmon )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SandyBrown )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SeaGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SeaShell )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Sienna )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Silver )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SkyBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SlateBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SlateGray )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Snow )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SpringGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SteelBlue )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Tan )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Teal )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Thistle )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Tomato )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Transparent )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Turquoise )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Violet )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Wheat )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_White )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_WhiteSmoke )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Yellow )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_YellowGreen )(
        __x_ABI_CMicrosoft_CUI_CIColorsStatics * This,
        /* [retval, out] */__x_ABI_CWindows_CUI_CColor * value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CIColorsStaticsVtbl;

interface __x_ABI_CMicrosoft_CUI_CIColorsStatics
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CIColorsStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_AliceBlue(This,value) \
    ( (This)->lpVtbl->get_AliceBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_AntiqueWhite(This,value) \
    ( (This)->lpVtbl->get_AntiqueWhite(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Aqua(This,value) \
    ( (This)->lpVtbl->get_Aqua(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Aquamarine(This,value) \
    ( (This)->lpVtbl->get_Aquamarine(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Azure(This,value) \
    ( (This)->lpVtbl->get_Azure(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Beige(This,value) \
    ( (This)->lpVtbl->get_Beige(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Bisque(This,value) \
    ( (This)->lpVtbl->get_Bisque(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Black(This,value) \
    ( (This)->lpVtbl->get_Black(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_BlanchedAlmond(This,value) \
    ( (This)->lpVtbl->get_BlanchedAlmond(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Blue(This,value) \
    ( (This)->lpVtbl->get_Blue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_BlueViolet(This,value) \
    ( (This)->lpVtbl->get_BlueViolet(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Brown(This,value) \
    ( (This)->lpVtbl->get_Brown(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_BurlyWood(This,value) \
    ( (This)->lpVtbl->get_BurlyWood(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_CadetBlue(This,value) \
    ( (This)->lpVtbl->get_CadetBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Chartreuse(This,value) \
    ( (This)->lpVtbl->get_Chartreuse(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Chocolate(This,value) \
    ( (This)->lpVtbl->get_Chocolate(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Coral(This,value) \
    ( (This)->lpVtbl->get_Coral(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_CornflowerBlue(This,value) \
    ( (This)->lpVtbl->get_CornflowerBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Cornsilk(This,value) \
    ( (This)->lpVtbl->get_Cornsilk(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Crimson(This,value) \
    ( (This)->lpVtbl->get_Crimson(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Cyan(This,value) \
    ( (This)->lpVtbl->get_Cyan(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkBlue(This,value) \
    ( (This)->lpVtbl->get_DarkBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkCyan(This,value) \
    ( (This)->lpVtbl->get_DarkCyan(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkGoldenrod(This,value) \
    ( (This)->lpVtbl->get_DarkGoldenrod(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkGray(This,value) \
    ( (This)->lpVtbl->get_DarkGray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkGreen(This,value) \
    ( (This)->lpVtbl->get_DarkGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkKhaki(This,value) \
    ( (This)->lpVtbl->get_DarkKhaki(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkMagenta(This,value) \
    ( (This)->lpVtbl->get_DarkMagenta(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkOliveGreen(This,value) \
    ( (This)->lpVtbl->get_DarkOliveGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkOrange(This,value) \
    ( (This)->lpVtbl->get_DarkOrange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkOrchid(This,value) \
    ( (This)->lpVtbl->get_DarkOrchid(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkRed(This,value) \
    ( (This)->lpVtbl->get_DarkRed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkSalmon(This,value) \
    ( (This)->lpVtbl->get_DarkSalmon(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkSeaGreen(This,value) \
    ( (This)->lpVtbl->get_DarkSeaGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkSlateBlue(This,value) \
    ( (This)->lpVtbl->get_DarkSlateBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkSlateGray(This,value) \
    ( (This)->lpVtbl->get_DarkSlateGray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkTurquoise(This,value) \
    ( (This)->lpVtbl->get_DarkTurquoise(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DarkViolet(This,value) \
    ( (This)->lpVtbl->get_DarkViolet(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DeepPink(This,value) \
    ( (This)->lpVtbl->get_DeepPink(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DeepSkyBlue(This,value) \
    ( (This)->lpVtbl->get_DeepSkyBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DimGray(This,value) \
    ( (This)->lpVtbl->get_DimGray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_DodgerBlue(This,value) \
    ( (This)->lpVtbl->get_DodgerBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Firebrick(This,value) \
    ( (This)->lpVtbl->get_Firebrick(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_FloralWhite(This,value) \
    ( (This)->lpVtbl->get_FloralWhite(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_ForestGreen(This,value) \
    ( (This)->lpVtbl->get_ForestGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Fuchsia(This,value) \
    ( (This)->lpVtbl->get_Fuchsia(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Gainsboro(This,value) \
    ( (This)->lpVtbl->get_Gainsboro(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_GhostWhite(This,value) \
    ( (This)->lpVtbl->get_GhostWhite(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Gold(This,value) \
    ( (This)->lpVtbl->get_Gold(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Goldenrod(This,value) \
    ( (This)->lpVtbl->get_Goldenrod(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Gray(This,value) \
    ( (This)->lpVtbl->get_Gray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Green(This,value) \
    ( (This)->lpVtbl->get_Green(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_GreenYellow(This,value) \
    ( (This)->lpVtbl->get_GreenYellow(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Honeydew(This,value) \
    ( (This)->lpVtbl->get_Honeydew(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_HotPink(This,value) \
    ( (This)->lpVtbl->get_HotPink(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_IndianRed(This,value) \
    ( (This)->lpVtbl->get_IndianRed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Indigo(This,value) \
    ( (This)->lpVtbl->get_Indigo(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Ivory(This,value) \
    ( (This)->lpVtbl->get_Ivory(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Khaki(This,value) \
    ( (This)->lpVtbl->get_Khaki(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Lavender(This,value) \
    ( (This)->lpVtbl->get_Lavender(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LavenderBlush(This,value) \
    ( (This)->lpVtbl->get_LavenderBlush(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LawnGreen(This,value) \
    ( (This)->lpVtbl->get_LawnGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LemonChiffon(This,value) \
    ( (This)->lpVtbl->get_LemonChiffon(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightBlue(This,value) \
    ( (This)->lpVtbl->get_LightBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightCoral(This,value) \
    ( (This)->lpVtbl->get_LightCoral(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightCyan(This,value) \
    ( (This)->lpVtbl->get_LightCyan(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightGoldenrodYellow(This,value) \
    ( (This)->lpVtbl->get_LightGoldenrodYellow(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightGreen(This,value) \
    ( (This)->lpVtbl->get_LightGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightGray(This,value) \
    ( (This)->lpVtbl->get_LightGray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightPink(This,value) \
    ( (This)->lpVtbl->get_LightPink(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightSalmon(This,value) \
    ( (This)->lpVtbl->get_LightSalmon(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightSeaGreen(This,value) \
    ( (This)->lpVtbl->get_LightSeaGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightSkyBlue(This,value) \
    ( (This)->lpVtbl->get_LightSkyBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightSlateGray(This,value) \
    ( (This)->lpVtbl->get_LightSlateGray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightSteelBlue(This,value) \
    ( (This)->lpVtbl->get_LightSteelBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LightYellow(This,value) \
    ( (This)->lpVtbl->get_LightYellow(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Lime(This,value) \
    ( (This)->lpVtbl->get_Lime(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_LimeGreen(This,value) \
    ( (This)->lpVtbl->get_LimeGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Linen(This,value) \
    ( (This)->lpVtbl->get_Linen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Magenta(This,value) \
    ( (This)->lpVtbl->get_Magenta(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Maroon(This,value) \
    ( (This)->lpVtbl->get_Maroon(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumAquamarine(This,value) \
    ( (This)->lpVtbl->get_MediumAquamarine(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumBlue(This,value) \
    ( (This)->lpVtbl->get_MediumBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumOrchid(This,value) \
    ( (This)->lpVtbl->get_MediumOrchid(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumPurple(This,value) \
    ( (This)->lpVtbl->get_MediumPurple(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumSeaGreen(This,value) \
    ( (This)->lpVtbl->get_MediumSeaGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumSlateBlue(This,value) \
    ( (This)->lpVtbl->get_MediumSlateBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumSpringGreen(This,value) \
    ( (This)->lpVtbl->get_MediumSpringGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumTurquoise(This,value) \
    ( (This)->lpVtbl->get_MediumTurquoise(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MediumVioletRed(This,value) \
    ( (This)->lpVtbl->get_MediumVioletRed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MidnightBlue(This,value) \
    ( (This)->lpVtbl->get_MidnightBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MintCream(This,value) \
    ( (This)->lpVtbl->get_MintCream(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_MistyRose(This,value) \
    ( (This)->lpVtbl->get_MistyRose(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Moccasin(This,value) \
    ( (This)->lpVtbl->get_Moccasin(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_NavajoWhite(This,value) \
    ( (This)->lpVtbl->get_NavajoWhite(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Navy(This,value) \
    ( (This)->lpVtbl->get_Navy(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_OldLace(This,value) \
    ( (This)->lpVtbl->get_OldLace(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Olive(This,value) \
    ( (This)->lpVtbl->get_Olive(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_OliveDrab(This,value) \
    ( (This)->lpVtbl->get_OliveDrab(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Orange(This,value) \
    ( (This)->lpVtbl->get_Orange(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_OrangeRed(This,value) \
    ( (This)->lpVtbl->get_OrangeRed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Orchid(This,value) \
    ( (This)->lpVtbl->get_Orchid(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PaleGoldenrod(This,value) \
    ( (This)->lpVtbl->get_PaleGoldenrod(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PaleGreen(This,value) \
    ( (This)->lpVtbl->get_PaleGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PaleTurquoise(This,value) \
    ( (This)->lpVtbl->get_PaleTurquoise(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PaleVioletRed(This,value) \
    ( (This)->lpVtbl->get_PaleVioletRed(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PapayaWhip(This,value) \
    ( (This)->lpVtbl->get_PapayaWhip(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PeachPuff(This,value) \
    ( (This)->lpVtbl->get_PeachPuff(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Peru(This,value) \
    ( (This)->lpVtbl->get_Peru(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Pink(This,value) \
    ( (This)->lpVtbl->get_Pink(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Plum(This,value) \
    ( (This)->lpVtbl->get_Plum(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_PowderBlue(This,value) \
    ( (This)->lpVtbl->get_PowderBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Purple(This,value) \
    ( (This)->lpVtbl->get_Purple(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Red(This,value) \
    ( (This)->lpVtbl->get_Red(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_RosyBrown(This,value) \
    ( (This)->lpVtbl->get_RosyBrown(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_RoyalBlue(This,value) \
    ( (This)->lpVtbl->get_RoyalBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SaddleBrown(This,value) \
    ( (This)->lpVtbl->get_SaddleBrown(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Salmon(This,value) \
    ( (This)->lpVtbl->get_Salmon(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SandyBrown(This,value) \
    ( (This)->lpVtbl->get_SandyBrown(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SeaGreen(This,value) \
    ( (This)->lpVtbl->get_SeaGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SeaShell(This,value) \
    ( (This)->lpVtbl->get_SeaShell(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Sienna(This,value) \
    ( (This)->lpVtbl->get_Sienna(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Silver(This,value) \
    ( (This)->lpVtbl->get_Silver(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SkyBlue(This,value) \
    ( (This)->lpVtbl->get_SkyBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SlateBlue(This,value) \
    ( (This)->lpVtbl->get_SlateBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SlateGray(This,value) \
    ( (This)->lpVtbl->get_SlateGray(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Snow(This,value) \
    ( (This)->lpVtbl->get_Snow(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SpringGreen(This,value) \
    ( (This)->lpVtbl->get_SpringGreen(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_SteelBlue(This,value) \
    ( (This)->lpVtbl->get_SteelBlue(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Tan(This,value) \
    ( (This)->lpVtbl->get_Tan(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Teal(This,value) \
    ( (This)->lpVtbl->get_Teal(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Thistle(This,value) \
    ( (This)->lpVtbl->get_Thistle(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Tomato(This,value) \
    ( (This)->lpVtbl->get_Tomato(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Transparent(This,value) \
    ( (This)->lpVtbl->get_Transparent(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Turquoise(This,value) \
    ( (This)->lpVtbl->get_Turquoise(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Violet(This,value) \
    ( (This)->lpVtbl->get_Violet(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Wheat(This,value) \
    ( (This)->lpVtbl->get_Wheat(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_White(This,value) \
    ( (This)->lpVtbl->get_White(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_WhiteSmoke(This,value) \
    ( (This)->lpVtbl->get_WhiteSmoke(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_Yellow(This,value) \
    ( (This)->lpVtbl->get_Yellow(This,value) )

#define __x_ABI_CMicrosoft_CUI_CIColorsStatics_get_YellowGreen(This,value) \
    ( (This)->lpVtbl->get_YellowGreen(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CIColorsStatics;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CIColorsStatics_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.ColorHelper
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.IColorHelperStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.IColorHelper ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_ColorHelper_DEFINED
#define RUNTIMECLASS_Microsoft_UI_ColorHelper_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_ColorHelper[] = L"Microsoft.UI.ColorHelper";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Colors
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass contains static methods.
 *   Static Methods exist on the Microsoft.UI.IColorsStatics interface starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.IColors ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Colors_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Colors_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Colors[] = L"Microsoft.UI.Colors";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000



#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI_p_h__

#endif // __Microsoft2EUI_h__
