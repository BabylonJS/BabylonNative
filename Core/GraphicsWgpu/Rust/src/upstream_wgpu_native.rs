#[cfg(feature = "upstream_wgpu_native")]
use std::ffi::{c_char, c_void, CStr};
#[cfg(feature = "upstream_wgpu_native")]
use std::ptr;

#[derive(Clone, Debug)]
pub struct AdapterProbeInfo {
    pub backend: u32,
    pub vendor_id: u32,
    pub device_id: u32,
    pub adapter_name: String,
}

#[cfg(feature = "upstream_wgpu_native")]
mod enabled {
    use super::*;
    use std::slice;
    use std::thread;
    use std::time::{Duration, Instant};

    type WGPUInstance = *mut c_void;
    type WGPUAdapter = *mut c_void;
    type WGPUDevice = *mut c_void;

    const WGPU_TRUE: u32 = 1;

    const WGPU_CALLBACK_MODE_WAIT_ANY_ONLY: u32 = 1;

    const WGPU_REQUEST_ADAPTER_STATUS_SUCCESS: u32 = 1;
    const WGPU_REQUEST_DEVICE_STATUS_SUCCESS: u32 = 1;

    const WGPU_WAIT_STATUS_SUCCESS: u32 = 1;
    const WGPU_WAIT_STATUS_TIMED_OUT: u32 = 2;

    const WGPU_STATUS_SUCCESS: u32 = 1;

    const WGPU_FEATURE_LEVEL_CORE: u32 = 2;

    const WGPU_POWER_PREFERENCE_LOW_POWER: u32 = 1;
    const WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE: u32 = 2;

