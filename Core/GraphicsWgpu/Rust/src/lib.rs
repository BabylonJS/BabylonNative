#[cfg(target_os = "android")]
use std::ffi::CString;
use std::ffi::{c_char, c_void, CStr};
use std::ptr;
use std::sync::atomic::{AtomicBool, AtomicU32, AtomicU64, Ordering};
use std::sync::{Mutex, OnceLock};

use wgpu::util::DeviceExt;

#[cfg(feature = "upstream_wgpu_native")]
use babylon_wgpu_native_shim as upstream_wgpu_native;

#[cfg(not(feature = "upstream_wgpu_native"))]
mod upstream_wgpu_native {
    #[derive(Clone, Debug)]
    pub struct AdapterProbeInfo {
        pub backend: u32,
        pub vendor_id: u32,
        pub device_id: u32,
        pub adapter_name: String,
    }

    pub fn version() -> u32 {
        0
    }

    #[allow(dead_code)]
    pub fn probe_adapter(_prefer_low_power: bool) -> Result<AdapterProbeInfo, String> {
        Err("upstream wgpu-native probe is disabled at compile time".to_string())
    }

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
}

static WEBGPU_DRAW_ENABLED: AtomicBool = AtomicBool::new(false);
static RENDER_FRAME_COUNTER: AtomicU64 = AtomicU64::new(0);
static UPSTREAM_WGPU_NATIVE_VERSION: AtomicU32 = AtomicU32::new(0);
static DEBUG_TEXTURE_HASH: AtomicU64 = AtomicU64::new(0);
static DEBUG_TEXTURE_WIDTH: AtomicU32 = AtomicU32::new(0);
static DEBUG_TEXTURE_HEIGHT: AtomicU32 = AtomicU32::new(0);
static LAST_ERROR: OnceLock<Mutex<String>> = OnceLock::new();
static DEBUG_TEXTURE_UPLOAD: OnceLock<Mutex<Option<DebugTextureUpload>>> = OnceLock::new();
#[cfg(not(feature = "upstream_wgpu_native"))]
static COMPUTE_DISPATCH_CONTEXT: OnceLock<Mutex<Option<ComputeDispatchContext>>> = OnceLock::new();

#[cfg(not(feature = "upstream_wgpu_native"))]
struct ComputeDispatchContext {
    device: wgpu::Device,
    queue: wgpu::Queue,
    cached_shader_source: String,
    cached_entry_point: String,
    cached_pipeline: Option<wgpu::ComputePipeline>,
}

#[repr(C)]
#[derive(Clone, Copy)]
struct BabylonCanvasNativeTextureHandle {
    texture: *const c_void,
    device: *const c_void,
    queue: *const c_void,
    width: u32,
    height: u32,
}

struct DebugTextureUpload {
    width: u32,
    height: u32,
    rgba: Vec<u8>,
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

#[repr(C)]
#[derive(Clone, Copy)]
struct Uniforms {
    mvp: [f32; 16],
}

#[repr(C)]
#[derive(Clone, Copy)]
struct Vertex {
    position: [f32; 3],
    color: [f32; 3],
    uv: [f32; 2],
    face_id: u32,
}

const CUBE_VERTICES: [Vertex; 24] = [
    // Back face (-Z)
    Vertex {
        position: [-1.0, -1.0, -1.0],
        color: [1.0, 0.2, 0.2],
        uv: [0.0, 1.0],
        face_id: 0,
    },
    Vertex {
        position: [1.0, -1.0, -1.0],
        color: [0.2, 1.0, 0.2],
        uv: [1.0, 1.0],
        face_id: 0,
    },
    Vertex {
        position: [1.0, 1.0, -1.0],
        color: [0.2, 0.2, 1.0],
        uv: [1.0, 0.0],
        face_id: 0,
    },
    Vertex {
        position: [-1.0, 1.0, -1.0],
        color: [1.0, 1.0, 0.2],
        uv: [0.0, 0.0],
        face_id: 0,
    },
    // Front face (+Z) -> textured face
    Vertex {
        position: [-1.0, -1.0, 1.0],
        color: [1.0, 0.2, 1.0],
        uv: [0.0, 1.0],
        face_id: 1,
    },
    Vertex {
        position: [1.0, -1.0, 1.0],
        color: [0.2, 1.0, 1.0],
        uv: [1.0, 1.0],
        face_id: 1,
    },
    Vertex {
        position: [1.0, 1.0, 1.0],
        color: [1.0, 0.6, 0.2],
        uv: [1.0, 0.0],
        face_id: 1,
    },
    Vertex {
        position: [-1.0, 1.0, 1.0],
        color: [0.8, 0.8, 0.8],
        uv: [0.0, 0.0],
        face_id: 1,
    },
    // Left face (-X)
    Vertex {
        position: [-1.0, -1.0, -1.0],
        color: [0.95, 0.25, 0.25],
        uv: [0.0, 1.0],
        face_id: 2,
    },
    Vertex {
        position: [-1.0, 1.0, -1.0],
        color: [0.95, 0.25, 0.25],
        uv: [1.0, 1.0],
        face_id: 2,
    },
    Vertex {
        position: [-1.0, 1.0, 1.0],
        color: [0.95, 0.25, 0.25],
        uv: [1.0, 0.0],
        face_id: 2,
    },
    Vertex {
        position: [-1.0, -1.0, 1.0],
        color: [0.95, 0.25, 0.25],
        uv: [0.0, 0.0],
        face_id: 2,
    },
    // Right face (+X)
    Vertex {
        position: [1.0, -1.0, -1.0],
        color: [0.25, 0.95, 0.25],
        uv: [0.0, 1.0],
        face_id: 3,
    },
    Vertex {
        position: [1.0, -1.0, 1.0],
        color: [0.25, 0.95, 0.25],
        uv: [1.0, 1.0],
        face_id: 3,
    },
    Vertex {
        position: [1.0, 1.0, 1.0],
        color: [0.25, 0.95, 0.25],
        uv: [1.0, 0.0],
        face_id: 3,
    },
    Vertex {
        position: [1.0, 1.0, -1.0],
        color: [0.25, 0.95, 0.25],
        uv: [0.0, 0.0],
        face_id: 3,
    },
    // Top face (+Y)
    Vertex {
        position: [-1.0, 1.0, -1.0],
        color: [0.25, 0.25, 0.95],
        uv: [0.0, 1.0],
        face_id: 4,
    },
    Vertex {
        position: [1.0, 1.0, -1.0],
        color: [0.25, 0.25, 0.95],
        uv: [1.0, 1.0],
        face_id: 4,
    },
    Vertex {
        position: [1.0, 1.0, 1.0],
        color: [0.25, 0.25, 0.95],
        uv: [1.0, 0.0],
        face_id: 4,
    },
    Vertex {
        position: [-1.0, 1.0, 1.0],
        color: [0.25, 0.25, 0.95],
        uv: [0.0, 0.0],
        face_id: 4,
    },
    // Bottom face (-Y)
    Vertex {
        position: [-1.0, -1.0, -1.0],
        color: [0.95, 0.95, 0.25],
        uv: [0.0, 1.0],
        face_id: 5,
    },
    Vertex {
        position: [-1.0, -1.0, 1.0],
        color: [0.95, 0.95, 0.25],
        uv: [1.0, 1.0],
        face_id: 5,
    },
    Vertex {
        position: [1.0, -1.0, 1.0],
        color: [0.95, 0.95, 0.25],
        uv: [1.0, 0.0],
        face_id: 5,
    },
    Vertex {
        position: [1.0, -1.0, -1.0],
        color: [0.95, 0.95, 0.25],
        uv: [0.0, 0.0],
        face_id: 5,
    },
];

const CUBE_INDICES: [u16; 36] = [
    0, 1, 2, 2, 3, 0, // back
    4, 5, 6, 6, 7, 4, // front (textured)
    8, 9, 10, 10, 11, 8, // left
    12, 13, 14, 14, 15, 12, // right
    16, 17, 18, 18, 19, 16, // top
    20, 21, 22, 22, 23, 20, // bottom
];

const CUBE_SHADER_WGSL: &str = r#"
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

