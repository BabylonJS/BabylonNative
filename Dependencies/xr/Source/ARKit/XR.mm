#include <XR.h>

#import <UIKit/UIKit.h>
#import <ARKit/ARKit.h>
#import <ARKit/ARConfiguration.h>
#import <MetalKit/MetalKit.h>

@interface SessionDelegate : NSObject <ARSessionDelegate>
@property (readonly) id<MTLTexture> cameraTextureY;
@property (readonly) id<MTLTexture> cameraTextureCbCr;
@end

namespace
{
    typedef struct
    {
        vector_float2 position;
        vector_float2 uv;
        vector_float2 cameraUV;
    } XRVertex;

    static XRVertex triangleVertices[] =
    {
        // 2D positions, UV         camera UV
        { { -1, -1 },   { 0, 0 },   { 0, 0} },
        { { -1, 1 },    { 0, 1 },   { 0, 0} },
        { { 1, -1 },    { 1, 0 },   { 0, 0} },
        { { 1, 1 },     { 1, 1 },   { 0, 0} },
    };
}

@implementation SessionDelegate
{
    std::vector<xr::System::Session::Frame::View>* activeFrameViews;
    CVMetalTextureCacheRef textureCache;
    id<MTLTexture> _cameraTextureY;
    id<MTLTexture> _cameraTextureCbCr;
    CVPixelBufferRef frameBuffer;
    
    UIInterfaceOrientation cameraUVReferenceOrientation;
    CGSize cameraUVReferenceSize;
}

- (id)init:(std::vector<xr::System::Session::Frame::View>*)activeFrameViews metalContext:(id<MTLDevice>)graphicsContext
{
    self = [super init];
    self->activeFrameViews = activeFrameViews;
    
    CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, nil, graphicsContext, nil, &textureCache);
    if (err)
    {
        throw std::runtime_error{"Unable to create Texture Cache"};
    }
    
    return self;
}

/**
 Returns the orientation of the app based on the current status bar orientation.
*/
- (UIInterfaceOrientation)orientation
{
    return [[UIApplication sharedApplication] statusBarOrientation];
}

/**
 Returns the viewportSize as determined by the texture size of the first active frame view.
*/
- (CGSize)viewportSize
{
    auto frameSize = activeFrameViews->front().ColorTextureSize;
    return CGSizeMake(frameSize.Width, frameSize.Height);
}

/**
 Called every frame during the active ARKit session.  Updates the AR Camera texture, UVs, and Camera pose.
*/
- (void)session:(ARSession *)session didUpdateFrame:(ARFrame *)frame
{
    // First copy the current ARFrame's image to our frame buffer, accounting for any change in image size.
    [self updateFrameBuffer:frame.capturedImage];
    
    // Next update both metal textures used by the renderer.
    _cameraTextureY = [self updateCameraTexture:frameBuffer plane:0];
    _cameraTextureCbCr = [self updateCameraTexture:frameBuffer plane:1];
     
    // Check if our orientation or size has changed and update camera UVs if necessary.
    if ([self checkAndUpdateCameraUVs:frame])
    {
        // If our camera UVs updated, then also update the FoV to match the updated UVs.
        [self updateFoV:frame.camera];
    }
    
    // Finally update the XR pose based on the current transform from ARKit.
    [self updateDisplayOrientedPose:(frame.camera)];
}

