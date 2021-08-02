#ifndef NOMINMAX
#define NOMINMAX // avoid including macro max definitions in favor of limit max definitions
#endif

#include "Input.h"
#include "XR.h"

namespace xr 
{
    namespace
    {
        // Returns true if the action is supported on the current input
        bool TryUpdateControllerFloatAction(XrAction controllerAction, XrSession session, float& currentFloatState)
        {
            // query input action state
            XrActionStateFloat floatActionState{XR_TYPE_ACTION_STATE_FLOAT};
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = controllerAction;
            XrCheck(xrGetActionStateFloat(session, &getInfo, &floatActionState));
            if (!floatActionState.isActive)
            {
                return false;
            }

            if (floatActionState.changedSinceLastSync)
            {
                currentFloatState = floatActionState.currentState;
            }

            return true;
        }

        // Returns true if the action is supported on the current input
        bool TryUpdateControllerBooleanAction(XrAction controllerAction, XrSession session, bool& currentBooleanState)
        {
            // query input action state
            XrActionStateBoolean booleanActionState{XR_TYPE_ACTION_STATE_BOOLEAN};
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = controllerAction;
            XrCheck(xrGetActionStateBoolean(session, &getInfo, &booleanActionState));
            if (!booleanActionState.isActive)
            {
                return false;
            }

            if (booleanActionState.changedSinceLastSync)
            {
               currentBooleanState = booleanActionState.currentState;
            }

            return true;
        }
        
        // Returns true if the action is supported on the current input
        bool TryUpdateControllerVector2fAction(XrAction controllerAction, XrSession session, float& currentXState, float& currentYState)
        {
            // query input action state
            XrActionStateVector2f vector2fActionState{XR_TYPE_ACTION_STATE_VECTOR2F};
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = controllerAction;
            XrCheck(xrGetActionStateVector2f(session, &getInfo, &vector2fActionState));
            if (!vector2fActionState.isActive)
            {
                return false;
            }

            if (vector2fActionState.changedSinceLastSync)
            {
                currentXState = vector2fActionState.currentState.x;
                currentYState = vector2fActionState.currentState.y;
            }

            return true;
        }

        void UpdatePoseData(xr::Pose& targetPose, XrPosef& sourcePose)
        {
            targetPose.Position.X = sourcePose.position.x;
            targetPose.Position.Y = sourcePose.position.y;
            targetPose.Position.Z = sourcePose.position.z;
            targetPose.Orientation.X = sourcePose.orientation.x;
            targetPose.Orientation.Y = sourcePose.orientation.y;
            targetPose.Orientation.Z = sourcePose.orientation.z;
            targetPose.Orientation.W = sourcePose.orientation.w;
        }
    }

    struct XrInput::Impl
    {
        struct
        {
            static constexpr char* DEFAULT_XR_ACTION_SET_NAME{ "default_xr_action_set" };
            static constexpr char* DEFAULT_XR_ACTION_SET_LOCALIZED_NAME{ "Default XR Action Set" };
            XrActionSet ActionSet{};

            static constexpr std::array<const char*, 2> CONTROLLER_SUBACTION_PATH_PREFIXES
            {
                "/user/hand/left",
                "/user/hand/right"
            };
            std::array<XrPath, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerSubactionPaths{};

            static constexpr char* CONTROLLER_GET_GRIP_POSE_ACTION_NAME{ "controller_get_pose_action" };
            static constexpr char* CONTROLLER_GET_GRIP_POSE_ACTION_LOCALIZED_NAME{ "Controller Pose" };
            static constexpr char* CONTROLLER_GET_GRIP_POSE_PATH_SUFFIX{ "/input/grip/pose" };
            XrAction ControllerGetGripPoseAction{};
            std::array<XrSpace, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGripPoseSpaces{};

            static constexpr char* CONTROLLER_GET_AIM_POSE_ACTION_NAME{ "controller_get_aim_action" };
            static constexpr char* CONTROLLER_GET_AIM_POSE_ACTION_LOCALIZED_NAME{ "Controller Aim" };
            static constexpr char* CONTROLLER_GET_AIM_POSE_PATH_SUFFIX{ "/input/aim/pose" };
            XrAction ControllerGetAimPoseAction{};
            std::array<XrSpace, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerAimPoseSpaces{};