    // Keep non-textured faces visibly distinct while proving texture interop.
    return mix(vec4<f32>(input.color, 1.0), sampled, 0.35);
}
"#;

struct BackendContext {
    device: wgpu::Device,
    queue: wgpu::Queue,
    surface: Option<wgpu::Surface<'static>>,
    surface_config: Option<wgpu::SurfaceConfiguration>,
    offscreen_texture: Option<wgpu::Texture>,
    offscreen_view: Option<wgpu::TextureView>,
    depth_texture: wgpu::Texture,
    depth_view: wgpu::TextureView,
    render_target_format: wgpu::TextureFormat,
    render_pipeline: wgpu::RenderPipeline,
    uniform_bind_group_layout: wgpu::BindGroupLayout,
    uniform_buffer: wgpu::Buffer,
    uniform_bind_group: wgpu::BindGroup,
    canvas_sampler: wgpu::Sampler,
    canvas_texture: wgpu::Texture,
    canvas_texture_view: wgpu::TextureView,
    canvas_texture_width: u32,
    canvas_texture_height: u32,
    vertex_buffer: wgpu::Buffer,
    index_buffer: wgpu::Buffer,
    index_count: u32,
    max_texture_dimension_2d: u32,
    width: u32,
    height: u32,
    frame_index: u64,
    info: BabylonWgpuInfo,
}

impl BackendContext {
    fn clamped_dimension(value: u32, max_dimension: u32) -> u32 {
        value.max(1).min(max_dimension.max(1))
    }

    fn clamped_extent(width: u32, height: u32, max_dimension: u32) -> (u32, u32) {
        (
            Self::clamped_dimension(width, max_dimension),
            Self::clamped_dimension(height, max_dimension),
        )
    }

