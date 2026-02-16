use std::collections::HashMap;
use std::ffi::{c_char, c_void, CStr};
use std::ptr;
use std::slice;

use femtovg::renderer::WGPURenderer;
use femtovg::{
    Color, CompositeOperation, FontId, ImageFlags, ImageId, LineCap, LineJoin, Paint, Path,
    Solidity, Transform2D,
};
use imgref::Img;
use rgb::RGBA8;

#[repr(C)]
#[derive(Copy, Clone, Default)]
pub struct NVGcolor {
    pub r: f32,
    pub g: f32,
    pub b: f32,
    pub a: f32,
}

#[repr(C)]
#[derive(Copy, Clone, Default)]
pub struct NVGpaint {
    pub image: i32,
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
    pub angle: f32,
    pub alpha: f32,
    pub kind: i32,
    pub inner_color: NVGcolor,
    pub outer_color: NVGcolor,
}

#[derive(Clone)]
struct StyleState {
    fill_paint: Paint,
    stroke_paint: Paint,
    stroke_width: f32,
    line_cap: LineCap,
    line_join: LineJoin,
    miter_limit: f32,
    font_size: f32,
    letter_spacing: f32,
    current_font_handle: i32,
    global_alpha: f32,
    filter_blur_sigma: f32,
}

#[repr(C)]
#[derive(Copy, Clone)]
struct BabylonCanvasNativeTextureHandle {
    texture: *const c_void,
    device: *const c_void,
    queue: *const c_void,
    width: u32,
    height: u32,
    generation: u64,
}

struct Backend {
    device: wgpu::Device,
    queue: wgpu::Queue,
    canvas: femtovg::Canvas<WGPURenderer>,
    render_texture: wgpu::Texture,
    render_texture_format: wgpu::TextureFormat,
    width: u32,
    height: u32,
    dpi: f32,
    current_path: Path,
    fill_paint: Paint,
    stroke_paint: Paint,
    stroke_width: f32,
    line_cap: LineCap,
    line_join: LineJoin,
    miter_limit: f32,
    font_size: f32,
    letter_spacing: f32,
    current_font_handle: i32,
    style_stack: Vec<StyleState>,
    next_image_handle: i32,
    images: HashMap<i32, ImageId>,
    next_font_handle: i32,
    fonts: HashMap<i32, FontId>,
    font_names: HashMap<String, i32>,
    font_blobs: Vec<Vec<u8>>,
    blur_offsets: Vec<(f32, f32, f32)>,
    global_alpha: f32,
    filter_blur_sigma: f32,
    // Sole interop version source: bumped once per submitted frame so
    // GraphicsWgpu can skip redundant native texture imports.
    interop_handle: BabylonCanvasNativeTextureHandle,
}

#[repr(C)]
pub struct NVGcontext {
    backend: Backend,
}

fn preferred_backends() -> wgpu::Backends {
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

fn create_instance() -> wgpu::Instance {
    #[allow(unused_mut)]
    let mut descriptor = wgpu::InstanceDescriptor {
        backends: preferred_backends(),
        ..Default::default()
    };

    #[cfg(target_os = "android")]
    {
        descriptor.flags |= wgpu::InstanceFlags::ALLOW_UNDERLYING_NONCOMPLIANT_ADAPTER;
    }

    wgpu::Instance::new(&descriptor)
}

fn create_device() -> Result<(wgpu::Device, wgpu::Queue), String> {
    let instance = create_instance();
    let adapter = pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
        power_preference: wgpu::PowerPreference::HighPerformance,
        force_fallback_adapter: false,
        compatible_surface: None,
    }))
    .map_err(|err| format!("request_adapter failed: {err}"))?;

    let descriptor = wgpu::DeviceDescriptor {
        label: Some("babylon-canvas-wgpu.device"),
        required_features: wgpu::Features::empty(),
        required_limits: wgpu::Limits::default(),
        experimental_features: wgpu::ExperimentalFeatures::disabled(),
        memory_hints: wgpu::MemoryHints::default(),
        trace: wgpu::Trace::default(),
    };

    pollster::block_on(adapter.request_device(&descriptor))
        .map_err(|err| format!("request_device failed: {err}"))
}

fn create_render_texture(
    device: &wgpu::Device,
    width: u32,
    height: u32,
    format: wgpu::TextureFormat,
) -> wgpu::Texture {
    device.create_texture(&wgpu::TextureDescriptor {
        label: Some("babylon-canvas-wgpu.render-target"),
        size: wgpu::Extent3d {
            width: width.max(1),
            height: height.max(1),
            depth_or_array_layers: 1,
        },
        mip_level_count: 1,
        sample_count: 1,
        dimension: wgpu::TextureDimension::D2,
        format,
        usage: wgpu::TextureUsages::RENDER_ATTACHMENT
            | wgpu::TextureUsages::TEXTURE_BINDING
            | wgpu::TextureUsages::COPY_SRC,
        view_formats: &[],
    })
}