            static constexpr char* DEFAULT_GET_SELECT_CLICK_ACTION_NAME{ "default_get_select_action" };
            static constexpr char* DEFAULT_GET_SELECT_CLICK_ACTION_LOCALIZED_NAME{ "Default Select" };
            static constexpr char* DEFAULT_GET_SELECT_CLICK_PATH_SUFFIX{ "/input/select/click" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> DefaultGetSelectValueAction{};

            static constexpr char* CONTROLLER_GET_TRIGGER_VALUE_ACTION_NAME{ "controller_get_trigger_action" };
            static constexpr char* CONTROLLER_GET_TRIGGER_VALUE_ACTION_LOCALIZED_NAME{ "Controller Trigger" };
            static constexpr char* CONTROLLER_GET_TRIGGER_VALUE_PATH_SUFFIX{ "/input/trigger/value" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetTriggerValueAction{};

            static constexpr char* CONTROLLER_GET_SQUEEZE_CLICK_ACTION_NAME{ "controller_get_squeeze_action" };
            static constexpr char* CONTROLLER_GET_SQUEEZE_CLICK_ACTION_LOCALIZED_NAME{ "Controller Squeeze" };
            static constexpr char* CONTROLLER_GET_SQUEEZE_CLICK_PATH_SUFFIX{ "/input/squeeze/click" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetSqueezeClickAction{};

            static constexpr char* CONTROLLER_GET_TRACKPAD_AXES_ACTION_NAME{ "controller_get_trackpad_axes_action" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_AXES_ACTION_LOCALIZED_NAME{ "Controller Trackpad Axes" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_AXES_PATH_SUFFIX{ "/input/trackpad" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetTrackpadAxesAction{};

            static constexpr char* CONTROLLER_GET_TRACKPAD_CLICK_ACTION_NAME{ "controller_get_trackpad_click_action" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_CLICK_ACTION_LOCALIZED_NAME{ "Controller Trackpad Click" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_CLICK_PATH_SUFFIX{ "/input/trackpad/click" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetTrackpadClickAction{};

            static constexpr char* CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_NAME{ "controller_get_trackpad_touch_action" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_LOCALIZED_NAME{ "Controller Trackpad Touch" };
            static constexpr char* CONTROLLER_GET_TRACKPAD_TOUCH_PATH_SUFFIX{ "/input/trackpad/touch" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetTrackpadTouchAction{};

            static constexpr char* CONTROLLER_GET_THUMBSTICK_AXES_ACTION_NAME{ "controller_get_thumbstick_axes_action" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_AXES_ACTION_LOCALIZED_NAME{ "Controller Thumbstick Axes" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_AXES_PATH_SUFFIX{ "/input/thumbstick" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetThumbstickAxesAction{};

            static constexpr char* CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_NAME{ "controller_get_thumbstick_click_action" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_LOCALIZED_NAME{ "Controller Thumbstick Click" };
            static constexpr char* CONTROLLER_GET_THUMBSTICK_CLICK_PATH_SUFFIX{ "/input/thumbstick/click" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> ControllerGetThumbstickClickAction{};

            static constexpr char* HAND_GET_SELECT_ACTION_NAME{ "hand_get_select_action" };
            static constexpr char* HAND_GET_SELECT_ACTION_LOCALIZED_NAME{ "Hand Select" };
            static constexpr char* HAND_GET_SELECT_PATH_SUFFIX{ "/input/select/value" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> HandGetSelectAction{};

            static constexpr char* HAND_GET_SQUEEZE_ACTION_NAME{ "hand_get_squeeze_action" };
            static constexpr char* HAND_GET_SQUEEZE_ACTION_LOCALIZED_NAME{ "Hand Squeeze" };
            static constexpr char* HAND_GET_SQUEEZE_PATH_SUFFIX{ "/input/squeeze/value" };
            std::array<XrAction, CONTROLLER_SUBACTION_PATH_PREFIXES.size()> HandGetSqueezeAction{};

            static constexpr char* EYE_GET_POSE_ACTION_NAME{ "eye_get_pose_action" };
            static constexpr char* EYE_GET_POSE_ACTION_LOCALIZED_NAME{ "Eye Pose" };
            static constexpr char* EYE_GET_POSE_PATH{ "/user/eyes_ext/input/gaze_ext/pose" };
            XrAction EyeGetPoseAction{};
            XrSpace EyePoseActionSpace{};

            static constexpr char* DEFAULT_XR_INTERACTION_PROFILE{ "/interaction_profiles/khr/simple_controller" };
            static constexpr char* MICROSOFT_XR_INTERACTION_PROFILE{ "/interaction_profiles/microsoft/motion_controller" };
            static constexpr char* MICROSOFT_HAND_INTERACTION_PROFILE{ "/interaction_profiles/microsoft/hand_interaction" };
            static constexpr char* XR_EYE_INTERACTION_PROFILE{ "/interaction_profiles/ext/eye_gaze_interaction" };
            XrPath DefaultXRInteractionPath{};
            XrPath MicrosoftXRInteractionPath{};
            XrPath MicrosoftHandInteractionPath{};
            XrPath XREyeInteractionPath{};
        } ActionResources;

        struct
        {
            // default buttons on a device
            static constexpr uint32_t TRIGGER_BUTTON = 0;
            static constexpr uint32_t SQUEEZE_BUTTON = 1;
            static constexpr uint32_t TRACKPAD_BUTTON = 2;
            static constexpr uint32_t THUMBSTICK_BUTTON = 3;

            // optional, hardware-specific buttons
            static constexpr uint32_t CUSTOM_HARDWARE_BUTTON = 4;

            static constexpr uint32_t TRACKPAD_X_AXIS = 0;
            static constexpr uint32_t TRACKPAD_Y_AXIS = 1;
            static constexpr uint32_t THUMBSTICK_X_AXIS = 2;
            static constexpr uint32_t THUMBSTICK_Y_AXIS = 3;
        } ControllerInfo;

        struct HandInfo
        {
            XrHandEXT Hand{};
            XrHandTrackerEXT HandTracker{};

            XrHandJointLocationEXT JointLocations[XR_HAND_JOINT_COUNT_EXT]{};
            XrHandJointLocationsEXT Locations{XR_TYPE_HAND_JOINT_LOCATIONS_EXT, nullptr, false, XR_HAND_JOINT_COUNT_EXT, JointLocations};

            // WebXR specs do not include joint 0, the palm joint, but OpenXR does
            static constexpr uint32_t UNUSED_HAND_JOINT_OFFSET{ 1 };
        };
        
        struct
        {
            std::array<HandInfo, 2> HandsInfo{};

            XrBool32 SupportsArticulatedHandTracking{ false };
            XrBool32 HandTrackersInitialized{ false };
        } HandData;

        XrBool32 SupportsEyeTracking{ false };

        std::function<void()> m_destroyHandTrackers;

        Impl()
        {
        }

        void Initialize(const InitOptions& args)
        {
            SupportsEyeTracking = args.EyeGazeInteractionProps.supportsEyeGazeInteraction;

            // Initialize the hand resources
            HandData.SupportsArticulatedHandTracking = args.HandTrackingInteractionProps.supportsHandTracking && args.Extensions.HandTrackingSupported;
            InitializeHandResources(args.Session, args.Extensions);

            // It's ok to do this because the context will outlive this module
            m_destroyHandTrackers = [this, extensions = args.Extensions]() {
                if (HandData.HandTrackersInitialized)
                {
                    for (const HandInfo& handInfo : HandData.HandsInfo)
                    {
                        XrCheck(extensions.xrDestroyHandTrackerEXT(handInfo.HandTracker));
                    }

                    HandData.HandTrackersInitialized = false;
                }
            };

            InitializeActionResources(args.Instance, args.Session, args.Extensions);
        }

        void InitializeHandResources(
            const XrSession session,
            const XrSupportedExtensions& extensions)
        {
            if (!HandData.SupportsArticulatedHandTracking || HandData.HandTrackersInitialized)
            {
                return;
            }

            constexpr std::array<XrHandEXT, 2> HANDEDNESS_EXT{XR_HAND_LEFT_EXT, XR_HAND_RIGHT_EXT};
            XrHandTrackerCreateInfoEXT trackerCreateInfo{XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT};
            trackerCreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;

            for (size_t i = 0; i < HandData.HandsInfo.size(); i++)
            {
                // Create the hand trackers
                HandData.HandsInfo[i].Hand = HANDEDNESS_EXT[i];
                trackerCreateInfo.hand = HANDEDNESS_EXT[i];
                XrCheck(extensions.xrCreateHandTrackerEXT(session, &trackerCreateInfo, &HandData.HandsInfo[i].HandTracker));
            }

            HandData.HandTrackersInitialized = true;
        }

        void CreateControllerActionAndBinding(
            XrActionType controllerActionType,
            char* controllerActionName,
            char* controllerLocalizedActionName,
            const char* controllerActionSuffix,
            XrAction* controllerAction,
            std::vector<XrActionSuggestedBinding> &bindings,
            XrInstance instance,
            const size_t idx)
        {
            XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
            actionInfo.actionType = controllerActionType;
            std::string controllerActionNameSuffixed{ controllerActionName + std::to_string(idx) };
            std::string controllerLocalizedActionNameSuffixed{ controllerLocalizedActionName + std::to_string(idx) };

            strcpy_s(actionInfo.actionName, controllerActionNameSuffixed.data());
            strcpy_s(actionInfo.localizedActionName, controllerLocalizedActionNameSuffixed.data());
            actionInfo.countSubactionPaths = static_cast<uint32_t>(ActionResources.ControllerSubactionPaths.size());
            actionInfo.subactionPaths = ActionResources.ControllerSubactionPaths.data();
            XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, controllerAction));

            // Create suggested binding
            std::string path{ ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx] };
            path.append(controllerActionSuffix);
            bindings.push_back({*controllerAction});
            XrCheck(xrStringToPath(instance, path.data(), &bindings.back().binding));
        }

        void InitializeActionResources(
            const XrInstance instance, 
            const XrSession session,
            const XrSupportedExtensions& extensions)
        {
            // Create action set
            XrActionSetCreateInfo actionSetInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };
            std::strcpy(actionSetInfo.actionSetName, ActionResources.DEFAULT_XR_ACTION_SET_NAME);
            std::strcpy(actionSetInfo.localizedActionSetName, ActionResources.DEFAULT_XR_ACTION_SET_LOCALIZED_NAME);
            XrCheck(xrCreateActionSet(instance, &actionSetInfo, &ActionResources.ActionSet));

            // Cache paths for subactions
            for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
            {
                XrCheck(xrStringToPath(instance, ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx], &ActionResources.ControllerSubactionPaths[idx]));
            }

            // Cache paths for interaction profiles.
            XrCheck(xrStringToPath(instance, ActionResources.DEFAULT_XR_INTERACTION_PROFILE, &ActionResources.DefaultXRInteractionPath));
            XrCheck(xrStringToPath(instance, ActionResources.MICROSOFT_XR_INTERACTION_PROFILE, &ActionResources.MicrosoftXRInteractionPath));
            XrCheck(xrStringToPath(instance, ActionResources.MICROSOFT_HAND_INTERACTION_PROFILE, &ActionResources.MicrosoftHandInteractionPath));
            XrCheck(xrStringToPath(instance, ActionResources.XR_EYE_INTERACTION_PROFILE, &ActionResources.XREyeInteractionPath));

            std::vector<XrActionSuggestedBinding> defaultBindings{};
            std::vector<XrActionSuggestedBinding> microsoftControllerBindings{};
            std::vector<XrActionSuggestedBinding> microsoftHandBindings{};
            XrActionSuggestedBinding eyeTrackingBinding{};

            // Create controller get grip pose action, suggested bindings, and spaces
            {
                XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
                actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
                strcpy_s(actionInfo.actionName, ActionResources.CONTROLLER_GET_GRIP_POSE_ACTION_NAME);
                strcpy_s(actionInfo.localizedActionName, ActionResources.CONTROLLER_GET_GRIP_POSE_ACTION_LOCALIZED_NAME);
                actionInfo.countSubactionPaths = static_cast<uint32_t>(ActionResources.ControllerSubactionPaths.size());
                actionInfo.subactionPaths = ActionResources.ControllerSubactionPaths.data();
                XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, &ActionResources.ControllerGetGripPoseAction));
                // For each controller subaction
                for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
                {
                    // Create suggested binding
                    std::string path{ ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx] };
                    path.append(ActionResources.CONTROLLER_GET_GRIP_POSE_PATH_SUFFIX);

                    defaultBindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &defaultBindings.back().binding));

                    microsoftControllerBindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &microsoftControllerBindings.back().binding));

                    if (extensions.HandInteractionSupported)
                    {
                        microsoftHandBindings.push_back({ ActionResources.ControllerGetGripPoseAction });
                        XrCheck(xrStringToPath(instance, path.data(), &microsoftHandBindings.back().binding));
                    }

                    // Create subaction space
                    XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                    actionSpaceCreateInfo.action = ActionResources.ControllerGetGripPoseAction;
                    actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                    actionSpaceCreateInfo.subactionPath = ActionResources.ControllerSubactionPaths[idx];
                    XrCheck(xrCreateActionSpace(session, &actionSpaceCreateInfo, &ActionResources.ControllerGripPoseSpaces[idx]));
                }
            }

            // Create controller get aim pose action, suggested bindings, and spaces
            {
                XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
                actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
                strcpy_s(actionInfo.actionName, ActionResources.CONTROLLER_GET_AIM_POSE_ACTION_NAME);
                strcpy_s(actionInfo.localizedActionName, ActionResources.CONTROLLER_GET_AIM_POSE_ACTION_LOCALIZED_NAME);
                actionInfo.countSubactionPaths = static_cast<uint32_t>(ActionResources.ControllerSubactionPaths.size());
                actionInfo.subactionPaths = ActionResources.ControllerSubactionPaths.data();
                XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, &ActionResources.ControllerGetAimPoseAction));
                // For each controller subaction
                for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
                {
                    // Create suggested binding
                    std::string path{ ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES[idx] };
                    path.append(ActionResources.CONTROLLER_GET_AIM_POSE_PATH_SUFFIX);

                    defaultBindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &defaultBindings.back().binding));
                    
                    microsoftControllerBindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                    XrCheck(xrStringToPath(instance, path.data(), &microsoftControllerBindings.back().binding));

                    if (extensions.HandInteractionSupported)
                    {
                        microsoftHandBindings.push_back({ ActionResources.ControllerGetAimPoseAction });
                        XrCheck(xrStringToPath(instance, path.data(), &microsoftHandBindings.back().binding));
                    }

                    // Create subaction space
                    XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                    actionSpaceCreateInfo.action = ActionResources.ControllerGetAimPoseAction;
                    actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                    actionSpaceCreateInfo.subactionPath = ActionResources.ControllerSubactionPaths[idx];
                    XrCheck(xrCreateActionSpace(session, &actionSpaceCreateInfo, &ActionResources.ControllerAimPoseSpaces[idx]));
                }
            }

            // Create eye tracking get pose action, suggested binding, and space
            if (SupportsEyeTracking)
            {
                XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
                actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
                strcpy_s(actionInfo.actionName, ActionResources.EYE_GET_POSE_ACTION_NAME);
                strcpy_s(actionInfo.localizedActionName, ActionResources.EYE_GET_POSE_ACTION_LOCALIZED_NAME);
                XrCheck(xrCreateAction(ActionResources.ActionSet, &actionInfo, &ActionResources.EyeGetPoseAction));

                // Create suggested binding
                std::string path{ ActionResources.EYE_GET_POSE_PATH };

                eyeTrackingBinding.action = ActionResources.EyeGetPoseAction;
                XrCheck(xrStringToPath(instance, path.data(), &eyeTrackingBinding.binding));

                // Create subaction space
                XrActionSpaceCreateInfo actionSpaceCreateInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
                actionSpaceCreateInfo.action = ActionResources.EyeGetPoseAction;
                actionSpaceCreateInfo.poseInActionSpace = IDENTITY_TRANSFORM;
                XrCheck(xrCreateActionSpace(session, &actionSpaceCreateInfo, &ActionResources.EyePoseActionSpace));
            }

            for (size_t idx = 0; idx < ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size(); ++idx)
            {
                // Create default action and suggested bindings for select
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.DEFAULT_GET_SELECT_CLICK_ACTION_NAME,
                    ActionResources.DEFAULT_GET_SELECT_CLICK_ACTION_LOCALIZED_NAME,
                    ActionResources.DEFAULT_GET_SELECT_CLICK_PATH_SUFFIX,
                    &ActionResources.DefaultGetSelectValueAction[idx],
                    defaultBindings,
                    instance,
                    idx);

                // Create controller get trigger value action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.CONTROLLER_GET_SQUEEZE_CLICK_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_SQUEEZE_CLICK_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_SQUEEZE_CLICK_PATH_SUFFIX,
                    &ActionResources.ControllerGetSqueezeClickAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                // Create controller get trigger value action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_FLOAT_INPUT, 
                    ActionResources.CONTROLLER_GET_TRIGGER_VALUE_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_TRIGGER_VALUE_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_TRIGGER_VALUE_PATH_SUFFIX,
                    &ActionResources.ControllerGetTriggerValueAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                // Create controller get trackpad axes action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_VECTOR2F_INPUT, 
                    ActionResources.CONTROLLER_GET_TRACKPAD_AXES_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_TRACKPAD_AXES_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_TRACKPAD_AXES_PATH_SUFFIX,
                    &ActionResources.ControllerGetTrackpadAxesAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                // Create controller get trackpad click action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.CONTROLLER_GET_TRACKPAD_CLICK_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_TRACKPAD_CLICK_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_TRACKPAD_CLICK_PATH_SUFFIX,
                    &ActionResources.ControllerGetTrackpadClickAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                // Create controller get trackpad touch action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_TRACKPAD_TOUCH_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_TRACKPAD_TOUCH_PATH_SUFFIX,
                    &ActionResources.ControllerGetTrackpadTouchAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                // Create controller get thumbstick axes action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_VECTOR2F_INPUT, 
                    ActionResources.CONTROLLER_GET_THUMBSTICK_AXES_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_THUMBSTICK_AXES_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_THUMBSTICK_AXES_PATH_SUFFIX,
                    &ActionResources.ControllerGetThumbstickAxesAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                // Create controller get thumbstick click action and suggested bindings
                CreateControllerActionAndBinding(
                    XR_ACTION_TYPE_BOOLEAN_INPUT, 
                    ActionResources.CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_NAME,
                    ActionResources.CONTROLLER_GET_THUMBSTICK_CLICK_ACTION_LOCALIZED_NAME,
                    ActionResources.CONTROLLER_GET_THUMBSTICK_CLICK_PATH_SUFFIX,
                    &ActionResources.ControllerGetThumbstickClickAction[idx],
                    microsoftControllerBindings,
                    instance,
                    idx);

                if (extensions.HandInteractionSupported)
                {
                    // Create action and suggested bindings specific to hands
                    CreateControllerActionAndBinding(
                        XR_ACTION_TYPE_BOOLEAN_INPUT, 
                        ActionResources.HAND_GET_SELECT_ACTION_NAME,
                        ActionResources.HAND_GET_SELECT_ACTION_LOCALIZED_NAME,
                        ActionResources.HAND_GET_SELECT_PATH_SUFFIX,
                        &ActionResources.HandGetSelectAction[idx],
                        microsoftHandBindings,
                        instance,
                        idx);

                    CreateControllerActionAndBinding(
                        XR_ACTION_TYPE_BOOLEAN_INPUT, 
                        ActionResources.HAND_GET_SQUEEZE_ACTION_NAME,
                        ActionResources.HAND_GET_SQUEEZE_ACTION_LOCALIZED_NAME,
                        ActionResources.HAND_GET_SQUEEZE_PATH_SUFFIX,
                        &ActionResources.HandGetSqueezeAction[idx],
                        microsoftHandBindings,
                        instance,
                        idx);
                }
            }

