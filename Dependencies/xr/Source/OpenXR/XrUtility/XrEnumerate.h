//*********************************************************
//    Copyright (c) Microsoft. All rights reserved.
//
//    Apache 2.0 License
//
//    You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
//*********************************************************
#pragma once

#include <openxr/openxr.h>
#include <vector>

// not in original
//#include "XrError.h"
#include <../XrPlatformCommon.h>
#define CHECK_XRCMD XrCheck

namespace xr {
    template <typename TArray, typename TValue>
    inline bool Contains(const TArray& array, const TValue& value) {
        return std::end(array) != std::find(std::begin(array), std::end(array), value);
    }

    inline std::vector<XrExtensionProperties> EnumerateInstanceExtensionProperties(const char* layerName = nullptr) {
        uint32_t extensionCount;
        CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(layerName, 0, &extensionCount, nullptr));
        std::vector<XrExtensionProperties> extensionProperties(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
        CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(layerName, extensionCount, &extensionCount, extensionProperties.data()));
        extensionProperties.resize(extensionCount);
        return extensionProperties;
    }

    inline std::vector<XrViewConfigurationType> EnumerateViewConfigurations(XrInstance instance, XrSystemId systemId) {
        uint32_t viewConfigurationCount = 0;
        CHECK_XRCMD(xrEnumerateViewConfigurations(instance, systemId, 0, &viewConfigurationCount, nullptr));

        std::vector<XrViewConfigurationType> viewConfigs(viewConfigurationCount);
        CHECK_XRCMD(xrEnumerateViewConfigurations(instance, systemId, viewConfigurationCount, &viewConfigurationCount, viewConfigs.data()));
        return viewConfigs;
    }

    inline std::vector<XrViewConfigurationView> EnumerateViewConfigurationViews(XrInstance instance,
                                                                                XrSystemId systemId,
                                                                                XrViewConfigurationType viewConfigurationType) {
        uint32_t viewCount;
        CHECK_XRCMD(xrEnumerateViewConfigurationViews(instance, systemId, viewConfigurationType, 0, &viewCount, nullptr));

        std::vector<XrViewConfigurationView> viewConfigViews(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        CHECK_XRCMD(xrEnumerateViewConfigurationViews(
            instance, systemId, viewConfigurationType, (uint32_t)viewConfigViews.size(), &viewCount, viewConfigViews.data()));

        return viewConfigViews;
    }

    inline std::vector<XrEnvironmentBlendMode> EnumerateEnvironmentBlendModes(XrInstance instance,
                                                                              XrSystemId systemId,
                                                                              XrViewConfigurationType viewConfigType) {
        uint32_t blendModeCount;
        CHECK_XRCMD(xrEnumerateEnvironmentBlendModes(instance, systemId, viewConfigType, 0, &blendModeCount, nullptr));

        std::vector<XrEnvironmentBlendMode> blendModes(blendModeCount);
        CHECK_XRCMD(
            xrEnumerateEnvironmentBlendModes(instance, systemId, viewConfigType, blendModeCount, &blendModeCount, blendModes.data()));

        return blendModes;
    }

    // Pick the first supported EnvironmentBlendMode from runtime's supported list.
    inline XrEnvironmentBlendMode PickEnvironmentBlendMode(const std::vector<XrEnvironmentBlendMode>& systemSupportedBlendModes,
                                                           const std::vector<XrEnvironmentBlendMode>& appSupportedBlendModes) {
        auto blendModeIt = std::find_first_of(systemSupportedBlendModes.begin(),
                                              systemSupportedBlendModes.end(),
                                              appSupportedBlendModes.begin(),
                                              appSupportedBlendModes.end());
        if (blendModeIt == std::end(systemSupportedBlendModes)) {
            throw std::runtime_error("No blend modes supported");
        }

        return *blendModeIt;
    }

    inline std::vector<int64_t> EnumerateSwapchainFormats(XrSession session) {
        uint32_t swapchainFormatsCount;
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session, 0, &swapchainFormatsCount, nullptr));

        std::vector<int64_t> runtimeFormats(swapchainFormatsCount);
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session, (uint32_t)runtimeFormats.size(), &swapchainFormatsCount, runtimeFormats.data()));
        return runtimeFormats;
    }

    // Pick the first supported swapchain format from runtime's supported format list.
    template <typename TSystemPixelFormat, typename TAppPixelFormat>
    inline TAppPixelFormat PickSwapchainFormat(const std::vector<TSystemPixelFormat>& systemSupportedFormats,
                                               const std::vector<TAppPixelFormat>& appSupportedFormats) {
        // Here we prioritize Runtime format preference over App format preference
        auto swapchainFormatIt = std::find_first_of(
            systemSupportedFormats.begin(), systemSupportedFormats.end(), appSupportedFormats.begin(), appSupportedFormats.end());

        if (swapchainFormatIt == std::end(systemSupportedFormats)) {
            throw std::runtime_error("No swapchain format supported");
        }

        return (DXGI_FORMAT)*swapchainFormatIt;
    }

    inline std::vector<XrReferenceSpaceType> EnumerateReferenceSpaceTypes(XrSession session) {
        uint32_t spaceCountOutput;
        CHECK_XRCMD(xrEnumerateReferenceSpaces(session, 0, &spaceCountOutput, nullptr));

        std::vector<XrReferenceSpaceType> runtimeSupportedReferenceSpaces(spaceCountOutput);
        CHECK_XRCMD(xrEnumerateReferenceSpaces(
            session, (uint32_t)runtimeSupportedReferenceSpaces.size(), &spaceCountOutput, runtimeSupportedReferenceSpaces.data()));
        return runtimeSupportedReferenceSpaces;
    }

} // namespace xr
