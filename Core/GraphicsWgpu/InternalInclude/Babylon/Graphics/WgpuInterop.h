#pragma once

#include <cstddef>
#include <cstdint>

// Shared C ABI declarations for the GraphicsWgpu Rust backend.
// Keep this header as the single declaration source used by both
// Core/GraphicsWgpu and Plugins/NativeWebGPU.

struct BabylonWgpuConfig final
{
    uint32_t width{};
    uint32_t height{};
    void* surface_layer{};
    uint8_t prefer_low_power{};
    uint8_t enable_validation{};
    uint8_t reserved0{};
    uint8_t reserved1{};
};

struct BabylonWgpuInfo final
{
    uint32_t backend{};
    uint32_t vendor_id{};
    uint32_t device_id{};
    char adapter_name[128]{};
};

extern "C"
{
    void* babylon_wgpu_create(const BabylonWgpuConfig* config);
    void babylon_wgpu_destroy(void* context);
    bool babylon_wgpu_resize(void* context, uint32_t width, uint32_t height);
    bool babylon_wgpu_render(void* context);
    bool babylon_wgpu_get_info(const void* context, BabylonWgpuInfo* output_info);
    bool babylon_wgpu_get_last_error(char* output, size_t output_len);

    bool babylon_wgpu_dispatch_compute_global(
        const char* shader_source,
        const char* entry_point,
        uint32_t x,
        uint32_t y,
        uint32_t z);
    void babylon_wgpu_mark_webgpu_draw_requested();
    bool babylon_wgpu_is_webgpu_draw_enabled();
    uint64_t babylon_wgpu_get_render_frame_count();
    uint64_t babylon_wgpu_get_canvas_texture_hash();
    uint32_t babylon_wgpu_get_canvas_texture_width();
    uint32_t babylon_wgpu_get_canvas_texture_height();
    // TODO(bgfx-removal): Remove these legacy debug_texture aliases once all call
    // sites have migrated to the canvas-prefixed names above.
    uint64_t babylon_wgpu_get_debug_texture_hash();
    uint32_t babylon_wgpu_get_debug_texture_width();
    uint32_t babylon_wgpu_get_debug_texture_height();
    uint64_t babylon_wgpu_get_estimated_gpu_memory_bytes();
    uint64_t babylon_wgpu_get_canvas_texture_import_skip_count();
    // TODO(bgfx-removal): Remove this legacy alias.
    uint64_t babylon_wgpu_get_debug_texture_import_skip_count();
    void babylon_wgpu_reset_webgpu_draw_requested();
    bool babylon_wgpu_import_canvas_texture_from_native(const void* native_texture, uint32_t width, uint32_t height);
    // TODO(bgfx-removal): Remove this legacy alias.
    bool babylon_wgpu_set_debug_texture_from_native(const void* native_texture, uint32_t width, uint32_t height);
}
