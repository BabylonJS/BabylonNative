/* Header file automatically generated from Microsoft.UI.Composition.Effects.idl */
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
#ifndef __Microsoft2EUI2EComposition2EEffects_h__
#define __Microsoft2EUI2EComposition2EEffects_h__
#ifndef __Microsoft2EUI2EComposition2EEffects_p_h__
#define __Microsoft2EUI2EComposition2EEffects_p_h__


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
#include "Windows.Graphics.Effects.h"

#if defined(__cplusplus) && !defined(CINTERFACE)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Effects {
                    interface ISceneLightingEffect;
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect ABI::Microsoft::UI::Composition::Effects::ISceneLightingEffect

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_FWD_DEFINED__
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Effects {
                    interface ISceneLightingEffect2;
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 ABI::Microsoft::UI::Composition::Effects::ISceneLightingEffect2

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_FWD_DEFINED__





#ifndef ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Graphics {
            namespace Effects {
                interface IGraphicsEffect;
            } /* Effects */
        } /* Graphics */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect ABI::Windows::Graphics::Effects::IGraphicsEffect

#endif // ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource_FWD_DEFINED__
namespace ABI {
    namespace Windows {
        namespace Graphics {
            namespace Effects {
                interface IGraphicsEffectSource;
            } /* Effects */
        } /* Graphics */
    } /* Windows */
} /* ABI */
#define __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource ABI::Windows::Graphics::Effects::IGraphicsEffectSource

#endif // ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource_FWD_DEFINED__





namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Effects {
                    
                    typedef enum SceneLightingEffectReflectanceModel : int SceneLightingEffectReflectanceModel;
                    
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */



namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Effects {
                    class SceneLightingEffect;
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */







/*
 *
 * Struct Microsoft.UI.Composition.Effects.SceneLightingEffectReflectanceModel
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
                namespace Effects {
                    /* [v1_enum, contract] */
                    enum SceneLightingEffectReflectanceModel : int
                    {
                        SceneLightingEffectReflectanceModel_BlinnPhong = 0,
                        SceneLightingEffectReflectanceModel_PhysicallyBasedBlinnPhong = 1,
                    };
                    
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Effects.ISceneLightingEffect
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Effects.SceneLightingEffect
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Effects_ISceneLightingEffect[] = L"Microsoft.UI.Composition.Effects.ISceneLightingEffect";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Effects {
                    /* [object, uuid("EB1E7316-114C-5950-8480-20A29A3BB1EE"), exclusiveto, contract] */
                    MIDL_INTERFACE("EB1E7316-114C-5950-8480-20A29A3BB1EE")
                    ISceneLightingEffect : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_AmbientAmount(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_AmbientAmount(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_DiffuseAmount(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_DiffuseAmount(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_NormalMapSource(
                            /* [retval, out] */ABI::Windows::Graphics::Effects::IGraphicsEffectSource * * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_NormalMapSource(
                            /* [in] */ABI::Windows::Graphics::Effects::IGraphicsEffectSource * value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SpecularAmount(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_SpecularAmount(
                            /* [in] */FLOAT value
                            ) = 0;
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_SpecularShine(
                            /* [retval, out] */FLOAT * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_SpecularShine(
                            /* [in] */FLOAT value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneLightingEffect=_uuidof(ISceneLightingEffect);
                    
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Effects.ISceneLightingEffect2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Effects.SceneLightingEffect
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Effects_ISceneLightingEffect2[] = L"Microsoft.UI.Composition.Effects.ISceneLightingEffect2";
namespace ABI {
    namespace Microsoft {
        namespace UI {
            namespace Composition {
                namespace Effects {
                    /* [object, uuid("6B6496B2-468D-50D1-BBE9-593B8263AD80"), exclusiveto, contract] */
                    MIDL_INTERFACE("6B6496B2-468D-50D1-BBE9-593B8263AD80")
                    ISceneLightingEffect2 : public IInspectable
                    {
                    public:
                        /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_ReflectanceModel(
                            /* [retval, out] */ABI::Microsoft::UI::Composition::Effects::SceneLightingEffectReflectanceModel * value
                            ) = 0;
                        /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_ReflectanceModel(
                            /* [in] */ABI::Microsoft::UI::Composition::Effects::SceneLightingEffectReflectanceModel value
                            ) = 0;
                        
                    };

                    MIDL_CONST_ID IID & IID_ISceneLightingEffect2=_uuidof(ISceneLightingEffect2);
                    
                } /* Effects */
            } /* Composition */
        } /* UI */
    } /* Microsoft */
} /* ABI */

EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Effects.SceneLightingEffect
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Effects.ISceneLightingEffect ** Default Interface **
 *    Microsoft.UI.Composition.Effects.ISceneLightingEffect2
 *    Windows.Graphics.Effects.IGraphicsEffect
 *    Windows.Graphics.Effects.IGraphicsEffectSource
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Effects_SceneLightingEffect_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Effects_SceneLightingEffect_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Effects_SceneLightingEffect[] = L"Microsoft.UI.Composition.Effects.SceneLightingEffect";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000





#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_FWD_DEFINED__

#ifndef ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_FWD_DEFINED__
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_FWD_DEFINED__
typedef interface __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2;

#endif // ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_FWD_DEFINED__




#ifndef ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect;

#endif // ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect_FWD_DEFINED__


#ifndef ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource_FWD_DEFINED__
#define ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource_FWD_DEFINED__
typedef interface __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource;

#endif // ____x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource_FWD_DEFINED__






typedef enum __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CSceneLightingEffectReflectanceModel __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CSceneLightingEffectReflectanceModel;









/*
 *
 * Struct Microsoft.UI.Composition.Effects.SceneLightingEffectReflectanceModel
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 */

#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
/* [v1_enum, contract] */
enum __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CSceneLightingEffectReflectanceModel
{
    SceneLightingEffectReflectanceModel_BlinnPhong = 0,
    SceneLightingEffectReflectanceModel_PhysicallyBasedBlinnPhong = 1,
};
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Effects.ISceneLightingEffect
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Effects.SceneLightingEffect
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Effects_ISceneLightingEffect[] = L"Microsoft.UI.Composition.Effects.ISceneLightingEffect";
/* [object, uuid("EB1E7316-114C-5950-8480-20A29A3BB1EE"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffectVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_AmbientAmount )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_AmbientAmount )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_DiffuseAmount )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_DiffuseAmount )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_NormalMapSource )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [retval, out] */__x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_NormalMapSource )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [in] */__x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffectSource * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SpecularAmount )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_SpecularAmount )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [in] */FLOAT value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_SpecularShine )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [retval, out] */FLOAT * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_SpecularShine )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect * This,
        /* [in] */FLOAT value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffectVtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_get_AmbientAmount(This,value) \
    ( (This)->lpVtbl->get_AmbientAmount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_put_AmbientAmount(This,value) \
    ( (This)->lpVtbl->put_AmbientAmount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_get_DiffuseAmount(This,value) \
    ( (This)->lpVtbl->get_DiffuseAmount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_put_DiffuseAmount(This,value) \
    ( (This)->lpVtbl->put_DiffuseAmount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_get_NormalMapSource(This,value) \
    ( (This)->lpVtbl->get_NormalMapSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_put_NormalMapSource(This,value) \
    ( (This)->lpVtbl->put_NormalMapSource(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_get_SpecularAmount(This,value) \
    ( (This)->lpVtbl->get_SpecularAmount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_put_SpecularAmount(This,value) \
    ( (This)->lpVtbl->put_SpecularAmount(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_get_SpecularShine(This,value) \
    ( (This)->lpVtbl->get_SpecularShine(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_put_SpecularShine(This,value) \
    ( (This)->lpVtbl->put_SpecularShine(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Interface Microsoft.UI.Composition.Effects.ISceneLightingEffect2
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * Interface is a part of the implementation of type Microsoft.UI.Composition.Effects.SceneLightingEffect
 *
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000
#if !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_INTERFACE_DEFINED__)
#define ____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_Microsoft_UI_Composition_Effects_ISceneLightingEffect2[] = L"Microsoft.UI.Composition.Effects.ISceneLightingEffect2";
/* [object, uuid("6B6496B2-468D-50D1-BBE9-593B8263AD80"), exclusiveto, contract] */
typedef struct __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2Vtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_ReflectanceModel )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This,
        /* [retval, out] */__x_ABI_CMicrosoft_CUI_CComposition_CEffects_CSceneLightingEffectReflectanceModel * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_ReflectanceModel )(
        __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2 * This,
        /* [in] */__x_ABI_CMicrosoft_CUI_CComposition_CEffects_CSceneLightingEffectReflectanceModel value
        );
    END_INTERFACE
    
} __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2Vtbl;

interface __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2
{
    CONST_VTBL struct __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_get_ReflectanceModel(This,value) \
    ( (This)->lpVtbl->get_ReflectanceModel(This,value) )

#define __x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_put_ReflectanceModel(This,value) \
    ( (This)->lpVtbl->put_ReflectanceModel(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2;
#endif /* !defined(____x_ABI_CMicrosoft_CUI_CComposition_CEffects_CISceneLightingEffect2_INTERFACE_DEFINED__) */
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000


/*
 *
 * Class Microsoft.UI.Composition.Effects.SceneLightingEffect
 *
 * Introduced to Microsoft.Foundation.WindowsAppSDKContract in version 1.0
 *
 *
 * RuntimeClass can be activated.
 *   Type can be activated via RoActivateInstance starting with version 1.0 of the Microsoft.Foundation.WindowsAppSDKContract API contract
 *
 * Class implements the following interfaces:
 *    Microsoft.UI.Composition.Effects.ISceneLightingEffect ** Default Interface **
 *    Microsoft.UI.Composition.Effects.ISceneLightingEffect2
 *    Windows.Graphics.Effects.IGraphicsEffect
 *    Windows.Graphics.Effects.IGraphicsEffectSource
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */
#if MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000

#ifndef RUNTIMECLASS_Microsoft_UI_Composition_Effects_SceneLightingEffect_DEFINED
#define RUNTIMECLASS_Microsoft_UI_Composition_Effects_SceneLightingEffect_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Microsoft_UI_Composition_Effects_SceneLightingEffect[] = L"Microsoft.UI.Composition.Effects.SceneLightingEffect";
#endif
#endif // MICROSOFT_FOUNDATION_WINDOWSAPPSDKCONTRACT_VERSION >= 0x10000





#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __Microsoft2EUI2EComposition2EEffects_p_h__

#endif // __Microsoft2EUI2EComposition2EEffects_h__
