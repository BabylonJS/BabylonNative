#import <XR.h>

#import <UIKit/UIKit.h>
#import <ARKit/ARKit.h>
#import <ARKit/ARConfiguration.h>
#import <MetalKit/MetalKit.h>

@interface SessionDelegate : NSObject <ARSessionDelegate>
@property (readonly) id<MTLTexture> cameraTextureY;
@property (readonly) id<MTLTexture> cameraTextureCbCr;
@end

namespace {
    typedef struct {
        vector_float2 position;
        vector_float2 uv;
        vector_float2 cameraUV;
    } XRVertex;

    static XRVertex vertices[] = {
        // 2D positions, UV,        camera UV
        { { -1, -1 },   { 1, 0 },   { 0, 0} },
        { { -1, 1 },    { 1, 1 },   { 0, 0} },
        { { 1, -1 },    { 0, 0 },   { 0, 0} },
        { { 1, 1 },     { 0, 1 },   { 0, 0} },
    };
}

/**
 Implementation of the ARSessionDelegate interface for more info see: https://developer.apple.com/documentation/arkit/arsessiondelegate
 */
@implementation SessionDelegate {
    std::vector<xr::System::Session::Frame::View>* activeFrameViews;
    CVMetalTextureCacheRef textureCache;
    id<MTLTexture> _cameraTextureY;
    id<MTLTexture> _cameraTextureCbCr;
    
    UIInterfaceOrientation cameraUVReferenceOrientation;
    CGSize cameraUVReferenceSize;
}

/**
 Initializes this session delgate with the given frame views and metal graphics context.
 */
- (id)init:(std::vector<xr::System::Session::Frame::View>*)activeFrameViews metalContext:(id<MTLDevice>)graphicsContext {
    self = [super init];
    self->activeFrameViews = activeFrameViews;
    
    CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, nil, graphicsContext, nil, &textureCache);
    if (err) {
        throw std::runtime_error{"Unable to create Texture Cache"};
    }
    
    return self;
}

/**
 Returns the orientation of the app based on the current status bar orientation.
*/
- (UIInterfaceOrientation)orientation {
    auto sharedApplication = [UIApplication sharedApplication];
    auto window = sharedApplication.windows.firstObject;
    if (@available(iOS 13.0, *)) {
        return window.windowScene.interfaceOrientation;
    }
    else {
        return [[UIApplication sharedApplication] statusBarOrientation];
    }	
}

/**
 Returns the viewportSize as determined by the texture size of the first active frame view.
*/
- (CGSize)viewportSize {
    auto frameSize = activeFrameViews->front().ColorTextureSize;
    return CGSizeMake(frameSize.Width, frameSize.Height);
}

/**
 Implementation of the ARSessionDelegate protocol. Called every frame during the active ARKit session.  Updates the AR Camera texture, and Camera pose.
 If a size change is detected also sets the UVs, and FoV values.
*/
- (void)session:(ARSession *)__unused session didUpdateFrame:(ARFrame *)frame {
    @autoreleasepool{
        [self cleanupTextures];
        
        // Next update both metal textures used by the renderer to display the camera image.
        _cameraTextureY = [self updateCameraTexture:frame.capturedImage plane:0];
        _cameraTextureCbCr = [self updateCameraTexture:frame.capturedImage plane:1];
         
        // Check if our orientation or size has changed and update camera UVs if necessary.
        if ([self checkAndUpdateCameraUVs:frame]) {
            // If our camera UVs updated, then also update the FoV to match the updated UVs.
            [self updateFoV:frame.camera];
        }
        
        // Finally update the XR pose based on the current transform from ARKit.
        [self updateDisplayOrientedPose:(frame.camera)];
    }
}