impl Backend {
    fn new(width: u32, height: u32) -> Result<Self, String> {
        let (device, queue) = create_device()?;
        let renderer = WGPURenderer::new(device.clone(), queue.clone());
        let mut canvas = femtovg::Canvas::new(renderer)
            .map_err(|err| format!("femtovg canvas create failed: {err:?}"))?;

        let render_texture_format = wgpu::TextureFormat::Rgba8Unorm;
        let render_texture = create_render_texture(&device, width, height, render_texture_format);

        let dpi = 1.0f32;
        canvas.set_size(width.max(1), height.max(1), dpi);

        let mut fill_paint = Paint::color(Color::rgba(255, 255, 255, 255));
        let mut stroke_paint = Paint::color(Color::rgba(255, 255, 255, 255));
        stroke_paint.set_line_width(1.0);
        fill_paint.set_font_size(16.0);
        stroke_paint.set_font_size(16.0);

        let mut backend = Self {
            device,
            queue,
            canvas,
            render_texture,
            render_texture_format,
            width: width.max(1),
            height: height.max(1),
            dpi,
            current_path: Path::new(),
            fill_paint,
            stroke_paint,
            stroke_width: 1.0,
            line_cap: LineCap::Butt,
            line_join: LineJoin::Miter,
            miter_limit: 10.0,
            font_size: 16.0,
            letter_spacing: 0.0,
            current_font_handle: -1,
            style_stack: Vec::new(),
            next_image_handle: 1,
            images: HashMap::new(),
            next_font_handle: 1,
            fonts: HashMap::new(),
            font_names: HashMap::new(),
            font_blobs: Vec::new(),
            blur_offsets: Vec::new(),
            global_alpha: 1.0,
            filter_blur_sigma: 0.0,
            interop_handle: BabylonCanvasNativeTextureHandle {
                texture: ptr::null(),
                device: ptr::null(),
                queue: ptr::null(),
                width: 0,
                height: 0,
                generation: 0,
            },
        };

        backend.refresh_interop_handle();
        Ok(backend)
    }

    fn resize(&mut self, width: u32, height: u32, dpi: f32) {
        let next_width = width.max(1);
        let next_height = height.max(1);
        let next_dpi = dpi.max(0.01);

        let size_changed = self.width != next_width || self.height != next_height;
        let dpi_changed = (self.dpi - next_dpi).abs() > f32::EPSILON;

        if !size_changed && !dpi_changed {
            return;
        }

        self.width = next_width;
        self.height = next_height;
        self.dpi = next_dpi;
        self.canvas.set_size(self.width, self.height, self.dpi);

        if size_changed {
            // Keep GPU residency bounded when callers resize canvases repeatedly.
            self.render_texture.destroy();
            self.render_texture = create_render_texture(
                &self.device,
                self.width,
                self.height,
                self.render_texture_format,
            );
            self.refresh_interop_handle();
        }
    }

    fn begin_frame(&mut self, width: f32, height: f32, dpi: f32) {
        self.resize(
            width.max(1.0).round() as u32,
            height.max(1.0).round() as u32,
            dpi,
        );
    }

    fn end_frame(&mut self) {
        let command_buffer = self.canvas.flush_to_surface(&self.render_texture);
        self.queue.submit(std::iter::once(command_buffer));
        #[cfg(all(target_os = "ios", target_abi = "sim"))]
        {
            let _ = self.device.poll(wgpu::PollType::wait_indefinitely());
        }

        #[cfg(not(all(target_os = "ios", target_abi = "sim")))]
        {
            let _ = self.device.poll(wgpu::PollType::Poll);
        }
        // Mark canvas content changed for cross-module texture import dedupe.
        self.interop_handle.generation = self.interop_handle.generation.wrapping_add(1);
        self.refresh_interop_handle();
    }

    fn refresh_interop_handle(&mut self) {
        self.interop_handle.texture =
            (&self.render_texture as *const wgpu::Texture).cast::<c_void>();
        self.interop_handle.device = (&self.device as *const wgpu::Device).cast::<c_void>();
        self.interop_handle.queue = (&self.queue as *const wgpu::Queue).cast::<c_void>();
        self.interop_handle.width = self.width;
        self.interop_handle.height = self.height;
    }

