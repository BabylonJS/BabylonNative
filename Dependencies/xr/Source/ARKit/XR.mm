#include <XR.h>

#import <UIKit/UIKit.h>
#import <ARKit/ARKit.h>
#import <ARKit/ARConfiguration.h>
#import <MetalKit/MetalKit.h>

@interface SessionDelegate : NSObject <ARSessionDelegate>
@property (readonly) id<MTLTexture> cameraTextureY;
@property (readonly) id<MTLTexture> cameraTextureCbCr;
@end

@implementation SessionDelegate
{
    int width;
    int height;
    std::vector<xr::System::Session::Frame::View>* activeFrameViews;
    CVMetalTextureCacheRef textureCache;
    id<MTLTexture> _cameraTextureY;
    id<MTLTexture> _cameraTextureCbCr;
}

- (id)init:(std::vector<xr::System::Session::Frame::View>*)activeFrameViews metalContext:(id<MTLDevice>)graphicsContext
{
    self = [super init];
    self->activeFrameViews = activeFrameViews;
    
    CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, 0, graphicsContext, 0, &textureCache);
    if (err)
    {
        throw std::runtime_error{"Unable to create Texture Cache"};
    }
    
    return self;
}

- (id<MTLTexture>)updateCapturedTexture:(CVPixelBufferRef)pixelBuffer plane:(int)planeIndex
{
    /*CVReturn ret = CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    if (ret != kCVReturnSuccess)
    {
        return {};
    }

    @try*/
    {
        width = (int) CVPixelBufferGetWidthOfPlane(pixelBuffer, planeIndex);
        height = (int) CVPixelBufferGetHeightOfPlane(pixelBuffer, planeIndex);
            
        auto pixelFormat = planeIndex ? MTLPixelFormatRG8Unorm : MTLPixelFormatR8Unorm;
        id<MTLTexture> mtlTexture;
        CVMetalTextureRef texture;
        auto status = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, textureCache, pixelBuffer, NULL, pixelFormat, width, height, planeIndex, &texture);
        if (status == kCVReturnSuccess)
        {
            mtlTexture = CVMetalTextureGetTexture(texture);
        }
        return mtlTexture;
    }
    /*@finally
    {
        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }*/
}

- (UIInterfaceOrientation)orientation
{
    return [[UIApplication sharedApplication] statusBarOrientation];
}

- (CGSize)viewportSize
{
    UIInterfaceOrientation orientation = [self orientation];
    CGSize viewport_size = CGSizeMake(width, height);
    if (orientation == UIInterfaceOrientationPortrait || orientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        if (width > height)
        {
            viewport_size = CGSizeMake(height, width);
        }
    }
    else
    {
        if (width < height)
        {
            viewport_size = CGSizeMake(height, width);
        }
    }
    return viewport_size;
}

- (void)session:(ARSession *)session didUpdateFrame:(ARFrame *)frame
{
    CVPixelBufferRef pixel_buffer = frame.capturedImage;
    if (CVPixelBufferGetPlaneCount(pixel_buffer) < 2)
    {
        return;
    }
    
    _cameraTextureY = [self updateCapturedTexture:pixel_buffer plane:0];
    _cameraTextureCbCr = [self updateCapturedTexture:pixel_buffer plane:1];
    [self updateCamera:frame.camera];
}

- (void)updateCamera:(ARCamera*)camera
{
    auto& frameView = activeFrameViews->at(0);
    
    UIInterfaceOrientation orientation = [self orientation];
    auto transform = [camera transform];
    auto transformOrientation = simd_quaternion(transform);
    frameView.Space.Pose.Orientation = {transformOrientation.vector.x
        , transformOrientation.vector.y
        , transformOrientation.vector.z
        , transformOrientation.vector.w};
    
    frameView.Space.Pose.Position = { transform.columns[0][3]
        , transform.columns[1][3]
        , transform.columns[2][3]};
    
    auto projection = [camera projectionMatrixForOrientation:orientation viewportSize:[self viewportSize] zNear:frameView.DepthNearZ zFar:frameView.DepthFarZ];
    float a = projection.columns[0][0];
    float b = projection.columns[1][1];
    float aspectRatio = b/a;
    float fov = atanf(1.f / b);
    
    frameView.FieldOfView.AngleDown = -(frameView.FieldOfView.AngleUp = fov);
    frameView.FieldOfView.AngleLeft = -(frameView.FieldOfView.AngleRight = fov * aspectRatio);
}

