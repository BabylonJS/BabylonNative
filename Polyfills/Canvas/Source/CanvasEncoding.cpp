#include "CanvasEncoding.h"

#include <basen.hpp>
#include <bimg/encode.h>
#include <bx/allocator.h>
#include <bx/readerwriter.h>
#include <iterator>
#include <stdexcept>

namespace Babylon::Polyfills::Internal
{
    std::string EncodeCanvasPNG(uint32_t width, uint32_t height, gsl::span<const uint8_t> rgba)
    {
        if (width == 0 || height == 0)
        {
            return "data:,";
        }
        if (static_cast<uint64_t>(width) * height * 4 != rgba.size())
        {
            throw std::invalid_argument{"Canvas.toDataURL: pixel dimensions do not match."};
        }

        bx::DefaultAllocator allocator;
        bx::MemoryBlock memory{&allocator};
        bx::MemoryWriter writer{&memory};
        bx::Error error{};
        bimg::imageWritePng(&writer, width, height, width * 4, rgba.data(), bimg::TextureFormat::RGBA8, false, &error);
        const auto size = static_cast<size_t>(bx::getSize(&writer));
        if (!error.isOk() || size == 0)
        {
            throw std::runtime_error{"Canvas.toDataURL: PNG encode failed."};
        }

        const auto* bytes = static_cast<const char*>(memory.more(0));
        std::string result{"data:image/png;base64,"};
        bn::encode_b64(bytes, bytes + size, std::back_inserter(result));
        return result;
    }
}
