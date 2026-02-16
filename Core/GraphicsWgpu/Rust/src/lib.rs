// Threading contract: The `BackendContext` returned by `babylon_wgpu_create` is
// exclusively owned by a single C++ thread (the render/JS thread). The C++ side
// must never call `babylon_wgpu_render`, `babylon_wgpu_resize`, or
// `babylon_wgpu_destroy` concurrently. Global state (atomics and mutexes) is
// safe for concurrent access from any thread.

use std::any::Any;
#[cfg(target_os = "android")]
use std::ffi::CString;
use std::ffi::{c_char, c_void, CStr};
use std::ptr;
use std::sync::atomic::{AtomicBool, AtomicU32, AtomicU64, Ordering};
use std::sync::{Mutex, OnceLock};

static WEBGPU_DRAW_ENABLED: AtomicBool = AtomicBool::new(false);
static RENDER_FRAME_COUNTER: AtomicU64 = AtomicU64::new(0);
static UPSTREAM_WGPU_NATIVE_VERSION: AtomicU32 = AtomicU32::new(0);
static ESTIMATED_GPU_MEMORY_BYTES: AtomicU64 = AtomicU64::new(0);
static LAST_ERROR: OnceLock<Mutex<String>> = OnceLock::new();

#[repr(C)]
#[derive(Clone, Copy)]
pub struct BabylonWgpuConfig {
    pub width: u32,
    pub height: u32,
    pub surface_layer: *mut c_void,
    pub prefer_low_power: u8,
    pub enable_validation: u8,
    pub _reserved0: u8,
    pub _reserved1: u8,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct BabylonWgpuInfo {
    pub backend: u32,
    pub vendor_id: u32,
    pub device_id: u32,
    pub adapter_name: [c_char; 128],
}

struct BackendContext {
    backend: upstream_wgpu_native::InteropBackendContext,
    info: BabylonWgpuInfo,
}

impl BackendContext {
    fn publish_estimated_gpu_memory_bytes(&self) {
        ESTIMATED_GPU_MEMORY_BYTES
            .store(self.backend.estimated_gpu_memory_bytes(), Ordering::Relaxed);
    }

    fn install_debug_texture(
        &mut self,
        upload: &upstream_wgpu_native::DebugTextureUploadData,
    ) -> bool {
        self.backend
            .install_debug_texture(upload.width, upload.height, &upload.rgba)
    }

    fn apply_pending_debug_texture(&mut self) {
        if let Some(upload) = upstream_wgpu_native::take_pending_debug_texture_upload() {
            let applied = self.install_debug_texture(&upload);
            if !applied {
                set_last_error("Failed to install native debug texture upload.");
            } else {
                // A successful texture import confirms JS -> native interop traffic.
                // Keep presentation enabled even when JS-side draw markers are delayed.
                WEBGPU_DRAW_ENABLED.store(true, Ordering::Release);
            }
            self.publish_estimated_gpu_memory_bytes();
            upstream_wgpu_native::recycle_debug_texture_upload(upload);
        }
    }

    fn resize(&mut self, width: u32, height: u32) {
        self.backend.resize(width, height);
        self.publish_estimated_gpu_memory_bytes();
    }