/**
 Copies the camera buffer from from the current ARFrame into our pixel buffer used for composing the metal texture.
*/
- (void)updateFrameBuffer:(CVPixelBufferRef)arCameraBuffer
{
    // Lock the frame buffer with read access to stop ARKit from updating the camera buffer during update.
    CVReturn ret = CVPixelBufferLockBaseAddress(arCameraBuffer, kCVPixelBufferLock_ReadOnly);
    if (ret != kCVReturnSuccess)
    {
        return;
    }
    
    @try
    {
        // Find the width, height, and format of the AR image.
        int bufferWidth = (int)CVPixelBufferGetWidth(arCameraBuffer);
        int bufferHeight = (int)CVPixelBufferGetHeight(arCameraBuffer);
        auto format = CVPixelBufferGetPixelFormatType(arCameraBuffer);
        
        // If we don't have two planes the arCamera has not yet been initialized so back out for now.
        if (CVPixelBufferGetPlaneCount(arCameraBuffer) < 2)
        {
            CVPixelBufferUnlockBaseAddress(arCameraBuffer, kCVPixelBufferLock_ReadOnly);
            return;
        }

        // Check if the size of the frame buffer has changed, if so then dispose of the old one and create a new buffer.
        if (frameBuffer == nil || bufferWidth != (int)CVPixelBufferGetWidth(frameBuffer) || bufferHeight != (int)CVPixelBufferGetHeight(frameBuffer))
        {
            if (frameBuffer != nil)
            {
                CVPixelBufferRelease(frameBuffer);
                frameBuffer = nil;
            }
            
            // We must specify PixelBufferMetalCompatibility on the frame buffer to make this composable into a Metal Texture.
            auto attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], kCVPixelBufferMetalCompatibilityKey, nil];
            
            // Create the actual pixel buffer.
            ret = CVPixelBufferCreate(kCFAllocatorDefault, bufferWidth, bufferHeight, format, (__bridge CFDictionaryRef)attributes, &frameBuffer);
            if (ret != kCVReturnSuccess)
            {
                throw std::runtime_error("Failed to allocate frame buffer.");
            }
        }
        
        // Lock the pixel buffer for write access.
        ret = CVPixelBufferLockBaseAddress(frameBuffer, 0);
        if (ret != kCVReturnSuccess)
        {
            return;
        }
        
        @try
        {
            // Copy both planes of the AR camera image to the frame buffer.
            void* ydestPlane = CVPixelBufferGetBaseAddressOfPlane(frameBuffer, 0);
            void* ysrcPlane = CVPixelBufferGetBaseAddressOfPlane(arCameraBuffer, 0);
            memcpy(ydestPlane, ysrcPlane, bufferWidth * bufferHeight);
            
            auto uvdestPlane = CVPixelBufferGetBaseAddressOfPlane(frameBuffer, 1);
            auto uvsrcPlane = CVPixelBufferGetBaseAddressOfPlane(arCameraBuffer, 1);
            memcpy(uvdestPlane, uvsrcPlane, bufferWidth * bufferHeight / 2);
        }
        @finally
        {
            CVPixelBufferUnlockBaseAddress(arCameraBuffer, 0);
        }
    }
    @finally
    {
        CVPixelBufferUnlockBaseAddress(frameBuffer, kCVPixelBufferLock_ReadOnly);
    }
}

/**
 Updates the captured texture with the current frame buffer.
*/
- (id<MTLTexture>)updateCameraTexture:(CVPixelBufferRef)pixelBuffer plane:(int)planeIndex
{
    CVReturn ret = CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    if (ret != kCVReturnSuccess)
    {
        return {};
    }

    @try
    {
        int planeWidth = (int) CVPixelBufferGetWidthOfPlane(pixelBuffer, planeIndex);
        int planeHeight = (int) CVPixelBufferGetHeightOfPlane(pixelBuffer, planeIndex);
            
        // Plane 0 is the Y plane, which is in R8Unorm format, and the second plane is the CBCR plane which is RG8Unorm format.
        auto pixelFormat = planeIndex ? MTLPixelFormatRG8Unorm : MTLPixelFormatR8Unorm;
        id<MTLTexture> mtlTexture = nil;
        CVMetalTextureRef texture;
        
        // Create a texture from the corresponding plane.
        auto status = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, textureCache, pixelBuffer, nil, pixelFormat, planeWidth, planeHeight, planeIndex, &texture);
        if (status == kCVReturnSuccess)
        {
            mtlTexture = CVMetalTextureGetTexture(texture);
        }
        
        return mtlTexture;
    }
    @finally
    {
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }
}