    fn to_color(color: NVGcolor) -> Color {
        Color::rgba(
            (color.r.clamp(0.0, 1.0) * 255.0).round() as u8,
            (color.g.clamp(0.0, 1.0) * 255.0).round() as u8,
            (color.b.clamp(0.0, 1.0) * 255.0).round() as u8,
            (color.a.clamp(0.0, 1.0) * 255.0).round() as u8,
        )
    }

    fn apply_stroke_style(&mut self) {
        self.stroke_paint.set_line_width(self.stroke_width.max(0.0));
        self.stroke_paint.set_line_cap(self.line_cap);
        self.stroke_paint.set_line_join(self.line_join);
        self.stroke_paint.set_miter_limit(self.miter_limit.max(0.0));
    }

    fn apply_text_style(&self, paint: &mut Paint) {
        paint.set_font_size(self.font_size.max(0.0));
        paint.set_letter_spacing(self.letter_spacing);
        if let Some(font_id) = self.fonts.get(&self.current_font_handle) {
            paint.set_font(&[*font_id]);
        }
    }

    fn paint_from_pattern(&self, paint: NVGpaint) -> Paint {
        if paint.kind == 1 {
            if let Some(image_id) = self.images.get(&paint.image) {
                return Paint::image(
                    *image_id,
                    paint.x,
                    paint.y,
                    paint.width.max(1.0),
                    paint.height.max(1.0),
                    paint.angle,
                    paint.alpha.clamp(0.0, 1.0),
                );
            }
        }

        Paint::color(Self::to_color(paint.inner_color))
    }

    fn push_style_state(&mut self) {
        self.style_stack.push(StyleState {
            fill_paint: self.fill_paint.clone(),
            stroke_paint: self.stroke_paint.clone(),
            stroke_width: self.stroke_width,
            line_cap: self.line_cap,
            line_join: self.line_join,
            miter_limit: self.miter_limit,
            font_size: self.font_size,
            letter_spacing: self.letter_spacing,
            current_font_handle: self.current_font_handle,
            global_alpha: self.global_alpha,
            filter_blur_sigma: self.filter_blur_sigma,
        });
    }

    fn pop_style_state(&mut self) {
        if let Some(style) = self.style_stack.pop() {
            self.fill_paint = style.fill_paint;
            self.stroke_paint = style.stroke_paint;
            self.stroke_width = style.stroke_width;
            self.line_cap = style.line_cap;
            self.line_join = style.line_join;
            self.miter_limit = style.miter_limit;
            self.font_size = style.font_size;
            self.letter_spacing = style.letter_spacing;
            self.current_font_handle = style.current_font_handle;
            self.global_alpha = style.global_alpha;
            self.filter_blur_sigma = style.filter_blur_sigma;
            self.canvas.set_global_alpha(self.global_alpha);
            self.apply_stroke_style();
        }
    }

    fn draw_with_blur(&mut self, mut draw_call: impl FnMut(&mut Self)) {
        let sigma = self.filter_blur_sigma.clamp(0.0, 48.0);
        if sigma <= f32::EPSILON {
            draw_call(self);
            return;
        }

        // Non-CTS: blur() is approximated with weighted offset redraws.
        let radius = (sigma * 1.5).ceil().clamp(1.0, 8.0) as i32;
        let sigma_sq_2 = (2.0 * sigma * sigma).max(0.0001);

        let mut offsets = std::mem::take(&mut self.blur_offsets);
        offsets.clear();
        offsets.reserve(((radius * 2 + 1) * (radius * 2 + 1)) as usize);
        let mut weight_sum = 0.0f32;
        for y in -radius..=radius {
            for x in -radius..=radius {
                let distance_sq = (x * x + y * y) as f32;
                let weight = (-distance_sq / sigma_sq_2).exp();
                offsets.push((x as f32, y as f32, weight));
                weight_sum += weight;
            }
        }

        if weight_sum <= f32::EPSILON {
            self.blur_offsets = offsets;
            draw_call(self);
            return;
        }

        for (dx, dy, weight) in offsets.iter().copied() {
            self.canvas.save();
            self.canvas.translate(dx, dy);
            self.canvas
                .set_global_alpha((self.global_alpha * (weight / weight_sum)).clamp(0.0, 1.0));
            draw_call(self);
            self.canvas.restore();
        }

        self.canvas.set_global_alpha(self.global_alpha);
        self.blur_offsets = offsets;
    }

    fn map_composite_operation(op: i32) -> CompositeOperation {
        match op {
            1 => CompositeOperation::SourceIn,
            2 => CompositeOperation::SourceOut,
            3 => CompositeOperation::Atop,
            4 => CompositeOperation::DestinationOver,
            5 => CompositeOperation::DestinationIn,
            6 => CompositeOperation::DestinationOut,
            7 => CompositeOperation::DestinationAtop,
            8 => CompositeOperation::Lighter,
            9 => CompositeOperation::Copy,
            10 => CompositeOperation::Xor,
            _ => CompositeOperation::SourceOver,
        }
    }

