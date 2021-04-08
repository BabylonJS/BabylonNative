#include "Windows/XrPlatform.h"
#include <unordered_set>

#ifndef XR_SUPPORTED_EXTENSIONS
#define XR_SUPPORTED_EXTENSIONS
namespace xr
{
    struct XrSupportedExtensions : ExtensionDispatchTable
    {
        XrSupportedExtensions()
            : Names{}
        {
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
            DepthExtensionSupported = TryEnableExtension(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
            UnboundedRefSpaceSupported = TryEnableExtension(XR_MSFT_UNBOUNDED_REFERENCE_SPACE_EXTENSION_NAME);
            SpatialAnchorSupported = TryEnableExtension(XR_MSFT_SPATIAL_ANCHOR_EXTENSION_NAME);
            SpatialAnchorInteropSupported = TryEnableExtension(XR_MSFT_PERCEPTION_ANCHOR_INTEROP_EXTENSION_NAME);
            SecondaryViewConfigurationSupported = TryEnableExtension(XR_MSFT_SECONDARY_VIEW_CONFIGURATION_EXTENSION_NAME);
            FirstPersonObserverSupported = TryEnableExtension(XR_MSFT_FIRST_PERSON_OBSERVER_EXTENSION_NAME);
            HandInteractionSupported = TryEnableExtension(XR_MSFT_HAND_INTERACTION_EXTENSION_NAME);
            HandTrackingSupported = TryEnableExtension(XR_EXT_HAND_TRACKING_EXTENSION_NAME);
            SceneUnderstandingSupported = TryEnableExtension(XR_MSFT_SCENE_UNDERSTANDING_PREVIEW2_EXTENSION_NAME);
            SceneUnderstandingSerializationSupported = TryEnableExtension(XR_MSFT_SCENE_UNDERSTANDING_SERIALIZATION_PREVIEW_EXTENSION_NAME);
        }

        bool TryEnableExtension(const char* extensionName)
        {
            if (m_supportedExtensionNames.count(extensionName) > 0)
            {
                return true;
            }

            for (const auto& extensionProperty : m_extensionProperties)
            {
                if (strcmp(extensionProperty.extensionName, extensionName) == 0)
                {
                    Names.push_back(extensionName);
                    m_supportedExtensionNames.insert(extensionName);
                    return true;
                }
            }
            return false;
        };

        bool IsExtensionSupported(const std::string& extensionName) const
        {
            return m_supportedExtensionNames.count(extensionName) > 0;
        }

        std::vector<const char*> Names{};
        bool DepthExtensionSupported{ false };
        bool UnboundedRefSpaceSupported{ false };
        bool SpatialAnchorSupported{ false };
        bool SpatialAnchorInteropSupported{ false };
        bool SecondaryViewConfigurationSupported{ false };
        bool FirstPersonObserverSupported{ false };
        bool HandInteractionSupported{ false };
        bool HandTrackingSupported{ false };
        bool SceneUnderstandingSupported{ false };
        bool SceneUnderstandingSerializationSupported{ false };

    private:
        std::vector<XrExtensionProperties> m_extensionProperties{};
        std::unordered_set<std::string> m_supportedExtensionNames{};
    };
}
#endif