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

//original
//#define FOR_EACH_EXTENSION_FUNCTION(_)           \
//    _(xrCreateSpatialAnchorMSFT)                 \
//    _(xrCreateSpatialAnchorSpaceMSFT)            \
//    _(xrDestroySpatialAnchorMSFT)                \
//    _(xrCreateHandTrackerEXT)                    \
//    _(xrDestroyHandTrackerEXT)                   \
//    _(xrLocateHandJointsEXT)                     \
//    _(xrCreateHandMeshSpaceMSFT)                 \
//    _(xrUpdateHandMeshMSFT)                      \
//    _(xrConvertWin32PerformanceCounterToTimeKHR) \
//    _(xrCreateSpatialGraphNodeSpaceMSFT)         \
//    _(xrGetD3D11GraphicsRequirementsKHR)         \
//    _(xrGetControllerModelKeyMSFT)               \
//    _(xrLoadControllerModelMSFT)                 \
//    _(xrGetControllerModelPropertiesMSFT)        \
//    _(xrGetControllerModelStateMSFT)             \
//    _(xrGetVisibilityMaskKHR)

#define FOR_EACH_EXTENSION_FUNCTION(_)           \
    _(xrCreateSpatialAnchorMSFT)                 \
    _(xrCreateSpatialAnchorSpaceMSFT)            \
    _(xrDestroySpatialAnchorMSFT)                \
    _(xrCreateHandTrackerEXT)                    \
    _(xrDestroyHandTrackerEXT)                   \
    _(xrLocateHandJointsEXT)                     \
    _(xrCreateHandMeshSpaceMSFT)                 \
    _(xrUpdateHandMeshMSFT)                      \
    _(xrCreateSpatialGraphNodeSpaceMSFT)         \
    _(xrGetD3D11GraphicsRequirementsKHR)         \
    _(xrGetVisibilityMaskKHR)


#define GET_INSTANCE_PROC_ADDRESS(name) \
    (void)xrGetInstanceProcAddr(instance, #name, reinterpret_cast<PFN_xrVoidFunction*>(const_cast<PFN_##name*>(&name)));
#define DEFINE_PROC_MEMBER(name) const PFN_##name name{nullptr};

namespace xr {
    struct ExtensionDispatchTable {
        FOR_EACH_EXTENSION_FUNCTION(DEFINE_PROC_MEMBER);

        ExtensionDispatchTable() = default;
        void PopulateDispatchTable(XrInstance instance) {
            FOR_EACH_EXTENSION_FUNCTION(GET_INSTANCE_PROC_ADDRESS);
        }
    };
} // namespace xr

#undef DEFINE_PROC_MEMBER
#undef GET_INSTANCE_PROC_ADDRESS
#undef FOR_EACH_EXTENSION_FUNCTION