- (void)triggerProgrammaticCapture:(id<MTLDevice>)captureObject
{
    MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];
    if (@available(iOS 13.0, *)) {
        MTLCaptureDescriptor* captureDescriptor = [[MTLCaptureDescriptor alloc] init];
        captureDescriptor.captureObject = captureObject;

        NSError *error;
        if (![captureManager startCaptureWithDescriptor: captureDescriptor error:&error])
        {
            NSLog(@"Failed to start capture, error %@", error);
        }
    } else {
        // Fallback on earlier versions
    }
}

@end
namespace xr
{
    namespace
    {
        typedef struct
        {
            vector_float2 position;
            vector_float2 uv;
        } XRVertex;

        static const XRVertex triangleVertices[] =
        {
            // 2D positions,    UV
            { { -3, -1 }, { -1, 0 } },
            { {  1,  3 }, {  1, 2 } },
            { {  1, -1 }, {  1, 0 } },
        };

        const char* shaderSource = R"(
            #include <metal_stdlib>
            #include <simd/simd.h>

            using namespace metal;

            #include <simd/simd.h>

            typedef struct
            {
                vector_float2 position;
                vector_float2 uv;
            } XRVertex;

            typedef struct
            {
                float4 position [[position]];
                float2 uv;
            } RasterizerData;

            vertex RasterizerData
            vertexShader(uint vertexID [[vertex_id]],
                         constant XRVertex *vertices [[buffer(0)]])
            {
                RasterizerData out;
                out.position = vector_float4(vertices[vertexID].position.xy, 0.0, 1.0);
                out.uv = vertices[vertexID].uv;
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
    
                const float4 cameraSampleY = cameraTextureY.sample(linearSampler, in.uv);
                const float4 cameraSampleCbCr = cameraTextureCbCr.sample(linearSampler, in.uv);

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
            if(NULL != error)
            {
                throw std::runtime_error{[error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]};
            }
            return lib;
        }
    }
    
    class System::Impl
    {
    public:
        Impl(const std::string& applicationName)
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

    class System::Session::Impl
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
                xrView = [[MTKView alloc] initWithFrame:[MainView bounds] device:MetalDevice];
                [MainView addSubview:xrView];
                xrView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
                xrView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
                MetalLayer = (CAMetalLayer *)xrView.layer;
                MetalLayer.device = MetalDevice;
                auto scale = UIScreen.mainScreen.scale;
                viewportSize.x = [MainView bounds].size.width * scale;
                viewportSize.y = [MainView bounds].size.height * scale;
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

        std::unique_ptr<System::Session::Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession)
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

        Size GetWidthAndHeightForViewIndex(size_t viewIndex) const
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
                [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];

                [renderEncoder endEncoding];

                // Schedule a present once the framebuffer is complete using the current drawable.
                [commandBuffer presentDrawable:drawable];
            }

            // Finalize rendering here & push the command buffer to the GPU.
            [commandBuffer commit];
        }

        void GetHitTestResults(std::vector<Pose>& filteredResults, xr::Ray offsetRay) const
        {
            // TODO
        }
        
        private:
            MTKView* xrView;
            bool SessionEnded{ false };
            id<MTLDevice> MetalDevice;
            CAMetalLayer* MetalLayer;
            ARSession* session{};
            SessionDelegate* sessionDelegate{};
            ARWorldTrackingConfiguration* configuration{};
            id<MTLRenderPipelineState> pipelineState;
            vector_uint2 viewportSize;
            id<MTLCommandQueue> commandQueue;
    };

    class System::Session::Frame::Impl
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

    void System::Session::Frame::GetHitTestResults(std::vector<Pose>& filteredResults, xr::Ray offsetRay) const
    {
        m_impl->sessionImpl.GetHitTestResults(filteredResults, offsetRay);
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

    System::Session::Session(System& system, void* graphicsDevice, void* window)
        : m_impl{ std::make_unique<System::Session::Impl>(*system.m_impl, graphicsDevice, window) }
    {}

    System::Session::~Session()
    {
        // Free textures
    }

    std::unique_ptr<System::Session::Frame> System::Session::GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession)
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
