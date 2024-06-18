namespace Babylon
{
    namespace Plugins
    {
        class NativeXr::Impl final : public std::enable_shared_from_this<NativeXr::Impl>
        {
        public:
            explicit Impl(Napi::Env);

            void UpdateWindow(void* windowPtr);
            void SetSessionStateChangedCallback(std::function<void(bool)> callback);

            arcana::task<void, std::exception_ptr> BeginSessionAsync();
            arcana::task<void, std::exception_ptr> EndSessionAsync();

            void ScheduleFrame(std::function<void(const xr::System::Session::Frame&)>&& callback);

            void SetRenderTextureFunctions(const Napi::Function& createFunction, const Napi::Function& destroyFunction)
            {
                m_sessionState->CreateRenderTexture = Napi::Persistent(createFunction);
                m_sessionState->DestroyRenderTexture = Napi::Persistent(destroyFunction);
            }

            Napi::Value GetRenderTargetForViewIndex(uint32_t viewIndex) const
            {
                const auto& activeViewConfigs = m_sessionState->ActiveViewConfigurations;
                if (activeViewConfigs.size() <= viewIndex ||
                    activeViewConfigs[viewIndex] == nullptr ||
                    !activeViewConfigs[viewIndex]->Initialized)
                {
                    return m_env.Null();
                }

                const auto viewConfig = activeViewConfigs[viewIndex];
                const auto startViewIdx = m_sessionState->ViewConfigurationStartViewIdx[viewConfig];
                return viewConfig->JsTextures[viewConfig->FrameBuffers[viewIndex - startViewIdx]].Value();
            }

            void SetDepthsNarFar(float depthNear, float depthFar)
            {
                m_sessionState->Session->SetDepthsNearFar(depthNear, depthFar);
            }

            void SetPlaneDetectionEnabled(bool enabled)
            {
                m_sessionState->Session->SetPlaneDetectionEnabled(enabled);
            }

            bool TrySetFeaturePointCloudEnabled(bool enabled)
            {
                return m_sessionState->Session->TrySetFeaturePointCloudEnabled(enabled);
            }

            bool TrySetPreferredPlaneDetectorOptions(const xr::GeometryDetectorOptions& options)
            {
                return m_sessionState->Session->TrySetPreferredPlaneDetectorOptions(options);
            }

            bool TrySetMeshDetectorEnabled(const bool enabled)
            {
                return m_sessionState->Session->TrySetMeshDetectorEnabled(enabled);
            }

            bool TrySetPreferredMeshDetectorOptions(const xr::GeometryDetectorOptions& options)
            {
                return m_sessionState->Session->TrySetPreferredMeshDetectorOptions(options);
            }

            std::vector<xr::ImageTrackingScore>* GetImageTrackingScores()
            {
                return m_sessionState->Session->GetImageTrackingScores();
            }

            void CreateAugmentedImageDatabase(const std::vector<xr::System::Session::ImageTrackingRequest>& requests)
            {
                m_sessionState->Session->CreateAugmentedImageDatabase(requests);
            }

            uintptr_t GetNativeXrContext()
            {
                return m_system.GetNativeXrContext();
            }

            std::string GetNativeXrContextType()
            {
                return m_system.GetNativeXrContextType();
            }

        private:
            Napi::Env m_env;
            JsRuntimeScheduler m_runtimeScheduler;
            std::mutex m_sessionStateChangedCallbackMutex{};
            std::function<void(bool)> m_sessionStateChangedCallback{};
            void* m_windowPtr{};
            std::optional<arcana::task<void, std::exception_ptr>> m_beginTask{};
            arcana::task<void, std::exception_ptr> m_endTask{arcana::task_from_result<std::exception_ptr>()};

            struct ViewConfiguration final
            {
                void* ColorTexturePointer{nullptr};
                void* DepthTexturePointer{nullptr};
                xr::Size ViewTextureSize{};
                std::vector<Graphics::FrameBuffer*> FrameBuffers{};
                std::map<Graphics::FrameBuffer*, Napi::ObjectReference> JsTextures{};
                bool Initialized{false};
            };

            struct SessionState final
            {
                explicit SessionState(Graphics::DeviceContext& graphicsContext)
                    : GraphicsContext{graphicsContext}
                    , Update{GraphicsContext.GetUpdate("update")}
                {
                }

                Graphics::DeviceContext& GraphicsContext;
                Graphics::Update Update;
                Napi::FunctionReference CreateRenderTexture{};
                Napi::FunctionReference DestroyRenderTexture{};
                std::vector<ViewConfiguration*> ActiveViewConfigurations{};
                std::unordered_map<ViewConfiguration*, uint32_t> ViewConfigurationStartViewIdx{};
                std::unordered_map<void*, ViewConfiguration> TextureToViewConfigurationMap{};
                std::shared_ptr<xr::System::Session> Session{};
                std::unique_ptr<xr::System::Session::Frame> Frame{};
                arcana::cancellation_source CancellationSource{};
                bool FrameScheduled{false};
                std::vector<std::function<void(const xr::System::Session::Frame&)>> ScheduleFrameCallbacks{};
                arcana::task<void, std::exception_ptr> FrameTask{arcana::task_from_result<std::exception_ptr>()};
            };

            std::unique_ptr<SessionState> m_sessionState{};
            xr::System m_system{};

            void BeginFrame();
            void BeginUpdate();
            void EndUpdate();
            void EndFrame();

            void NotifySessionStateChanged(bool isSessionActive);
        };
    } // Plugins
} // Babylon