    unsafe fn read_text(ptr: *const c_char, end: *const c_char) -> String {
        if ptr.is_null() {
            return String::new();
        }

        if end.is_null() {
            return CStr::from_ptr(ptr).to_string_lossy().into_owned();
        }

        let len = end.offset_from(ptr);
        if len <= 0 {
            return String::new();
        }

        let bytes = slice::from_raw_parts(ptr as *const u8, len as usize);
        String::from_utf8_lossy(bytes).into_owned()
    }
}

fn with_ctx_mut<R>(ctx: *mut NVGcontext, default: R, f: impl FnOnce(&mut Backend) -> R) -> R {
    if ctx.is_null() {
        return default;
    }

    // SAFETY: The pointer is owned by the caller and expected to be valid for the duration of the call.
    let backend = unsafe { &mut (*ctx).backend };
    f(backend)
}

fn with_ctx_ref<R>(ctx: *mut NVGcontext, default: R, f: impl FnOnce(&Backend) -> R) -> R {
    if ctx.is_null() {
        return default;
    }

    // SAFETY: The pointer is owned by the caller and expected to be valid for the duration of the call.
    let backend = unsafe { &(*ctx).backend };
    f(backend)
}

#[no_mangle]
pub extern "C" fn nvgCreate(_flags: i32) -> *mut NVGcontext {
    match std::panic::catch_unwind(|| Backend::new(1, 1)) {
        Ok(Ok(backend)) => Box::into_raw(Box::new(NVGcontext { backend })),
        _ => ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn nvgDelete(ctx: *mut NVGcontext) {
    if ctx.is_null() {
        return;
    }

    // SAFETY: The pointer was created by nvgCreate and is owned by the caller.
    unsafe {
        drop(Box::from_raw(ctx));
    }
}

#[no_mangle]
pub extern "C" fn nvgBeginFrame(
    ctx: *mut NVGcontext,
    window_width: f32,
    window_height: f32,
    device_pixel_ratio: f32,
) {
    with_ctx_mut(ctx, (), |backend| {
        backend.begin_frame(window_width, window_height, device_pixel_ratio)
    });
}

#[no_mangle]
pub extern "C" fn nvgEndFrame(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| backend.end_frame());
}

#[no_mangle]
pub extern "C" fn nvgSave(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| {
        backend.canvas.save();
        backend.push_style_state();
    });
}

#[no_mangle]
pub extern "C" fn nvgRestore(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| {
        backend.canvas.restore();
        backend.pop_style_state();
    });
}

#[no_mangle]
pub extern "C" fn nvgResetTransform(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| backend.canvas.reset_transform());
}

#[no_mangle]
pub extern "C" fn nvgTransform(
    ctx: *mut NVGcontext,
    a: f32,
    b: f32,
    c: f32,
    d: f32,
    e: f32,
    f_: f32,
) {
    with_ctx_mut(ctx, (), |backend| {
        backend
            .canvas
            .set_transform(&Transform2D::new(a, b, c, d, e, f_))
    });
}

#[no_mangle]
pub extern "C" fn nvgCurrentTransform(ctx: *mut NVGcontext, xform: *mut f32) {
    with_ctx_ref(ctx, (), |backend| {
        if xform.is_null() {
            return;
        }

        let tx = backend.canvas.transform().0;
        // SAFETY: The caller passes a writable array with at least six floats.
        unsafe {
            for (idx, value) in tx.iter().enumerate().take(6) {
                *xform.add(idx) = *value;
            }
        }
    });
}

#[no_mangle]
pub extern "C" fn nvgTranslate(ctx: *mut NVGcontext, x: f32, y: f32) {
    with_ctx_mut(ctx, (), |backend| backend.canvas.translate(x, y));
}

#[no_mangle]
pub extern "C" fn nvgRotate(ctx: *mut NVGcontext, angle: f32) {
    with_ctx_mut(ctx, (), |backend| backend.canvas.rotate(angle));
}

#[no_mangle]
pub extern "C" fn nvgScale(ctx: *mut NVGcontext, x: f32, y: f32) {
    with_ctx_mut(ctx, (), |backend| backend.canvas.scale(x, y));
}

#[no_mangle]
pub extern "C" fn nvgScissor(ctx: *mut NVGcontext, x: f32, y: f32, w: f32, h: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.canvas.scissor(x, y, w.max(0.0), h.max(0.0))
    });
}

#[no_mangle]
pub extern "C" fn nvgBeginPath(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| backend.current_path = Path::new());
}

