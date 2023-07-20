#include "Windows/XrPlatform.h"
#include <unordered_set>

#ifndef XR_SUPPORTED_EXTENSIONS
#define XR_SUPPORTED_EXTENSIONS
namespace xr
{
    struct XrSupportedExtensions : ExtensionDispatchTable
    {
        XrSupportedExtensions()
            : m_names{}
        {
        }

        bool TryEnableExtension(const char* extensionName)
        {
            assert(m_initialized);
            if (m_supportedExtensionNames.count(extensionName) > 0)
            {
                return true;
            }

            for (const auto& extensionProperty : m_extensionProperties)
            {
                if (strcmp(extensionProperty.extensionName, extensionName) == 0)
                {
                    m_names.push_back(extensionName);
                    m_supportedExtensionNames.insert(extensionName);
                    return true;
                }
            }
            return false;
        };

        bool IsExtensionSupported(const std::string& extensionName) const
        {
            assert(m_initialized);
            return m_supportedExtensionNames.count(extensionName) > 0;
        }

        bool DepthExtensionSupported() const
        {
            assert(m_initialized);
            return m_depthExtensionSupported;
        }
        bool UnboundedRefSpaceSupported() const
        {
            assert(m_initialized);
            return m_unboundedRefSpaceSupported;
        }
        bool SpatialAnchorSupported() const
        {
            assert(m_initialized);
            return m_spatialAnchorSupported;
        }
        bool SpatialAnchorInteropSupported() const
        {
            assert(m_initialized);
            return m_spatialAnchorInteropSupported;
        }
        bool SecondaryViewConfigurationSupported() const
        {
            assert(m_initialized);
            return m_secondaryViewConfigurationSupported;
        }
        bool FirstPersonObserverSupported() const
        {
            assert(m_initialized);
            return m_firstPersonObserverSupported;
        }
        bool HandInteractionSupported() const
        {
            assert(m_initialized);
            return m_handInteractionSupported;
        }
        bool HandTrackingSupported() const
        {
            assert(m_initialized);
            return m_handTrackingSupported;
        }
        bool SceneUnderstandingSupported() const
        {
            assert(m_initialized);
            return m_sceneUnderstandingSupported;
        }
        bool SceneUnderstandingSerializationSupported() const
        {
            assert(m_initialized);
            return m_sceneUnderstandingSerializationSupported;
        }
        bool EyeTrackingSupported() const
        {
            assert(m_initialized);
            return m_eyeTrackingSupported;
        }

        void Initialize()
        {
            if (m_initialized)
            {
                return;
            }
            m_initialized = true;
            uint32_t extensionCount{};
            XrResult result{ xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr) };
            if (result != XR_SUCCESS)
            {
                // Avoid failing if device doesn't support OpenXR
                return;
            }

            m_extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
            XrCheck(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, m_extensionProperties.data()));

            // D3D11 extension is required, so check if it's supported.
            for (const char* extensionName : REQUIRED_EXTENSIONS)
            {
                if (!TryEnableExtension(extensionName))
                {
                    throw std::runtime_error{ "Required extension not supported" };
                }
            }

            // Additional optional extensions for enhanced functionality. Track whether enabled in m_optionalExtensions.
            m_depthExtensionSupported = TryEnableExtension(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
            m_unboundedRefSpaceSupported = TryEnableExtension(XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME);
            m_spatialAnchorSupported = TryEnableExtension(XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME);
            m_spatialAnchorInteropSupported = TryEnableExtension(XR_MSFT_PERCEPTION_ANCHOR_INTEROP_EXTENSION_NAME);
            m_secondaryViewConfigurationSupported = TryEnableExtension(XR_MSFT_SECONDARY_VIEW_CONFIGURATION_EXTENSION_NAME);
            m_firstPersonObserverSupported = TryEnableExtension(XR_MSFT_FIRST_PERSON_OBSERVER_EXTENSION_NAME);
            m_handInteractionSupported = TryEnableExtension(XR_MSFT_HAND_INTERACTION_EXTENSION_NAME);
            m_handTrackingSupported = TryEnableExtension(XR_EXT_HAND_TRACKING_EXTENSION_NAME);
            m_sceneUnderstandingSupported = TryEnableExtension(XR_MSFT_SCENE_UNDERSTANDING_EXTENSION_NAME);
            m_sceneUnderstandingSerializationSupported = TryEnableExtension(XR_MSFT_SCENE_UNDERSTANDING_SERIALIZATION_EXTENSION_NAME);
            m_eyeTrackingSupported = TryEnableExtension(XR_EXT_EYE_GAZE_INTERACTION_EXTENSION_NAME);
        }
        const std::vector<const char*>& Names() const
        {
            assert(m_initialized);
            return m_names;
        }
    private:
        std::vector<const char*> m_names{};
        std::vector<XrExtensionProperties> m_extensionProperties{};
        std::unordered_set<std::string> m_supportedExtensionNames{};

        bool m_depthExtensionSupported{ false };
        bool m_unboundedRefSpaceSupported{ false };
        bool m_spatialAnchorSupported{ false };
        bool m_spatialAnchorInteropSupported{ false };
        bool m_secondaryViewConfigurationSupported{ false };
        bool m_firstPersonObserverSupported{ false };
        bool m_handInteractionSupported{ false };
        bool m_handTrackingSupported{ false };
        bool m_sceneUnderstandingSupported{ false };
        bool m_sceneUnderstandingSerializationSupported{ false };
        bool m_eyeTrackingSupported{ false };

        bool m_initialized{ false };
    };
}
#endif