/**
 Updates the captured texture with the current frame buffer.
*/
- (id<MTLTexture>)updateCameraTexture:(CVPixelBufferRef)pixelBuffer plane:(int)planeIndex {
    CVReturn ret = CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    if (ret != kCVReturnSuccess) {
        return {};
    }

    @try {
        size_t planeWidth = CVPixelBufferGetWidthOfPlane(pixelBuffer, planeIndex);
        size_t planeHeight = CVPixelBufferGetHeightOfPlane(pixelBuffer, planeIndex);
            
        // Plane 0 is the Y plane, which is in R8Unorm format, and the second plane is the CBCR plane which is RG8Unorm format.
        auto pixelFormat = planeIndex ? MTLPixelFormatRG8Unorm : MTLPixelFormatR8Unorm;
        id<MTLTexture> mtlTexture = nil;
        CVMetalTextureRef texture;
        
        // Create a texture from the corresponding plane.
        auto status = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, textureCache, pixelBuffer, nil, pixelFormat, planeWidth, planeHeight, planeIndex, &texture);
        if (status == kCVReturnSuccess) {
            mtlTexture = CVMetalTextureGetTexture(texture);
            CVBufferRelease(texture);
        }
        
        return mtlTexture;
    }
    @finally {
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }
}

/**
 Checks whether the camera UVs need to be updated based on the orientation and size of the view port, and updates them if necessary.
 @return True if the camera UVs were updated, false otherwise.
*/
- (Boolean)checkAndUpdateCameraUVs:(ARFrame *)frame {
    // When the orientation or viewport size changes loop over triangleVerts, apply transform to the UV to generate camera UVs.
    auto orientation = [self orientation];
    CGSize viewportSize = [self viewportSize];
    if (cameraUVReferenceOrientation != orientation || cameraUVReferenceSize.height != viewportSize.height || cameraUVReferenceSize.width != viewportSize.width) {
        // The default transform is for converting normalized image coordinates to UVs, we want the inverse as we are converting
        // UVs to normalized image coordinates.
        auto transform = CGAffineTransformInvert([frame displayTransformForOrientation:orientation viewportSize:[self viewportSize]]);
        for(size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
            CGPoint transformedPoint = CGPointApplyAffineTransform({vertices[i].uv[0], vertices[i].uv[1]}, transform);
            
            // In the inverse transform the camera image is represented bottom->top right->left, so we have to flip the vertical component of the source image.
            if (orientation == UIInterfaceOrientationPortrait || orientation == UIInterfaceOrientationPortraitUpsideDown) {
                vertices[i].cameraUV[0] = 1 - transformedPoint.x;
                vertices[i].cameraUV[1] = 1 - transformedPoint.y;
            }
            else {
                vertices[i].cameraUV[0] = 1 - transformedPoint.x;
                vertices[i].cameraUV[1] = 1 - transformedPoint.y;
            }
        }
        
        // Keep track of the last known orientation and viewport size.
        cameraUVReferenceOrientation = orientation;
        cameraUVReferenceSize = viewportSize;
        return true;
    }
    
    return false;
}

/**
 Finds the FoV of the AR Camera, and applies it to the frameView.
*/
- (void)updateFoV:(ARCamera*)camera {
    // Get the viewport size and the orientation of the device.
    auto& frameView = activeFrameViews->at(0);
    auto viewportSize = [self viewportSize];
    auto orientation = [self orientation];
    
    // Grab the projection matrix for the image based on the viewport.
    auto projection = [camera projectionMatrixForOrientation:orientation viewportSize:viewportSize zNear:frameView.DepthNearZ zFar:frameView.DepthFarZ];
    
    // Pull out the xScale, and yScale values from the projection matrix.
    float xScale = projection.columns[0][0];
    float yScale = projection.columns[1][1];
    
    // Calculate the aspect ratio of the projection.
    float aspectRatio = yScale / xScale;

    // Calculate FoV and apply it to the frame view.
    // TODO: Validate if this actually gives the right FoV, it seems to be stretched vertically in Landscape
    // mode likely due to the image being cropped by the transformation in checkAndUpdateCameraUVs vs being
    // aspect fitted by projectionMatrixForOrientation.
    float fov =  atanf(1 / yScale);
    frameView.FieldOfView.AngleDown = -(frameView.FieldOfView.AngleUp = fov);
    frameView.FieldOfView.AngleLeft = -(frameView.FieldOfView.AngleRight = fov * aspectRatio);
}