#[no_mangle]
pub extern "C" fn nvgClosePath(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| backend.current_path.close());
}

#[no_mangle]
pub extern "C" fn nvgMoveTo(ctx: *mut NVGcontext, x: f32, y: f32) {
    with_ctx_mut(ctx, (), |backend| backend.current_path.move_to(x, y));
}

#[no_mangle]
pub extern "C" fn nvgLineTo(ctx: *mut NVGcontext, x: f32, y: f32) {
    with_ctx_mut(ctx, (), |backend| backend.current_path.line_to(x, y));
}

#[no_mangle]
pub extern "C" fn nvgBezierTo(
    ctx: *mut NVGcontext,
    c1x: f32,
    c1y: f32,
    c2x: f32,
    c2y: f32,
    x: f32,
    y: f32,
) {
    with_ctx_mut(ctx, (), |backend| {
        backend.current_path.bezier_to(c1x, c1y, c2x, c2y, x, y)
    });
}

#[no_mangle]
pub extern "C" fn nvgQuadTo(ctx: *mut NVGcontext, cx: f32, cy: f32, x: f32, y: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.current_path.quad_to(cx, cy, x, y)
    });
}

#[no_mangle]
pub extern "C" fn nvgArc(
    ctx: *mut NVGcontext,
    cx: f32,
    cy: f32,
    r: f32,
    a0: f32,
    a1: f32,
    dir: i32,
) {
    with_ctx_mut(ctx, (), |backend| {
        let winding = if dir == 1 {
            Solidity::Hole
        } else {
            Solidity::Solid
        };
        backend
            .current_path
            .arc(cx, cy, r.max(0.0), a0, a1, winding);
    });
}

#[no_mangle]
pub extern "C" fn nvgArcTo(ctx: *mut NVGcontext, x1: f32, y1: f32, x2: f32, y2: f32, radius: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.current_path.arc_to(x1, y1, x2, y2, radius.max(0.0))
    });
}

#[no_mangle]
pub extern "C" fn nvgRect(ctx: *mut NVGcontext, x: f32, y: f32, w: f32, h: f32) {
    with_ctx_mut(ctx, (), |backend| backend.current_path.rect(x, y, w, h));
}

#[no_mangle]
pub extern "C" fn nvgRoundedRect(ctx: *mut NVGcontext, x: f32, y: f32, w: f32, h: f32, r: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.current_path.rounded_rect(x, y, w, h, r.max(0.0))
    });
}

#[no_mangle]
pub extern "C" fn nvgRoundedRectVarying(
    ctx: *mut NVGcontext,
    x: f32,
    y: f32,
    w: f32,
    h: f32,
    rad_top_left: f32,
    rad_top_right: f32,
    rad_bottom_right: f32,
    rad_bottom_left: f32,
) {
    with_ctx_mut(ctx, (), |backend| {
        backend.current_path.rounded_rect_varying(
            x,
            y,
            w,
            h,
            rad_top_left.max(0.0),
            rad_top_right.max(0.0),
            rad_bottom_right.max(0.0),
            rad_bottom_left.max(0.0),
        );
    });
}

#[no_mangle]
pub extern "C" fn nvgRoundedRectElliptic(
    ctx: *mut NVGcontext,
    x: f32,
    y: f32,
    w: f32,
    h: f32,
    rtlx: f32,
    rtly: f32,
    rtrx: f32,
    rtry: f32,
    rbrx: f32,
    rbry: f32,
    rblx: f32,
    rbly: f32,
) {
    with_ctx_mut(ctx, (), |backend| {
        // femtovg has varying rounded corners but no direct elliptic-per-corner primitive.
        // Approximate each corner by averaging the x/y radius pair.
        backend.current_path.rounded_rect_varying(
            x,
            y,
            w,
            h,
            ((rtlx.abs() + rtly.abs()) * 0.5).max(0.0),
            ((rtrx.abs() + rtry.abs()) * 0.5).max(0.0),
            ((rbrx.abs() + rbry.abs()) * 0.5).max(0.0),
            ((rblx.abs() + rbly.abs()) * 0.5).max(0.0),
        );
    });
}

#[no_mangle]
pub extern "C" fn nvgEllipse(ctx: *mut NVGcontext, cx: f32, cy: f32, rx: f32, ry: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend
            .current_path
            .ellipse(cx, cy, rx.max(0.0), ry.max(0.0))
    });
}

#[no_mangle]
pub extern "C" fn nvgFillColor(ctx: *mut NVGcontext, color: NVGcolor) {
    with_ctx_mut(ctx, (), |backend| {
        let mut paint = Paint::color(Backend::to_color(color));
        backend.apply_text_style(&mut paint);
        backend.fill_paint = paint;
    });
}