    const WGPU_BACKEND_TYPE_D3D11: u32 = 3;
    const WGPU_BACKEND_TYPE_D3D12: u32 = 4;
    const WGPU_BACKEND_TYPE_METAL: u32 = 5;
    const WGPU_BACKEND_TYPE_VULKAN: u32 = 6;
    const WGPU_BACKEND_TYPE_OPEN_GL: u32 = 7;
    const WGPU_BACKEND_TYPE_OPEN_GLES: u32 = 8;

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUStringView {
        data: *const c_char,
        length: usize,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUFuture {
        id: u64,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPURequestAdapterOptions {
        next_in_chain: *mut c_void,
        feature_level: u32,
        power_preference: u32,
        force_fallback_adapter: u32,
        backend_type: u32,
        compatible_surface: *mut c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPURequestAdapterCallbackInfo {
        next_in_chain: *mut c_void,
        mode: u32,
        callback: Option<WGPURequestAdapterCallback>,
        userdata1: *mut c_void,
        userdata2: *mut c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPURequestDeviceCallbackInfo {
        next_in_chain: *mut c_void,
        mode: u32,
        callback: Option<WGPURequestDeviceCallback>,
        userdata1: *mut c_void,
        userdata2: *mut c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUFutureWaitInfo {
        future: WGPUFuture,
        completed: u32,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUAdapterInfo {
        next_in_chain: *mut c_void,
        vendor: WGPUStringView,
        architecture: WGPUStringView,
        device: WGPUStringView,
        description: WGPUStringView,
        backend_type: u32,
        adapter_type: u32,
        vendor_id: u32,
        device_id: u32,
        subgroup_min_size: u32,
        subgroup_max_size: u32,
    }

    impl Default for WGPUAdapterInfo {
        fn default() -> Self {
            Self {
                next_in_chain: ptr::null_mut(),
                vendor: WGPUStringView {
                    data: ptr::null(),
                    length: usize::MAX,
                },
                architecture: WGPUStringView {
                    data: ptr::null(),
                    length: usize::MAX,
                },
                device: WGPUStringView {
                    data: ptr::null(),
                    length: usize::MAX,
                },
                description: WGPUStringView {
                    data: ptr::null(),
                    length: usize::MAX,
                },
                backend_type: 0,
                adapter_type: 0,
                vendor_id: 0,
                device_id: 0,
                subgroup_min_size: 0,
                subgroup_max_size: 0,
            }
        }
    }

    type WGPURequestAdapterCallback =
        extern "C" fn(u32, WGPUAdapter, WGPUStringView, *mut c_void, *mut c_void);
    type WGPURequestDeviceCallback =
        extern "C" fn(u32, WGPUDevice, WGPUStringView, *mut c_void, *mut c_void);

    unsafe extern "C" {
        fn wgpuGetVersion() -> u32;
        fn wgpuCreateInstance(descriptor: *const c_void) -> WGPUInstance;
        fn wgpuInstanceRequestAdapter(
            instance: WGPUInstance,
            options: *const WGPURequestAdapterOptions,
            callback_info: WGPURequestAdapterCallbackInfo,
        ) -> WGPUFuture;
        fn wgpuAdapterRequestDevice(
            adapter: WGPUAdapter,
            descriptor: *const c_void,
            callback_info: WGPURequestDeviceCallbackInfo,
        ) -> WGPUFuture;
        fn wgpuInstanceWaitAny(
            instance: WGPUInstance,
            future_count: usize,
            futures: *mut WGPUFutureWaitInfo,
            timeout_ns: u64,
        ) -> u32;
        fn wgpuAdapterGetInfo(adapter: WGPUAdapter, info: *mut WGPUAdapterInfo) -> u32;
        fn wgpuAdapterInfoFreeMembers(adapter_info: WGPUAdapterInfo);
        fn wgpuAdapterRelease(adapter: WGPUAdapter);
        fn wgpuDeviceRelease(device: WGPUDevice);
        fn wgpuInstanceRelease(instance: WGPUInstance);
    }

    struct AdapterRequestState {
        status: u32,
        adapter: WGPUAdapter,
        message: String,
        completed: bool,
    }

    impl Default for AdapterRequestState {
        fn default() -> Self {
            Self {
                status: 0,
                adapter: ptr::null_mut(),
                message: String::new(),
                completed: false,
            }
        }
    }

    struct DeviceRequestState {
        status: u32,
        device: WGPUDevice,
        message: String,
        completed: bool,
    }

    impl Default for DeviceRequestState {
        fn default() -> Self {
            Self {
                status: 0,
                device: ptr::null_mut(),
                message: String::new(),
                completed: false,
            }
        }
    }

    fn string_view_to_string(view: WGPUStringView) -> String {
        if view.data.is_null() {
            return String::new();
        }

        if view.length == usize::MAX {
            // SAFETY: `view.data` is expected to point at a NUL-terminated string
            // when using the WGPU_STRLEN sentinel.
            let c_str = unsafe { CStr::from_ptr(view.data) };
            return c_str.to_string_lossy().into_owned();
        }

        // SAFETY: `view.data` is valid for `view.length` bytes according to the
        // WebGPU C API contract for output string views.
        let bytes = unsafe { slice::from_raw_parts(view.data.cast::<u8>(), view.length) };
        String::from_utf8_lossy(bytes).into_owned()
    }

    extern "C" fn request_adapter_callback(
        status: u32,
        adapter: WGPUAdapter,
        message: WGPUStringView,
        userdata1: *mut c_void,
        _userdata2: *mut c_void,
    ) {
        if userdata1.is_null() {
            return;
        }

        // SAFETY: userdata points to the `AdapterRequestState` allocated in
        // `probe_adapter` and kept alive until the callback completes.
        let state = unsafe { &mut *(userdata1.cast::<AdapterRequestState>()) };
        state.status = status;
        state.adapter = adapter;
        state.message = string_view_to_string(message);
        state.completed = true;
    }

    extern "C" fn request_device_callback(
        status: u32,
        device: WGPUDevice,
        message: WGPUStringView,
        userdata1: *mut c_void,
        _userdata2: *mut c_void,
    ) {
        if userdata1.is_null() {
            return;
        }

        // SAFETY: userdata points to the `DeviceRequestState` allocated in
        // `probe_adapter` and kept alive until the callback completes.
        let state = unsafe { &mut *(userdata1.cast::<DeviceRequestState>()) };
        state.status = status;
        state.device = device;
        state.message = string_view_to_string(message);
        state.completed = true;
    }

    fn wait_for_future(
        instance: WGPUInstance,
        future: WGPUFuture,
        name: &str,
    ) -> Result<(), String> {
        let deadline = Instant::now() + Duration::from_secs(10);
        let mut wait_info = WGPUFutureWaitInfo {
            future,
            completed: 0,
        };

        loop {
            // SAFETY: All pointers remain valid for the duration of the call.
            let status = unsafe { wgpuInstanceWaitAny(instance, 1, &mut wait_info as *mut _, 0) };
            match status {
                WGPU_WAIT_STATUS_SUCCESS if wait_info.completed == WGPU_TRUE => return Ok(()),
                WGPU_WAIT_STATUS_SUCCESS | WGPU_WAIT_STATUS_TIMED_OUT => {
                    if Instant::now() >= deadline {
                        return Err(format!("{name} timed out waiting for completion"));
                    }
                    thread::yield_now();
                }
                _ => {
                    return Err(format!(
                        "{name} failed during waitAny with status code {status}"
                    ));
                }
            }
        }
    }

    fn preferred_backend_type() -> u32 {
        #[cfg(any(target_os = "macos", target_os = "ios"))]
        {
            return WGPU_BACKEND_TYPE_METAL;
        }

        #[cfg(target_os = "windows")]
        {
            return WGPU_BACKEND_TYPE_D3D12;
        }

        #[cfg(not(any(target_os = "macos", target_os = "ios", target_os = "windows")))]
        {
            return WGPU_BACKEND_TYPE_VULKAN;
        }
    }

    fn map_backend_to_babylon_backend(backend_type: u32) -> u32 {
        match backend_type {
            WGPU_BACKEND_TYPE_VULKAN => 1,
            WGPU_BACKEND_TYPE_METAL => 2,
            WGPU_BACKEND_TYPE_D3D12 => 3,
            WGPU_BACKEND_TYPE_D3D11 => 3,
            WGPU_BACKEND_TYPE_OPEN_GL | WGPU_BACKEND_TYPE_OPEN_GLES => 4,
            _ => 0,
        }
    }

    pub fn version() -> u32 {
        // SAFETY: Symbol is provided by upstream wgpu-native staticlib.
        unsafe { wgpuGetVersion() }
    }

    pub fn probe_adapter(prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
        // SAFETY: Null descriptor is explicitly supported by webgpu.h APIs for
        // default instance creation.
        let instance = unsafe { wgpuCreateInstance(ptr::null()) };
        if instance.is_null() {
            return Err("wgpuCreateInstance returned null".to_string());
        }

        let mut adapter: WGPUAdapter = ptr::null_mut();
        let result = (|| -> Result<AdapterProbeInfo, String> {
            let mut adapter_state = AdapterRequestState::default();
            let mut request_options = WGPURequestAdapterOptions {
                next_in_chain: ptr::null_mut(),
                feature_level: WGPU_FEATURE_LEVEL_CORE,
                power_preference: if prefer_low_power {
                    WGPU_POWER_PREFERENCE_LOW_POWER
                } else {
                    WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE
                },
                force_fallback_adapter: 0,
                backend_type: preferred_backend_type(),
                compatible_surface: ptr::null_mut(),
            };

            let adapter_callback_info = WGPURequestAdapterCallbackInfo {
                next_in_chain: ptr::null_mut(),
                mode: WGPU_CALLBACK_MODE_WAIT_ANY_ONLY,
                callback: Some(request_adapter_callback),
                userdata1: (&mut adapter_state as *mut AdapterRequestState).cast::<c_void>(),
                userdata2: ptr::null_mut(),
            };

            // SAFETY: Input pointers and callback userdata stay valid until
            // request completion.
            let adapter_future = unsafe {
                wgpuInstanceRequestAdapter(
                    instance,
                    &mut request_options as *mut WGPURequestAdapterOptions,
                    adapter_callback_info,
                )
            };
            wait_for_future(instance, adapter_future, "requestAdapter")?;

            if !adapter_state.completed {
                return Err("requestAdapter callback did not complete".to_string());
            }
            if adapter_state.status != WGPU_REQUEST_ADAPTER_STATUS_SUCCESS
                || adapter_state.adapter.is_null()
            {
                let message = if adapter_state.message.is_empty() {
                    "no adapter message".to_string()
                } else {
                    adapter_state.message
                };
                return Err(format!(
                    "requestAdapter failed with status {} ({message})",
                    adapter_state.status
                ));
            }

            adapter = adapter_state.adapter;

            let mut device_state = DeviceRequestState::default();
            let device_callback_info = WGPURequestDeviceCallbackInfo {
                next_in_chain: ptr::null_mut(),
                mode: WGPU_CALLBACK_MODE_WAIT_ANY_ONLY,
                callback: Some(request_device_callback),
                userdata1: (&mut device_state as *mut DeviceRequestState).cast::<c_void>(),
                userdata2: ptr::null_mut(),
            };

            // SAFETY: Null descriptor is valid and requests a default device.
            let device_future =
                unsafe { wgpuAdapterRequestDevice(adapter, ptr::null(), device_callback_info) };
            wait_for_future(instance, device_future, "requestDevice")?;

            if !device_state.completed {
                return Err("requestDevice callback did not complete".to_string());
            }
            if device_state.status != WGPU_REQUEST_DEVICE_STATUS_SUCCESS
                || device_state.device.is_null()
            {
                let message = if device_state.message.is_empty() {
                    "no device message".to_string()
                } else {
                    device_state.message
                };
                return Err(format!(
                    "requestDevice failed with status {} ({message})",
                    device_state.status
                ));
            }

            // SAFETY: Device handle is valid on successful request and can be
            // immediately released after probing.
            unsafe {
                wgpuDeviceRelease(device_state.device);
            }

            let mut adapter_info = WGPUAdapterInfo::default();
            // SAFETY: `adapter` and `adapter_info` are valid pointers.
            let status = unsafe { wgpuAdapterGetInfo(adapter, &mut adapter_info) };
            if status != WGPU_STATUS_SUCCESS {
                return Err(format!(
                    "wgpuAdapterGetInfo failed with status code {status}"
                ));
            }

            let description = {
                let primary = string_view_to_string(adapter_info.description);
                if !primary.is_empty() {
                    primary
                } else {
                    let fallback = string_view_to_string(adapter_info.device);
                    if !fallback.is_empty() {
                        fallback
                    } else {
                        "wgpu-native adapter".to_string()
                    }
                }
            };

            let probe = AdapterProbeInfo {
                backend: map_backend_to_babylon_backend(adapter_info.backend_type),
                vendor_id: adapter_info.vendor_id,
                device_id: adapter_info.device_id,
                adapter_name: description,
            };

            // SAFETY: adapter_info contains owned output strings from the C API.
            unsafe {
                wgpuAdapterInfoFreeMembers(adapter_info);
            }

            Ok(probe)
        })();

        if !adapter.is_null() {
            // SAFETY: adapter came from a successful callback and still owns a ref.
            unsafe {
                wgpuAdapterRelease(adapter);
            }
        }

        // SAFETY: instance was created by `wgpuCreateInstance` and is still live.
        unsafe {
            wgpuInstanceRelease(instance);
        }

        result
    }
}

#[cfg(feature = "upstream_wgpu_native")]
pub use enabled::{probe_adapter, version};

#[cfg(not(feature = "upstream_wgpu_native"))]
pub fn version() -> u32 {
    0
}

#[cfg(not(feature = "upstream_wgpu_native"))]
#[allow(dead_code)]
pub fn probe_adapter(_prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
    Err("upstream wgpu-native probe is disabled at compile time".to_string())
}
