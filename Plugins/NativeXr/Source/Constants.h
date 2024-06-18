#pragma once

namespace Babylon
{
    namespace Plugins
    {
        class XRFrame;
    }

    namespace
    {
        struct XRSessionType
        {
            static constexpr auto IMMERSIVE_VR{"immersive-vr"};
            static constexpr auto IMMERSIVE_AR{"immersive-ar"};
            static constexpr auto INLINE{"inline"};
        };

        struct XRReferenceSpaceType
        {
            static constexpr auto VIEWER{"viewer"};
            // static constexpr auto LOCAL{"local"};
            // static constexpr auto LOCAL_FLOOR{"local-floor"};
            // static constexpr auto BOUNDED_FLOOR{"bounded-floor"};
            static constexpr auto UNBOUNDED{"unbounded"};
        };

        struct XRHitTestTrackableType
        {
            static constexpr auto POINT{"point"};
            static constexpr auto PLANE{"plane"};
            static constexpr auto MESH{"mesh"};
        };

        struct XRImageTrackingState
        {
            //static constexpr auto UNTRACKED{"untracked"};
            static constexpr auto TRACKED{"tracked"};
            static constexpr auto EMULATED{"emulated"};
        };

        struct XREye
        {
            static constexpr auto NONE{"none"};
            static constexpr auto LEFT{"left"};
            static constexpr auto RIGHT{"right"};

            static auto IndexToEye(size_t idx)
            {
                switch (idx)
                {
                    case 0:
                        return LEFT;
                    case 1:
                        return RIGHT;
                    case 2:
                        return NONE;
                    default:
                        throw std::runtime_error{"Unsupported index"};
                }
            }

            static uint32_t EyeToIndex(const std::string& eye)
            {
                if (eye == LEFT)
                {
                    return 0;
                }
                else if (eye == RIGHT)
                {
                    return 1;
                }
                else if (eye == NONE)
                {
                    return 2;
                }
                else
                {
                    throw std::runtime_error{"Unsupported eye"};
                }
            }
        };
    }
} // Babylon
