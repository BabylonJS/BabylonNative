// wgpu-native C-API compute dispatch path.
//
// This module wraps the upstream wgpu-native generated bindings (`webgpu.h`)
// to provide a standalone compute pipeline: adapter/device bootstrap,
// shader compilation, and workgroup dispatch via the C ABI.

use std::ffi::{c_char, c_void, CStr};
use std::ptr;
use std::slice;
use std::sync::{Mutex, OnceLock};
use std::time::{Duration, Instant};

mod native {
    #![allow(non_upper_case_globals)]
    #![allow(non_camel_case_types)]
    #![allow(non_snake_case)]
    #![allow(dead_code)]
    #![allow(improper_ctypes)]
    include!(concat!(env!("OUT_DIR"), "/webgpu_bindings.rs"));
}

use native::*;

struct AdapterRequestState {
    status: WGPURequestAdapterStatus,
    adapter: WGPUAdapter,
    message: String,
    completed: bool,
}

impl Default for AdapterRequestState {
    fn default() -> Self {
        Self {
            status: WGPURequestAdapterStatus_Force32,
            adapter: ptr::null_mut(),
            message: String::new(),
            completed: false,
        }
    }
}

struct DeviceRequestState {
    status: WGPURequestDeviceStatus,
    device: WGPUDevice,
    message: String,
    completed: bool,
}