    fn create_offscreen_target(
        device: &wgpu::Device,
        width: u32,
        height: u32,
        max_texture_dimension_2d: u32,
        format: wgpu::TextureFormat,
    ) -> (wgpu::Texture, wgpu::TextureView) {
        let (width, height) = Self::clamped_extent(width, height, max_texture_dimension_2d);

        let texture = device.create_texture(&wgpu::TextureDescriptor {
            label: Some("babylon-native-webgpu.offscreen-color"),
            size: wgpu::Extent3d {
                width,
                height,
                depth_or_array_layers: 1,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format,
            usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
            view_formats: &[],
        });

        let view = texture.create_view(&wgpu::TextureViewDescriptor {
            label: Some("babylon-native-webgpu.offscreen-color-view"),
            ..Default::default()
        });

        (texture, view)
    }

    fn create_depth_target(
        device: &wgpu::Device,
        width: u32,
        height: u32,
        max_texture_dimension_2d: u32,
    ) -> (wgpu::Texture, wgpu::TextureView) {
        let (width, height) = Self::clamped_extent(width, height, max_texture_dimension_2d);

        let texture = device.create_texture(&wgpu::TextureDescriptor {
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

    fn create_pipeline_resources(
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        render_target_format: wgpu::TextureFormat,
    ) -> (
        wgpu::RenderPipeline,
        wgpu::BindGroupLayout,
        wgpu::Buffer,
        wgpu::BindGroup,
        wgpu::Sampler,
        wgpu::Texture,
        wgpu::TextureView,
        u32,
        u32,
        wgpu::Buffer,
        wgpu::Buffer,
        u32,
    ) {
        let shader = device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: Some("babylon-native-webgpu.cube-shader"),
            source: wgpu::ShaderSource::Wgsl(CUBE_SHADER_WGSL.into()),
        });

        let uniform_buffer = device.create_buffer(&wgpu::BufferDescriptor {
            label: Some("babylon-native-webgpu.uniform-buffer"),
            size: std::mem::size_of::<Uniforms>() as u64,
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
            mapped_at_creation: false,
        });

        let canvas_sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            label: Some("babylon-native-webgpu.canvas-sampler"),
            address_mode_u: wgpu::AddressMode::ClampToEdge,
            address_mode_v: wgpu::AddressMode::ClampToEdge,
            address_mode_w: wgpu::AddressMode::ClampToEdge,
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            mipmap_filter: wgpu::MipmapFilterMode::Linear,
            ..Default::default()
        });

        let canvas_texture = device.create_texture(&wgpu::TextureDescriptor {
            label: Some("babylon-native-webgpu.canvas-texture"),
            size: wgpu::Extent3d {
                width: 1,
                height: 1,
                depth_or_array_layers: 1,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8Unorm,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        });
        queue.write_texture(
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
        let canvas_texture_view = canvas_texture.create_view(&wgpu::TextureViewDescriptor {
            label: Some("babylon-native-webgpu.canvas-texture-view"),
            ..Default::default()
        });

        let bind_group_layout = device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
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
        });

        let uniform_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("babylon-native-webgpu.uniform-bind-group"),
            layout: &bind_group_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: uniform_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&canvas_sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(&canvas_texture_view),
                },
            ],
        });

        let pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("babylon-native-webgpu.pipeline-layout"),
            bind_group_layouts: &[&bind_group_layout],
            immediate_size: 0,
        });

        let vertex_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("babylon-native-webgpu.vertex-buffer"),
            contents: bytes_of_slice(&CUBE_VERTICES),
            usage: wgpu::BufferUsages::VERTEX,
        });

        let index_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("babylon-native-webgpu.index-buffer"),
            contents: bytes_of_slice(&CUBE_INDICES),
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

        let render_pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: Some("babylon-native-webgpu.cube-pipeline"),
            layout: Some(&pipeline_layout),
            vertex: wgpu::VertexState {
                module: &shader,
                entry_point: Some("vs_main"),
                compilation_options: wgpu::PipelineCompilationOptions::default(),
                buffers: &[wgpu::VertexBufferLayout {
                    array_stride: std::mem::size_of::<Vertex>() as u64,
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
                    format: render_target_format,
                    blend: Some(wgpu::BlendState::REPLACE),
                    write_mask: wgpu::ColorWrites::ALL,
                })],
            }),
            multiview_mask: None,
            cache: None,
        });

        (
            render_pipeline,
            bind_group_layout,
            uniform_buffer,
            uniform_bind_group,
            canvas_sampler,
            canvas_texture,
            canvas_texture_view,
            1,
            1,
            vertex_buffer,
            index_buffer,
            CUBE_INDICES.len() as u32,
        )
    }

    fn rebuild_uniform_bind_group(&mut self) {
        self.uniform_bind_group = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("babylon-native-webgpu.uniform-bind-group"),
            layout: &self.uniform_bind_group_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: self.uniform_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&self.canvas_sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(&self.canvas_texture_view),
                },
            ],
        });
    }

    fn install_debug_texture(&mut self, upload: DebugTextureUpload) -> bool {
        if upload.width == 0 || upload.height == 0 {
            return false;
        }

        let width = Self::clamped_dimension(upload.width, self.max_texture_dimension_2d);
        let height = Self::clamped_dimension(upload.height, self.max_texture_dimension_2d);
        let expected_len = (width as usize)
            .saturating_mul(height as usize)
            .saturating_mul(4);
        if upload.rgba.len() < expected_len {
            return false;
        }

        if self.canvas_texture_width != width || self.canvas_texture_height != height {
            // Recreate only on dimension changes; steady-state updates reuse the same GPU objects.
            let texture = self.device.create_texture(&wgpu::TextureDescriptor {
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
            let texture_view = texture.create_view(&wgpu::TextureViewDescriptor {
                label: Some("babylon-native-webgpu.canvas-texture-view"),
                ..Default::default()
            });

            self.canvas_texture = texture;
            self.canvas_texture_view = texture_view;
            self.canvas_texture_width = width;
            self.canvas_texture_height = height;
            self.rebuild_uniform_bind_group();
        }

        self.queue.write_texture(
            wgpu::TexelCopyTextureInfo {
                texture: &self.canvas_texture,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
                aspect: wgpu::TextureAspect::All,
            },
            &upload.rgba[..expected_len],
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

    fn apply_pending_debug_texture(&mut self) {
        if let Some(upload) = take_debug_texture_upload() {
            if !self.install_debug_texture(upload) {
                set_last_error("Failed to install native debug texture upload.");
            }
        }
    }

    fn reconfigure_surface(&mut self) {
        if let (Some(surface), Some(surface_config)) =
            (self.surface.as_ref(), self.surface_config.as_mut())
        {
            surface_config.width = self.width;
            surface_config.height = self.height;
            surface.configure(&self.device, surface_config);
        }
    }

    fn resize(&mut self, width: u32, height: u32) {
        (self.width, self.height) =
            Self::clamped_extent(width, height, self.max_texture_dimension_2d);

        if self.surface.is_some() {
            self.reconfigure_surface();
        } else {
            let (texture, view) = Self::create_offscreen_target(
                &self.device,
                self.width,
                self.height,
                self.max_texture_dimension_2d,
                self.render_target_format,
            );
            self.offscreen_texture = Some(texture);
            self.offscreen_view = Some(view);
        }

        let (depth_texture, depth_view) = Self::create_depth_target(
            &self.device,
            self.width,
            self.height,
            self.max_texture_dimension_2d,
        );
        self.depth_texture = depth_texture;
        self.depth_view = depth_view;
    }

    fn update_uniforms(&mut self) {
        let aspect = (self.width as f32 / self.height.max(1) as f32).max(0.0001);
        let t = self.frame_index as f32 * 0.016;

        let projection = perspective_rh_zo(60.0_f32.to_radians(), aspect, 0.1, 100.0);
        let view = translation_matrix(0.0, 0.0, -4.5);
        // Keep the textured face visible during validation while still animating.
        let model = mul_matrix(rotation_y_matrix(0.55 + t * 0.35), rotation_x_matrix(-0.20));
        let mvp = mul_matrix(mul_matrix(projection, view), model);

        let uniforms = Uniforms { mvp };
        self.queue
            .write_buffer(&self.uniform_buffer, 0, bytes_of(&uniforms));
    }

    fn render(&mut self) {
        RENDER_FRAME_COUNTER.fetch_add(1, Ordering::Relaxed);
        self.apply_pending_debug_texture();
        self.update_uniforms();

        let mut surface_frame: Option<wgpu::SurfaceTexture> = None;
        let color_view = if let Some(surface) = self.surface.as_ref() {
            match surface.get_current_texture() {
                Ok(frame) => {
                    surface_frame = Some(frame);
                    surface_frame
                        .as_ref()
                        .expect("surface frame available")
                        .texture
                        .create_view(&wgpu::TextureViewDescriptor {
                            label: Some("babylon-native-webgpu.surface-view"),
                            ..Default::default()
                        })
                }
                Err(wgpu::SurfaceError::Lost | wgpu::SurfaceError::Outdated) => {
                    self.reconfigure_surface();
                    return;
                }
                Err(wgpu::SurfaceError::Timeout) => {
                    return;
                }
                Err(wgpu::SurfaceError::OutOfMemory) => {
                    return;
                }
                Err(wgpu::SurfaceError::Other) => {
                    return;
                }
            }
        } else {
            if self.offscreen_view.is_none() {
                let (texture, view) = Self::create_offscreen_target(
                    &self.device,
                    self.width,
                    self.height,
                    self.max_texture_dimension_2d,
                    self.render_target_format,
                );
                self.offscreen_texture = Some(texture);
                self.offscreen_view = Some(view);
            }

            self.offscreen_view
                .as_ref()
                .expect("offscreen view available")
                .clone()
        };

        let mut encoder = self
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                label: Some("babylon-native-webgpu.encoder"),
            });

        let draw_enabled = WEBGPU_DRAW_ENABLED.load(Ordering::Acquire);

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
                multiview_mask: None,
            });

            if draw_enabled {
                render_pass.set_pipeline(&self.render_pipeline);
                render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
                render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
                render_pass
                    .set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
                render_pass.draw_indexed(0..self.index_count, 0, 0..1);
            }
        }

        self.queue.submit(Some(encoder.finish()));
        if let Some(frame) = surface_frame {
            frame.present();
        }

        self.frame_index = self.frame_index.wrapping_add(1);
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
    DEBUG_TEXTURE_HASH.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_debug_texture_width() -> u32 {
    DEBUG_TEXTURE_WIDTH.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_debug_texture_height() -> u32 {
    DEBUG_TEXTURE_HEIGHT.load(Ordering::Relaxed)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_reset_webgpu_draw_requested() {
    WEBGPU_DRAW_ENABLED.store(false, Ordering::Release);
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_set_debug_texture_from_native(
    native_texture: *const c_void,
    width: u32,
    height: u32,
) -> bool {
    if native_texture.is_null() {
        return false;
    }

    let result = std::panic::catch_unwind(|| {
        // SAFETY: The caller passes a pointer to a BabylonCanvasNativeTextureHandle.
        let native_handle =
            unsafe { &*(native_texture as *const BabylonCanvasNativeTextureHandle) };
        read_texture_rgba_from_native_handle(native_handle, width, height)
    });

    match result {
        Ok(Ok(upload)) => {
            set_debug_texture_upload(upload);
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

fn bytes_of<T>(value: &T) -> &[u8] {
    // SAFETY: T is plain-old-data for our FFI/vertex/uniform structs.
    unsafe {
        std::slice::from_raw_parts((value as *const T).cast::<u8>(), std::mem::size_of::<T>())
    }
}

fn bytes_of_slice<T>(slice: &[T]) -> &[u8] {
    // SAFETY: T is plain-old-data for our FFI/vertex structs.
    unsafe { std::slice::from_raw_parts(slice.as_ptr().cast::<u8>(), std::mem::size_of_val(slice)) }
}

fn mul_matrix(a: [f32; 16], b: [f32; 16]) -> [f32; 16] {
    let mut out = [0.0f32; 16];
    for col in 0..4 {
        for row in 0..4 {
            out[col * 4 + row] = a[0 * 4 + row] * b[col * 4 + 0]
                + a[1 * 4 + row] * b[col * 4 + 1]
                + a[2 * 4 + row] * b[col * 4 + 2]
                + a[3 * 4 + row] * b[col * 4 + 3];
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

fn backend_to_u32(backend: wgpu::Backend) -> u32 {
    match backend {
        wgpu::Backend::Noop => 0,
        wgpu::Backend::Vulkan => 1,
        wgpu::Backend::Metal => 2,
        wgpu::Backend::Dx12 => 3,
        wgpu::Backend::Gl => 4,
        wgpu::Backend::BrowserWebGpu => 5,
    }
}

fn fill_adapter_name(name: &str) -> [c_char; 128] {
    let mut output = [0 as c_char; 128];
    let bytes = name.as_bytes();
    let max_count = output.len().saturating_sub(1);
    let copy_count = bytes.len().min(max_count);

    // SAFETY: output and bytes are valid for copy_count bytes and non-overlapping.
    unsafe {
        std::ptr::copy_nonoverlapping(bytes.as_ptr(), output.as_mut_ptr().cast::<u8>(), copy_count);
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
    if let Ok(mut value) = storage.lock() {
        value.clear();
        value.push_str(message);
    }

    log_backend_error(message);
}

fn clear_last_error() {
    let storage = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    if let Ok(mut value) = storage.lock() {
        value.clear();
    }
}

fn copy_last_error(output: *mut c_char, output_len: usize) -> bool {
    if output.is_null() || output_len == 0 {
        return false;
    }

    let storage = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    let message = match storage.lock() {
        Ok(value) if !value.is_empty() => value,
        _ => return false,
    };

    let bytes = message.as_bytes();
    let max_copy = output_len.saturating_sub(1);
    let copy_len = bytes.len().min(max_copy);

    // SAFETY: Caller provides a valid writable output buffer with output_len bytes.
    unsafe {
        std::ptr::copy_nonoverlapping(bytes.as_ptr(), output.cast::<u8>(), copy_len);
        *output.add(copy_len) = 0;
    }

    true
}

fn set_debug_texture_upload(upload: DebugTextureUpload) {
    DEBUG_TEXTURE_WIDTH.store(upload.width, Ordering::Relaxed);
    DEBUG_TEXTURE_HEIGHT.store(upload.height, Ordering::Relaxed);
    DEBUG_TEXTURE_HASH.store(hash_bytes(&upload.rgba), Ordering::Relaxed);

    let storage = DEBUG_TEXTURE_UPLOAD.get_or_init(|| Mutex::new(None));
    if let Ok(mut value) = storage.lock() {
        *value = Some(upload);
    }
}

fn take_debug_texture_upload() -> Option<DebugTextureUpload> {
    let storage = DEBUG_TEXTURE_UPLOAD.get_or_init(|| Mutex::new(None));
    match storage.lock() {
        Ok(mut value) => value.take(),
        Err(_) => None,
    }
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

fn read_texture_rgba_from_native_handle(
    handle: &BabylonCanvasNativeTextureHandle,
    requested_width: u32,
    requested_height: u32,
) -> Result<DebugTextureUpload, String> {
    let source_texture = if handle.texture.is_null() {
        return Err("native texture pointer was null".to_string());
    } else {
        // SAFETY: `handle.texture` originates from CanvasWgpu Rust and points to a live wgpu::Texture.
        unsafe { (handle.texture as *const wgpu::Texture).as_ref() }
            .ok_or_else(|| "native texture pointer was invalid".to_string())?
    };

    let source_device = if handle.device.is_null() {
        return Err("native device pointer was null".to_string());
    } else {
        // SAFETY: `handle.device` originates from CanvasWgpu Rust and points to a live wgpu::Device.
        unsafe { (handle.device as *const wgpu::Device).as_ref() }
            .ok_or_else(|| "native device pointer was invalid".to_string())?
    };

    let source_queue = if handle.queue.is_null() {
        return Err("native queue pointer was null".to_string());
    } else {
        // SAFETY: `handle.queue` originates from CanvasWgpu Rust and points to a live wgpu::Queue.
        unsafe { (handle.queue as *const wgpu::Queue).as_ref() }
            .ok_or_else(|| "native queue pointer was invalid".to_string())?
    };

    let width = if requested_width == 0 {
        handle.width
    } else {
        requested_width
    }
    .max(1);
    let height = if requested_height == 0 {
        handle.height
    } else {
        requested_height
    }
    .max(1);

    let unpadded_bytes_per_row = width.saturating_mul(4);
    if unpadded_bytes_per_row == 0 {
        return Err("invalid native texture width".to_string());
    }

    let padded_bytes_per_row = align_to(unpadded_bytes_per_row, wgpu::COPY_BYTES_PER_ROW_ALIGNMENT);
    let buffer_size = (padded_bytes_per_row as u64).saturating_mul(height as u64);
    if buffer_size == 0 {
        return Err("invalid native texture size".to_string());
    }

    let staging_buffer = source_device.create_buffer(&wgpu::BufferDescriptor {
        label: Some("babylon-native-webgpu.native-debug-readback"),
        size: buffer_size,
        usage: wgpu::BufferUsages::COPY_DST | wgpu::BufferUsages::MAP_READ,
        mapped_at_creation: false,
    });

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
    let mut rgba = vec![
        0u8;
        (width as usize)
            .saturating_mul(height as usize)
            .saturating_mul(4)
    ];
    for row in 0..(height as usize) {
        let src_start = row.saturating_mul(padded_bytes_per_row as usize);
        let src_end = src_start.saturating_add(unpadded_bytes_per_row as usize);
        let dst_start = row.saturating_mul(unpadded_bytes_per_row as usize);
        let dst_end = dst_start.saturating_add(unpadded_bytes_per_row as usize);
        rgba[dst_start..dst_end].copy_from_slice(&mapped[src_start..src_end]);
    }
    drop(mapped);
    staging_buffer.unmap();

    Ok(DebugTextureUpload {
        width,
        height,
        rgba,
    })
}

fn create_surface(
    instance: &wgpu::Instance,
    surface_layer: *mut c_void,
) -> Result<Option<wgpu::Surface<'static>>, String> {
    if surface_layer.is_null() {
        return Ok(None);
    }

    #[cfg(any(target_os = "macos", target_os = "ios"))]
    {
        // SAFETY: The host passes a valid CoreAnimation layer pointer that stays alive
        // for the lifetime of this backend context.
        return unsafe {
            instance
                .create_surface_unsafe(wgpu::SurfaceTargetUnsafe::CoreAnimationLayer(surface_layer))
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

        // SAFETY: The host passes an ANativeWindow* that remains valid while the
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

        // SAFETY: The host passes a valid HWND that remains alive while the surface
        // is alive.
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

fn create_context(config: BabylonWgpuConfig) -> Result<Box<BackendContext>, String> {
    let requested_width = config.width.max(1);
    let requested_height = config.height.max(1);

    let instance = create_instance();

    let surface = create_surface(&instance, config.surface_layer)?;

    let power_preference = if config.prefer_low_power != 0 {
        wgpu::PowerPreference::LowPower
    } else {
        wgpu::PowerPreference::HighPerformance
    };

    #[cfg(feature = "upstream_wgpu_native")]
    let upstream_probe =
        match upstream_wgpu_native::ensure_bootstrap_runtime(config.prefer_low_power != 0) {
            Ok(info) => Some(info),
            Err(error) => {
                log_backend_error(&format!(
                    "GraphicsWgpu upstream adapter probe failed: {error}"
                ));
                None
            }
        };

    #[cfg(not(feature = "upstream_wgpu_native"))]
    let upstream_probe: Option<upstream_wgpu_native::AdapterProbeInfo> = None;

    let mut used_fallback_adapter = false;
    let mut adapter_errors: Vec<String> = Vec::new();
    let mut adapter: Option<wgpu::Adapter> = None;

    #[cfg(target_os = "android")]
    {
        match pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
            power_preference,
            force_fallback_adapter: false,
            compatible_surface: None,
        })) {
            Ok(candidate) => {
                adapter = Some(candidate);
            }
            Err(error) => {
                adapter_errors.push(format!("android_without_surface={error}"));
            }
        }
    }

    if let Some(surface_ref) = surface.as_ref() {
        match pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
            power_preference,
            force_fallback_adapter: false,
            compatible_surface: Some(surface_ref),
        })) {
            Ok(candidate) => {
                adapter = Some(candidate);
            }
            Err(error) => {
                adapter_errors.push(format!("with_surface={error}"));
            }
        }
    }

    if adapter.is_none() {
        match pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
            power_preference,
            force_fallback_adapter: false,
            compatible_surface: None,
        })) {
            Ok(candidate) => {
                adapter = Some(candidate);
            }
            Err(error) => {
                adapter_errors.push(format!("without_surface={error}"));
            }
        }
    }

    if adapter.is_none() {
        if let Some(surface_ref) = surface.as_ref() {
            match pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
                power_preference,
                force_fallback_adapter: true,
                compatible_surface: Some(surface_ref),
            })) {
                Ok(candidate) => {
                    used_fallback_adapter = true;
                    adapter = Some(candidate);
                }
                Err(error) => {
                    adapter_errors.push(format!("with_surface_fallback={error}"));
                }
            }
        }
    }

    if adapter.is_none() {
        match pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
            power_preference,
            force_fallback_adapter: true,
            compatible_surface: None,
        })) {
            Ok(candidate) => {
                used_fallback_adapter = true;
                adapter = Some(candidate);
            }
            Err(error) => {
                adapter_errors.push(format!("without_surface_fallback={error}"));
            }
        }
    }

    #[allow(unused_mut)]
    let mut adapter = adapter.ok_or_else(|| {
        format!(
            "Failed to acquire GPU adapter. {}",
            adapter_errors.join("; ")
        )
    })?;

    if used_fallback_adapter {
        log_backend_error(
            "No hardware Vulkan adapter found; continuing with fallback Vulkan adapter.",
        );
    }

    #[allow(unused_mut)]
    let mut adapter_info = adapter.get_info();
    #[allow(unused_mut)]
    let mut adapter_limits = adapter.limits();
    let max_texture_dimension_2d = adapter_limits.max_texture_dimension_2d.max(1);

    let (width, height) =
        BackendContext::clamped_extent(requested_width, requested_height, max_texture_dimension_2d);

    let request_device = |candidate: &wgpu::Adapter, limits: &wgpu::Limits| {
        let device_descriptor = wgpu::DeviceDescriptor {
            label: Some("babylon-native-webgpu.device"),
            required_features: wgpu::Features::empty(),
            required_limits: limits.clone(),
            experimental_features: wgpu::ExperimentalFeatures::disabled(),
            memory_hints: wgpu::MemoryHints::default(),
            trace: wgpu::Trace::default(),
        };

        pollster::block_on(candidate.request_device(&device_descriptor))
    };

    let (device, queue) = match request_device(&adapter, &adapter_limits) {
        Ok(result) => result,
        Err(error) => {
            #[allow(unused_mut, unused_variables)]
            let mut request_errors = vec![format!(
                "adapter=\"{}\" backend={:?} error={error}",
                adapter_info.name, adapter_info.backend
            )];

            #[cfg(target_os = "android")]
            {
                let mut recovered: Option<(wgpu::Device, wgpu::Queue)> = None;

                'retry: for retry_power_preference in
                    [power_preference, wgpu::PowerPreference::LowPower]
                {
                    for force_fallback_adapter in [false, true] {
                        let retry_adapter = match pollster::block_on(instance.request_adapter(
                            &wgpu::RequestAdapterOptions {
                                power_preference: retry_power_preference,
                                force_fallback_adapter,
                                compatible_surface: None,
                            },
                        )) {
                            Ok(candidate) => candidate,
                            Err(retry_error) => {
                                request_errors.push(format!(
                                    "retry_adapter power={retry_power_preference:?} fallback={force_fallback_adapter} error={retry_error}"
                                ));
                                continue;
                            }
                        };

                        let retry_info = retry_adapter.get_info();
                        let retry_limits = retry_adapter.limits();

                        match request_device(&retry_adapter, &retry_limits) {
                            Ok((retry_device, retry_queue)) => {
                                adapter = retry_adapter;
                                adapter_info = retry_info;
                                adapter_limits = retry_limits;
                                if force_fallback_adapter {
                                    used_fallback_adapter = true;
                                }
                                recovered = Some((retry_device, retry_queue));
                                break 'retry;
                            }
                            Err(retry_error) => {
                                request_errors.push(format!(
                                    "retry_device adapter=\"{}\" backend={:?} power={retry_power_preference:?} fallback={force_fallback_adapter} error={retry_error}",
                                    retry_info.name, retry_info.backend
                                ));
                            }
                        }
                    }
                }

                if let Some(result) = recovered {
                    result
                } else {
                    return Err(format!(
                        "Failed to create GPU device: {error}. {}",
                        request_errors.join("; ")
                    ));
                }
            }

            #[cfg(not(target_os = "android"))]
            {
                return Err(format!("Failed to create GPU device: {error}"));
            }
        }
    };

    let mut surface_config = None;
    if let Some(surface_ref) = surface.as_ref() {
        let mut config = surface_ref
            .get_default_config(&adapter, width, height)
            .ok_or_else(|| "Surface returned no default configuration.".to_string())?;

        let caps = surface_ref.get_capabilities(&adapter);
        if caps.formats.contains(&wgpu::TextureFormat::Bgra8UnormSrgb) {
            config.format = wgpu::TextureFormat::Bgra8UnormSrgb;
        } else if caps.formats.contains(&wgpu::TextureFormat::Bgra8Unorm) {
            config.format = wgpu::TextureFormat::Bgra8Unorm;
        }
        if caps.alpha_modes.contains(&wgpu::CompositeAlphaMode::Opaque) {
            config.alpha_mode = wgpu::CompositeAlphaMode::Opaque;
        }

        surface_ref.configure(&device, &config);
        surface_config = Some(config);
    }

    let render_target_format = surface_config
        .as_ref()
        .map(|config| config.format)
        .unwrap_or(wgpu::TextureFormat::Rgba8Unorm);

    let (offscreen_texture, offscreen_view) = if surface.is_none() {
        let (texture, view) = BackendContext::create_offscreen_target(
            &device,
            width,
            height,
            max_texture_dimension_2d,
            render_target_format,
        );
        (Some(texture), Some(view))
    } else {
        (None, None)
    };

    let (depth_texture, depth_view) =
        BackendContext::create_depth_target(&device, width, height, max_texture_dimension_2d);

    let (
        render_pipeline,
        uniform_bind_group_layout,
        uniform_buffer,
        uniform_bind_group,
        canvas_sampler,
        canvas_texture,
        canvas_texture_view,
        canvas_texture_width,
        canvas_texture_height,
        vertex_buffer,
        index_buffer,
        index_count,
    ) = BackendContext::create_pipeline_resources(&device, &queue, render_target_format);

    let context_info = if let Some(upstream_info) = upstream_probe.as_ref() {
        BabylonWgpuInfo {
            backend: upstream_info.backend,
            vendor_id: upstream_info.vendor_id,
            device_id: upstream_info.device_id,
            adapter_name: fill_adapter_name(
                decorated_adapter_name(upstream_info.adapter_name.as_str()).as_str(),
            ),
        }
    } else {
        BabylonWgpuInfo {
            backend: backend_to_u32(adapter_info.backend),
            vendor_id: adapter_info.vendor,
            device_id: adapter_info.device,
            adapter_name: fill_adapter_name(
                decorated_adapter_name(adapter_info.name.as_str()).as_str(),
            ),
        }
    };

    let context = BackendContext {
        device,
        queue,
        surface,
        surface_config,
        offscreen_texture,
        offscreen_view,
        depth_texture,
        depth_view,
        render_target_format,
        render_pipeline,
        uniform_bind_group_layout,
        uniform_buffer,
        uniform_bind_group,
        canvas_sampler,
        canvas_texture,
        canvas_texture_view,
        canvas_texture_width,
        canvas_texture_height,
        vertex_buffer,
        index_buffer,
        index_count,
        max_texture_dimension_2d,
        width,
        height,
        frame_index: 0,
        info: context_info,
    };

    Ok(Box::new(context))
}