#[no_mangle]
pub extern "C" fn nvgStrokeColor(ctx: *mut NVGcontext, color: NVGcolor) {
    with_ctx_mut(ctx, (), |backend| {
        let mut paint = Paint::color(Backend::to_color(color));
        backend.apply_text_style(&mut paint);
        backend.stroke_paint = paint;
        backend.apply_stroke_style();
    });
}

#[no_mangle]
pub extern "C" fn nvgFillPaint(ctx: *mut NVGcontext, paint: NVGpaint) {
    with_ctx_mut(ctx, (), |backend| {
        let mut converted = backend.paint_from_pattern(paint);
        backend.apply_text_style(&mut converted);
        backend.fill_paint = converted;
    });
}

#[no_mangle]
pub extern "C" fn nvgStrokeWidth(ctx: *mut NVGcontext, width: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.stroke_width = width.max(0.0);
        backend.apply_stroke_style();
    });
}

#[no_mangle]
pub extern "C" fn nvgLineCap(ctx: *mut NVGcontext, cap: i32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.line_cap = match cap {
            1 => LineCap::Round,
            2 => LineCap::Square,
            _ => LineCap::Butt,
        };
        backend.apply_stroke_style();
    });
}

#[no_mangle]
pub extern "C" fn nvgLineJoin(ctx: *mut NVGcontext, join: i32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.line_join = match join {
            3 => LineJoin::Bevel,
            1 => LineJoin::Round,
            _ => LineJoin::Miter,
        };
        backend.apply_stroke_style();
    });
}

#[no_mangle]
pub extern "C" fn nvgMiterLimit(ctx: *mut NVGcontext, limit: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.miter_limit = limit.max(0.0);
        backend.apply_stroke_style();
    });
}

#[no_mangle]
pub extern "C" fn nvgGlobalAlpha(ctx: *mut NVGcontext, alpha: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.global_alpha = alpha.clamp(0.0, 1.0);
        backend.canvas.set_global_alpha(backend.global_alpha);
    });
}

#[no_mangle]
pub extern "C" fn nvgGlobalCompositeOperation(ctx: *mut NVGcontext, op: i32) {
    with_ctx_mut(ctx, (), |backend| {
        backend
            .canvas
            .global_composite_operation(Backend::map_composite_operation(op))
    });
}

#[no_mangle]
pub extern "C" fn nvgFill(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| {
        // Fast path: skip clone when blur is zero (the common case).
        if backend.filter_blur_sigma <= f32::EPSILON {
            backend.canvas.fill_path(&backend.current_path, &backend.fill_paint);
        } else {
            let path = backend.current_path.clone();
            let paint = backend.fill_paint.clone();
            backend.draw_with_blur(|inner| {
                inner.canvas.fill_path(&path, &paint);
            });
        }
    });
}

#[no_mangle]
pub extern "C" fn nvgStroke(ctx: *mut NVGcontext) {
    with_ctx_mut(ctx, (), |backend| {
        // Fast path: skip clone when blur is zero (the common case).
        if backend.filter_blur_sigma <= f32::EPSILON {
            backend.canvas.stroke_path(&backend.current_path, &backend.stroke_paint);
        } else {
            let path = backend.current_path.clone();
            let paint = backend.stroke_paint.clone();
            backend.draw_with_blur(|inner| {
                inner.canvas.stroke_path(&path, &paint);
            });
        }
    });
}

#[no_mangle]
pub extern "C" fn nvgSetFilterBlur(ctx: *mut NVGcontext, sigma: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.filter_blur_sigma = sigma.clamp(0.0, 48.0);
    });
}

#[no_mangle]
pub extern "C" fn nvgCreateImageRGBA(
    ctx: *mut NVGcontext,
    w: i32,
    h: i32,
    _image_flags: i32,
    data: *const u8,
) -> i32 {
    with_ctx_mut(ctx, -1, |backend| {
        if w <= 0 || h <= 0 {
            return -1;
        }

        let width = w as usize;
        let height = h as usize;
        let pixel_count = width.saturating_mul(height);
        let byte_count = pixel_count.saturating_mul(4);
        let mut storage = vec![RGBA8::new(0, 0, 0, 0); pixel_count];

        if !data.is_null() && byte_count > 0 {
            // SAFETY: The caller guarantees the byte range is valid for reads.
            let bytes = unsafe { slice::from_raw_parts(data, byte_count) };
            for (idx, pixel) in storage.iter_mut().enumerate().take(pixel_count) {
                let base = idx * 4;
                *pixel = RGBA8::new(
                    bytes[base],
                    bytes[base + 1],
                    bytes[base + 2],
                    bytes[base + 3],
                );
            }
        }

        let img = Img::new(storage.as_slice(), width, height);
        match backend.canvas.create_image(img, ImageFlags::empty()) {
            Ok(image_id) => {
                let handle = backend.next_image_handle;
                backend.next_image_handle += 1;
                backend.images.insert(handle, image_id);
                handle
            }
            Err(_) => -1,
        }
    })
}

