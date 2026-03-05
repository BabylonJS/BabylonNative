#include "App.h"

#include <Metal/Metal.hpp>

#include <mach-o/dyld.h>

#include <vector>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>

namespace
{
    std::filesystem::path GetExecutablePath()
    {
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0)
        {
            throw std::runtime_error("Failed to get executable path");
        }
        return std::filesystem::canonical(path).parent_path();
    }

    MTL::Texture* CreateMetalRenderTargetTexture(MTL::Device* device)
    {
        MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(
            MTL::PixelFormatRGBA8Unorm, WIDTH, HEIGHT, false);
        descriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        descriptor->setStorageMode(MTL::StorageModeManaged);

        MTL::Texture* texture = device->newTexture(descriptor);
        if (!texture)
        {
            throw std::runtime_error("Failed to create Metal render target texture");
        }
        return texture;
    }

    void SaveTextureAsPNG(MTL::Texture* texture, const std::filesystem::path& filePath)
    {
        const uint32_t width = static_cast<uint32_t>(texture->width());
        const uint32_t height = static_cast<uint32_t>(texture->height());
        const uint32_t bytesPerRow = width * 4;
        std::vector<uint8_t> pixels(bytesPerRow * height);

        // Read back the texture data.
        MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
        texture->getBytes(pixels.data(), bytesPerRow, region, 0);

        // Create a CGImage from the pixel data.
        CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
        CGContextRef context = CGBitmapContextCreate(
            pixels.data(), width, height, 8, bytesPerRow,
            colorSpace, kCGImageAlphaPremultipliedLast);

        CGImageRef image = CGBitmapContextCreateImage(context);

        // Save as PNG using NSBitmapImageRep.
        NSBitmapImageRep* rep = [[NSBitmapImageRep alloc] initWithCGImage:image];
        NSData* pngData = [rep representationUsingType:NSBitmapImageRepFileTypePNG properties:@{}];

        NSString* nsPath = [NSString stringWithUTF8String:filePath.string().c_str()];
        [pngData writeToFile:nsPath atomically:YES];

        CGImageRelease(image);
        CGContextRelease(context);
        CGColorSpaceRelease(colorSpace);
    }

    ImageData LoadPNG(const std::filesystem::path& filePath)
    {
        @autoreleasepool
        {
            NSString* nsPath = [NSString stringWithUTF8String:filePath.string().c_str()];
            NSData* fileData = [NSData dataWithContentsOfFile:nsPath];
            if (!fileData)
            {
                throw std::runtime_error("Failed to read PNG file: " + filePath.string());
            }

            CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)fileData);
            CGImageRef cgImage = CGImageCreateWithPNGDataProvider(provider, nullptr, true, kCGRenderingIntentDefault);
            CGDataProviderRelease(provider);

            if (!cgImage)
            {
                throw std::runtime_error("Failed to decode PNG: " + filePath.string());
            }

            const uint32_t width = static_cast<uint32_t>(CGImageGetWidth(cgImage));
            const uint32_t height = static_cast<uint32_t>(CGImageGetHeight(cgImage));
            const uint32_t bytesPerRow = width * 4;
            std::vector<uint8_t> pixels(static_cast<size_t>(bytesPerRow) * height);

            CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
            CGContextRef context = CGBitmapContextCreate(
                pixels.data(), width, height, 8, bytesPerRow,
                colorSpace, kCGImageAlphaPremultipliedLast);

            CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);

            CGContextRelease(context);
            CGColorSpaceRelease(colorSpace);
            CGImageRelease(cgImage);

            return {std::move(pixels), width, height};
        }
    }
}

int main()
{
    @autoreleasepool
    {
        auto executablePath = GetExecutablePath();

        // Create a Metal device.
        MTL::Device* metalDevice = MTL::CreateSystemDefaultDevice();
        if (!metalDevice)
        {
            throw std::runtime_error("Failed to create Metal device");
        }

        // Create a render target texture for the output.
        MTL::Texture* outputTexture = CreateMetalRenderTargetTexture(metalDevice);

        Babylon::Graphics::Configuration config{};
        config.Device = metalDevice;
        config.Width = WIDTH;
        config.Height = HEIGHT;

        int result = RunApp(
            executablePath,
            config,
            {outputTexture},
            [&](const std::filesystem::path& filePath) {
                SaveTextureAsPNG(outputTexture, filePath);
            },
            LoadPNG);

        // Clean up.
        outputTexture->release();
        metalDevice->release();

        return result;
    }
}
