use std::ffi::c_void;
#[cfg(feature = "upstream_wgpu_native")]
use std::ffi::{c_char, CStr};
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
    use std::sync::{Mutex, OnceLock};
    use std::thread;
    use std::time::{Duration, Instant};

    type WGPUInstance = *mut c_void;
    type WGPUAdapter = *mut c_void;
    type WGPUDevice = *mut c_void;
    type WGPUQueue = *mut c_void;
    type WGPUSurface = *mut c_void;
    type WGPUTexture = *mut c_void;
    type WGPUShaderModule = *mut c_void;
    type WGPUComputePipeline = *mut c_void;
    type WGPUCommandEncoder = *mut c_void;
    type WGPUComputePassEncoder = *mut c_void;
    type WGPUCommandBuffer = *mut c_void;
    type WGPUPipelineLayout = *mut c_void;

    const WGPU_CALLBACK_MODE_ALLOW_PROCESS_EVENTS: u32 = 2;

    const WGPU_REQUEST_ADAPTER_STATUS_SUCCESS: u32 = 1;
    const WGPU_REQUEST_DEVICE_STATUS_SUCCESS: u32 = 1;

    const WGPU_STATUS_SUCCESS: u32 = 1;
    const WGPU_SURFACE_GET_CURRENT_TEXTURE_STATUS_SUCCESS_OPTIMAL: u32 = 1;
    const WGPU_SURFACE_GET_CURRENT_TEXTURE_STATUS_SUCCESS_SUBOPTIMAL: u32 = 2;

    const WGPU_FEATURE_LEVEL_CORE: u32 = 2;

    const WGPU_POWER_PREFERENCE_LOW_POWER: u32 = 1;
    const WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE: u32 = 2;

    const WGPU_BACKEND_TYPE_D3D11: u32 = 3;
    const WGPU_BACKEND_TYPE_D3D12: u32 = 4;
    const WGPU_BACKEND_TYPE_METAL: u32 = 5;
    const WGPU_BACKEND_TYPE_VULKAN: u32 = 6;
    const WGPU_BACKEND_TYPE_OPEN_GL: u32 = 7;
    const WGPU_BACKEND_TYPE_OPEN_GLES: u32 = 8;

    const WGPU_STYPE_SHADER_SOURCE_WGSL: u32 = 2;
    const WGPU_STYPE_SURFACE_SOURCE_METAL_LAYER: u32 = 4;
    #[cfg(target_os = "windows")]
    const WGPU_STYPE_SURFACE_SOURCE_WINDOWS_HWND: u32 = 5;
    #[cfg(target_os = "android")]
    const WGPU_STYPE_SURFACE_SOURCE_ANDROID_NATIVE_WINDOW: u32 = 8;
    const WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT: u64 = 0x10;
    const WGPU_PRESENT_MODE_FIFO: u32 = 1;
    const WGPU_COMPOSITE_ALPHA_MODE_AUTO: u32 = 0;

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUStringView {
        data: *const c_char,
        length: usize,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUChainedStruct {
        next: *const WGPUChainedStruct,
        s_type: u32,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUFuture {
        id: u64,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPURequestAdapterOptions {
        next_in_chain: *const WGPUChainedStruct,
        feature_level: u32,
        power_preference: u32,
        force_fallback_adapter: u32,
        backend_type: u32,
        compatible_surface: WGPUSurface,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPURequestAdapterCallbackInfo {
        next_in_chain: *const WGPUChainedStruct,
        mode: u32,
        callback: Option<WGPURequestAdapterCallback>,
        userdata1: *mut c_void,
        userdata2: *mut c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPURequestDeviceCallbackInfo {
        next_in_chain: *const WGPUChainedStruct,
        mode: u32,
        callback: Option<WGPURequestDeviceCallback>,
        userdata1: *mut c_void,
        userdata2: *mut c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceCapabilities {
        next_in_chain: *mut c_void,
        usages: u64,
        format_count: usize,
        formats: *const u32,
        present_mode_count: usize,
        present_modes: *const u32,
        alpha_mode_count: usize,
        alpha_modes: *const u32,
    }

    impl Default for WGPUSurfaceCapabilities {
        fn default() -> Self {
            Self {
                next_in_chain: ptr::null_mut(),
                usages: 0,
                format_count: 0,
                formats: ptr::null(),
                present_mode_count: 0,
                present_modes: ptr::null(),
                alpha_mode_count: 0,
                alpha_modes: ptr::null(),
            }
        }
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceConfiguration {
        next_in_chain: *const WGPUChainedStruct,
        device: WGPUDevice,
        format: u32,
        usage: u64,
        width: u32,
        height: u32,
        view_format_count: usize,
        view_formats: *const u32,
        alpha_mode: u32,
        present_mode: u32,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        label: WGPUStringView,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceSourceMetalLayer {
        chain: WGPUChainedStruct,
        layer: *mut c_void,
    }

    #[cfg(target_os = "android")]
    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceSourceAndroidNativeWindow {
        chain: WGPUChainedStruct,
        window: *mut c_void,
    }

    #[cfg(target_os = "windows")]
    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceSourceWindowsHWND {
        chain: WGPUChainedStruct,
        hinstance: *mut c_void,
        hwnd: *mut c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUSurfaceTexture {
        next_in_chain: *mut c_void,
        texture: WGPUTexture,
        status: u32,
    }

    impl Default for WGPUSurfaceTexture {
        fn default() -> Self {
            Self {
                next_in_chain: ptr::null_mut(),
                texture: ptr::null_mut(),
                status: 0,
            }
        }
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

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUShaderSourceWGSL {
        chain: WGPUChainedStruct,
        code: WGPUStringView,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUShaderModuleDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        label: WGPUStringView,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUProgrammableStageDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        module: WGPUShaderModule,
        entry_point: WGPUStringView,
        constant_count: usize,
        constants: *const c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUComputePipelineDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        label: WGPUStringView,
        layout: WGPUPipelineLayout,
        compute: WGPUProgrammableStageDescriptor,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUCommandEncoderDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        label: WGPUStringView,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUComputePassDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        label: WGPUStringView,
        timestamp_writes: *const c_void,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    struct WGPUCommandBufferDescriptor {
        next_in_chain: *const WGPUChainedStruct,
        label: WGPUStringView,
    }

    type WGPURequestAdapterCallback =
        extern "C" fn(u32, WGPUAdapter, WGPUStringView, *mut c_void, *mut c_void);
    type WGPURequestDeviceCallback =
        extern "C" fn(u32, WGPUDevice, WGPUStringView, *mut c_void, *mut c_void);

    unsafe extern "C" {
        fn wgpuGetVersion() -> u32;
        fn wgpuCreateInstance(descriptor: *const c_void) -> WGPUInstance;
        fn wgpuInstanceProcessEvents(instance: WGPUInstance);
        fn wgpuInstanceCreateSurface(
            instance: WGPUInstance,
            descriptor: *const WGPUSurfaceDescriptor,
        ) -> WGPUSurface;
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
        fn wgpuAdapterGetInfo(adapter: WGPUAdapter, info: *mut WGPUAdapterInfo) -> u32;
        fn wgpuAdapterInfoFreeMembers(adapter_info: WGPUAdapterInfo);
        fn wgpuSurfaceGetCapabilities(
            surface: WGPUSurface,
            adapter: WGPUAdapter,
            capabilities: *mut WGPUSurfaceCapabilities,
        ) -> u32;
        fn wgpuSurfaceCapabilitiesFreeMembers(capabilities: WGPUSurfaceCapabilities);
        fn wgpuSurfaceConfigure(surface: WGPUSurface, config: *const WGPUSurfaceConfiguration);
        fn wgpuSurfaceGetCurrentTexture(surface: WGPUSurface, surface_texture: *mut WGPUSurfaceTexture);
        fn wgpuSurfacePresent(surface: WGPUSurface) -> u32;
        fn wgpuSurfaceUnconfigure(surface: WGPUSurface);

        fn wgpuDeviceCreateShaderModule(
            device: WGPUDevice,
            descriptor: *const WGPUShaderModuleDescriptor,
        ) -> WGPUShaderModule;
        fn wgpuDeviceCreateComputePipeline(
            device: WGPUDevice,
            descriptor: *const WGPUComputePipelineDescriptor,
        ) -> WGPUComputePipeline;
        fn wgpuDeviceCreateCommandEncoder(
            device: WGPUDevice,
            descriptor: *const WGPUCommandEncoderDescriptor,
        ) -> WGPUCommandEncoder;
        fn wgpuCommandEncoderBeginComputePass(
            command_encoder: WGPUCommandEncoder,
            descriptor: *const WGPUComputePassDescriptor,
        ) -> WGPUComputePassEncoder;
        fn wgpuComputePassEncoderSetPipeline(
            compute_pass_encoder: WGPUComputePassEncoder,
            pipeline: WGPUComputePipeline,
        );
        fn wgpuComputePassEncoderDispatchWorkgroups(
            compute_pass_encoder: WGPUComputePassEncoder,
            x: u32,
            y: u32,
            z: u32,
        );
        fn wgpuComputePassEncoderEnd(compute_pass_encoder: WGPUComputePassEncoder);
        fn wgpuCommandEncoderFinish(
            command_encoder: WGPUCommandEncoder,
            descriptor: *const WGPUCommandBufferDescriptor,
        ) -> WGPUCommandBuffer;

        fn wgpuDeviceGetQueue(device: WGPUDevice) -> WGPUQueue;
        fn wgpuQueueSubmit(
            queue: WGPUQueue,
            command_count: usize,
            commands: *const WGPUCommandBuffer,
        );

        fn wgpuAdapterRelease(adapter: WGPUAdapter);
        fn wgpuDeviceRelease(device: WGPUDevice);
        fn wgpuQueueRelease(queue: WGPUQueue);
        fn wgpuSurfaceRelease(surface: WGPUSurface);
        fn wgpuTextureRelease(texture: WGPUTexture);
        fn wgpuShaderModuleRelease(shader_module: WGPUShaderModule);
        fn wgpuComputePipelineRelease(compute_pipeline: WGPUComputePipeline);
        fn wgpuComputePassEncoderRelease(compute_pass_encoder: WGPUComputePassEncoder);
        fn wgpuCommandEncoderRelease(command_encoder: WGPUCommandEncoder);
        fn wgpuCommandBufferRelease(command_buffer: WGPUCommandBuffer);
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

    struct ComputeRuntime {
        instance: WGPUInstance,
        adapter: WGPUAdapter,
        device: WGPUDevice,
        queue: WGPUQueue,
        prefer_low_power: bool,
        adapter_probe: AdapterProbeInfo,
        cached_shader_source: String,
        cached_entry_point: String,
        cached_pipeline: WGPUComputePipeline,
    }

    // SAFETY: Access is synchronized through `COMPUTE_RUNTIME`'s mutex and all
    // handles are only touched while holding that lock.
    unsafe impl Send for ComputeRuntime {}

    impl Drop for ComputeRuntime {
        fn drop(&mut self) {
            if !self.cached_pipeline.is_null() {
                // SAFETY: Handle belongs to this runtime and is released exactly once.
                unsafe {
                    wgpuComputePipelineRelease(self.cached_pipeline);
                }
                self.cached_pipeline = ptr::null_mut();
            }
            if !self.queue.is_null() {
                // SAFETY: Handle belongs to this runtime and is released exactly once.
                unsafe {
                    wgpuQueueRelease(self.queue);
                }
                self.queue = ptr::null_mut();
            }
            if !self.device.is_null() {
                // SAFETY: Handle belongs to this runtime and is released exactly once.
                unsafe {
                    wgpuDeviceRelease(self.device);
                }
                self.device = ptr::null_mut();
            }
            if !self.adapter.is_null() {
                // SAFETY: Handle belongs to this runtime and is released exactly once.
                unsafe {
                    wgpuAdapterRelease(self.adapter);
                }
                self.adapter = ptr::null_mut();
            }
            if !self.instance.is_null() {
                // SAFETY: Handle belongs to this runtime and is released exactly once.
                unsafe {
                    wgpuInstanceRelease(self.instance);
                }
                self.instance = ptr::null_mut();
            }
        }
    }

    static COMPUTE_RUNTIME: OnceLock<Mutex<Option<ComputeRuntime>>> = OnceLock::new();

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

    fn make_string_view(input: &str) -> WGPUStringView {
        WGPUStringView {
            data: input.as_ptr().cast::<c_char>(),
            length: input.len(),
        }
    }

    fn empty_string_view() -> WGPUStringView {
        WGPUStringView {
            data: ptr::null(),
            length: usize::MAX,
        }
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
        // `request_adapter` and kept alive until callback completion.
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
        // `request_device` and kept alive until callback completion.
        let state = unsafe { &mut *(userdata1.cast::<DeviceRequestState>()) };
        state.status = status;
        state.device = device;
        state.message = string_view_to_string(message);
        state.completed = true;
    }

    fn wait_for_callback(
        instance: WGPUInstance,
        is_completed: impl Fn() -> bool,
        name: &str,
    ) -> Result<(), String> {
        let deadline = Instant::now() + Duration::from_secs(10);
        loop {
            if is_completed() {
                return Ok(());
            }

            // SAFETY: Instance handle is valid while this request is in flight.
            unsafe {
                wgpuInstanceProcessEvents(instance);
            }

            if is_completed() {
                return Ok(());
            }

            if Instant::now() >= deadline {
                return Err(format!("{name} timed out waiting for completion"));
            }
            thread::yield_now();
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

    #[cfg(target_os = "windows")]
    unsafe extern "system" {
        fn GetModuleHandleW(lp_module_name: *const u16) -> *mut c_void;
    }

    fn create_surface(
        instance: WGPUInstance,
        surface_layer: *mut c_void,
    ) -> Result<WGPUSurface, String> {
        if surface_layer.is_null() {
            return Err("surface layer pointer was null".to_string());
        }

        #[cfg(any(target_os = "macos", target_os = "ios"))]
        {
            let source = WGPUSurfaceSourceMetalLayer {
                chain: WGPUChainedStruct {
                    next: ptr::null(),
                    s_type: WGPU_STYPE_SURFACE_SOURCE_METAL_LAYER,
                },
                layer: surface_layer,
            };
            let descriptor = WGPUSurfaceDescriptor {
                next_in_chain: &source.chain,
                label: empty_string_view(),
            };
            // SAFETY: Instance handle is valid and chained descriptor is live for this call.
            let surface = unsafe { wgpuInstanceCreateSurface(instance, &descriptor) };
            if surface.is_null() {
                return Err("wgpuInstanceCreateSurface returned null for Metal layer".to_string());
            }
            return Ok(surface);
        }

        #[cfg(target_os = "android")]
        {
            let source = WGPUSurfaceSourceAndroidNativeWindow {
                chain: WGPUChainedStruct {
                    next: ptr::null(),
                    s_type: WGPU_STYPE_SURFACE_SOURCE_ANDROID_NATIVE_WINDOW,
                },
                window: surface_layer,
            };
            let descriptor = WGPUSurfaceDescriptor {
                next_in_chain: &source.chain,
                label: empty_string_view(),
            };
            // SAFETY: Instance handle is valid and chained descriptor is live for this call.
            let surface = unsafe { wgpuInstanceCreateSurface(instance, &descriptor) };
            if surface.is_null() {
                return Err(
                    "wgpuInstanceCreateSurface returned null for Android native window".to_string(),
                );
            }
            return Ok(surface);
        }

        #[cfg(target_os = "windows")]
        {
            // SAFETY: null module name queries current process executable module.
            let hinstance = unsafe { GetModuleHandleW(ptr::null()) };
            if hinstance.is_null() {
                return Err("GetModuleHandleW returned null".to_string());
            }
            let source = WGPUSurfaceSourceWindowsHWND {
                chain: WGPUChainedStruct {
                    next: ptr::null(),
                    s_type: WGPU_STYPE_SURFACE_SOURCE_WINDOWS_HWND,
                },
                hinstance,
                hwnd: surface_layer,
            };
            let descriptor = WGPUSurfaceDescriptor {
                next_in_chain: &source.chain,
                label: empty_string_view(),
            };
            // SAFETY: Instance handle is valid and chained descriptor is live for this call.
            let surface = unsafe { wgpuInstanceCreateSurface(instance, &descriptor) };
            if surface.is_null() {
                return Err("wgpuInstanceCreateSurface returned null for Win32 HWND".to_string());
            }
            return Ok(surface);
        }

        #[cfg(not(any(
            target_os = "macos",
            target_os = "ios",
            target_os = "android",
            target_os = "windows"
        )))]
        {
            let _ = instance;
            let _ = surface_layer;
            Err("surface probing is not supported on this platform".to_string())
        }
    }

    fn request_adapter(
        instance: WGPUInstance,
        prefer_low_power: bool,
        compatible_surface: WGPUSurface,
    ) -> Result<WGPUAdapter, String> {
        let mut state = AdapterRequestState::default();
        let request_options = WGPURequestAdapterOptions {
            next_in_chain: ptr::null(),
            feature_level: WGPU_FEATURE_LEVEL_CORE,
            power_preference: if prefer_low_power {
                WGPU_POWER_PREFERENCE_LOW_POWER
            } else {
                WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE
            },
            force_fallback_adapter: 0,
            backend_type: preferred_backend_type(),
            compatible_surface,
        };

        let callback_info = WGPURequestAdapterCallbackInfo {
            next_in_chain: ptr::null(),
            mode: WGPU_CALLBACK_MODE_ALLOW_PROCESS_EVENTS,
            callback: Some(request_adapter_callback),
            userdata1: (&mut state as *mut AdapterRequestState).cast::<c_void>(),
            userdata2: ptr::null_mut(),
        };

        // SAFETY: Input pointers and callback userdata remain valid until callback completion.
        let _future =
            unsafe { wgpuInstanceRequestAdapter(instance, &request_options, callback_info) };
        wait_for_callback(instance, || state.completed, "requestAdapter")?;

        if !state.completed {
            return Err("requestAdapter callback did not complete".to_string());
        }
        if state.status != WGPU_REQUEST_ADAPTER_STATUS_SUCCESS || state.adapter.is_null() {
            let message = if state.message.is_empty() {
                "no adapter message".to_string()
            } else {
                state.message
            };
            return Err(format!(
                "requestAdapter failed with status {} ({message})",
                state.status
            ));
        }

        Ok(state.adapter)
    }

    fn request_device(instance: WGPUInstance, adapter: WGPUAdapter) -> Result<WGPUDevice, String> {
        let mut state = DeviceRequestState::default();
        let callback_info = WGPURequestDeviceCallbackInfo {
            next_in_chain: ptr::null(),
            mode: WGPU_CALLBACK_MODE_ALLOW_PROCESS_EVENTS,
            callback: Some(request_device_callback),
            userdata1: (&mut state as *mut DeviceRequestState).cast::<c_void>(),
            userdata2: ptr::null_mut(),
        };

        // SAFETY: Null descriptor requests a default device from the adapter.
        let _future = unsafe { wgpuAdapterRequestDevice(adapter, ptr::null(), callback_info) };
        wait_for_callback(instance, || state.completed, "requestDevice")?;

        if !state.completed {
            return Err("requestDevice callback did not complete".to_string());
        }
        if state.status != WGPU_REQUEST_DEVICE_STATUS_SUCCESS || state.device.is_null() {
            let message = if state.message.is_empty() {
                "no device message".to_string()
            } else {
                state.message
            };
            return Err(format!(
                "requestDevice failed with status {} ({message})",
                state.status
            ));
        }

        Ok(state.device)
    }

    fn query_adapter_probe_info(adapter: WGPUAdapter) -> Result<AdapterProbeInfo, String> {
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
    }

    fn probe_surface_texture_flow(
        surface: WGPUSurface,
        adapter: WGPUAdapter,
        device: WGPUDevice,
        width: u32,
        height: u32,
    ) -> Result<(), String> {
        let mut capabilities = WGPUSurfaceCapabilities::default();
        // SAFETY: handles and out-pointer are valid for this call.
        let caps_status = unsafe { wgpuSurfaceGetCapabilities(surface, adapter, &mut capabilities) };
        if caps_status != WGPU_STATUS_SUCCESS {
            return Err(format!(
                "wgpuSurfaceGetCapabilities failed with status {caps_status}"
            ));
        }

        let pick_or = |count: usize, values: *const u32, fallback: u32| -> u32 {
            if count == 0 || values.is_null() {
                fallback
            } else {
                // SAFETY: pointer is valid for `count` entries when count > 0.
                unsafe { *values }
            }
        };

        let surface_format = pick_or(capabilities.format_count, capabilities.formats, 0);
        if surface_format == 0 {
            // SAFETY: capabilities may hold heap allocations owned by the C API.
            unsafe {
                wgpuSurfaceCapabilitiesFreeMembers(capabilities);
            }
            return Err("wgpuSurfaceGetCapabilities returned no formats".to_string());
        }

        let config = WGPUSurfaceConfiguration {
            next_in_chain: ptr::null(),
            device,
            format: surface_format,
            usage: WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT,
            width: width.max(1),
            height: height.max(1),
            view_format_count: 0,
            view_formats: ptr::null(),
            alpha_mode: pick_or(
                capabilities.alpha_mode_count,
                capabilities.alpha_modes,
                WGPU_COMPOSITE_ALPHA_MODE_AUTO,
            ),
            present_mode: pick_or(
                capabilities.present_mode_count,
                capabilities.present_modes,
                WGPU_PRESENT_MODE_FIFO,
            ),
        };

        // SAFETY: capabilities may hold heap allocations owned by the C API.
        unsafe {
            wgpuSurfaceCapabilitiesFreeMembers(capabilities);
        }

        // SAFETY: handles and config pointer are valid for this call.
        unsafe {
            wgpuSurfaceConfigure(surface, &config);
        }

        let mut surface_texture = WGPUSurfaceTexture::default();
        // SAFETY: handles and out-pointer are valid for this call.
        unsafe {
            wgpuSurfaceGetCurrentTexture(surface, &mut surface_texture);
        }

        let status_ok = matches!(
            surface_texture.status,
            WGPU_SURFACE_GET_CURRENT_TEXTURE_STATUS_SUCCESS_OPTIMAL
                | WGPU_SURFACE_GET_CURRENT_TEXTURE_STATUS_SUCCESS_SUBOPTIMAL
        );
        if !status_ok || surface_texture.texture.is_null() {
            // SAFETY: surface handle is valid and configured.
            unsafe {
                wgpuSurfaceUnconfigure(surface);
            }
            return Err(format!(
                "wgpuSurfaceGetCurrentTexture failed with status {}",
                surface_texture.status
            ));
        }

        // SAFETY: surface is configured and has a current texture.
        let present_status = unsafe { wgpuSurfacePresent(surface) };

        // SAFETY: surface texture is returned with ownership.
        unsafe {
            wgpuTextureRelease(surface_texture.texture);
            wgpuSurfaceUnconfigure(surface);
        }

        if present_status != WGPU_STATUS_SUCCESS {
            return Err(format!("wgpuSurfacePresent failed with status {present_status}"));
        }

        Ok(())
    }

    fn compute_runtime_cell() -> &'static Mutex<Option<ComputeRuntime>> {
        COMPUTE_RUNTIME.get_or_init(|| Mutex::new(None))
    }

    fn initialize_compute_runtime(prefer_low_power: bool) -> Result<ComputeRuntime, String> {
        // SAFETY: Null descriptor is explicitly supported by webgpu.h APIs for
        // default instance creation.
        let instance = unsafe { wgpuCreateInstance(ptr::null()) };
        if instance.is_null() {
            return Err("wgpuCreateInstance returned null".to_string());
        }

        let mut adapter: WGPUAdapter = ptr::null_mut();
        let mut device: WGPUDevice = ptr::null_mut();
        let mut queue: WGPUQueue = ptr::null_mut();
        let result = (|| -> Result<ComputeRuntime, String> {
            adapter = request_adapter(instance, prefer_low_power, ptr::null_mut())?;
            device = request_device(instance, adapter)?;
            // SAFETY: Device handle is valid on successful request.
            queue = unsafe { wgpuDeviceGetQueue(device) };
            if queue.is_null() {
                return Err("wgpuDeviceGetQueue returned null".to_string());
            }

            Ok(ComputeRuntime {
                instance,
                adapter,
                device,
                queue,
                prefer_low_power,
                adapter_probe: query_adapter_probe_info(adapter)?,
                cached_shader_source: String::new(),
                cached_entry_point: String::new(),
                cached_pipeline: ptr::null_mut(),
            })
        })();

        if let Err(error) = result {
            if !queue.is_null() {
                // SAFETY: Queue was acquired from device before failure.
                unsafe {
                    wgpuQueueRelease(queue);
                }
            }
            if !device.is_null() {
                // SAFETY: Device was acquired before failure.
                unsafe {
                    wgpuDeviceRelease(device);
                }
            }
            if !adapter.is_null() {
                // SAFETY: Adapter was acquired before failure.
                unsafe {
                    wgpuAdapterRelease(adapter);
                }
            }
            // SAFETY: Instance was created by this function.
            unsafe {
                wgpuInstanceRelease(instance);
            }

            return Err(error);
        }

        result
    }

    fn ensure_runtime_locked(
        runtime_slot: &mut Option<ComputeRuntime>,
        prefer_low_power: bool,
    ) -> Result<&mut ComputeRuntime, String> {
        let needs_rebuild = match runtime_slot.as_ref() {
            Some(runtime) => runtime.prefer_low_power != prefer_low_power,
            None => true,
        };

        if needs_rebuild {
            *runtime_slot = Some(initialize_compute_runtime(prefer_low_power)?);
        }

        Ok(runtime_slot
            .as_mut()
            .expect("runtime initialized before returning mutable reference"))
    }

    pub fn version() -> u32 {
        // SAFETY: Symbol is provided by upstream wgpu-native staticlib.
        unsafe { wgpuGetVersion() }
    }

    pub fn ensure_bootstrap_runtime(prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
        let mut runtime_guard = match compute_runtime_cell().lock() {
            Ok(lock) => lock,
            Err(poisoned) => poisoned.into_inner(),
        };

        let runtime = ensure_runtime_locked(&mut runtime_guard, prefer_low_power)?;
        Ok(runtime.adapter_probe.clone())
    }

    pub fn probe_adapter(prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
        ensure_bootstrap_runtime(prefer_low_power)
    }

    pub fn probe_surface_adapter(
        surface_layer: *mut c_void,
        prefer_low_power: bool,
    ) -> Result<AdapterProbeInfo, String> {
        // SAFETY: Null descriptor is explicitly supported by webgpu.h APIs for
        // default instance creation.
        let instance = unsafe { wgpuCreateInstance(ptr::null()) };
        if instance.is_null() {
            return Err("wgpuCreateInstance returned null".to_string());
        }

        let mut surface: WGPUSurface = ptr::null_mut();
        let mut adapter: WGPUAdapter = ptr::null_mut();
        let mut device: WGPUDevice = ptr::null_mut();
        let mut queue: WGPUQueue = ptr::null_mut();

        let result = (|| -> Result<AdapterProbeInfo, String> {
            surface = create_surface(instance, surface_layer)?;
            adapter = request_adapter(instance, prefer_low_power, surface)?;
            let probe = query_adapter_probe_info(adapter)?;
            device = request_device(instance, adapter)?;
            // SAFETY: Device handle is valid on successful request.
            queue = unsafe { wgpuDeviceGetQueue(device) };
            if queue.is_null() {
                return Err("wgpuDeviceGetQueue returned null".to_string());
            }
            Ok(probe)
        })();

        if !queue.is_null() {
            // SAFETY: Queue was acquired from device before teardown.
            unsafe {
                wgpuQueueRelease(queue);
            }
        }
        if !device.is_null() {
            // SAFETY: Device was acquired before teardown.
            unsafe {
                wgpuDeviceRelease(device);
            }
        }
        if !adapter.is_null() {
            // SAFETY: Adapter was acquired before teardown.
            unsafe {
                wgpuAdapterRelease(adapter);
            }
        }
        if !surface.is_null() {
            // SAFETY: Surface was acquired before teardown.
            unsafe {
                wgpuSurfaceRelease(surface);
            }
        }
        // SAFETY: Instance was created by this function.
        unsafe {
            wgpuInstanceRelease(instance);
        }

        result
    }

    pub fn probe_surface_present(
        surface_layer: *mut c_void,
        width: u32,
        height: u32,
        prefer_low_power: bool,
    ) -> Result<AdapterProbeInfo, String> {
        // SAFETY: Null descriptor is explicitly supported by webgpu.h APIs for
        // default instance creation.
        let instance = unsafe { wgpuCreateInstance(ptr::null()) };
        if instance.is_null() {
            return Err("wgpuCreateInstance returned null".to_string());
        }

        let mut surface: WGPUSurface = ptr::null_mut();
        let mut adapter: WGPUAdapter = ptr::null_mut();
        let mut device: WGPUDevice = ptr::null_mut();
        let mut queue: WGPUQueue = ptr::null_mut();

        let result = (|| -> Result<AdapterProbeInfo, String> {
            surface = create_surface(instance, surface_layer)?;
            adapter = request_adapter(instance, prefer_low_power, surface)?;
            let probe = query_adapter_probe_info(adapter)?;
            device = request_device(instance, adapter)?;
            // SAFETY: Device handle is valid on successful request.
            queue = unsafe { wgpuDeviceGetQueue(device) };
            if queue.is_null() {
                return Err("wgpuDeviceGetQueue returned null".to_string());
            }
            probe_surface_texture_flow(surface, adapter, device, width, height)?;
            Ok(probe)
        })();

        if !queue.is_null() {
            // SAFETY: Queue was acquired from device before teardown.
            unsafe {
                wgpuQueueRelease(queue);
            }
        }
        if !device.is_null() {
            // SAFETY: Device was acquired before teardown.
            unsafe {
                wgpuDeviceRelease(device);
            }
        }
        if !adapter.is_null() {
            // SAFETY: Adapter was acquired before teardown.
            unsafe {
                wgpuAdapterRelease(adapter);
            }
        }
        if !surface.is_null() {
            // SAFETY: Surface was acquired before teardown.
            unsafe {
                wgpuSurfaceRelease(surface);
            }
        }
        // SAFETY: Instance was created by this function.
        unsafe {
            wgpuInstanceRelease(instance);
        }

        result
    }

    pub fn dispatch_compute_global(
        shader_source: &str,
        entry_point: &str,
        x: u32,
        y: u32,
        z: u32,
        prefer_low_power: bool,
    ) -> Result<(), String> {
        let mut runtime_guard = match compute_runtime_cell().lock() {
            Ok(lock) => lock,
            Err(poisoned) => poisoned.into_inner(),
        };

        let runtime = ensure_runtime_locked(&mut runtime_guard, prefer_low_power)?;

        let entry = if entry_point.is_empty() {
            "main"
        } else {
            entry_point
        };

        let pipeline_needs_rebuild = runtime.cached_pipeline.is_null()
            || runtime.cached_shader_source != shader_source
            || runtime.cached_entry_point != entry;

        if pipeline_needs_rebuild {
            if !runtime.cached_pipeline.is_null() {
                // SAFETY: Existing cached pipeline belongs to this runtime.
                unsafe {
                    wgpuComputePipelineRelease(runtime.cached_pipeline);
                }
                runtime.cached_pipeline = ptr::null_mut();
            }

            let shader_chain = WGPUShaderSourceWGSL {
                chain: WGPUChainedStruct {
                    next: ptr::null(),
                    s_type: WGPU_STYPE_SHADER_SOURCE_WGSL,
                },
                code: make_string_view(shader_source),
            };
            let shader_descriptor = WGPUShaderModuleDescriptor {
                next_in_chain: &shader_chain.chain as *const WGPUChainedStruct,
                label: empty_string_view(),
            };

            // SAFETY: Device and descriptor are valid for the duration of the call.
            let shader_module =
                unsafe { wgpuDeviceCreateShaderModule(runtime.device, &shader_descriptor) };
            if shader_module.is_null() {
                return Err("wgpuDeviceCreateShaderModule returned null".to_string());
            }

            let stage = WGPUProgrammableStageDescriptor {
                next_in_chain: ptr::null(),
                module: shader_module,
                entry_point: make_string_view(entry),
                constant_count: 0,
                constants: ptr::null(),
            };
            let pipeline_descriptor = WGPUComputePipelineDescriptor {
                next_in_chain: ptr::null(),
                label: empty_string_view(),
                layout: ptr::null_mut(),
                compute: stage,
            };

            // SAFETY: Device and descriptor are valid for the duration of the call.
            let compute_pipeline =
                unsafe { wgpuDeviceCreateComputePipeline(runtime.device, &pipeline_descriptor) };
            // SAFETY: Shader module is no longer needed once pipeline creation returns.
            unsafe {
                wgpuShaderModuleRelease(shader_module);
            }
            if compute_pipeline.is_null() {
                return Err("wgpuDeviceCreateComputePipeline returned null".to_string());
            }

            runtime.cached_shader_source.clear();
            runtime.cached_shader_source.push_str(shader_source);
            runtime.cached_entry_point.clear();
            runtime.cached_entry_point.push_str(entry);
            runtime.cached_pipeline = compute_pipeline;
        }

        let mut command_encoder: WGPUCommandEncoder = ptr::null_mut();
        let mut compute_pass: WGPUComputePassEncoder = ptr::null_mut();
        let mut command_buffer: WGPUCommandBuffer = ptr::null_mut();

        let result = (|| -> Result<(), String> {
            let encoder_descriptor = WGPUCommandEncoderDescriptor {
                next_in_chain: ptr::null(),
                label: empty_string_view(),
            };
            // SAFETY: Device and descriptor are valid for the duration of the call.
            command_encoder =
                unsafe { wgpuDeviceCreateCommandEncoder(runtime.device, &encoder_descriptor) };
            if command_encoder.is_null() {
                return Err("wgpuDeviceCreateCommandEncoder returned null".to_string());
            }

            let pass_descriptor = WGPUComputePassDescriptor {
                next_in_chain: ptr::null(),
                label: empty_string_view(),
                timestamp_writes: ptr::null(),
            };
            // SAFETY: Encoder and descriptor are valid for the duration of the call.
            compute_pass =
                unsafe { wgpuCommandEncoderBeginComputePass(command_encoder, &pass_descriptor) };
            if compute_pass.is_null() {
                return Err("wgpuCommandEncoderBeginComputePass returned null".to_string());
            }

            // SAFETY: All handles are valid and owned for the duration of this block.
            unsafe {
                wgpuComputePassEncoderSetPipeline(compute_pass, runtime.cached_pipeline);
                wgpuComputePassEncoderDispatchWorkgroups(
                    compute_pass,
                    x.max(1),
                    y.max(1),
                    z.max(1),
                );
                wgpuComputePassEncoderEnd(compute_pass);
            }

            let command_buffer_descriptor = WGPUCommandBufferDescriptor {
                next_in_chain: ptr::null(),
                label: empty_string_view(),
            };
            // SAFETY: Encoder and descriptor are valid for the duration of the call.
            command_buffer =
                unsafe { wgpuCommandEncoderFinish(command_encoder, &command_buffer_descriptor) };
            if command_buffer.is_null() {
                return Err("wgpuCommandEncoderFinish returned null".to_string());
            }

            // SAFETY: Queue and command buffer are valid handles.
            unsafe {
                wgpuQueueSubmit(runtime.queue, 1, &command_buffer as *const WGPUCommandBuffer);
            }

            Ok(())
        })();

        if !compute_pass.is_null() {
            // SAFETY: Handle is valid if creation succeeded.
            unsafe {
                wgpuComputePassEncoderRelease(compute_pass);
            }
        }
        if !command_buffer.is_null() {
            // SAFETY: Handle is valid if creation succeeded.
            unsafe {
                wgpuCommandBufferRelease(command_buffer);
            }
        }
        if !command_encoder.is_null() {
            // SAFETY: Handle is valid if creation succeeded.
            unsafe {
                wgpuCommandEncoderRelease(command_encoder);
            }
        }

        result
    }
}

#[cfg(feature = "upstream_wgpu_native")]
pub use enabled::{
    dispatch_compute_global, ensure_bootstrap_runtime, probe_adapter, probe_surface_adapter,
    probe_surface_present, version,
};

#[cfg(not(feature = "upstream_wgpu_native"))]
pub fn version() -> u32 {
    0
}

#[cfg(not(feature = "upstream_wgpu_native"))]
#[allow(dead_code)]
pub fn probe_adapter(_prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
    Err("upstream wgpu-native probe is disabled at compile time".to_string())
}

#[cfg(not(feature = "upstream_wgpu_native"))]
#[allow(dead_code)]
pub fn ensure_bootstrap_runtime(_prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
    Err("upstream wgpu-native bootstrap runtime is disabled at compile time".to_string())
}

#[cfg(not(feature = "upstream_wgpu_native"))]
#[allow(dead_code)]
pub fn probe_surface_adapter(
    _surface_layer: *mut c_void,
    _prefer_low_power: bool,
) -> Result<AdapterProbeInfo, String> {
    Err("upstream wgpu-native surface probe is disabled at compile time".to_string())
}

#[cfg(not(feature = "upstream_wgpu_native"))]
#[allow(dead_code)]
pub fn probe_surface_present(
    _surface_layer: *mut c_void,
    _width: u32,
    _height: u32,
    _prefer_low_power: bool,
) -> Result<AdapterProbeInfo, String> {
    Err("upstream wgpu-native surface present probe is disabled at compile time".to_string())
}

#[cfg(not(feature = "upstream_wgpu_native"))]
#[allow(dead_code)]
pub fn dispatch_compute_global(
    _shader_source: &str,
    _entry_point: &str,
    _x: u32,
    _y: u32,
    _z: u32,
    _prefer_low_power: bool,
) -> Result<(), String> {
    Err("upstream wgpu-native dispatch path is disabled at compile time".to_string())
}