            // Provide default suggested bindings to instance
            XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
            XrCheck(xrStringToPath(instance, ActionResources.DEFAULT_XR_INTERACTION_PROFILE, &suggestedBindings.interactionProfile));
            suggestedBindings.suggestedBindings = defaultBindings.data();
            suggestedBindings.countSuggestedBindings = (uint32_t)defaultBindings.size();
            XrCheck(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));

            // Provide Microsoft controller suggested binding to instance
            XrInteractionProfileSuggestedBinding microsoftControllerSuggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
            XrCheck(xrStringToPath(instance, ActionResources.MICROSOFT_XR_INTERACTION_PROFILE, &microsoftControllerSuggestedBindings.interactionProfile));
            microsoftControllerSuggestedBindings.suggestedBindings = microsoftControllerBindings.data();
            microsoftControllerSuggestedBindings.countSuggestedBindings = (uint32_t)microsoftControllerBindings.size();
            XrCheck(xrSuggestInteractionProfileBindings(instance, &microsoftControllerSuggestedBindings));

            if (extensions.HandInteractionSupported)
            {
                // Provide Microsoft hand suggested binding to instance
                XrInteractionProfileSuggestedBinding microsoftHandSuggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
                XrCheck(xrStringToPath(instance, ActionResources.MICROSOFT_HAND_INTERACTION_PROFILE, &microsoftHandSuggestedBindings.interactionProfile));
                microsoftHandSuggestedBindings.suggestedBindings = microsoftHandBindings.data();
                microsoftHandSuggestedBindings.countSuggestedBindings = (uint32_t)microsoftHandBindings.size();
                XrCheck(xrSuggestInteractionProfileBindings(instance, &microsoftHandSuggestedBindings));
            }