/**
 The ARKit camera transform is always a local right hand coordinate space WRT landscape right orientation, so this function takes the transform and converts
 it into a display oriented pose see: (https://developer.apple.com/documentation/arkit/arcamera/2866108-transform)
 */
-(void)updateDisplayOrientedPose:(ARCamera*)camera {
    auto& frameView = activeFrameViews->at(0);
    UIInterfaceOrientation orientation = [self orientation];
    simd_float4x4 transform = [camera transform];
    simd_quatf displayOrientationQuat;
    
    // Create the display orientation quaternion based on the current orientation of the device.
    if (orientation == UIInterfaceOrientationLandscapeRight) {
        displayOrientationQuat = simd_quaternion((float)M_PI, simd_make_float3(0, 0, 1));
    }
    else if (orientation == UIInterfaceOrientationLandscapeLeft) {
        displayOrientationQuat = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (orientation == UIInterfaceOrientationPortraitUpsideDown) {
        displayOrientationQuat = simd_quaternion((float)M_PI * .5f, simd_make_float3(0, 0, 1));
    }
    else if (orientation == UIInterfaceOrientationPortrait) {
        displayOrientationQuat = simd_quaternion((float)M_PI * -.5f, simd_make_float3(0, 0, 1));
    }
    
    // Convert the display orientation quaternion to a transform matrix.
    simd_float4x4 rotationMatrix = simd_matrix4x4(displayOrientationQuat);
    
    // Multiply the transform by the rotation matrix to generate the display oriented transform.
    auto displayOrientedTransform = simd_mul(transform, rotationMatrix);
    
    //Pull out the display oriented rotation.
    auto displayOrientation = simd_quaternion(displayOrientedTransform);
    
    // Set the orientation of the camera
    frameView.Space.Pose.Orientation = { displayOrientation.vector.x
        , displayOrientation.vector.y
        , displayOrientation.vector.z
        , displayOrientation.vector.w};
    
    // Set the translation.
    frameView.Space.Pose.Position = { displayOrientedTransform.columns[3][0]
        , displayOrientedTransform.columns[3][1]
        , displayOrientedTransform.columns[3][2] };
}

-(void)cleanupTextures {
    if (_cameraTextureY != nil) {
        [_cameraTextureY setPurgeableState:(MTLPurgeableStateEmpty)];
        _cameraTextureY = nil;
        [_cameraTextureCbCr setPurgeableState:(MTLPurgeableStateEmpty)];
        _cameraTextureCbCr = nil;
    }
}

-(void)dealloc {
  [self cleanupTextures];

  if (textureCache != nil) {
      CVMetalTextureCacheFlush(textureCache, 0);
      CFRelease(textureCache);
      textureCache = nil;
  }
  
  [super dealloc];
}

@end
namespace xr {
    namespace {
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

        id<MTLLibrary> CompileShader(id<MTLDevice> metalDevice, const char* source) {
            NSError* error;
            id<MTLLibrary> lib = [metalDevice newLibraryWithSource:@(source) options:nil error:&error];
            if(nil != error) {
                throw std::runtime_error{[error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]};
            }
            return lib;
        }
    }
    
    struct System::Impl {
    public:
        Impl(const std::string&) {}

        bool IsInitialized() const {
            return true;
        }

        bool TryInitialize() {
            return true;
        }
    };

    struct System::Session::Impl {
    public:
        const System::Impl& SystemImpl;
        std::vector<Frame::View> ActiveFrameViews{ {} };
        std::vector<Frame::InputSource> InputSources;
        float DepthNearZ{ DEFAULT_DEPTH_NEAR_Z };
        float DepthFarZ{ DEFAULT_DEPTH_FAR_Z };

        Impl(System::Impl& systemImpl, void* graphicsContext, void* window)
            : SystemImpl{ systemImpl } {
            metalDevice = id<MTLDevice>(graphicsContext);
            UIView* MainView = (UIView*)window;
            
            // Create the XR View to stay within the safe area of the main view.
            dispatch_sync(dispatch_get_main_queue(), ^{
                xrView = [[MTKView alloc] initWithFrame:MainView.bounds device:metalDevice];
                [MainView addSubview:xrView];
                xrView.userInteractionEnabled = false;
                xrView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
                xrView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
// NOTE: There is an incorrect warning about CAMetalLayer specifically when compiling for the simulator.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpartial-availability"
                metalLayer = (CAMetalLayer *)xrView.layer;
#pragma clang diagnostic pop
                metalLayer.device = metalDevice;
                auto scale = UIScreen.mainScreen.scale;
                viewportSize.x = MainView.bounds.size.width * scale;
                viewportSize.y = MainView.bounds.size.height * scale;
            });

            // If the singleton session and configuraiton have not yet been created, create them here now.
            if (session == nil) {
                session = [ARSession new];
                configuration = [ARWorldTrackingConfiguration new];
                configuration.planeDetection = ARPlaneDetectionHorizontal | ARPlaneDetectionVertical;
                configuration.lightEstimationEnabled = false;
                configuration.worldAlignment = ARWorldAlignmentGravity;
            }
            
            metalDevice = id<MTLDevice>(graphicsContext);
            sessionDelegate = [[SessionDelegate new]init:&ActiveFrameViews metalContext:metalDevice];
            session.delegate = sessionDelegate;
            [session runWithConfiguration:configuration];

            id<MTLLibrary> lib = CompileShader(metalDevice, shaderSource);
            id<MTLFunction> vertexFunction = [lib newFunctionWithName:@"vertexShader"];
            id<MTLFunction> fragmentFunction = [lib newFunctionWithName:@"fragmentShader"];

            // Configure a pipeline descriptor that is used to create a pipeline state.
            MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"XR Pipeline";
            pipelineStateDescriptor.vertexFunction = vertexFunction;
            pipelineStateDescriptor.fragmentFunction = fragmentFunction;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = metalLayer.pixelFormat;

            // build pipeline
            NSError* error;
            pipelineState = [metalDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            if (!pipelineState) {
                NSLog(@"Failed to create pipeline state: %@", error);
            }
            
            [pipelineStateDescriptor release];
            commandQueue = [metalDevice newCommandQueue];
            
            // default fov values to avoid NaN at startup
            auto& frameView = ActiveFrameViews[0];
            frameView.FieldOfView.AngleDown = -(frameView.FieldOfView.AngleUp = 0.5);
            frameView.FieldOfView.AngleLeft = -(frameView.FieldOfView.AngleRight = 0.5);
        }

        ~Impl() {
            if (ActiveFrameViews[0].ColorTexturePointer != nil) {
                id<MTLTexture> oldColorTexture = reinterpret_cast<id<MTLTexture>>(ActiveFrameViews[0].ColorTexturePointer);
                [oldColorTexture setPurgeableState:MTLPurgeableStateEmpty];
                [oldColorTexture release];
                ActiveFrameViews[0].ColorTexturePointer = nil;
            }
            
            if (ActiveFrameViews[0].DepthTexturePointer != nil) {
                id<MTLTexture> oldDepthTexture = reinterpret_cast<id<MTLTexture>>(ActiveFrameViews[0].DepthTexturePointer);
                [oldDepthTexture setPurgeableState:MTLPurgeableStateEmpty];
                [oldDepthTexture release];
                ActiveFrameViews[0].DepthTexturePointer = nil;
            }

            [session pause];
            session.delegate = nil;
            [sessionDelegate release];
            [pipelineState release];
            dispatch_sync(dispatch_get_main_queue(), ^{
                [xrView removeFromSuperview]; });
            [xrView releaseDrawables];
            [xrView dealloc];
            xrView = nil;
        }

        std::unique_ptr<System::Session::Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession) {
            uint32_t width = viewportSize.x;
            uint32_t height = viewportSize.y;
            shouldEndSession = sessionEnded;
            shouldRestartSession = false;
            
            if (ActiveFrameViews[0].ColorTextureSize.Width != width || ActiveFrameViews[0].ColorTextureSize.Height != height) {
                // Color texture
                {
                    if (ActiveFrameViews[0].ColorTexturePointer != nil) {
                        id<MTLTexture> oldColorTexture = reinterpret_cast<id<MTLTexture>>(ActiveFrameViews[0].ColorTexturePointer);
                        [oldColorTexture setPurgeableState:MTLPurgeableStateEmpty];
                        [oldColorTexture release];
                        ActiveFrameViews[0].ColorTexturePointer = nil;
                    }
                    
                    MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];
                    textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
                    textureDescriptor.width = width;
                    textureDescriptor.height = height;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
                    id<MTLTexture> texture = [metalDevice newTextureWithDescriptor:textureDescriptor];
                    [textureDescriptor dealloc];
                                        
                    ActiveFrameViews[0].ColorTexturePointer = reinterpret_cast<void *>(texture);
                    ActiveFrameViews[0].ColorTextureFormat = TextureFormat::RGBA8_SRGB;
                    ActiveFrameViews[0].ColorTextureSize = {width, height};
                }

                // Allocate and store the depth texture
                {
                    if (ActiveFrameViews[0].DepthTexturePointer != nil) {
                        id<MTLTexture> oldDepthTexture = reinterpret_cast<id<MTLTexture>>(ActiveFrameViews[0].DepthTexturePointer);
                        [oldDepthTexture setPurgeableState:MTLPurgeableStateEmpty];
                        [oldDepthTexture release];
                        ActiveFrameViews[0].DepthTexturePointer = nil;
                    }

                    MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];
                    textureDescriptor.pixelFormat = MTLPixelFormatDepth32Float_Stencil8;
                    textureDescriptor.width = width;
                    textureDescriptor.height = height;
                    textureDescriptor.storageMode = MTLStorageModePrivate;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    id<MTLTexture> texture = [metalDevice newTextureWithDescriptor:textureDescriptor];
                    [textureDescriptor dealloc];
                    
                    ActiveFrameViews[0].DepthTexturePointer = reinterpret_cast<void*>(texture);
                    ActiveFrameViews[0].DepthTextureFormat = TextureFormat::D24S8;
                    ActiveFrameViews[0].DepthTextureSize = {width, height};
                }
            }
            else {
                @autoreleasepool {
                    // Clear the color and depth texture before handing it off to Babylon
                    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
                    commandBuffer.label = @"BabylonTextureClearBuffer";
                    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
                    if(renderPassDescriptor != nil) {
                        // Set up the clear for the color texture.
                        renderPassDescriptor.colorAttachments[0].texture = reinterpret_cast<id<MTLTexture>>(ActiveFrameViews[0].ColorTexturePointer);
                        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0,0.0,0.0,0.0);
                        
                        // Set up the clear for the depth texture.
                        renderPassDescriptor.depthAttachment.texture = reinterpret_cast<id<MTLTexture>>(ActiveFrameViews[0].DepthTexturePointer);
                        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
                        renderPassDescriptor.depthAttachment.clearDepth = 1.0f;
                        
                        // Create and end the render encoder.
                        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
                        renderEncoder.label = @"BabylonTextureClearEncoder";
                        [renderEncoder endEncoding];
                    }
                    
                    // Finalize rendering here & push the command buffer to the GPU.
                    [commandBuffer commit];
                    [commandBuffer waitUntilCompleted];
                }
            }
            
            return std::make_unique<Frame>(*this);
        }

        void RequestEndSession() {
            // Note the end session has been requested, and respond to the request in the next call to GetNextFrame
            sessionEnded = true;
        }

        Size GetWidthAndHeightForViewIndex(size_t) const {
            // Return a valid (non-zero) size, but otherwise it doesn't matter as the render texture created from this isn't currently used
            return {1,1};
        }
        
        void DrawFrame() {
            @autoreleasepool {
                // Create a new command buffer for each render pass to the current drawable.
                id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
                commandBuffer.label = @"XRDisplayCommandBuffer";
                
                id<CAMetalDrawable> drawable = [metalLayer nextDrawable];
                MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

                if(renderPassDescriptor != nil) {
                    renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0,0.0,0.0,1.0);
                    
                    // Create a render command encoder.
                    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
                    renderEncoder.label = @"XRDisplayEncoder";

                    // Set the region of the drawable to draw into.
                    [renderEncoder setViewport:(MTLViewport){0.0, 0.0, static_cast<double>(viewportSize.x), static_cast<double>(viewportSize.y), 0.0, 1.0 }];
                    
                    [renderEncoder setRenderPipelineState:pipelineState];

                    // Pass in the parameter data.
                    [renderEncoder setVertexBytes:vertices length:sizeof(vertices) atIndex:0];

                    [renderEncoder setFragmentTexture:id<MTLTexture>(ActiveFrameViews[0].ColorTexturePointer) atIndex:0];
                    [renderEncoder setFragmentTexture:sessionDelegate.cameraTextureY atIndex:1];
                    [renderEncoder setFragmentTexture:sessionDelegate.cameraTextureCbCr atIndex:2];

                    // Draw the triangles.
                    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

                    [renderEncoder endEncoding];

                    // Schedule a present once the framebuffer is complete using the current drawable.
                    [commandBuffer presentDrawable:drawable];
                }

                // Finalize rendering here & push the command buffer to the GPU.
                [commandBuffer commit];
                [commandBuffer waitUntilCompleted];
            }
        }

        void GetHitTestResults(std::vector<HitResult>& filteredResults, xr::Ray offsetRay) const {
            @autoreleasepool {
                if (session != nil && session.currentFrame != nil && session.currentFrame.camera != nil && [session.currentFrame.camera trackingState] == ARTrackingStateNormal) {
                    if (@available(iOS 13.0, *)) {
                        // Push the camera origin into a simd_float3.
                        auto cameraOrigin = simd_make_float3(
                                                             ActiveFrameViews[0].Space.Pose.Position.X,
                                                             ActiveFrameViews[0].Space.Pose.Position.Y,
                                                             ActiveFrameViews[0].Space.Pose.Position.Z);
                        
                        // Push the camera direction into a simd_quaternion.
                        auto cameraDirection = simd_quaternion(
                                                               ActiveFrameViews[0].Space.Pose.Orientation.X,
                                                               ActiveFrameViews[0].Space.Pose.Orientation.Y,
                                                               ActiveFrameViews[0].Space.Pose.Orientation.Z,
                                                               ActiveFrameViews[0].Space.Pose.Orientation.W);
                        
                        // Load the offset ray and direction into simd equivalents.
                        auto offsetOrigin = simd_make_float3(offsetRay.Origin.X, offsetRay.Origin.Y, offsetRay.Origin.Z);
                        auto offsetDirection = simd_make_float3(offsetRay.Direction.X, offsetRay.Direction.Y, offsetRay.Direction.Z);

                        // Construct the ARRaycast query compositing the camera and offset ray origin + direction targetting existing plane geometry.
                        auto raycastQuery = [[ARRaycastQuery alloc]
                                             initWithOrigin:(cameraOrigin + offsetOrigin)
                                             direction:simd_act(cameraDirection, offsetDirection)
                                             allowingTarget:ARRaycastTargetExistingPlaneGeometry
                                             alignment:ARRaycastTargetAlignmentAny];
                        
                        // Perform the actual raycast.
                        auto rayCastResults = [session raycast:raycastQuery];
                        [raycastQuery release];
                        
                        // Process the results and push them into the results list.
                        for (ARRaycastResult* result in rayCastResults) {
                            filteredResults.push_back(transformToHitResult(result.worldTransform));
                        }
                    } else {
                        // On iOS versions prior to 13, fall back to doing a raycast from a screen point, for now don't support translating the offset ray.
                        auto hitTestResults = [session.currentFrame hitTest:CGPointMake(.5, .5) types:(ARHitTestResultTypeExistingPlane)];
                        for (ARHitTestResult* result in hitTestResults) {
                            filteredResults.push_back(transformToHitResult(result.worldTransform));
                        }
                    }
                }
            }
        }
        private:
            inline static ARSession* session{};
            inline static ARWorldTrackingConfiguration* configuration{};
            MTKView* xrView{};
            bool sessionEnded{ false };
            id<MTLDevice> metalDevice{};