#[no_mangle]
pub extern "C" fn nvgDeleteImage(ctx: *mut NVGcontext, image: i32) {
    with_ctx_mut(ctx, (), |backend| {
        if let Some(image_id) = backend.images.remove(&image) {
            backend.canvas.delete_image(image_id);
        }
    });
}

#[no_mangle]
pub extern "C" fn nvgImagePattern(
    _ctx: *mut NVGcontext,
    ox: f32,
    oy: f32,
    ex: f32,
    ey: f32,
    angle: f32,
    image: i32,
    alpha: f32,
) -> NVGpaint {
    NVGpaint {
        image,
        x: ox,
        y: oy,
        width: ex,
        height: ey,
        angle,
        alpha,
        kind: 1,
        inner_color: NVGcolor {
            r: 1.0,
            g: 1.0,
            b: 1.0,
            a: alpha,
        },
        outer_color: NVGcolor {
            r: 1.0,
            g: 1.0,
            b: 1.0,
            a: alpha,
        },
    }
}

#[no_mangle]
pub extern "C" fn nvgCreateFontMem(
    ctx: *mut NVGcontext,
    name: *const c_char,
    data: *mut u8,
    size: i32,
    _free_data: i32,
) -> i32 {
    with_ctx_mut(ctx, -1, |backend| {
        if data.is_null() || size <= 0 {
            return -1;
        }

        let font_name = if name.is_null() {
            String::new()
        } else {
            // SAFETY: The caller provides a valid C string.
            unsafe { CStr::from_ptr(name).to_string_lossy().into_owned() }
        };

        if let Some(existing) = backend.font_names.get(&font_name) {
            return *existing;
        }

        // SAFETY: The caller guarantees a readable font byte range.
        let bytes = unsafe { slice::from_raw_parts(data as *const u8, size as usize) };
        let owned = bytes.to_vec();
        let font_id = match backend.canvas.add_font_mem(&owned) {
            Ok(id) => id,
            Err(_) => return -1,
        };

        backend.font_blobs.push(owned);
        let handle = backend.next_font_handle;
        backend.next_font_handle += 1;
        backend.fonts.insert(handle, font_id);
        if !font_name.is_empty() {
            backend.font_names.insert(font_name, handle);
        }

        handle
    })
}

#[no_mangle]
pub extern "C" fn nvgFontFaceId(ctx: *mut NVGcontext, font: i32) {
    with_ctx_mut(ctx, (), |backend| {
        if backend.fonts.contains_key(&font) {
            backend.current_font_handle = font;
        }
    });
}

#[no_mangle]
pub extern "C" fn nvgFontSize(ctx: *mut NVGcontext, size: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.font_size = size.max(0.0);
        backend.fill_paint.set_font_size(backend.font_size);
        backend.stroke_paint.set_font_size(backend.font_size);
    });
}

#[no_mangle]
pub extern "C" fn nvgTextLetterSpacing(ctx: *mut NVGcontext, spacing: f32) {
    with_ctx_mut(ctx, (), |backend| {
        backend.letter_spacing = spacing;
        backend.fill_paint.set_letter_spacing(spacing);
        backend.stroke_paint.set_letter_spacing(spacing);
    });
}

#[no_mangle]
pub extern "C" fn nvgText(
    ctx: *mut NVGcontext,
    x: f32,
    y: f32,
    string: *const c_char,
    end: *const c_char,
) -> f32 {
    with_ctx_mut(ctx, 0.0, |backend| {
        // SAFETY: `string` and `end` follow the nanovg C API contract.
        let text = unsafe { Backend::read_text(string, end) };
        let mut paint = backend.fill_paint.clone();
        backend.apply_text_style(&mut paint);
        let sigma = backend.filter_blur_sigma;
        if sigma > f32::EPSILON {
            backend.draw_with_blur(|inner| {
                let _ = inner.canvas.fill_text(x, y, &text, &paint);
            });
        }

        match backend.canvas.fill_text(x, y, &text, &paint) {
            Ok(metrics) => metrics.width(),
            Err(_) => 0.0,
        }
    })
}