            if (SupportsEyeTracking)
            {
                // Provide Microsoft hand suggested binding to instance
                XrInteractionProfileSuggestedBinding xrEyeSuggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
                XrCheck(xrStringToPath(instance, ActionResources.XR_EYE_INTERACTION_PROFILE, &xrEyeSuggestedBindings.interactionProfile));
                xrEyeSuggestedBindings.suggestedBindings = &eyeTrackingBinding;
                xrEyeSuggestedBindings.countSuggestedBindings = 1;
                XrCheck(xrSuggestInteractionProfileBindings(instance, &xrEyeSuggestedBindings));
            }

            XrSessionActionSetsAttachInfo attachInfo{ XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
            attachInfo.countActionSets = 1;
            attachInfo.actionSets = &ActionResources.ActionSet;
            XrCheck(xrAttachSessionActionSets(session, &attachInfo));
        }

        void UpdateFrame(const UpdateFrameArgs& args)
        {
            // Locate all the things.
            auto& actionResources = ActionResources;

            auto& activeInputSources = args.activeInputSources;
            auto& eyeTrackerSpace = args.eyeTrackerSpace;

            std::vector<XrActiveActionSet> activeActionSets = { { actionResources.ActionSet, XR_NULL_PATH } };
            XrActionsSyncInfo syncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
            syncInfo.countActiveActionSets = (uint32_t)activeActionSets.size();
            syncInfo.activeActionSets = activeActionSets.data();
            XrCheck(xrSyncActions(args.Session, &syncInfo));

            activeInputSources.resize(actionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size());
            for (size_t idx = 0; idx < activeInputSources.size(); ++idx)
            {
                auto& inputSource = activeInputSources[idx];

                // Reset the tracked flags
                inputSource.TrackedThisFrame = false;
                inputSource.GamepadTrackedThisFrame = false;
                inputSource.HandTrackedThisFrame = false;
                inputSource.JointsTrackedThisFrame = false;

                // Get interaction data based on input profile. It is safe to hold off on populating input
                // source data until we get an interaction profile changed event (and know what the source is)
                if (!inputSource.InteractionProfileName.empty())
                {
                    // Get grip space
                    bool gripSpaceTracked = false;
                    {
                        XrSpace space = actionResources.ControllerGripPoseSpaces[idx];
                        XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                        XrCheck(xrLocateSpace(space, args.SceneSpace, args.DisplayTime, &location));

                        constexpr XrSpaceLocationFlags RequiredFlags =
                            XR_SPACE_LOCATION_POSITION_VALID_BIT |
                            XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                            XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                            XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                        gripSpaceTracked = (location.locationFlags & RequiredFlags) == RequiredFlags;
                        if (gripSpaceTracked)
                        {
                            inputSource.Handedness = static_cast<System::Session::Frame::InputSource::HandednessEnum>(idx);
                            UpdatePoseData(inputSource.GripSpace.Pose, location.pose);
                        }
                    }

                    // Get aim space
                    bool aimSpaceTracked = false;
                    {
                        XrSpace space = actionResources.ControllerAimPoseSpaces[idx];
                        XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                        XrCheck(xrLocateSpace(space, args.SceneSpace, args.DisplayTime, &location));

                        constexpr XrSpaceLocationFlags RequiredFlags =
                            XR_SPACE_LOCATION_POSITION_VALID_BIT |
                            XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                            XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                            XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                        aimSpaceTracked = (location.locationFlags & RequiredFlags) == RequiredFlags;
                        if (aimSpaceTracked)
                        {
                            inputSource.Handedness = static_cast<System::Session::Frame::InputSource::HandednessEnum>(idx);
                            UpdatePoseData(inputSource.AimSpace.Pose, location.pose);
                        }
                    }

                    inputSource.TrackedThisFrame = aimSpaceTracked && gripSpaceTracked;

                    XrPath interactionProfilePath;
                    XrCheck(xrStringToPath(args.Instance, inputSource.InteractionProfileName.data(), &interactionProfilePath));

                    if (interactionProfilePath == actionResources.MicrosoftXRInteractionPath)
                    {
                        // Get gamepad data 
                        const auto& controllerInfo = ControllerInfo;
                        auto& gamepadObject = inputSource.GamepadObject;

                        gamepadObject.Axes.resize(xr::System::DEFAULT_CONTROLLER_AXES_COUNT);
                        gamepadObject.Buttons.resize(xr::System::DEFAULT_CONTROLLER_BUTTONS_COUNT);

                        // Update gamepad data
                        if ((TryUpdateControllerFloatAction(actionResources.ControllerGetTriggerValueAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value)) &&
                            (TryUpdateControllerBooleanAction(actionResources.ControllerGetSqueezeClickAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed)) &&
                            (TryUpdateControllerVector2fAction(actionResources.ControllerGetTrackpadAxesAction[idx], args.Session, gamepadObject.Axes[controllerInfo.TRACKPAD_X_AXIS], gamepadObject.Axes[controllerInfo.TRACKPAD_Y_AXIS])) &&
                            (TryUpdateControllerBooleanAction(actionResources.ControllerGetTrackpadClickAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Pressed)) &&
                            (TryUpdateControllerBooleanAction(actionResources.ControllerGetTrackpadTouchAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Touched)) &&
                            (TryUpdateControllerVector2fAction(actionResources.ControllerGetThumbstickAxesAction[idx], args.Session, gamepadObject.Axes[controllerInfo.THUMBSTICK_X_AXIS], gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS])) &&
                            (TryUpdateControllerBooleanAction(actionResources.ControllerGetThumbstickClickAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Pressed)))
                        {
                            // map the openxr values to populate other states of a button and axes that webxr expects
                            gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value == 1);
                            gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value > 0);
                            gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed);
                            gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.SQUEEZE_BUTTON].Pressed);
                            gamepadObject.Axes[controllerInfo.TRACKPAD_Y_AXIS] = -(gamepadObject.Axes[controllerInfo.TRACKPAD_Y_AXIS]);
                            gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.TRACKPAD_BUTTON].Pressed);
                            gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS] = -(gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS]);
                            gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Pressed);
                            gamepadObject.Buttons[controllerInfo.THUMBSTICK_BUTTON].Touched = (gamepadObject.Axes[controllerInfo.THUMBSTICK_X_AXIS] != 0 || gamepadObject.Axes[controllerInfo.THUMBSTICK_Y_AXIS] != 0);

                            // Only signal that gamepad data is available if the actions were available
                            inputSource.GamepadTrackedThisFrame = true;
                        }
                    }
                    else if (interactionProfilePath == actionResources.MicrosoftHandInteractionPath)
                    {
                        // Get hand interaction data
                        if (args.Extensions.HandInteractionSupported)
                        {
                            const auto& controllerInfo = ControllerInfo;
                            auto& gamepadObject = inputSource.GamepadObject;

                            // Hands use a controller-specific 5th button, but have no axes
                            gamepadObject.Axes.resize(0);
                            gamepadObject.Buttons.resize(xr::System::DEFAULT_CONTROLLER_BUTTONS_COUNT + 1);

                            // Get interaction data
                            if ((TryUpdateControllerBooleanAction(actionResources.HandGetSelectAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed)) &&
                                (TryUpdateControllerBooleanAction(actionResources.HandGetSqueezeAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.CUSTOM_HARDWARE_BUTTON].Pressed)))
                            {
                                gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed);
                                gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed);
                                gamepadObject.Buttons[controllerInfo.CUSTOM_HARDWARE_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.CUSTOM_HARDWARE_BUTTON].Pressed);
                                gamepadObject.Buttons[controllerInfo.CUSTOM_HARDWARE_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.CUSTOM_HARDWARE_BUTTON].Pressed);

                                inputSource.HandTrackedThisFrame = true;
                            }
                        }

                        // Get hand joint data
                        if (HandData.HandTrackersInitialized)
                        {

                            XrHandJointsLocateInfoEXT jointLocateInfo{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
                            jointLocateInfo.baseSpace = args.SceneSpace;
                            jointLocateInfo.time = args.DisplayTime;

                            auto handInfo = HandData.HandsInfo[idx];
                            XrCheck(args.Extensions.xrLocateHandJointsEXT(handInfo.HandTracker, &jointLocateInfo, &handInfo.Locations));
                
                            inputSource.JointsTrackedThisFrame = handInfo.Locations.isActive;
                    
                            // Set up the handJoints vector, skipping the palm joint as webXR doesn't support it
                            uint32_t JointCountWithoutPalm = handInfo.Locations.jointCount - handInfo.UNUSED_HAND_JOINT_OFFSET;

                            // xrLocateHandJointsEXT will always return the full joint set (or an error), so jointCount should never change
                            // We have to wait until here to initialize the vector though, as we don't "know" the number of joints until an input report comes in
                            if (inputSource.HandJoints.size() != JointCountWithoutPalm)
                            {
                                inputSource.HandJoints = std::vector<System::Session::Frame::JointSpace>(JointCountWithoutPalm);
                            }
                
                            // Set the joints to tracked, and populate the fields. Otherwise, set joints to untracked
                            if (inputSource.JointsTrackedThisFrame)
                            {
                                constexpr XrSpaceLocationFlags RequiredFlags =
                                    XR_SPACE_LOCATION_POSITION_VALID_BIT |
                                    XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                                    XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                                    XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                                for (uint32_t i = 0; i < JointCountWithoutPalm; i++)
                                {
                                    auto joint = handInfo.JointLocations[i + handInfo.UNUSED_HAND_JOINT_OFFSET];

                                    inputSource.HandJoints[i].PoseRadius = joint.radius;
                                    inputSource.HandJoints[i].PoseTracked = (joint.locationFlags & RequiredFlags) == RequiredFlags;
                                    UpdatePoseData(inputSource.HandJoints[i].Pose, joint.pose);
                                }
                            }
                            else
                            {
                                for (auto& joint : inputSource.HandJoints)
                                {
                                    joint.PoseTracked = false;
                                }
                            }
                        }
                    }
                    else if (interactionProfilePath == actionResources.DefaultXRInteractionPath)
                    {
                        const auto& controllerInfo = ControllerInfo;
                        auto& gamepadObject = inputSource.GamepadObject;

                        // Set up the default number of axes/buttons, despite not populating them
                        gamepadObject.Axes.resize(xr::System::DEFAULT_CONTROLLER_AXES_COUNT);
                        gamepadObject.Buttons.resize(xr::System::DEFAULT_CONTROLLER_BUTTONS_COUNT);

                        // Get interaction data for select
                        if ((TryUpdateControllerBooleanAction(actionResources.DefaultGetSelectValueAction[idx], args.Session, gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed)))
                        {
                            gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Value = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed);
                            gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Touched = (gamepadObject.Buttons[controllerInfo.TRIGGER_BUTTON].Pressed);
                        }
                    }
                }
            }

            // Get eye space
            bool hasValidEyeData = false;
            if (SupportsEyeTracking)
            {
                XrActionStatePose actionStatePose{XR_TYPE_ACTION_STATE_POSE};
                XrActionStateGetInfo getActionStateInfo{XR_TYPE_ACTION_STATE_GET_INFO};
                getActionStateInfo.action = actionResources.EyeGetPoseAction;
                XrCheck(xrGetActionStatePose(args.Session, &getActionStateInfo, &actionStatePose));

                // Check if the user has accepted the "allow eye tracking" prompt
                // For VS solutions, this prompt appears on apps that have Gaze Input checked in the manifest
                if (actionStatePose.isActive)
                {
                    XrSpace space = actionResources.EyePoseActionSpace;
                    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
                    XrCheck(xrLocateSpace(space, args.SceneSpace, args.DisplayTime, &location));

                    constexpr XrSpaceLocationFlags RequiredFlags =
                        XR_SPACE_LOCATION_POSITION_VALID_BIT |
                        XR_SPACE_LOCATION_ORIENTATION_VALID_BIT |
                        XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                        XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;

                    if ((location.locationFlags & RequiredFlags) == RequiredFlags)
                    {
                        if (!eyeTrackerSpace.has_value())
                        {
                            eyeTrackerSpace.emplace();
                        }
                        UpdatePoseData(eyeTrackerSpace.value().Pose, location.pose);
                        hasValidEyeData = true;
                    }
                }
            }

            if (!hasValidEyeData)
            {
                eyeTrackerSpace.reset();
            }
        }

        void RefreshInputSources(
            const XrInstance instance,
            const XrSession session,
            std::vector<System::Session::Frame::InputSource>& activeInputSources) 
        {
            // Refresh all input sources, as babylon.JS performs different setup depending on which interaction profile is used
            activeInputSources.clear();
            activeInputSources.resize(ActionResources.CONTROLLER_SUBACTION_PATH_PREFIXES.size());

            for (size_t idx = 0; idx < activeInputSources.size(); ++idx)
            {
                XrInteractionProfileState state{XR_TYPE_INTERACTION_PROFILE_STATE};
                auto& inputSource = activeInputSources[idx];

                XrCheck(xrGetCurrentInteractionProfile(session, ActionResources.ControllerSubactionPaths[idx], &state));

                if (state.interactionProfile == XR_NULL_PATH)
                {
                    inputSource.InteractionProfileName = "";
                    continue;
                }

                uint32_t count;
                XrCheck(xrPathToString(instance, state.interactionProfile, 0, &count, nullptr));
                inputSource.InteractionProfileName.resize(count);
                XrCheck(xrPathToString(instance, state.interactionProfile, count, &count, inputSource.InteractionProfileName.data()));
            }
        }

        ~Impl() 
        {
            m_destroyHandTrackers();
        }
    };

    XrInput::XrInput()
        : m_impl{std::make_unique<Impl>()}
    { }

    XrInput::~XrInput()
    {
        m_impl.reset();
    }

    void XrInput::Initialize(const InitOptions& options) const
    {
        m_impl->Initialize(options);
    }

    void XrInput::UpdateFrame(const UpdateFrameArgs& args) const
    {
        m_impl->UpdateFrame(args);
    }

    void XrInput::RefreshInputSources(
        const XrInstance instance, 
        const XrSession session, 
        std::vector<System::Session::Frame::InputSource>& activeInputSources) const
    {
        m_impl->RefreshInputSources(instance, session, activeInputSources);
    }
}