/**
 Checks whether the camera UVs need to be updated (based on the orientation and size of the view port, and updates them if necessary.
 @return True if the camera UVs were updated, false otherwise.
*/
- (Boolean)checkAndUpdateCameraUVs:(ARFrame *)frame
{
    // When the orientation or viewport size changes Loop over triangleVerts, apply transform to the UV to generate camera UVs.
    auto orientation = [self orientation];
    CGSize viewportSize = [self viewportSize];
    if (cameraUVReferenceOrientation != orientation || cameraUVReferenceSize.height != viewportSize.height || cameraUVReferenceSize.width != viewportSize.width)
    {
        // The default transform is for converting normalized image coordinates to UVs, we want the inverse as we are converting
        // UVs to normalized image coordinates.
        auto transform = CGAffineTransformInvert([frame displayTransformForOrientation:orientation viewportSize:[self viewportSize]]);
        for(size_t i = 0; i < sizeof(triangleVertices) / sizeof(*triangleVertices); i++)
        {
            CGPoint transformedPoint = CGPointApplyAffineTransform({triangleVertices[i].uv[0], triangleVertices[i].uv[1]}, transform);
            
            // The camera image is represented as bottom->top, so we have to flip the vertical component of the source image.
            if (orientation == UIInterfaceOrientationPortrait || orientation == UIInterfaceOrientationPortraitUpsideDown)
            {
                triangleVertices[i].cameraUV[0] = 1 - transformedPoint.x;
                triangleVertices[i].cameraUV[1] = transformedPoint.y;
            }
            else
            {
                triangleVertices[i].cameraUV[0] = transformedPoint.x;
                triangleVertices[i].cameraUV[1] = 1 - transformedPoint.y;
            }
        }
        
        cameraUVReferenceOrientation = orientation;
        cameraUVReferenceSize = viewportSize;
        return true;
    }
    
    return false;
}

/**
 Updates the tracked FoV of the AR Camera.
*/
- (void)updateFoV:(ARCamera*)camera
{
    // First grab the projection matrix for the image, this uses an aspect fill algorithm.
    auto& frameView = activeFrameViews->at(0);
    auto viewportSize = [self viewportSize];
    auto orientation = [self orientation];
    auto projection = [camera projectionMatrixForOrientation:[self orientation] viewportSize:viewportSize zNear:frameView.DepthNearZ zFar:frameView.DepthFarZ];
    float yScale = projection.columns[1][1];
    float aspectRatio = viewportSize.width/viewportSize.height;
    
    // In order to adjust the vertical FOV correctly we need to find the amount cropped off the image
    // to find the amount of FOV ignored from the display's vertical FOV.
    float min = 1.0f;
    float max = 0.0f;
    
    // Since the image from the camera is always presented in landscape mode, the camera UV index of the
    // display oriented vertical dimension is 1 for landscape orientation, and 0 for portrait mode.
    int uvIndex = 0;
    if (orientation == UIInterfaceOrientationLandscapeLeft || orientation == UIInterfaceOrientationLandscapeRight)
    {
        uvIndex = 1;
    }
    
    // Loop over all the UVs and find the min and max crop values.
    for(size_t i = 0; i < sizeof(triangleVertices) / sizeof(*triangleVertices); i++)
    {
        
        if (triangleVertices[i].cameraUV[uvIndex] > max)
        {
            max = triangleVertices[i].cameraUV[uvIndex];
        }
        else if (triangleVertices[i].cameraUV[uvIndex] < min)
        {
            min = triangleVertices[i].cameraUV[uvIndex];
        }
    }

    // Calculate FoV and apply it to the frame view.
    float fov =  atanf((max - min) / yScale);
    frameView.FieldOfView.AngleDown = -(frameView.FieldOfView.AngleUp = fov);
    frameView.FieldOfView.AngleLeft = -(frameView.FieldOfView.AngleRight = fov * aspectRatio);
}