#[no_mangle]
pub extern "C" fn nvgStrokeText(
    ctx: *mut NVGcontext,
    x: f32,
    y: f32,
    string: *const c_char,
    end: *const c_char,
) -> f32 {
    with_ctx_mut(ctx, 0.0, |backend| {
        // SAFETY: `string` and `end` follow the nanovg C API contract.
        let text = unsafe { Backend::read_text(string, end) };
        let mut paint = backend.stroke_paint.clone();
        backend.apply_text_style(&mut paint);
        let sigma = backend.filter_blur_sigma;
        if sigma > f32::EPSILON {
            backend.draw_with_blur(|inner| {
                let _ = inner.canvas.stroke_text(x, y, &text, &paint);
            });
        }

        match backend.canvas.stroke_text(x, y, &text, &paint) {
            Ok(metrics) => metrics.width(),
            Err(_) => 0.0,
        }
    })
}

#[no_mangle]
pub extern "C" fn nvgGetRenderTexture(ctx: *mut NVGcontext) -> *const c_void {
    with_ctx_ref(ctx, ptr::null(), |backend| {
        (&backend.interop_handle as *const BabylonCanvasNativeTextureHandle).cast::<c_void>()
    })
}

#[no_mangle]
pub extern "C" fn nvgTextBounds(
    ctx: *mut NVGcontext,
    x: f32,
    y: f32,
    string: *const c_char,
    end: *const c_char,
    bounds: *mut f32,
) -> f32 {
    with_ctx_mut(ctx, 0.0, |backend| {
        // SAFETY: `string` and `end` follow the nanovg C API contract.
        let text = unsafe { Backend::read_text(string, end) };
        let mut paint = backend.fill_paint.clone();
        backend.apply_text_style(&mut paint);
        match backend.canvas.measure_text(x, y, &text, &paint) {
            Ok(metrics) => {
                if !bounds.is_null() {
                    let left = metrics.x;
                    let right = metrics.x + metrics.width();
                    let top = metrics.y - metrics.height();
                    let bottom = metrics.y;
                    // SAFETY: The caller passes writable storage for four floats.
                    unsafe {
                        *bounds.add(0) = left;
                        *bounds.add(1) = top;
                        *bounds.add(2) = right;
                        *bounds.add(3) = bottom;
                    }
                }
                metrics.width()
            }
            Err(_) => 0.0,
        }
    })
}

#[no_mangle]
pub extern "C" fn nvgTextMetrics(
    ctx: *mut NVGcontext,
    ascender: *mut f32,
    descender: *mut f32,
    lineh: *mut f32,
) {
    with_ctx_mut(ctx, (), |backend| {
        let mut paint = backend.fill_paint.clone();
        backend.apply_text_style(&mut paint);
        if let Ok(metrics) = backend.canvas.measure_font(&paint) {
            // SAFETY: Each pointer is optional and writable when non-null.
            unsafe {
                if !ascender.is_null() {
                    *ascender = metrics.ascender();
                }
                if !descender.is_null() {
                    *descender = metrics.descender();
                }
                if !lineh.is_null() {
                    *lineh = metrics.height();
                }
            }
        }
    });
}

#[no_mangle]
pub extern "C" fn babylon_canvas_decode_image_rgba(
    data: *const u8,
    len: usize,
    out_width: *mut u32,
    out_height: *mut u32,
    out_rgba: *mut *mut u8,
    out_len: *mut usize,
) -> i32 {
    if data.is_null()
        || len == 0
        || out_width.is_null()
        || out_height.is_null()
        || out_rgba.is_null()
        || out_len.is_null()
    {
        return 0;
    }

    // SAFETY: The caller guarantees `data` points to `len` readable bytes.
    let encoded = unsafe { slice::from_raw_parts(data, len) };
    let dynamic = match image::load_from_memory(encoded) {
        Ok(image) => image,
        Err(_) => return 0,
    };

    let rgba = dynamic.to_rgba8();
    let (width, height) = rgba.dimensions();
    let boxed = rgba.into_raw().into_boxed_slice();
    let decoded_len = boxed.len();
    let decoded_ptr = Box::into_raw(boxed) as *mut u8;

    // SAFETY: Output pointers were validated as non-null above.
    unsafe {
        *out_width = width;
        *out_height = height;
        *out_rgba = decoded_ptr;
        *out_len = decoded_len;
    }

    1
}

#[no_mangle]
pub extern "C" fn babylon_canvas_free_bytes(data: *mut u8, len: usize) {
    if data.is_null() {
        return;
    }

    let slice_ptr = ptr::slice_from_raw_parts_mut(data, len);
    // SAFETY: `data` was allocated by `babylon_canvas_decode_image_rgba` via `Box<[u8]>`.
    unsafe {
        drop(Box::from_raw(slice_ptr));
    }
}