    fn render(&mut self) {
        RENDER_FRAME_COUNTER.fetch_add(1, Ordering::Relaxed);
        self.apply_pending_debug_texture();
        let draw_enabled = WEBGPU_DRAW_ENABLED.load(Ordering::Acquire);
        if let Err(error) = self.backend.render(draw_enabled) {
            log_backend_error(&format!("Render submission failed: {error}"));
        }
    }
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_mark_webgpu_draw_requested() {
    WEBGPU_DRAW_ENABLED.store(true, Ordering::Release);
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_is_webgpu_draw_enabled() -> bool {
    WEBGPU_DRAW_ENABLED.load(Ordering::Acquire)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_render_frame_count() -> u64 {
    RENDER_FRAME_COUNTER.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_debug_texture_hash() -> u64 {
    upstream_wgpu_native::debug_texture_import_stats().hash
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_canvas_texture_hash() -> u64 {
    babylon_wgpu_get_debug_texture_hash()
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_debug_texture_width() -> u32 {
    upstream_wgpu_native::debug_texture_import_stats().width
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_canvas_texture_width() -> u32 {
    babylon_wgpu_get_debug_texture_width()
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_debug_texture_height() -> u32 {
    upstream_wgpu_native::debug_texture_import_stats().height
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_canvas_texture_height() -> u32 {
    babylon_wgpu_get_debug_texture_height()
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_estimated_gpu_memory_bytes() -> u64 {
    ESTIMATED_GPU_MEMORY_BYTES.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_debug_texture_import_skip_count() -> u64 {
    upstream_wgpu_native::debug_texture_import_stats().import_skip_count
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_canvas_texture_import_skip_count() -> u64 {
    babylon_wgpu_get_debug_texture_import_skip_count()
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_reset_webgpu_draw_requested() {
    WEBGPU_DRAW_ENABLED.store(false, Ordering::Release);
}

fn read_config_or_default(config: *const BabylonWgpuConfig) -> BabylonWgpuConfig {
    let config_ptr = config.cast::<c_void>();
    opaque_ptr_as_ref::<BabylonWgpuConfig>(config_ptr)
        .copied()
        .unwrap_or_else(default_config)
}

fn opaque_ptr_as_ref<'a, T>(opaque_ptr: *const c_void) -> Option<&'a T> {
    if opaque_ptr.is_null() {
        return None;
    }

    debug_assert!(
        opaque_ptr.align_offset(std::mem::align_of::<T>()) == 0,
        "opaque_ptr_as_ref: pointer is not properly aligned for {}",
        std::any::type_name::<T>()
    );

    // SAFETY: The caller guarantees `opaque_ptr` points to a valid `T` for the
    // duration of the borrow.
    unsafe { (opaque_ptr as *const T).as_ref() }
}

fn clear_debug_texture_uploads() {
    upstream_wgpu_native::clear_debug_texture_import_state();
}

fn import_canvas_texture_from_native(
    native_texture: *const c_void,
    width: u32,
    height: u32,
) -> bool {
    if native_texture.is_null() {
        return false;
    }

    let result = std::panic::catch_unwind(std::panic::AssertUnwindSafe(|| {
        upstream_wgpu_native::set_debug_texture_from_native(native_texture, width, height)
    }));
    match result {
        Ok(Ok(_stats)) => {
            // Treat native texture import as an active WebGPU draw-path signal.
            WEBGPU_DRAW_ENABLED.store(true, Ordering::Release);
            true
        }
        Ok(Err(error)) => {
            set_last_error(&format!("Failed to import native debug texture: {error}"));
            false
        }
        Err(_) => {
            set_last_error("Native debug texture import panicked.");
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_import_canvas_texture_from_native(
    native_texture: *const c_void,
    width: u32,
    height: u32,
) -> bool {
    import_canvas_texture_from_native(native_texture, width, height)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_set_debug_texture_from_native(
    native_texture: *const c_void,
    width: u32,
    height: u32,
) -> bool {
    // Back-compat alias retained for migration scripts and tests.
    import_canvas_texture_from_native(native_texture, width, height)
}

fn fill_adapter_name(name: &str) -> [c_char; 128] {
    let mut output = [0 as c_char; 128];
    let bytes = name.as_bytes();
    let max_count = output.len().saturating_sub(1);
    let copy_count = bytes.len().min(max_count);

    for (dst, src) in output.iter_mut().zip(bytes.iter()).take(copy_count) {
        *dst = *src as c_char;
    }

    output
}

fn format_upstream_wgpu_native_version(version: u32) -> Option<String> {
    if version == 0 {
        return None;
    }

    let major = (version >> 24) & 0xFF;
    let minor = (version >> 16) & 0xFF;
    let patch = (version >> 8) & 0xFF;
    let build = version & 0xFF;

    let formatted = if build != 0 {
        format!("{major}.{minor}.{patch}.{build}")
    } else if patch != 0 {
        format!("{major}.{minor}.{patch}")
    } else {
        format!("{major}.{minor}")
    };

    Some(formatted)
}

fn decorated_adapter_name(name: &str) -> String {
    let upstream_version = UPSTREAM_WGPU_NATIVE_VERSION.load(Ordering::Relaxed);
    if let Some(version_text) = format_upstream_wgpu_native_version(upstream_version) {
        return format!("{name} (wgpu-native {version_text})");
    }

    name.to_owned()
}

#[cfg(target_os = "android")]
fn log_backend_error(message: &str) {
    unsafe extern "C" {
        fn __android_log_write(prio: i32, tag: *const c_char, text: *const c_char) -> i32;
    }

    const ANDROID_LOG_ERROR: i32 = 6;
    if let (Ok(tag), Ok(text)) = (CString::new("BabylonNative"), CString::new(message)) {
        // SAFETY: Strings are NUL-terminated and valid for the call duration.
        unsafe {
            let _ = __android_log_write(ANDROID_LOG_ERROR, tag.as_ptr(), text.as_ptr());
        }
    }
}

#[cfg(not(target_os = "android"))]
fn log_backend_error(_message: &str) {}

fn set_last_error(message: &str) {
    let storage = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    let mut value = match storage.lock() {
        Ok(lock) => lock,
        Err(poisoned) => poisoned.into_inner(),
    };
    value.clear();
    value.push_str(message);

    log_backend_error(message);
}

fn clear_last_error() {
    let storage = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    let mut value = match storage.lock() {
        Ok(lock) => lock,
        Err(poisoned) => poisoned.into_inner(),
    };
    value.clear();
}

fn panic_payload_to_string(payload: &(dyn Any + Send)) -> String {
    if let Some(message) = payload.downcast_ref::<&str>() {
        return (*message).to_owned();
    }
    if let Some(message) = payload.downcast_ref::<String>() {
        return message.clone();
    }

    "non-string panic payload".to_owned()
}

fn copy_last_error(output: *mut c_char, output_len: usize) -> bool {
    if output.is_null() || output_len == 0 {
        return false;
    }

    let storage = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    let message = match storage.lock() {
        Ok(value) if !value.is_empty() => value,
        Err(poisoned) => {
            let value = poisoned.into_inner();
            if value.is_empty() {
                return false;
            }
            value
        }
        _ => return false,
    };

    let bytes = message.as_bytes();
    let max_copy = output_len.saturating_sub(1);
    let copy_len = bytes.len().min(max_copy);

    // SAFETY: Caller provides a valid writable output buffer with `output_len`
    // bytes by C ABI contract.
    let output_slice = unsafe { std::slice::from_raw_parts_mut(output.cast::<u8>(), output_len) };
    output_slice[..copy_len].copy_from_slice(&bytes[..copy_len]);
    output_slice[copy_len] = 0;

    true
}

fn create_context(config: BabylonWgpuConfig) -> Result<Box<BackendContext>, String> {
    let backend = upstream_wgpu_native::InteropBackendContext::create(
        upstream_wgpu_native::LocalBootstrapConfig {
            width: config.width.max(1),
            height: config.height.max(1),
            surface_layer: config.surface_layer,
            prefer_low_power: config.prefer_low_power != 0,
        },
    )?;

    if backend.used_fallback_adapter() {
        log_backend_error(
            "No hardware Vulkan adapter found; continuing with fallback Vulkan adapter.",
        );
    }
    let adapter_info = backend.resolved_adapter_info();

    let context_info = BabylonWgpuInfo {
        backend: adapter_info.backend,
        vendor_id: adapter_info.vendor_id,
        device_id: adapter_info.device_id,
        adapter_name: fill_adapter_name(
            decorated_adapter_name(adapter_info.adapter_name.as_str()).as_str(),
        ),
    };

    let context = BackendContext {
        backend,
        info: context_info,
    };

    context.publish_estimated_gpu_memory_bytes();
    Ok(Box::new(context))
}

fn dispatch_compute_global(shader_source: &str, entry_point: &str, x: u32, y: u32, z: u32) -> bool {
    if let Err(error) =
        upstream_wgpu_native::dispatch_compute_global(shader_source, entry_point, x, y, z, false)
    {
        log_backend_error(&format!(
            "upstream wgpu-native compute dispatch failed: {error}"
        ));
        return false;
    }

    true
}

fn default_config() -> BabylonWgpuConfig {
    BabylonWgpuConfig {
        width: 1,
        height: 1,
        surface_layer: ptr::null_mut(),
        prefer_low_power: 0,
        enable_validation: 0,
        _reserved0: 0,
        _reserved1: 0,
    }
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_create(config: *const BabylonWgpuConfig) -> *mut c_void {
    clear_last_error();
    clear_debug_texture_uploads();

    let upstream_version = upstream_wgpu_native::version();
    UPSTREAM_WGPU_NATIVE_VERSION.store(upstream_version, Ordering::Relaxed);
    if let Some(version_text) = format_upstream_wgpu_native_version(upstream_version) {
        log_backend_error(&format!(
            "GraphicsWgpu upstream probe active: wgpu-native {version_text}"
        ));
    }

    let config_value = read_config_or_default(config);

    let result = std::panic::catch_unwind(|| create_context(config_value));
    match result {
        Ok(Ok(context)) => Box::into_raw(context) as *mut c_void,
        Ok(Err(error)) => {
            set_last_error(&error);
            ptr::null_mut()
        }
        Err(payload) => {
            set_last_error(
                format!(
                    "WGPU backend initialization panicked: {}",
                    panic_payload_to_string(payload.as_ref())
                )
                .as_str(),
            );
            ptr::null_mut()
        }
    }
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_destroy(context: *mut c_void) {
    if context.is_null() {
        return;
    }

    // SAFETY: The pointer was allocated by babylon_wgpu_create and is owned by the caller.
    unsafe {
        drop(Box::from_raw(context as *mut BackendContext));
    }
    clear_debug_texture_uploads();
    ESTIMATED_GPU_MEMORY_BYTES.store(0, Ordering::Relaxed);
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_resize(context: *mut c_void, width: u32, height: u32) -> bool {
    if context.is_null() {
        set_last_error("WGPU resize received null backend context pointer.");
        return false;
    }

    let result = std::panic::catch_unwind(std::panic::AssertUnwindSafe(|| {
        // SAFETY: `context` comes from `babylon_wgpu_create` and remains exclusively
        // owned by the caller for the duration of this call.
        let context_ref = unsafe { &mut *(context as *mut BackendContext) };
        context_ref.resize(width, height);
    }));

    match result {
        Ok(()) => true,
        Err(payload) => {
            set_last_error(
                format!(
                    "WGPU resize panicked: {}",
                    panic_payload_to_string(payload.as_ref())
                )
                .as_str(),
            );
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_render(context: *mut c_void) -> bool {
    if context.is_null() {
        set_last_error("WGPU render received null backend context pointer.");
        return false;
    }

    let result = std::panic::catch_unwind(std::panic::AssertUnwindSafe(|| {
        // SAFETY: `context` comes from `babylon_wgpu_create` and remains valid for
        // this call by C ABI contract.
        let context_ref = unsafe { &mut *(context as *mut BackendContext) };
        context_ref.render();
    }));

    match result {
        Ok(()) => true,
        Err(payload) => {
            set_last_error(
                format!(
                    "WGPU render panicked: {}",
                    panic_payload_to_string(payload.as_ref())
                )
                .as_str(),
            );
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_info(
    context: *const c_void,
    output_info: *mut BabylonWgpuInfo,
) -> bool {
    if output_info.is_null() {
        return false;
    }

    if context.is_null() {
        set_last_error("WGPU info requested with null backend context pointer.");
        return false;
    }

    // This interop layer is trusted app code (not untrusted web content), so
    // we keep ABI checks minimal and rely on upstream/runtime validation for
    // deeper invariants that are covered by WebGPU conformance paths.
    // SAFETY: pointers come from the C++ layer and are expected to remain valid.
    let context_ref = unsafe { &*(context as *const BackendContext) };
    // SAFETY: caller provides a valid writable output pointer.
    let output_info_ref = unsafe { &mut *output_info };
    *output_info_ref = context_ref.info;
    true
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_last_error(output: *mut c_char, output_len: usize) -> bool {
    copy_last_error(output, output_len)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_dispatch_compute_global(
    shader_source: *const c_char,
    entry_point: *const c_char,
    x: u32,
    y: u32,
    z: u32,
) -> bool {
    if shader_source.is_null() {
        set_last_error("Compute dispatch shader source pointer was null.");
        return false;
    }
    if entry_point.is_null() {
        set_last_error("Compute dispatch entry point pointer was null.");
        return false;
    }

    // Keep conversion strict for deterministic diagnostics while avoiding
    // extra wrapper layers that duplicate downstream validation.
    // SAFETY: pointers are expected to reference NUL-terminated strings.
    let shader = match unsafe { CStr::from_ptr(shader_source) }.to_str() {
        Ok(value) => value,
        Err(_) => {
            set_last_error("Compute dispatch shader source was not valid UTF-8.");
            return false;
        }
    };
    // SAFETY: pointers are expected to reference NUL-terminated strings.
    let entry = match unsafe { CStr::from_ptr(entry_point) }.to_str() {
        Ok(value) => value,
        Err(_) => {
            set_last_error("Compute dispatch entry point was not valid UTF-8.");
            return false;
        }
    };

    let result = std::panic::catch_unwind(|| dispatch_compute_global(shader, entry, x, y, z));
    match result {
        Ok(value) => value,
        Err(payload) => {
            set_last_error(
                format!(
                    "Compute dispatch panicked: {}",
                    panic_payload_to_string(payload.as_ref())
                )
                .as_str(),
            );
            false
        }
    }
}

mod upstream_wgpu_native {
    use super::opaque_ptr_as_ref;
    use bytemuck::{Pod, Zeroable};
    use std::ffi::c_void;
    use std::sync::atomic::{AtomicBool, Ordering};
    use std::sync::{Mutex, OnceLock};
    use wgpu::util::DeviceExt;

    #[derive(Clone, Debug)]
    pub struct AdapterProbeInfo {
        pub backend: u32,
        pub vendor_id: u32,
        pub device_id: u32,
        pub adapter_name: String,
    }

    pub struct LocalBootstrapRuntime {
        pub adapter: wgpu::Adapter,
        pub adapter_info: wgpu::AdapterInfo,
        pub limits: wgpu::Limits,
        pub device: wgpu::Device,
        pub queue: wgpu::Queue,
        pub used_fallback_adapter: bool,
    }

    pub struct LocalBootstrapConfig {
        pub width: u32,
        pub height: u32,
        pub surface_layer: *mut c_void,
        pub prefer_low_power: bool,
    }

    pub struct LocalRuntimeState {
        pub device: wgpu::Device,
        pub queue: wgpu::Queue,
        pub surface: Option<wgpu::Surface<'static>>,
        pub surface_config: Option<wgpu::SurfaceConfiguration>,
        pub resolved_adapter_info: AdapterProbeInfo,
        pub max_texture_dimension_2d: u32,
        pub width: u32,
        pub height: u32,
        pub render_target_format: wgpu::TextureFormat,
        pub used_fallback_adapter: bool,
        pub surface_acquire_failures: u32,
    }

    #[repr(C)]
    #[derive(Clone, Copy)]
    pub struct CanvasNativeTextureHandle {
        pub texture: *const c_void,
        pub device: *const c_void,
        pub queue: *const c_void,
        pub width: u32,
        pub height: u32,
        pub generation: u64,
    }

    pub struct DebugCubeRenderer {
        offscreen_texture: Option<wgpu::Texture>,
        offscreen_view: Option<wgpu::TextureView>,
        depth_texture: wgpu::Texture,
        depth_view: wgpu::TextureView,
        render_pipeline: wgpu::RenderPipeline,
        uniform_bind_group_layout: wgpu::BindGroupLayout,
        uniform_buffer: wgpu::Buffer,
        uniform_buffer_size: u64,
        uniform_bind_group: wgpu::BindGroup,
        canvas_sampler: wgpu::Sampler,
        canvas_texture: wgpu::Texture,
        canvas_texture_view: wgpu::TextureView,
        canvas_texture_width: u32,
        canvas_texture_height: u32,
        vertex_buffer: wgpu::Buffer,
        vertex_buffer_size: u64,
        index_buffer: wgpu::Buffer,
        index_buffer_size: u64,
        index_count: u32,
        width: u32,
        height: u32,
        frame_index: u64,
    }

    pub struct InteropBackendContext {
        runtime: LocalRuntimeState,
        renderer: DebugCubeRenderer,
    }

    impl InteropBackendContext {
        pub fn create(config: LocalBootstrapConfig) -> Result<Self, String> {
            let runtime = LocalRuntimeState::bootstrap(config)?;
            let renderer = create_default_debug_cube_renderer(&runtime);
            Ok(Self { runtime, renderer })
        }

        pub fn resolved_adapter_info(&self) -> &AdapterProbeInfo {
            &self.runtime.resolved_adapter_info
        }

        pub fn used_fallback_adapter(&self) -> bool {
            self.runtime.used_fallback_adapter
        }

        pub fn estimated_gpu_memory_bytes(&self) -> u64 {
            self.renderer.estimated_gpu_memory_bytes_base(&self.runtime)
        }

        pub fn install_debug_texture(&mut self, width: u32, height: u32, rgba: &[u8]) -> bool {
            self.renderer
                .install_debug_texture(&self.runtime, width, height, rgba)
        }

        pub fn resize(&mut self, width: u32, height: u32) {
            self.renderer.resize(&mut self.runtime, width, height);
        }

        pub fn render(&mut self, draw_enabled: bool) -> Result<(), String> {
            self.renderer.render(&mut self.runtime, draw_enabled)
        }
    }

    #[derive(Default)]
    struct NativeReadbackCache {
        device_key: usize,
        size: u64,
        buffer: Option<wgpu::Buffer>,
    }

    static NATIVE_READBACK_CACHE: OnceLock<Mutex<NativeReadbackCache>> = OnceLock::new();
    static NATIVE_READBACK_IMPORT_GUARD: OnceLock<Mutex<()>> = OnceLock::new();
    static DEBUG_TEXTURE_IMPORT_STATE: OnceLock<Mutex<DebugTextureImportState>> = OnceLock::new();
    static DEBUG_TEXTURE_UPLOAD_PENDING: AtomicBool = AtomicBool::new(false);

    fn native_readback_cache() -> &'static Mutex<NativeReadbackCache> {
        NATIVE_READBACK_CACHE.get_or_init(|| Mutex::new(NativeReadbackCache::default()))
    }

    fn native_readback_import_guard() -> &'static Mutex<()> {
        NATIVE_READBACK_IMPORT_GUARD.get_or_init(|| Mutex::new(()))
    }

    #[derive(Clone, Copy, PartialEq, Eq)]
    struct DebugTextureSourceSignature {
        texture_ptr: usize,
        width: u32,
        height: u32,
        generation: u64,
    }

    #[derive(Default)]
    pub struct DebugTextureUploadData {
        pub width: u32,
        pub height: u32,
        pub rgba: Vec<u8>,
    }

    #[derive(Clone, Copy, Default)]
    pub struct DebugTextureImportStats {
        pub hash: u64,
        pub width: u32,
        pub height: u32,
        pub import_skip_count: u64,
    }

    #[derive(Default)]
    struct DebugTextureImportState {
        source_signature: Option<DebugTextureSourceSignature>,
        pending: Option<DebugTextureUploadData>,
        reusable: Option<DebugTextureUploadData>,
        stats: DebugTextureImportStats,
    }

    fn debug_texture_import_state() -> &'static Mutex<DebugTextureImportState> {
        DEBUG_TEXTURE_IMPORT_STATE.get_or_init(|| Mutex::new(DebugTextureImportState::default()))
    }

    fn hash_bytes(bytes: &[u8]) -> u64 {
        // 64-bit FNV-1a keeps this lightweight and deterministic across platforms.
        let mut hash = 0xcbf29ce484222325u64;
        for byte in bytes {
            hash ^= u64::from(*byte);
            hash = hash.wrapping_mul(0x100000001b3);
        }
        hash
    }

    fn make_debug_texture_source_signature(
        native_texture: *const c_void,
        width: u32,
        height: u32,
    ) -> DebugTextureSourceSignature {
        let mut texture_ptr = native_texture as usize;
        let mut generation = 0u64;

        if let Some(handle) = opaque_ptr_as_ref::<CanvasNativeTextureHandle>(native_texture) {
            if !handle.texture.is_null() {
                texture_ptr = handle.texture as usize;
            }

            generation = handle.generation;
        }

        DebugTextureSourceSignature {
            texture_ptr,
            width: width.max(1),
            height: height.max(1),
            generation,
        }
    }

    pub fn clear_debug_texture_import_state() {
        let mut state = match debug_texture_import_state().lock() {
            Ok(lock) => lock,
            Err(poisoned) => poisoned.into_inner(),
        };
        state.source_signature = None;
        state.pending = None;
        state.reusable = None;
        state.stats = DebugTextureImportStats::default();
        DEBUG_TEXTURE_UPLOAD_PENDING.store(false, Ordering::Release);
    }

    pub fn debug_texture_import_stats() -> DebugTextureImportStats {
        let state = match debug_texture_import_state().lock() {
            Ok(lock) => lock,
            Err(poisoned) => poisoned.into_inner(),
        };
        state.stats
    }

    pub fn take_pending_debug_texture_upload() -> Option<DebugTextureUploadData> {
        if !DEBUG_TEXTURE_UPLOAD_PENDING.load(Ordering::Acquire) {
            return None;
        }

        let mut state = match debug_texture_import_state().lock() {
            Ok(lock) => lock,
            Err(poisoned) => poisoned.into_inner(),
        };
        let pending = state.pending.take();
        if pending.is_none() {
            DEBUG_TEXTURE_UPLOAD_PENDING.store(false, Ordering::Release);
        }
        pending
    }

    pub fn recycle_debug_texture_upload(upload: DebugTextureUploadData) {
        let mut state = match debug_texture_import_state().lock() {
            Ok(lock) => lock,
            Err(poisoned) => poisoned.into_inner(),
        };
        state.reusable = Some(upload);
    }

    pub fn set_debug_texture_from_native(
        native_texture: *const c_void,
        width: u32,
        height: u32,
    ) -> Result<DebugTextureImportStats, String> {
        if native_texture.is_null() {
            return Err("native texture handle pointer was null".to_string());
        }

        let signature = make_debug_texture_source_signature(native_texture, width, height);
        let mut upload = {
            let mut state = debug_texture_import_state()
                .lock()
                .map_err(|_| "debug texture import state lock poisoned".to_string())?;

            if state
                .source_signature
                .as_ref()
                .is_some_and(|last| *last == signature)
            {
                state.stats.import_skip_count = state.stats.import_skip_count.saturating_add(1);
                return Ok(state.stats);
            }

            state
                .pending
                .take()
                .or_else(|| state.reusable.take())
                .unwrap_or_default()
        };

        let (imported_width, imported_height) = import_native_texture_rgba_into(
            native_texture,
            signature.width,
            signature.height,
            &mut upload.rgba,
        )?;

        upload.width = imported_width;
        upload.height = imported_height;

        let mut state = debug_texture_import_state()
            .lock()
            .map_err(|_| "debug texture import state lock poisoned".to_string())?;
        let hash = hash_bytes(&upload.rgba);
        state.stats.hash = hash;
        state.stats.width = upload.width;
        state.stats.height = upload.height;
        state.source_signature = Some(signature);
        if let Some(previous) = state.pending.replace(upload) {
            state.reusable = Some(previous);
        }
        DEBUG_TEXTURE_UPLOAD_PENDING.store(true, Ordering::Release);

        Ok(state.stats)
    }

    fn acquire_native_readback_buffer(
        source_device: &wgpu::Device,
        required_size: u64,
    ) -> Result<wgpu::Buffer, String> {
        let mut cache = native_readback_cache()
            .lock()
            .map_err(|_| "native readback cache lock poisoned".to_string())?;

        let device_key = (source_device as *const wgpu::Device) as usize;
        let needs_rebuild =
            cache.device_key != device_key || cache.size < required_size || cache.buffer.is_none();

        if needs_rebuild {
            cache.buffer = Some(source_device.create_buffer(&wgpu::BufferDescriptor {
                label: Some("babylon-native-webgpu.native-debug-readback"),
                size: required_size,
                usage: wgpu::BufferUsages::COPY_DST | wgpu::BufferUsages::MAP_READ,
                mapped_at_creation: false,
            }));
            cache.device_key = device_key;
            cache.size = required_size;
        }

        cache
            .buffer
            .as_ref()
            .cloned()
            .ok_or_else(|| "native readback buffer allocation failed".to_string())
    }

    fn align_to(value: u32, alignment: u32) -> u32 {
        if alignment <= 1 {
            return value;
        }

        let remainder = value % alignment;
        if remainder == 0 {
            value
        } else {
            value.saturating_add(alignment - remainder)
        }
    }

    fn import_native_texture_rgba_inner(
        native_texture: *const c_void,
        requested_width: u32,
        requested_height: u32,
        rgba: &mut Vec<u8>,
    ) -> Result<(u32, u32), String> {
        let _import_guard = native_readback_import_guard()
            .lock()
            .map_err(|_| "native texture import lock poisoned".to_string())?;

        if native_texture.is_null() {
            return Err("native texture handle pointer was null".to_string());
        }

        let native_handle = opaque_ptr_as_ref::<CanvasNativeTextureHandle>(native_texture)
            .ok_or_else(|| "native texture handle pointer was invalid".to_string())?;
        let source_texture = opaque_ptr_as_ref::<wgpu::Texture>(native_handle.texture)
            .ok_or_else(|| "native texture pointer was invalid".to_string())?;
        let source_device = opaque_ptr_as_ref::<wgpu::Device>(native_handle.device)
            .ok_or_else(|| "native device pointer was invalid".to_string())?;
        let source_queue = opaque_ptr_as_ref::<wgpu::Queue>(native_handle.queue)
            .ok_or_else(|| "native queue pointer was invalid".to_string())?;

        let width = if requested_width == 0 {
            native_handle.width
        } else {
            requested_width
        }
        .max(1);
        let height = if requested_height == 0 {
            native_handle.height
        } else {
            requested_height
        }
        .max(1);

        let unpadded_bytes_per_row = width.saturating_mul(4);
        if unpadded_bytes_per_row == 0 {
            return Err("invalid native texture width".to_string());
        }
        let padded_bytes_per_row =
            align_to(unpadded_bytes_per_row, wgpu::COPY_BYTES_PER_ROW_ALIGNMENT);
        let buffer_size = (padded_bytes_per_row as u64).saturating_mul(height as u64);
        if buffer_size == 0 {
            return Err("invalid native texture size".to_string());
        }

        let staging_buffer = acquire_native_readback_buffer(source_device, buffer_size)?;

        let mut encoder = source_device.create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("babylon-native-webgpu.native-debug-readback-encoder"),
        });
        encoder.copy_texture_to_buffer(
            wgpu::TexelCopyTextureInfo {
                texture: source_texture,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
                aspect: wgpu::TextureAspect::All,
            },
            wgpu::TexelCopyBufferInfo {
                buffer: &staging_buffer,
                layout: wgpu::TexelCopyBufferLayout {
                    offset: 0,
                    bytes_per_row: Some(padded_bytes_per_row),
                    rows_per_image: Some(height),
                },
            },
            wgpu::Extent3d {
                width,
                height,
                depth_or_array_layers: 1,
            },
        );
        source_queue.submit(Some(encoder.finish()));

        let slice = staging_buffer.slice(..);
        let (tx, rx) = std::sync::mpsc::channel::<Result<(), String>>();
        slice.map_async(wgpu::MapMode::Read, move |result| {
            let _ = tx.send(result.map_err(|error| error.to_string()));
        });

        source_device
            .poll(wgpu::PollType::wait_indefinitely())
            .map_err(|error| format!("native texture poll failed: {error}"))?;
        match rx.recv() {
            Ok(Ok(())) => {}
            Ok(Err(error)) => return Err(format!("native texture map_async failed: {error}")),
            Err(error) => return Err(format!("native texture map_async channel failed: {error}")),
        }

        let mapped = slice.get_mapped_range();
        let expected_len = (width as usize)
            .saturating_mul(height as usize)
            .saturating_mul(4);
        rgba.resize(expected_len, 0);
        for row in 0..(height as usize) {
            let src_start = row.saturating_mul(padded_bytes_per_row as usize);
            let src_end = src_start.saturating_add(unpadded_bytes_per_row as usize);
            let dst_start = row.saturating_mul(unpadded_bytes_per_row as usize);
            let dst_end = dst_start.saturating_add(unpadded_bytes_per_row as usize);
            rgba[dst_start..dst_end].copy_from_slice(&mapped[src_start..src_end]);
        }
        drop(mapped);
        staging_buffer.unmap();

        Ok((width, height))
    }

    pub fn import_native_texture_rgba_into(
        native_texture: *const c_void,
        requested_width: u32,
        requested_height: u32,
        rgba: &mut Vec<u8>,
    ) -> Result<(u32, u32), String> {
        import_native_texture_rgba_inner(native_texture, requested_width, requested_height, rgba)
    }

    #[repr(C)]
    #[derive(Clone, Copy, Pod, Zeroable)]
    struct DebugCubeVertex {
        position: [f32; 3],
        color: [f32; 3],
        uv: [f32; 2],
        face_id: u32,
    }

    const DEBUG_CUBE_SHADER_WGSL: &str = r#"
    struct Uniforms {
        mvp: mat4x4<f32>,
    };
    
    @group(0) @binding(0)
    var<uniform> uniforms: Uniforms;
    
    struct VertexIn {
        @location(0) position: vec3<f32>,
        @location(1) color: vec3<f32>,
        @location(2) uv: vec2<f32>,
        @location(3) face_id: u32,
    };
    
    struct VertexOut {
        @builtin(position) position: vec4<f32>,
        @location(0) color: vec3<f32>,
        @location(1) uv: vec2<f32>,
        @location(2) @interpolate(flat) face_id: u32,
    };
    
    @group(0) @binding(1)
    var canvas_sampler: sampler;
    
    @group(0) @binding(2)
    var canvas_texture: texture_2d<f32>;
    
    @vertex
    fn vs_main(input: VertexIn) -> VertexOut {
        var output: VertexOut;
        output.position = uniforms.mvp * vec4<f32>(input.position, 1.0);
        output.color = input.color;
        output.uv = input.uv;
        output.face_id = input.face_id;
        return output;
    }
    
    @fragment
    fn fs_main(input: VertexOut) -> @location(0) vec4<f32> {
        let sampled_uv = vec2<f32>(1.0 - input.uv.x, input.uv.y);
        let sampled = textureSample(canvas_texture, canvas_sampler, sampled_uv);
        if (input.face_id == 1u) {
            return sampled;
        }
        return mix(vec4<f32>(input.color, 1.0), sampled, 0.35);
    }
    "#;

    const DEBUG_CUBE_INDICES: [u16; 36] = [
        0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17,
        18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
    ];

    fn build_debug_cube_vertices() -> [DebugCubeVertex; 24] {
        const POSITIONS: [[f32; 3]; 8] = [
            [-1.0, -1.0, -1.0],
            [1.0, -1.0, -1.0],
            [1.0, 1.0, -1.0],
            [-1.0, 1.0, -1.0],
            [-1.0, -1.0, 1.0],
            [1.0, -1.0, 1.0],
            [1.0, 1.0, 1.0],
            [-1.0, 1.0, 1.0],
        ];
        const UVS: [[f32; 2]; 4] = [[0.0, 1.0], [1.0, 1.0], [1.0, 0.0], [0.0, 0.0]];
        const FACE_LAYOUT: [([usize; 4], [f32; 3], u32); 6] = [
            ([0, 1, 2, 3], [1.0, 0.2, 0.2], 0),    // back
            ([4, 5, 6, 7], [1.0, 0.2, 1.0], 1),    // front textured
            ([0, 3, 7, 4], [0.95, 0.25, 0.25], 2), // left
            ([1, 5, 6, 2], [0.25, 0.95, 0.25], 3), // right
            ([3, 2, 6, 7], [0.25, 0.25, 0.95], 4), // top
            ([0, 4, 5, 1], [0.95, 0.95, 0.25], 5), // bottom
        ];

        let mut vertices = [DebugCubeVertex::zeroed(); 24];
        for (face_index, (corners, color, face_id)) in FACE_LAYOUT.iter().enumerate() {
            let base = face_index * 4;
            for corner_index in 0..4 {
                vertices[base + corner_index] = DebugCubeVertex {
                    position: POSITIONS[corners[corner_index]],
                    color: *color,
                    uv: UVS[corner_index],
                    face_id: *face_id,
                };
            }
        }
        vertices
    }

    pub fn create_default_debug_cube_renderer(runtime: &LocalRuntimeState) -> DebugCubeRenderer {
        let vertices = build_debug_cube_vertices();
        DebugCubeRenderer::new(
            runtime,
            DEBUG_CUBE_SHADER_WGSL,
            std::mem::size_of::<[f32; 16]>() as u64,
            bytemuck::cast_slice(&vertices),
            std::mem::size_of::<DebugCubeVertex>() as u64,
            bytemuck::cast_slice(&DEBUG_CUBE_INDICES),
            DEBUG_CUBE_INDICES.len() as u32,
        )
    }

    fn create_canvas_texture_with_view(
        device: &wgpu::Device,
        width: u32,
        height: u32,
    ) -> (wgpu::Texture, wgpu::TextureView) {
        let texture = device.create_texture(&wgpu::TextureDescriptor {
            label: Some("babylon-native-webgpu.canvas-texture"),
            size: wgpu::Extent3d {
                width,
                height,
                depth_or_array_layers: 1,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8Unorm,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        });
        let view = texture.create_view(&wgpu::TextureViewDescriptor {
            label: Some("babylon-native-webgpu.canvas-texture-view"),
            ..Default::default()
        });
        (texture, view)
    }

    pub fn create_debug_cube_bind_group(
        device: &wgpu::Device,
        layout: &wgpu::BindGroupLayout,
        uniform_buffer: &wgpu::Buffer,
        canvas_sampler: &wgpu::Sampler,
        canvas_texture_view: &wgpu::TextureView,
    ) -> wgpu::BindGroup {
        device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("babylon-native-webgpu.uniform-bind-group"),
            layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: uniform_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(canvas_sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(canvas_texture_view),
                },
            ],
        })
    }

    fn create_debug_cube_bind_group_layout(device: &wgpu::Device) -> wgpu::BindGroupLayout {
        device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("babylon-native-webgpu.uniform-bind-group-layout"),
            entries: &[
                wgpu::BindGroupLayoutEntry {
                    binding: 0,
                    visibility: wgpu::ShaderStages::VERTEX,
                    ty: wgpu::BindingType::Buffer {
                        ty: wgpu::BufferBindingType::Uniform,
                        has_dynamic_offset: false,
                        min_binding_size: None,
                    },
                    count: None,
                },
                wgpu::BindGroupLayoutEntry {
                    binding: 1,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Sampler(wgpu::SamplerBindingType::Filtering),
                    count: None,
                },
                wgpu::BindGroupLayoutEntry {
                    binding: 2,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Texture {
                        sample_type: wgpu::TextureSampleType::Float { filterable: true },
                        view_dimension: wgpu::TextureViewDimension::D2,
                        multisampled: false,
                    },
                    count: None,
                },
            ],
        })
    }

    fn bytes_per_pixel(format: wgpu::TextureFormat) -> u64 {
        match format {
            wgpu::TextureFormat::Rgba8Unorm
            | wgpu::TextureFormat::Rgba8UnormSrgb
            | wgpu::TextureFormat::Bgra8Unorm
            | wgpu::TextureFormat::Bgra8UnormSrgb
            | wgpu::TextureFormat::Depth24Plus
            | wgpu::TextureFormat::Depth24PlusStencil8
            | wgpu::TextureFormat::Depth32Float => 4,
            wgpu::TextureFormat::Rgba16Float => 8,
            _ => 4,
        }
    }

    fn estimated_texture_bytes(width: u32, height: u32, format: wgpu::TextureFormat) -> u64 {
        u64::from(width.max(1))
            .saturating_mul(u64::from(height.max(1)))
            .saturating_mul(bytes_per_pixel(format))
    }

    fn mul_matrix(a: [f32; 16], b: [f32; 16]) -> [f32; 16] {
        let mut out = [0.0f32; 16];
        for col in 0..4 {
            for row in 0..4 {
                out[col * 4 + row] = a[row] * b[col * 4]
                    + a[4 + row] * b[col * 4 + 1]
                    + a[8 + row] * b[col * 4 + 2]
                    + a[12 + row] * b[col * 4 + 3];
            }
        }

        out
    }

    fn translation_matrix(x: f32, y: f32, z: f32) -> [f32; 16] {
        [
            1.0, 0.0, 0.0, 0.0, // col 0
            0.0, 1.0, 0.0, 0.0, // col 1
            0.0, 0.0, 1.0, 0.0, // col 2
            x, y, z, 1.0, // col 3
        ]
    }

    fn rotation_x_matrix(angle: f32) -> [f32; 16] {
        let c = angle.cos();
        let s = angle.sin();

        [
            1.0, 0.0, 0.0, 0.0, // col 0
            0.0, c, s, 0.0, // col 1
            0.0, -s, c, 0.0, // col 2
            0.0, 0.0, 0.0, 1.0, // col 3
        ]
    }

    fn rotation_y_matrix(angle: f32) -> [f32; 16] {
        let c = angle.cos();
        let s = angle.sin();

        [
            c, 0.0, -s, 0.0, // col 0
            0.0, 1.0, 0.0, 0.0, // col 1
            s, 0.0, c, 0.0, // col 2
            0.0, 0.0, 0.0, 1.0, // col 3
        ]
    }

    fn perspective_rh_zo(fovy: f32, aspect: f32, near: f32, far: f32) -> [f32; 16] {
        let f = 1.0 / (fovy * 0.5).tan();

        [
            f / aspect,
            0.0,
            0.0,
            0.0, // col 0
            0.0,
            f,
            0.0,
            0.0, // col 1
            0.0,
            0.0,
            far / (near - far),
            -1.0, // col 2
            0.0,
            0.0,
            (near * far) / (near - far),
            0.0, // col 3
        ]
    }

    impl DebugCubeRenderer {
        pub fn new(
            runtime: &LocalRuntimeState,
            shader_source: &str,
            uniform_buffer_size: u64,
            vertex_data: &[u8],
            vertex_stride: u64,
            index_data: &[u8],
            index_count: u32,
        ) -> Self {
            let (offscreen_texture, offscreen_view) = if runtime.surface.is_none() {
                let (texture, view) =
                    runtime.create_offscreen_target(runtime.width, runtime.height);
                (Some(texture), Some(view))
            } else {
                (None, None)
            };

            let (depth_texture, depth_view) =
                runtime.create_depth_target(runtime.width, runtime.height);

            let shader = runtime
                .device
                .create_shader_module(wgpu::ShaderModuleDescriptor {
                    label: Some("babylon-native-webgpu.cube-shader"),
                    source: wgpu::ShaderSource::Wgsl(shader_source.into()),
                });
            let uniform_buffer = runtime.device.create_buffer(&wgpu::BufferDescriptor {
                label: Some("babylon-native-webgpu.uniform-buffer"),
                size: uniform_buffer_size,
                usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
                mapped_at_creation: false,
            });
            let canvas_sampler = runtime.device.create_sampler(&wgpu::SamplerDescriptor {
                label: Some("babylon-native-webgpu.canvas-sampler"),
                address_mode_u: wgpu::AddressMode::ClampToEdge,
                address_mode_v: wgpu::AddressMode::ClampToEdge,
                address_mode_w: wgpu::AddressMode::ClampToEdge,
                mag_filter: wgpu::FilterMode::Linear,
                min_filter: wgpu::FilterMode::Linear,
                mipmap_filter: wgpu::FilterMode::Linear,
                ..Default::default()
            });
            let (canvas_texture, canvas_texture_view) =
                create_canvas_texture_with_view(&runtime.device, 1, 1);
            runtime.queue.write_texture(
                wgpu::TexelCopyTextureInfo {
                    texture: &canvas_texture,
                    mip_level: 0,
                    origin: wgpu::Origin3d::ZERO,
                    aspect: wgpu::TextureAspect::All,
                },
                &[255u8, 255u8, 255u8, 255u8],
                wgpu::TexelCopyBufferLayout {
                    offset: 0,
                    bytes_per_row: Some(4),
                    rows_per_image: Some(1),
                },
                wgpu::Extent3d {
                    width: 1,
                    height: 1,
                    depth_or_array_layers: 1,
                },
            );
            let uniform_bind_group_layout = create_debug_cube_bind_group_layout(&runtime.device);
            let uniform_bind_group = create_debug_cube_bind_group(
                &runtime.device,
                &uniform_bind_group_layout,
                &uniform_buffer,
                &canvas_sampler,
                &canvas_texture_view,
            );
            let pipeline_layout =
                runtime
                    .device
                    .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                        label: Some("babylon-native-webgpu.pipeline-layout"),
                        bind_group_layouts: &[&uniform_bind_group_layout],
                        push_constant_ranges: &[],
                    });
            let vertex_buffer =
                runtime
                    .device
                    .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                        label: Some("babylon-native-webgpu.vertex-buffer"),
                        contents: vertex_data,
                        usage: wgpu::BufferUsages::VERTEX,
                    });
            let index_buffer =
                runtime
                    .device
                    .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                        label: Some("babylon-native-webgpu.index-buffer"),
                        contents: index_data,
                        usage: wgpu::BufferUsages::INDEX,
                    });
            let vertex_attributes = [
                wgpu::VertexAttribute {
                    offset: 0,
                    shader_location: 0,
                    format: wgpu::VertexFormat::Float32x3,
                },
                wgpu::VertexAttribute {
                    offset: 12,
                    shader_location: 1,
                    format: wgpu::VertexFormat::Float32x3,
                },
                wgpu::VertexAttribute {
                    offset: 24,
                    shader_location: 2,
                    format: wgpu::VertexFormat::Float32x2,
                },
                wgpu::VertexAttribute {
                    offset: 32,
                    shader_location: 3,
                    format: wgpu::VertexFormat::Uint32,
                },
            ];
            let render_pipeline =
                runtime
                    .device
                    .create_render_pipeline(&wgpu::RenderPipelineDescriptor {
                        label: Some("babylon-native-webgpu.cube-pipeline"),
                        layout: Some(&pipeline_layout),
                        vertex: wgpu::VertexState {
                            module: &shader,
                            entry_point: Some("vs_main"),
                            compilation_options: wgpu::PipelineCompilationOptions::default(),
                            buffers: &[wgpu::VertexBufferLayout {
                                array_stride: vertex_stride,
                                step_mode: wgpu::VertexStepMode::Vertex,
                                attributes: &vertex_attributes,
                            }],
                        },
                        primitive: wgpu::PrimitiveState {
                            topology: wgpu::PrimitiveTopology::TriangleList,
                            strip_index_format: None,
                            front_face: wgpu::FrontFace::Ccw,
                            cull_mode: None,
                            unclipped_depth: false,
                            polygon_mode: wgpu::PolygonMode::Fill,
                            conservative: false,
                        },
                        depth_stencil: Some(wgpu::DepthStencilState {
                            format: wgpu::TextureFormat::Depth32Float,
                            depth_write_enabled: true,
                            depth_compare: wgpu::CompareFunction::Less,
                            stencil: wgpu::StencilState::default(),
                            bias: wgpu::DepthBiasState::default(),
                        }),
                        multisample: wgpu::MultisampleState::default(),
                        fragment: Some(wgpu::FragmentState {
                            module: &shader,
                            entry_point: Some("fs_main"),
                            compilation_options: wgpu::PipelineCompilationOptions::default(),
                            targets: &[Some(wgpu::ColorTargetState {
                                format: runtime.render_target_format,
                                blend: Some(wgpu::BlendState::REPLACE),
                                write_mask: wgpu::ColorWrites::ALL,
                            })],
                        }),
                        multiview: None,
                        cache: None,
                    });

            Self {
                offscreen_texture,
                offscreen_view,
                depth_texture,
                depth_view,
                render_pipeline,
                uniform_bind_group_layout,
                uniform_buffer,
                uniform_buffer_size,
                uniform_bind_group,
                canvas_sampler,
                canvas_texture,
                canvas_texture_view,
                canvas_texture_width: 1,
                canvas_texture_height: 1,
                vertex_buffer,
                vertex_buffer_size: vertex_data.len() as u64,
                index_buffer,
                index_buffer_size: index_data.len() as u64,
                index_count,
                width: runtime.width,
                height: runtime.height,
                frame_index: 0,
            }
        }

        pub fn estimated_gpu_memory_bytes_base(&self, runtime: &LocalRuntimeState) -> u64 {
            let mut total = self
                .uniform_buffer_size
                .saturating_add(self.vertex_buffer_size)
                .saturating_add(self.index_buffer_size);

            total = total.saturating_add(estimated_texture_bytes(
                self.canvas_texture_width,
                self.canvas_texture_height,
                wgpu::TextureFormat::Rgba8Unorm,
            ));

            total = total.saturating_add(estimated_texture_bytes(
                self.width,
                self.height,
                wgpu::TextureFormat::Depth32Float,
            ));

            if runtime.surface.is_some() {
                if let Some(config) = runtime.surface_config.as_ref() {
                    // Swapchain depth is driver-managed; estimate double-buffered color allocations only.
                    total = total.saturating_add(
                        estimated_texture_bytes(config.width, config.height, config.format)
                            .saturating_mul(2),
                    );
                }
            } else {
                total = total.saturating_add(estimated_texture_bytes(
                    self.width,
                    self.height,
                    runtime.render_target_format,
                ));
            }

            total
        }

        fn update_uniforms(&mut self, runtime: &LocalRuntimeState) {
            let aspect = (self.width as f32 / self.height.max(1) as f32).max(0.0001);
            let t = self.frame_index as f32 * 0.016;

            let projection = perspective_rh_zo(60.0_f32.to_radians(), aspect, 0.1, 100.0);
            let view = translation_matrix(0.0, 0.0, -4.5);
            // Keep the textured face visible during validation while still animating.
            let model = mul_matrix(rotation_y_matrix(0.55 + t * 0.35), rotation_x_matrix(-0.20));
            let mvp = mul_matrix(mul_matrix(projection, view), model);
            runtime
                .queue
                .write_buffer(&self.uniform_buffer, 0, bytemuck::cast_slice(&mvp));
        }

        pub fn install_debug_texture(
            &mut self,
            runtime: &LocalRuntimeState,
            upload_width: u32,
            upload_height: u32,
            rgba: &[u8],
        ) -> bool {
            if upload_width == 0 || upload_height == 0 {
                return false;
            }

            let width = runtime.clamped_dimension(upload_width);
            let height = runtime.clamped_dimension(upload_height);
            let expected_len = (width as usize)
                .saturating_mul(height as usize)
                .saturating_mul(4);
            if rgba.len() < expected_len {
                return false;
            }

            if self.canvas_texture_width != width || self.canvas_texture_height != height {
                // Recreate only on dimension changes; steady-state updates reuse the same GPU objects.
                // Eagerly destroy replaced storage to avoid long-lived allocations when
                // workloads repeatedly resize upload textures.
                self.canvas_texture.destroy();
                let (texture, view) =
                    create_canvas_texture_with_view(&runtime.device, width, height);
                self.canvas_texture = texture;
                self.canvas_texture_view = view;
                self.canvas_texture_width = width;
                self.canvas_texture_height = height;
                self.uniform_bind_group = create_debug_cube_bind_group(
                    &runtime.device,
                    &self.uniform_bind_group_layout,
                    &self.uniform_buffer,
                    &self.canvas_sampler,
                    &self.canvas_texture_view,
                );
            }

            runtime.queue.write_texture(
                wgpu::TexelCopyTextureInfo {
                    texture: &self.canvas_texture,
                    mip_level: 0,
                    origin: wgpu::Origin3d::ZERO,
                    aspect: wgpu::TextureAspect::All,
                },
                &rgba[..expected_len],
                wgpu::TexelCopyBufferLayout {
                    offset: 0,
                    bytes_per_row: Some(width.saturating_mul(4)),
                    rows_per_image: Some(height),
                },
                wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
            );
            true
        }

        pub fn resize(&mut self, runtime: &mut LocalRuntimeState, width: u32, height: u32) {
            (self.width, self.height) = runtime.clamped_extent(width, height);

            if runtime.surface.is_some() {
                runtime.reconfigure_surface(self.width, self.height);
            } else {
                let (texture, view) = runtime.create_offscreen_target(self.width, self.height);
                self.offscreen_texture = Some(texture);
                self.offscreen_view = Some(view);
            }

            let (depth_texture, depth_view) = runtime.create_depth_target(self.width, self.height);
            self.depth_texture = depth_texture;
            self.depth_view = depth_view;
        }

        pub fn render(
            &mut self,
            runtime: &mut LocalRuntimeState,
            draw_enabled: bool,
        ) -> Result<(), String> {
            // The interop cube path is a temporary presentation shim while
            // BabylonJS WebGPU command coverage is incrementally replaced with
            // upstream wgpu-native C-ABI ownership. Keep rendering gated by
            // observed WebGPU JS draw traffic so native output reflects the
            // JS->C++->Rust path instead of free-running independently.
            if !draw_enabled {
                return Ok(());
            }

            self.update_uniforms(runtime);

            let (color_view, surface_frame) = match acquire_draw_target(
                runtime,
                self.width,
                self.height,
                &mut self.offscreen_texture,
                &mut self.offscreen_view,
            )? {
                DrawTargetAcquireResult::Ready {
                    color_view,
                    surface_frame,
                } => (color_view, surface_frame),
                DrawTargetAcquireResult::Reconfigure => {
                    runtime.reconfigure_surface(self.width, self.height);
                    return Ok(());
                }
                DrawTargetAcquireResult::SkipFrame => {
                    return Ok(());
                }
            };

            let mut encoder =
                runtime
                    .device
                    .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                        label: Some("babylon-native-webgpu.encoder"),
                    });

            {
                let mut render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                    label: Some("babylon-native-webgpu.cube-pass"),
                    color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                        view: &color_view,
                        depth_slice: None,
                        resolve_target: None,
                        ops: wgpu::Operations {
                            load: wgpu::LoadOp::Clear(wgpu::Color {
                                r: 0.03,
                                g: 0.05,
                                b: 0.08,
                                a: 1.0,
                            }),
                            store: wgpu::StoreOp::Store,
                        },
                    })],
                    depth_stencil_attachment: Some(wgpu::RenderPassDepthStencilAttachment {
                        view: &self.depth_view,
                        depth_ops: Some(wgpu::Operations {
                            load: wgpu::LoadOp::Clear(1.0),
                            store: wgpu::StoreOp::Store,
                        }),
                        stencil_ops: None,
                    }),
                    occlusion_query_set: None,
                    timestamp_writes: None,
                });

                render_pass.set_pipeline(&self.render_pipeline);
                render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
                render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
                render_pass
                    .set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
                render_pass.draw_indexed(0..self.index_count, 0, 0..1);
            }

            submit_and_present(
                &runtime.device,
                &runtime.queue,
                encoder.finish(),
                surface_frame,
            );

            self.frame_index = self.frame_index.wrapping_add(1);
            Ok(())
        }
    }

    impl LocalRuntimeState {
        pub fn bootstrap(config: LocalBootstrapConfig) -> Result<Self, String> {
            let requested_width = config.width.max(1);
            let requested_height = config.height.max(1);

            let instance = create_local_instance();
            let surface = create_local_surface(&instance, config.surface_layer)?;

            let bootstrap = bootstrap_local_wgpu_runtime(
                &instance,
                surface.as_ref(),
                config.prefer_low_power,
            )?;
            let max_texture_dimension_2d = bootstrap.limits.max_texture_dimension_2d.max(1);
            let (width, height) =
                clamped_extent(requested_width, requested_height, max_texture_dimension_2d);

            let mut surface_config = None;
            if let Some(surface_ref) = surface.as_ref() {
                let config = configure_local_surface(
                    surface_ref,
                    &bootstrap.adapter,
                    &bootstrap.device,
                    width,
                    height,
                )?;
                surface_config = Some(config);
            }

            let render_target_format = surface_config
                .as_ref()
                .map(|config| config.format)
                .unwrap_or(wgpu::TextureFormat::Rgba8Unorm);
            let resolved_adapter_info = AdapterProbeInfo {
                backend: map_local_backend_to_babylon_backend(bootstrap.adapter_info.backend),
                vendor_id: bootstrap.adapter_info.vendor,
                device_id: bootstrap.adapter_info.device,
                adapter_name: bootstrap.adapter_info.name.clone(),
            };

            Ok(Self {
                device: bootstrap.device,
                queue: bootstrap.queue,
                surface,
                surface_config,
                resolved_adapter_info,
                max_texture_dimension_2d,
                width,
                height,
                render_target_format,
                used_fallback_adapter: bootstrap.used_fallback_adapter,
                surface_acquire_failures: 0,
            })
        }

        pub fn reconfigure_surface(&mut self, width: u32, height: u32) {
            self.width = width.max(1);
            self.height = height.max(1);

            if let (Some(surface), Some(config)) =
                (self.surface.as_ref(), self.surface_config.as_mut())
            {
                reconfigure_local_surface(surface, &self.device, config, self.width, self.height);
            }
        }

        pub fn clamped_dimension(&self, value: u32) -> u32 {
            clamp_dimension(value, self.max_texture_dimension_2d)
        }

        pub fn clamped_extent(&self, width: u32, height: u32) -> (u32, u32) {
            clamped_extent(width, height, self.max_texture_dimension_2d)
        }

        pub fn create_offscreen_target(
            &self,
            width: u32,
            height: u32,
        ) -> (wgpu::Texture, wgpu::TextureView) {
            let (width, height) = self.clamped_extent(width, height);
            let texture = self.device.create_texture(&wgpu::TextureDescriptor {
                label: Some("babylon-native-webgpu.offscreen-color"),
                size: wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgpu::TextureDimension::D2,
                format: self.render_target_format,
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                view_formats: &[],
            });

            let view = texture.create_view(&wgpu::TextureViewDescriptor {
                label: Some("babylon-native-webgpu.offscreen-color-view"),
                ..Default::default()
            });
            (texture, view)
        }

        pub fn create_depth_target(
            &self,
            width: u32,
            height: u32,
        ) -> (wgpu::Texture, wgpu::TextureView) {
            let (width, height) = self.clamped_extent(width, height);
            let texture = self.device.create_texture(&wgpu::TextureDescriptor {
                label: Some("babylon-native-webgpu.depth"),
                size: wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgpu::TextureDimension::D2,
                format: wgpu::TextureFormat::Depth32Float,
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                view_formats: &[],
            });

            let view = texture.create_view(&wgpu::TextureViewDescriptor {
                label: Some("babylon-native-webgpu.depth-view"),
                ..Default::default()
            });
            (texture, view)
        }
    }

    fn clamp_dimension(value: u32, max_dimension: u32) -> u32 {
        value.max(1).min(max_dimension.max(1))
    }

    fn clamped_extent(width: u32, height: u32, max_dimension: u32) -> (u32, u32) {
        (
            clamp_dimension(width, max_dimension),
            clamp_dimension(height, max_dimension),
        )
    }

    pub fn create_local_surface(
        instance: &wgpu::Instance,
        surface_layer: *mut c_void,
    ) -> Result<Option<wgpu::Surface<'static>>, String> {
        if surface_layer.is_null() {
            return Ok(None);
        }

        #[cfg(any(target_os = "macos", target_os = "ios"))]
        {
            // SAFETY: The caller passes a valid CoreAnimation layer pointer that stays alive
            // for the lifetime of the created surface.
            return unsafe {
                instance.create_surface_unsafe(wgpu::SurfaceTargetUnsafe::CoreAnimationLayer(
                    surface_layer,
                ))
            }
            .map(Some)
            .map_err(|error| format!("Failed to create CoreAnimation surface: {error}"));
        }

        #[cfg(target_os = "android")]
        {
            use raw_window_handle::{
                AndroidDisplayHandle, AndroidNdkWindowHandle, RawDisplayHandle, RawWindowHandle,
            };
            use std::ptr::NonNull;

            let native_window = NonNull::new(surface_layer)
                .ok_or_else(|| "Android surface pointer was null.".to_string())?;
            let raw_display_handle = RawDisplayHandle::Android(AndroidDisplayHandle::new());
            let raw_window_handle =
                RawWindowHandle::AndroidNdk(AndroidNdkWindowHandle::new(native_window));

            // SAFETY: The caller passes an ANativeWindow* that remains valid while the
            // surface is alive.
            return unsafe {
                instance.create_surface_unsafe(wgpu::SurfaceTargetUnsafe::RawHandle {
                    raw_display_handle,
                    raw_window_handle,
                })
            }
            .map(Some)
            .map_err(|error| format!("Failed to create Android Vulkan surface: {error}"));
        }

        #[cfg(target_os = "windows")]
        {
            use raw_window_handle::{
                RawDisplayHandle, RawWindowHandle, Win32WindowHandle, WindowsDisplayHandle,
            };
            use std::num::NonZeroIsize;

            let hwnd = NonZeroIsize::new(surface_layer as isize)
                .ok_or_else(|| "Windows HWND pointer was null.".to_string())?;
            let raw_display_handle = RawDisplayHandle::Windows(WindowsDisplayHandle::new());
            let raw_window_handle = RawWindowHandle::Win32(Win32WindowHandle::new(hwnd));

            // SAFETY: The caller passes a valid HWND that remains alive while the
            // surface is alive.
            return unsafe {
                instance.create_surface_unsafe(wgpu::SurfaceTargetUnsafe::RawHandle {
                    raw_display_handle,
                    raw_window_handle,
                })
            }
            .map(Some)
            .map_err(|error| format!("Failed to create Win32 DX12 surface: {error}"));
        }

        #[allow(unreachable_code)]
        {
            Ok(None)
        }
    }

    pub fn configure_local_surface(
        surface: &wgpu::Surface<'static>,
        adapter: &wgpu::Adapter,
        device: &wgpu::Device,
        width: u32,
        height: u32,
    ) -> Result<wgpu::SurfaceConfiguration, String> {
        let mut config = surface
            .get_default_config(adapter, width.max(1), height.max(1))
            .ok_or_else(|| "Surface returned no default configuration.".to_string())?;

        let caps = surface.get_capabilities(adapter);
        if caps.formats.contains(&wgpu::TextureFormat::Bgra8UnormSrgb) {
            config.format = wgpu::TextureFormat::Bgra8UnormSrgb;
        } else if caps.formats.contains(&wgpu::TextureFormat::Bgra8Unorm) {
            config.format = wgpu::TextureFormat::Bgra8Unorm;
        }
        if caps.alpha_modes.contains(&wgpu::CompositeAlphaMode::Opaque) {
            config.alpha_mode = wgpu::CompositeAlphaMode::Opaque;
        }

        surface.configure(device, &config);
        Ok(config)
    }

    pub fn reconfigure_local_surface(
        surface: &wgpu::Surface<'static>,
        device: &wgpu::Device,
        surface_config: &mut wgpu::SurfaceConfiguration,
        width: u32,
        height: u32,
    ) {
        surface_config.width = width.max(1);
        surface_config.height = height.max(1);
        surface.configure(device, surface_config);
    }

    pub enum DrawTargetAcquireResult {
        Ready {
            color_view: wgpu::TextureView,
            surface_frame: Option<wgpu::SurfaceTexture>,
        },
        Reconfigure,
        SkipFrame,
    }

    pub fn acquire_draw_target(
        runtime: &mut LocalRuntimeState,
        width: u32,
        height: u32,
        offscreen_texture: &mut Option<wgpu::Texture>,
        offscreen_view: &mut Option<wgpu::TextureView>,
    ) -> Result<DrawTargetAcquireResult, String> {
        if let Some(surface) = runtime.surface.as_ref() {
            let surface_texture_result =
                std::panic::catch_unwind(std::panic::AssertUnwindSafe(|| {
                    surface.get_current_texture()
                }));
            return match surface_texture_result {
                Err(_) => Ok(DrawTargetAcquireResult::Reconfigure),
                Ok(surface_result) => match surface_result {
                    Ok(surface_frame) => {
                        runtime.surface_acquire_failures = 0;
                        let color_view =
                            surface_frame
                                .texture
                                .create_view(&wgpu::TextureViewDescriptor {
                                    label: Some("babylon-native-webgpu.surface-view"),
                                    ..Default::default()
                                });
                        Ok(DrawTargetAcquireResult::Ready {
                            color_view,
                            surface_frame: Some(surface_frame),
                        })
                    }
                    Err(wgpu::SurfaceError::Lost | wgpu::SurfaceError::Outdated) => {
                        runtime.surface_acquire_failures = 0;
                        Ok(DrawTargetAcquireResult::Reconfigure)
                    }
                    Err(wgpu::SurfaceError::Timeout | wgpu::SurfaceError::OutOfMemory) => {
                        // Recover from transient acquire failures by forcing a
                        // reconfigure after a few consecutive skips.
                        runtime.surface_acquire_failures =
                            runtime.surface_acquire_failures.saturating_add(1);
                        if runtime.surface_acquire_failures >= 4 {
                            runtime.surface_acquire_failures = 0;
                            Ok(DrawTargetAcquireResult::Reconfigure)
                        } else {
                            Ok(DrawTargetAcquireResult::SkipFrame)
                        }
                    }
                    Err(_error) => Ok(DrawTargetAcquireResult::Reconfigure),
                },
            };
        }

        if offscreen_view.is_none() {
            let (texture, view) = runtime.create_offscreen_target(width, height);
            *offscreen_texture = Some(texture);
            *offscreen_view = Some(view);
        }

        let color_view = offscreen_view
            .as_ref()
            .ok_or_else(|| "offscreen render target view was not available after creation".to_string())?
            .clone();

        Ok(DrawTargetAcquireResult::Ready {
            color_view,
            surface_frame: None,
        })
    }

    pub fn submit_and_present(
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        command_buffer: wgpu::CommandBuffer,
        surface_frame: Option<wgpu::SurfaceTexture>,
    ) {
        queue.submit(Some(command_buffer));
        if let Some(frame) = surface_frame {
            frame.present();
        }

        // Keep backend housekeeping progressing so completed submissions are
        // reclaimed without waiting for explicit map/poll paths.
        //
        // iOS simulator builds are especially sensitive to delayed submission
        // retirement and can exhibit sustained RSS growth unless we block for
        // completion. Use the stronger mode only on simulator targets.
        #[cfg(all(target_os = "ios", target_abi = "sim"))]
        {
            let _ = device.poll(wgpu::PollType::wait_indefinitely());
        }

        #[cfg(all(target_os = "ios", not(target_abi = "sim")))]
        {
            static IOS_SUBMIT_POLL_TICK: std::sync::atomic::AtomicU32 =
                std::sync::atomic::AtomicU32::new(0);
            let tick = IOS_SUBMIT_POLL_TICK.fetch_add(1, Ordering::Relaxed);
            let poll_mode = if tick % 8 == 0 {
                wgpu::PollType::wait_indefinitely()
            } else {
                wgpu::PollType::Poll
            };
            let _ = device.poll(poll_mode);
        }

        #[cfg(not(target_os = "ios"))]
        {
            static NON_IOS_SUBMIT_POLL_TICK: std::sync::atomic::AtomicU32 =
                std::sync::atomic::AtomicU32::new(0);
            let tick = NON_IOS_SUBMIT_POLL_TICK.fetch_add(1, Ordering::Relaxed);
            let poll_mode = if tick % 120 == 0 {
                // Bound queue residency drift in long-running sessions while
                // keeping steady-state frame pacing non-blocking.
                wgpu::PollType::wait_indefinitely()
            } else {
                wgpu::PollType::Poll
            };
            let _ = device.poll(poll_mode);
        }
    }

    fn map_local_backend_to_babylon_backend(backend: wgpu::Backend) -> u32 {
        match backend {
            wgpu::Backend::Vulkan => 1,
            wgpu::Backend::Metal => 2,
            wgpu::Backend::Dx12 => 3,
            wgpu::Backend::Gl => 4,
            _ => 0,
        }
    }

    pub fn preferred_wgpu_backends() -> wgpu::Backends {
        #[cfg(any(target_os = "macos", target_os = "ios"))]
        {
            return wgpu::Backends::METAL;
        }

        #[cfg(target_os = "windows")]
        {
            return wgpu::Backends::DX12;
        }

        #[cfg(not(any(target_os = "macos", target_os = "ios", target_os = "windows")))]
        {
            return wgpu::Backends::VULKAN;
        }
    }

    pub fn create_local_instance() -> wgpu::Instance {
        #[allow(unused_mut)]
        let mut descriptor = wgpu::InstanceDescriptor {
            backends: preferred_wgpu_backends(),
            ..Default::default()
        };

        #[cfg(target_os = "android")]
        {
            descriptor.flags |= wgpu::InstanceFlags::ALLOW_UNDERLYING_NONCOMPLIANT_ADAPTER;
        }

        wgpu::Instance::new(&descriptor)
    }

    pub fn bootstrap_local_wgpu_runtime(
        instance: &wgpu::Instance,
        compatible_surface: Option<&wgpu::Surface<'_>>,
        prefer_low_power: bool,
    ) -> Result<LocalBootstrapRuntime, String> {
        let power_preference = if prefer_low_power {
            wgpu::PowerPreference::LowPower
        } else {
            wgpu::PowerPreference::HighPerformance
        };

        fn try_adapter(
            adapter_errors: &mut Vec<String>,
            instance: &wgpu::Instance,
            power_preference: wgpu::PowerPreference,
            force_fallback_adapter: bool,
            surface: Option<&wgpu::Surface<'_>>,
            label: &'static str,
        ) -> Option<(wgpu::Adapter, bool)> {
            let options = wgpu::RequestAdapterOptions {
                power_preference,
                force_fallback_adapter,
                compatible_surface: surface,
            };
            match pollster::block_on(instance.request_adapter(&options)) {
                Ok(adapter) => Some((adapter, force_fallback_adapter)),
                Err(error) => {
                    adapter_errors.push(format!("{label}={error}"));
                    None
                }
            }
        }

        let mut adapter_errors: Vec<String> = Vec::new();
        #[cfg(target_os = "android")]
        const ADAPTER_ATTEMPTS: &[(bool, bool, &str)] = &[
            // Android emulator/device behavior can vary with surface-backed
            // selection, so prefer unsurfaced probing first.
            (false, false, "without_surface"),
            (true, false, "with_surface"),
            (false, true, "without_surface_fallback"),
            (true, true, "with_surface_fallback"),
        ];
        #[cfg(not(target_os = "android"))]
        const ADAPTER_ATTEMPTS: &[(bool, bool, &str)] = &[
            (true, false, "with_surface"),
            (false, false, "without_surface"),
            (true, true, "with_surface_fallback"),
            (false, true, "without_surface_fallback"),
        ];

        let adapter_result =
            ADAPTER_ATTEMPTS
                .iter()
                .find_map(|(use_surface, force_fallback_adapter, label)| {
                    let surface = if *use_surface {
                        compatible_surface
                    } else {
                        None
                    };
                    if *use_surface && surface.is_none() {
                        return None;
                    }

                    try_adapter(
                        &mut adapter_errors,
                        instance,
                        power_preference,
                        *force_fallback_adapter,
                        surface,
                        label,
                    )
                });

        #[allow(unused_mut)]
        let (mut adapter, mut used_fallback_adapter) = adapter_result.ok_or_else(|| {
            format!(
                "Failed to acquire GPU adapter. {}",
                adapter_errors.join("; ")
            )
        })?;

        #[allow(unused_mut)]
        let mut adapter_info = adapter.get_info();
        #[allow(unused_mut)]
        let mut adapter_limits = adapter.limits();
        let make_device = |selected_adapter: &wgpu::Adapter, limits: &wgpu::Limits| {
            let descriptor = wgpu::DeviceDescriptor {
                label: Some("babylon-native-webgpu.device"),
                required_features: wgpu::Features::empty(),
                required_limits: limits.clone(),
                experimental_features: wgpu::ExperimentalFeatures::disabled(),
                memory_hints: wgpu::MemoryHints::default(),
                trace: wgpu::Trace::default(),
            };

            pollster::block_on(selected_adapter.request_device(&descriptor))
        };
        #[allow(unused_mut)]
        let mut device_result = make_device(&adapter, &adapter_limits);

        #[cfg(target_os = "android")]
        if device_result.is_err() {
            // Keep one strict recovery path for Android emulator variability.
            if let Some((retry_adapter, retry_used_fallback)) = try_adapter(
                &mut adapter_errors,
                instance,
                wgpu::PowerPreference::LowPower,
                true,
                None,
                "retry_low_power_fallback",
            ) {
                adapter = retry_adapter;
                used_fallback_adapter = retry_used_fallback;
                adapter_info = adapter.get_info();
                adapter_limits = adapter.limits();
                device_result = make_device(&adapter, &adapter_limits);
            }
        }

        let (device, queue) = device_result.map_err(|error| {
            format!(
                "Failed to create GPU device: {error} (adapter=\"{}\" backend={:?})",
                adapter_info.name, adapter_info.backend
            )
        })?;

        Ok(LocalBootstrapRuntime {
            adapter,
            adapter_info,
            limits: adapter_limits,
            device,
            queue,
            used_fallback_adapter,
        })
    }

    // The `enabled` submodule lives in compute.rs but is declared as a child of
    // `upstream_wgpu_native` so that `use super::*` inside compute.rs resolves to
    // the types and helpers defined in this module.
    #[path = "compute.rs"]
    mod enabled;

    pub use enabled::{dispatch_compute_global, version};
}

// Consolidate Rust backend code into a single staticlib so the native binary
// carries one wgpu code instance for both GraphicsWgpu and CanvasWgpu exports.
//
// TODO: Convert CanvasWgpu into a workspace member crate once the backend code
// stabilizes. The `#[path]` include avoids a second staticlib link target during
// the current rapid-iteration phase but should be replaced with a proper crate
// dependency (e.g. `canvas_wgpu_backend = { path = "..." }`) to get independent
// compilation units and cleaner module boundaries.
#[path = "../../../../Polyfills/CanvasWgpu/Rust/src/lib.rs"]
mod canvas_wgpu_backend_exports;