/**
 The ARKit camera transform is always a local right hand coordinate space WRT landscape right orientation, so this function takes the transform and converts
 it into a display oriented pose see: (https://developer.apple.com/documentation/arkit/arcamera/2866108-transform)
*/
-(void)updateDisplayOrientedPose:(ARCamera*)camera
{
    auto& frameView = activeFrameViews->at(0);
    UIInterfaceOrientation orientation = [self orientation];
    simd_float4x4 transform = [camera transform];
    simd_quatf displayOrientationQuat;
    
    // Create the display orientation quaternion based on the current orientation of the device.
    if (orientation == UIInterfaceOrientationLandscapeRight)
    {
        displayOrientationQuat = simd_quaternion((float)M_PI, simd_make_float3(0, 0, 1));
    }
    else if (orientation == UIInterfaceOrientationLandscapeLeft)
    {
        displayOrientationQuat = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (orientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        displayOrientationQuat = simd_quaternion((float)M_PI * .5f, simd_make_float3(0, 0, 1));
    }
    else if (orientation == UIInterfaceOrientationPortrait)
    {
        displayOrientationQuat = simd_quaternion((float)M_PI * 1.5f, simd_make_float3(0, 0, 1));
    }
    
    // Convert the display orientation quaternion to a transform matrix.
    simd_float4x4 rotationMatrix = simd_matrix4x4(displayOrientationQuat);
    
    // Multiply the transform by the rotation matrix to generate the display oriented transform.
    auto displayOrientedTransform = simd_mul(transform, rotationMatrix);
    
    //Pull out the display oriented rotation.
    auto displayOrientation = simd_quaternion(displayOrientedTransform);
    
    // Set the orientation of the camera, the Y and Z values of the quaternion need to be inverted.
    frameView.Space.Pose.Orientation = { displayOrientation.vector.x
        , -1 * displayOrientation.vector.y
        , -1 * displayOrientation.vector.z
        , displayOrientation.vector.w};
    
    // Set the translation, the X value must be inverted as the default axis points left instead of right.
    frameView.Space.Pose.Position = { -1 * displayOrientedTransform.columns[3][0]
        , displayOrientedTransform.columns[3][1]
        , displayOrientedTransform.columns[3][2] };
}

@end
namespace xr
{
    namespace
    {
        const char* shaderSource = R"(
            #include <metal_stdlib>
            #include <simd/simd.h>

            using namespace metal;

            #include <simd/simd.h>

            typedef struct
            {
                vector_float2 position;
                vector_float2 uv;
                vector_float2 cameraUV;
            } XRVertex;

            typedef struct
            {
                float4 position [[position]];
                float2 uv;
                float2 cameraUV;
            } RasterizerData;

            vertex RasterizerData
            vertexShader(uint vertexID [[vertex_id]],
                         constant XRVertex *vertices [[buffer(0)]])
            {
                RasterizerData out;
                out.position = vector_float4(vertices[vertexID].position.xy, 0.0, 1.0);
                out.uv = vertices[vertexID].uv;
                out.cameraUV = vertices[vertexID].cameraUV;
                return out;
            }

            fragment float4 fragmentShader(RasterizerData in [[stage_in]],
                texture2d<float, access::sample> babylonTexture [[ texture(0) ]],
                texture2d<float, access::sample> cameraTextureY [[ texture(1) ]],
                texture2d<float, access::sample> cameraTextureCbCr [[ texture(2) ]])
            {
                constexpr sampler linearSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);

                const float4 babylonSample = babylonTexture.sample(linearSampler, in.uv);
                if (is_null_texture(cameraTextureY))
                {
                    return babylonSample;
                }
    
                const float4 cameraSampleY = cameraTextureY.sample(linearSampler, in.cameraUV);
                const float4 cameraSampleCbCr = cameraTextureCbCr.sample(linearSampler, in.cameraUV);

                const float4x4 ycbcrToRGBTransform = float4x4(
                    float4(+1.0000f, +1.0000f, +1.0000f, +0.0000f),
                    float4(+0.0000f, -0.3441f, +1.7720f, +0.0000f),
                    float4(+1.4020f, -0.7141f, +0.0000f, +0.0000f),
                    float4(-0.7010f, +0.5291f, -0.8860f, +1.0000f)
                );

                float4 ycbcr = float4(cameraSampleY.r, cameraSampleCbCr.rg, 1.0);
                float4 cameraSample = ycbcrToRGBTransform * ycbcr;
                cameraSample.a = 1.0;

                const float4 mixed = mix(cameraSample, babylonSample, babylonSample.a);

                return mixed;
            }
        )";

        id<MTLLibrary> CompileShader(id<MTLDevice> metalDevice, const char* source)
        {
            NSError* error;
            id<MTLLibrary> lib = [metalDevice newLibraryWithSource:@(source) options:nil error:&error];
            if(nil != error)
            {
                throw std::runtime_error{[error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]};
            }
            return lib;
        }
    }
    
    struct System::Impl
    {
    public:
        Impl(const std::string&)
        {
        }

        bool IsInitialized() const
        {
            return true;
        }

        bool TryInitialize()
        {
            return true;
        }
    };

    struct System::Session::Impl
    {
    public:
        const System::Impl& SystemImpl;
        std::vector<Frame::View> ActiveFrameViews{ {} };
        std::vector<Frame::InputSource> InputSources;
        float DepthNearZ{ DEFAULT_DEPTH_NEAR_Z };
        float DepthFarZ{ DEFAULT_DEPTH_FAR_Z };

        Impl(System::Impl& systemImpl, void* graphicsContext, void* window)
            : SystemImpl{ systemImpl }
        {
            MetalDevice = id<MTLDevice>(graphicsContext);
            UIView* MainView = (UIView*)window;
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                auto guide = MainView.safeAreaLayoutGuide;
                xrView = [[MTKView alloc] initWithFrame:guide.layoutFrame device:MetalDevice];
                [MainView addSubview:xrView];
                xrView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
                xrView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
                MetalLayer = (CAMetalLayer *)xrView.layer;
                MetalLayer.device = MetalDevice;
                auto scale = UIScreen.mainScreen.scale;
                viewportSize.x = guide.layoutFrame.size.width * scale;
                viewportSize.y = guide.layoutFrame.size.height * scale;
            });

            configuration = [ARWorldTrackingConfiguration new];
            session = [ARSession new];
            MetalDevice = id<MTLDevice>(graphicsContext);
            sessionDelegate = [[SessionDelegate new]init:&ActiveFrameViews metalContext:MetalDevice];
            session.delegate = sessionDelegate;
            configuration.planeDetection = ARPlaneDetectionHorizontal;
            configuration.lightEstimationEnabled = false;
            [session runWithConfiguration:configuration];

            // build pipeline
            NSError* error;
            id<MTLLibrary> lib = CompileShader(MetalDevice, shaderSource);
            id<MTLFunction> vertexFunction = [lib newFunctionWithName:@"vertexShader"];
            id<MTLFunction> fragmentFunction = [lib newFunctionWithName:@"fragmentShader"];

            // Configure a pipeline descriptor that is used to create a pipeline state.
            MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"XR Pipeline";
            pipelineStateDescriptor.vertexFunction = vertexFunction;
            pipelineStateDescriptor.fragmentFunction = fragmentFunction;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = MetalLayer.pixelFormat;

            pipelineState = [MetalDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            if (!pipelineState)
            {
                NSLog(@"Failed to create pipeline state: %@", error);
            }
            commandQueue = [MetalDevice newCommandQueue];
            // default fov values to avoid NaN at startup
            auto& frameView = ActiveFrameViews[0];
            frameView.FieldOfView.AngleDown = -(frameView.FieldOfView.AngleUp = 0.5);
            frameView.FieldOfView.AngleLeft = -(frameView.FieldOfView.AngleRight = 0.5);
        }

        ~Impl()
        {
            [sessionDelegate release];
            [configuration release];
            [session release];
            dispatch_sync(dispatch_get_main_queue(), ^{
                [xrView removeFromSuperview]; });
        }

        std::unique_ptr<System::Session::Frame> GetNextFrame(bool&, bool&)
        {
            unsigned int width = viewportSize.x;
            unsigned int height = viewportSize.y;
            
            if (ActiveFrameViews[0].ColorTextureSize.Width != width || ActiveFrameViews[0].ColorTextureSize.Height != height)
            {
                // Color texture
                {
                    MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];
                    textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
                    textureDescriptor.width = width;
                    textureDescriptor.height = height;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
                    id<MTLTexture> texture = [MetalDevice newTextureWithDescriptor:textureDescriptor];
                    
                    ActiveFrameViews[0].ColorTexturePointer = reinterpret_cast<void *>(texture);
                    ActiveFrameViews[0].ColorTextureFormat = TextureFormat::RGBA8_SRGB;
                    ActiveFrameViews[0].ColorTextureSize = {width, height};
                }

                // Allocate and store the depth texture
                {
                    MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];
                    textureDescriptor.pixelFormat = MTLPixelFormatDepth32Float_Stencil8;
                    textureDescriptor.width = width;
                    textureDescriptor.height = height;
                    textureDescriptor.storageMode = MTLStorageModePrivate;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    id<MTLTexture> texture = [MetalDevice newTextureWithDescriptor:textureDescriptor];
                    
                    ActiveFrameViews[0].DepthTexturePointer = reinterpret_cast<void*>(texture);
                    ActiveFrameViews[0].DepthTextureFormat = TextureFormat::D24S8;
                    ActiveFrameViews[0].DepthTextureSize = {width, height};
                }
            }
            return std::make_unique<Frame>(*this);
        }

        void RequestEndSession()
        {
            // Note the end session has been requested, and respond to the request in the next call to GetNextFrame
            SessionEnded = true;
        }

        Size GetWidthAndHeightForViewIndex(size_t) const
        {
            // Return a valid (non-zero) size, but otherwise it doesn't matter as the render texture created from this isn't currently used
            return {1,1};
        }
        
        void DrawFrame()
        {
            // Create a new command buffer for each render pass to the current drawable.
            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            commandBuffer.label = @"XRDisplayCommandBuffer";
            
            id<CAMetalDrawable> drawable = [MetalLayer nextDrawable];
            MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

            if(renderPassDescriptor != nil)
            {
                renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0,0.0,0.0,1.0);
                
                // Create a render command encoder.
                id<MTLRenderCommandEncoder> renderEncoder =
                [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
                renderEncoder.label = @"XRDisplayEncoder";

                // Set the region of the drawable to draw into.
                [renderEncoder setViewport:(MTLViewport){0.0, 0.0, static_cast<double>(viewportSize.x), static_cast<double>(viewportSize.y), 0.0, 1.0 }];
                
                [renderEncoder setRenderPipelineState:pipelineState];

                // Pass in the parameter data.
                [renderEncoder setVertexBytes:triangleVertices length:sizeof(triangleVertices) atIndex:0];

                [renderEncoder setFragmentTexture:id<MTLTexture>(ActiveFrameViews[0].ColorTexturePointer) atIndex:0];
                [renderEncoder setFragmentTexture:sessionDelegate.cameraTextureY atIndex:1];
                [renderEncoder setFragmentTexture:sessionDelegate.cameraTextureCbCr atIndex:2];

                // Draw the triangle.
                [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

                [renderEncoder endEncoding];

                // Schedule a present once the framebuffer is complete using the current drawable.
                [commandBuffer presentDrawable:drawable];
            }

            // Finalize rendering here & push the command buffer to the GPU.
            [commandBuffer commit];
        }

        void GetHitTestResults(std::vector<HitResult>&, xr::Ray) const
        {
            // TODO
        }
        
        private:
            MTKView* xrView{};
            bool SessionEnded{ false };
            id<MTLDevice> MetalDevice{};
            CAMetalLayer* MetalLayer{};
            ARSession* session{};
            SessionDelegate* sessionDelegate{};
            ARWorldTrackingConfiguration* configuration{};
            id<MTLRenderPipelineState> pipelineState{};
            vector_uint2 viewportSize{};
            id<MTLCommandQueue> commandQueue;
    };

    struct System::Session::Frame::Impl
    {
    public:
        Impl(Session::Impl& sessionImpl)
            : sessionImpl{sessionImpl}
        {
        }

        Session::Impl& sessionImpl;
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.ActiveFrameViews }
        , InputSources{ sessionImpl.InputSources}
        , m_impl{ std::make_unique<System::Session::Frame::Impl>(sessionImpl) }
    {
        Views[0].DepthNearZ = sessionImpl.DepthNearZ;
        Views[0].DepthFarZ = sessionImpl.DepthFarZ;
    }

    System::Session::Frame::~Frame()
    {
        m_impl->sessionImpl.DrawFrame();
    }

    void System::Session::Frame::GetHitTestResults(std::vector<HitResult>& filteredResults, xr::Ray offsetRay) const
    {
        m_impl->sessionImpl.GetHitTestResults(filteredResults, offsetRay);
    }

    Anchor System::Session::Frame::CreateAnchor(Pose, NativeTrackablePtr) const
    {
        throw std::runtime_error("Not yet implemented");
    }

    void System::Session::Frame::UpdateAnchor(xr::Anchor&) const
    {
        throw std::runtime_error("Not yet implemented");
    }

    void System::Session::Frame::DeleteAnchor(xr::Anchor&) const
    {
        throw std::runtime_error("Not yet implemented");
    }

    System::System(const char* appName)
        : m_impl{ std::make_unique<System::Impl>(appName) }
    {}

    System::~System() {}

    bool System::IsInitialized() const
    {
        return m_impl->IsInitialized();
    }

    bool System::TryInitialize()
    {
        return m_impl->TryInitialize();
    }

    arcana::task<bool, std::exception_ptr> System::IsSessionSupportedAsync(SessionType sessionType)
    {
        // Only immersive_VR is supported for now.
        return arcana::task_from_result<std::exception_ptr>(sessionType == SessionType::IMMERSIVE_AR);
    }

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(System& system, void* graphicsDevice, void* window)
    {
        return arcana::task_from_result<std::exception_ptr>(std::make_shared<System::Session>(system, graphicsDevice, window));
    }

    System::Session::Session(System& system, void* graphicsDevice, void* window)
        : m_impl{ std::make_unique<System::Session::Impl>(*system.m_impl, graphicsDevice, window) }
    {}

    System::Session::~Session()
    {
        // Free textures
    }

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)>)
    {
        return m_impl->GetNextFrame(shouldEndSession, shouldRestartSession);
    }

    void System::Session::RequestEndSession()
    {
        m_impl->RequestEndSession();
    }

    Size System::Session::GetWidthAndHeightForViewIndex(size_t viewIndex) const
    {
        return m_impl->GetWidthAndHeightForViewIndex(viewIndex);
    }

    void System::Session::SetDepthsNearFar(float depthNear, float depthFar)
    {
        m_impl->DepthNearZ = depthNear;
        m_impl->DepthFarZ = depthFar;
    }
}