// NOTE: There is an incorrect warning about CAMetalLayer specifically when compiling for the simulator.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpartial-availability"
            CAMetalLayer* metalLayer{};
#pragma clang diagnostic pop
            SessionDelegate* sessionDelegate{};
            id<MTLRenderPipelineState> pipelineState{};
            vector_uint2 viewportSize{};
            id<MTLCommandQueue> commandQueue;
        
        /*
         Helper function to translate a world transform into a hit test result.
         */
        HitResult transformToHitResult(simd_float4x4 transform) const {
            auto orientation = simd_quaternion(transform);
            HitResult hitResult{};
            hitResult.Pose.Orientation = {
                orientation.vector.x,
                orientation.vector.y,
                orientation.vector.z,
                orientation.vector.w
            };
            hitResult.Pose.Position = {
                transform.columns[3][0],
                transform.columns[3][1],
                transform.columns[3][2]
            };
            
            return hitResult;
        }
    };

    struct System::Session::Frame::Impl {
    public:
        Impl(Session::Impl& sessionImpl)
            : sessionImpl{sessionImpl} { }

        Session::Impl& sessionImpl;
    };

    System::Session::Frame::Frame(Session::Impl& sessionImpl)
        : Views{ sessionImpl.ActiveFrameViews }
        , InputSources{ sessionImpl.InputSources}
        , m_impl{ std::make_unique<System::Session::Frame::Impl>(sessionImpl) } {
        Views[0].DepthNearZ = sessionImpl.DepthNearZ;
        Views[0].DepthFarZ = sessionImpl.DepthFarZ;
    }

    System::Session::Frame::~Frame() {
        m_impl->sessionImpl.DrawFrame();
    }

    void System::Session::Frame::GetHitTestResults(std::vector<HitResult>& filteredResults, xr::Ray offsetRay) const {
        m_impl->sessionImpl.GetHitTestResults(filteredResults, offsetRay);
    }

    Anchor System::Session::Frame::CreateAnchor(Pose, NativeTrackablePtr) const {
        throw std::runtime_error("Not yet implemented");
    }

    void System::Session::Frame::UpdateAnchor(xr::Anchor&) const {
        throw std::runtime_error("Not yet implemented");
    }

    void System::Session::Frame::DeleteAnchor(xr::Anchor&) const {
        throw std::runtime_error("Not yet implemented");
    }

    System::System(const char* appName)
        : m_impl{ std::make_unique<System::Impl>(appName) } {}

    System::~System() {}

    bool System::IsInitialized() const {
        return m_impl->IsInitialized();
    }

    bool System::TryInitialize() {
        return m_impl->TryInitialize();
    }

    arcana::task<bool, std::exception_ptr> System::IsSessionSupportedAsync(SessionType sessionType) {
        // Only IMMERSIVE_AR is supported for now.
        return arcana::task_from_result<std::exception_ptr>(sessionType == SessionType::IMMERSIVE_AR);
    }

    arcana::task<std::shared_ptr<System::Session>, std::exception_ptr> System::Session::CreateAsync(System& system, void* graphicsDevice, void* window) {
        return arcana::task_from_result<std::exception_ptr>(std::make_shared<System::Session>(system, graphicsDevice, window));
    }

    System::Session::Session(System& system, void* graphicsDevice, void* window)
        : m_impl{ std::make_unique<System::Session::Impl>(*system.m_impl, graphicsDevice, window) } {}

    System::Session::~Session() {
        // Free textures
    }

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)>) {
        return m_impl->GetNextFrame(shouldEndSession, shouldRestartSession);
    }

    void System::Session::RequestEndSession() {
        m_impl->RequestEndSession();
    }

    Size System::Session::GetWidthAndHeightForViewIndex(size_t viewIndex) const {
        return m_impl->GetWidthAndHeightForViewIndex(viewIndex);
    }

    void System::Session::SetDepthsNearFar(float depthNear, float depthFar) {
        m_impl->DepthNearZ = depthNear;
        m_impl->DepthFarZ = depthFar;
    }
}