impl Default for DeviceRequestState {
    fn default() -> Self {
        Self {
            status: WGPURequestDeviceStatus_Force32,
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
    cached_shader_source: String,
    cached_entry_point: String,
    cached_pipeline: WGPUComputePipeline,
}

// SAFETY: `ComputeRuntime` is only accessed behind `COMPUTE_RUNTIME` mutex.
// The raw handles are opaque tokens from the wgpu-native C API and are not
// dereferenced directly in Rust; they are only passed back to C ABI calls
// while holding synchronization around runtime ownership.
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

fn as_wgpu_bool(value: bool) -> WGPUBool {
    if value {
        1
    } else {
        0
    }
}

unsafe extern "C" fn request_adapter_callback(
    status: WGPURequestAdapterStatus,
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

unsafe extern "C" fn request_device_callback(
    status: WGPURequestDeviceStatus,
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

        // SAFETY: ProcessEvents is valid for callback modes used by this shim.
        // We intentionally do not call `wgpuInstanceWaitAny` because the
        // currently linked wgpu-native path may return NULL_FUTURE for async
        // operations and does not reliably support future-ID waiting APIs on
        // our target matrix, which can panic and abort the app. Completion is
        // callback-driven via ProcessEvents instead.
        unsafe {
            wgpuInstanceProcessEvents(instance);
        }

        if is_completed() {
            return Ok(());
        }

        if Instant::now() >= deadline {
            return Err(format!("{name} timed out waiting for completion"));
        }
    }
}

fn preferred_backend_type() -> WGPUBackendType {
    #[cfg(any(target_os = "macos", target_os = "ios"))]
    {
        return WGPUBackendType_Metal;
    }

    #[cfg(target_os = "windows")]
    {
        return WGPUBackendType_D3D12;
    }

    #[cfg(not(any(target_os = "macos", target_os = "ios", target_os = "windows")))]
    {
        return WGPUBackendType_Vulkan;
    }
}

fn request_adapter(
    instance: WGPUInstance,
    prefer_low_power: bool,
) -> Result<WGPUAdapter, String> {
    let preferred_power = if prefer_low_power {
        WGPUPowerPreference_LowPower
    } else {
        WGPUPowerPreference_HighPerformance
    };

    let attempts = [
        (
            preferred_backend_type(),
            preferred_power,
            false,
            "preferred-backend",
        ),
        (
            preferred_backend_type(),
            preferred_power,
            true,
            "preferred-backend-fallback",
        ),
        (
            WGPUBackendType_Undefined,
            preferred_power,
            false,
            "any-backend",
        ),
        (
            WGPUBackendType_Undefined,
            WGPUPowerPreference_LowPower,
            true,
            "any-backend-fallback-low-power",
        ),
    ];

    let mut failures: Vec<String> = Vec::new();
    for (backend_type, power_preference, force_fallback, label) in attempts {
        let mut state = AdapterRequestState::default();
        let callback_info = WGPURequestAdapterCallbackInfo {
            nextInChain: ptr::null(),
            mode: WGPUCallbackMode_AllowProcessEvents,
            callback: Some(request_adapter_callback),
            userdata1: (&mut state as *mut AdapterRequestState).cast::<c_void>(),
            userdata2: ptr::null_mut(),
        };
        let options = WGPURequestAdapterOptions {
            nextInChain: ptr::null(),
            featureLevel: WGPUFeatureLevel_Core,
            powerPreference: power_preference,
            forceFallbackAdapter: as_wgpu_bool(force_fallback),
            backendType: backend_type,
            compatibleSurface: ptr::null_mut(),
        };

        // SAFETY: Instance/options/callback info are valid for the call.
        // wgpu-native currently does not expose stable future IDs on this
        // path (NULL_FUTURE on our target matrix). Adapter completion is
        // callback-driven and pumped via ProcessEvents.
        unsafe {
            wgpuInstanceRequestAdapter(instance, &options, callback_info);
        }

        wait_for_callback(instance, || state.completed, "requestAdapter")?;

        if state.status == WGPURequestAdapterStatus_Success && !state.adapter.is_null() {
            return Ok(state.adapter);
        }

        failures.push(format!(
            "{label}: status={} message={}",
            state.status,
            if state.message.is_empty() {
                "no adapter message".to_string()
            } else {
                state.message.clone()
            }
        ));
    }

    Err(format!(
        "requestAdapter failed across attempts: {}",
        failures.join("; ")
    ))
}

fn request_device(
    instance: WGPUInstance,
    adapter: WGPUAdapter,
) -> Result<WGPUDevice, String> {
    let mut state = DeviceRequestState::default();
    let callback_info = WGPURequestDeviceCallbackInfo {
        nextInChain: ptr::null(),
        mode: WGPUCallbackMode_AllowProcessEvents,
        callback: Some(request_device_callback),
        userdata1: (&mut state as *mut DeviceRequestState).cast::<c_void>(),
        userdata2: ptr::null_mut(),
    };

    // SAFETY: Null descriptor requests a default device from the adapter.
    // wgpu-native currently does not expose stable future IDs on this
    // path (NULL_FUTURE on our target matrix). Device completion is
    // callback-driven and pumped via ProcessEvents.
    unsafe {
        wgpuAdapterRequestDevice(adapter, ptr::null(), callback_info);
    }
    wait_for_callback(instance, || state.completed, "requestDevice")?;

    if !state.completed {
        return Err("requestDevice callback did not complete".to_string());
    }
    if state.status != WGPURequestDeviceStatus_Success || state.device.is_null() {
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
        adapter = request_adapter(instance, prefer_low_power)?;
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
                sType: WGPUSType_ShaderSourceWGSL,
            },
            code: make_string_view(shader_source),
        };
        let shader_descriptor = WGPUShaderModuleDescriptor {
            nextInChain: &shader_chain.chain as *const WGPUChainedStruct,
            label: empty_string_view(),
        };

        // SAFETY: Device and descriptor are valid for the duration of the call.
        let shader_module =
            unsafe { wgpuDeviceCreateShaderModule(runtime.device, &shader_descriptor) };
        if shader_module.is_null() {
            return Err("wgpuDeviceCreateShaderModule returned null".to_string());
        }

        let stage = WGPUProgrammableStageDescriptor {
            nextInChain: ptr::null(),
            module: shader_module,
            entryPoint: make_string_view(entry),
            constantCount: 0,
            constants: ptr::null(),
        };
        let pipeline_descriptor = WGPUComputePipelineDescriptor {
            nextInChain: ptr::null(),
            label: empty_string_view(),
            layout: ptr::null_mut(),
            compute: stage,
        };

        // SAFETY: Device and descriptor are valid for the duration of the call.
        let compute_pipeline = unsafe {
            wgpuDeviceCreateComputePipeline(runtime.device, &pipeline_descriptor)
        };
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
            nextInChain: ptr::null(),
            label: empty_string_view(),
        };
        // SAFETY: Device and descriptor are valid for the duration of the call.
        command_encoder =
            unsafe { wgpuDeviceCreateCommandEncoder(runtime.device, &encoder_descriptor) };
        if command_encoder.is_null() {
            return Err("wgpuDeviceCreateCommandEncoder returned null".to_string());
        }

        let pass_descriptor = WGPUComputePassDescriptor {
            nextInChain: ptr::null(),
            label: empty_string_view(),
            timestampWrites: ptr::null(),
        };
        // SAFETY: Encoder and descriptor are valid for the duration of the call.
        compute_pass = unsafe {
            wgpuCommandEncoderBeginComputePass(command_encoder, &pass_descriptor)
        };
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
            nextInChain: ptr::null(),
            label: empty_string_view(),
        };
        // SAFETY: Encoder and descriptor are valid for the duration of the call.
        command_buffer = unsafe {
            wgpuCommandEncoderFinish(command_encoder, &command_buffer_descriptor)
        };
        if command_buffer.is_null() {
            return Err("wgpuCommandEncoderFinish returned null".to_string());
        }

        // SAFETY: Queue and command buffer are valid handles.
        unsafe {
            wgpuQueueSubmit(
                runtime.queue,
                1,
                &command_buffer as *const WGPUCommandBuffer,
            );
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