#[cfg(feature = "upstream_wgpu_native")]
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

#[cfg(not(feature = "upstream_wgpu_native"))]
fn dispatch_compute_global(shader_source: &str, entry_point: &str, x: u32, y: u32, z: u32) -> bool {
    fn compute_dispatch_context() -> &'static Mutex<Option<ComputeDispatchContext>> {
        COMPUTE_DISPATCH_CONTEXT.get_or_init(|| Mutex::new(None))
    }

    fn initialize_compute_dispatch_context() -> Result<ComputeDispatchContext, String> {
        let instance = create_instance();

        let adapter =
            match pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::HighPerformance,
                force_fallback_adapter: false,
                compatible_surface: None,
            })) {
                Ok(adapter) => adapter,
                Err(_) => {
                    pollster::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
                        power_preference: wgpu::PowerPreference::HighPerformance,
                        force_fallback_adapter: true,
                        compatible_surface: None,
                    }))
                    .map_err(|error| {
                        format!("request_adapter failed for compute context: {error}")
                    })?
                }
            };

        let device_descriptor = wgpu::DeviceDescriptor {
            label: Some("babylon-native-webgpu.compute-device"),
            required_features: wgpu::Features::empty(),
            required_limits: adapter.limits(),
            experimental_features: wgpu::ExperimentalFeatures::disabled(),
            memory_hints: wgpu::MemoryHints::default(),
            trace: wgpu::Trace::default(),
        };

        let (device, queue) = pollster::block_on(adapter.request_device(&device_descriptor))
            .map_err(|error| format!("request_device failed for compute context: {error}"))?;

        Ok(ComputeDispatchContext {
            device,
            queue,
            cached_shader_source: String::new(),
            cached_entry_point: String::new(),
            cached_pipeline: None,
        })
    }

    let mut guard = match compute_dispatch_context().lock() {
        Ok(lock) => lock,
        Err(poisoned) => poisoned.into_inner(),
    };

    if guard.is_none() {
        match initialize_compute_dispatch_context() {
            Ok(context) => {
                *guard = Some(context);
            }
            Err(error) => {
                log_backend_error(&format!(
                    "local compute dispatch context initialization failed: {error}"
                ));
                return false;
            }
        }
    }

    let context = guard
        .as_mut()
        .expect("compute dispatch context initialized before use");

    let pipeline_needs_rebuild = context.cached_pipeline.is_none()
        || context.cached_shader_source != shader_source
        || context.cached_entry_point != entry_point;

    if pipeline_needs_rebuild {
        let shader_module = context
            .device
            .create_shader_module(wgpu::ShaderModuleDescriptor {
                label: Some("babylon-native-webgpu.compute-shader"),
                source: wgpu::ShaderSource::Wgsl(shader_source.into()),
            });

        let pipeline_layout =
            context
                .device
                .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                    label: Some("babylon-native-webgpu.compute-layout"),
                    bind_group_layouts: &[],
                    immediate_size: 0,
                });

        let compute_pipeline =
            context
                .device
                .create_compute_pipeline(&wgpu::ComputePipelineDescriptor {
                    label: Some("babylon-native-webgpu.compute-pipeline"),
                    layout: Some(&pipeline_layout),
                    module: &shader_module,
                    entry_point: Some(entry_point),
                    compilation_options: wgpu::PipelineCompilationOptions::default(),
                    cache: None,
                });

        context.cached_shader_source.clear();
        context.cached_shader_source.push_str(shader_source);
        context.cached_entry_point.clear();
        context.cached_entry_point.push_str(entry_point);
        context.cached_pipeline = Some(compute_pipeline);
    }

    let compute_pipeline = context
        .cached_pipeline
        .as_ref()
        .expect("compute pipeline cached before encoding dispatch");

    let mut encoder = context
        .device
        .create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("babylon-native-webgpu.compute-encoder"),
        });

    {
        let mut pass = encoder.begin_compute_pass(&wgpu::ComputePassDescriptor {
            label: Some("babylon-native-webgpu.compute-pass"),
            timestamp_writes: None,
        });
        pass.set_pipeline(compute_pipeline);
        pass.dispatch_workgroups(x.max(1), y.max(1), z.max(1));
    }

    context.queue.submit(Some(encoder.finish()));
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
        // Android emulator Vulkan stacks often report non-compliant conformance
        // metadata while still being usable for development/testing.
        descriptor.flags |= wgpu::InstanceFlags::ALLOW_UNDERLYING_NONCOMPLIANT_ADAPTER;
    }

    wgpu::Instance::new(&descriptor)
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_create(config: *const BabylonWgpuConfig) -> *mut c_void {
    clear_last_error();

    let upstream_version = upstream_wgpu_native::version();
    UPSTREAM_WGPU_NATIVE_VERSION.store(upstream_version, Ordering::Relaxed);
    if let Some(version_text) = format_upstream_wgpu_native_version(upstream_version) {
        log_backend_error(&format!(
            "GraphicsWgpu upstream probe active: wgpu-native {version_text}"
        ));
    }

    let config_value = if config.is_null() {
        default_config()
    } else {
        // SAFETY: The caller guarantees the pointer validity for reads.
        unsafe { *config }
    };

    let result = std::panic::catch_unwind(|| create_context(config_value));
    match result {
        Ok(Ok(context)) => Box::into_raw(context) as *mut c_void,
        Ok(Err(error)) => {
            set_last_error(&error);
            ptr::null_mut()
        }
        Err(_) => {
            set_last_error("WGPU backend initialization panicked.");
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
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_resize(context: *mut c_void, width: u32, height: u32) -> bool {
    if context.is_null() {
        return false;
    }

    let result = std::panic::catch_unwind(|| {
        // SAFETY: The pointer was created by babylon_wgpu_create and remains valid for the call duration.
        let context_ref = unsafe { &mut *(context as *mut BackendContext) };
        context_ref.resize(width, height);
    });

    result.is_ok()
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_render(context: *mut c_void) -> bool {
    if context.is_null() {
        return false;
    }

    let result = std::panic::catch_unwind(|| {
        // SAFETY: The pointer was created by babylon_wgpu_create and remains valid for the call duration.
        let context_ref = unsafe { &mut *(context as *mut BackendContext) };
        context_ref.render();
    });

    result.is_ok()
}

#[no_mangle]
pub extern "C" fn babylon_wgpu_get_info(
    context: *const c_void,
    output_info: *mut BabylonWgpuInfo,
) -> bool {
    if context.is_null() || output_info.is_null() {
        return false;
    }

    // SAFETY: The pointers are non-null and expected to be valid by the C ABI contract.
    let context_ref = unsafe { &*(context as *const BackendContext) };
    // SAFETY: output_info is valid for writes by contract.
    unsafe {
        *output_info = context_ref.info;
    }

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
    if shader_source.is_null() || entry_point.is_null() {
        return false;
    }

    let shader = {
        // SAFETY: Pointers are expected to be valid NUL-terminated UTF-8 strings by C ABI contract.
        let cstr = unsafe { CStr::from_ptr(shader_source) };
        match cstr.to_str() {
            Ok(text) => text,
            Err(_) => return false,
        }
    };

    let entry = {
        // SAFETY: Pointers are expected to be valid NUL-terminated UTF-8 strings by C ABI contract.
        let cstr = unsafe { CStr::from_ptr(entry_point) };
        match cstr.to_str() {
            Ok(text) => text,
            Err(_) => return false,
        }
    };

    let result = std::panic::catch_unwind(|| dispatch_compute_global(shader, entry, x, y, z));
    match result {
        Ok(value) => value,
        Err(_) => false,
    }
}

#[cfg(test)]
mod tests {
    use super::BackendContext;

    #[test]
    fn clamped_extent_limits_to_adapter_max_dimension() {
        let (width, height) = BackendContext::clamped_extent(2560, 1440, 2048);
        assert_eq!(width, 2048);
        assert_eq!(height, 1440);
    }

    #[test]
    fn clamped_extent_enforces_minimum_of_one() {
        let (width, height) = BackendContext::clamped_extent(0, 0, 0);
        assert_eq!(width, 1);
        assert_eq!(height, 1);
    }
}
