// Shared pimpl code (not an actual header)
namespace Babylon::Plugins
{
    CameraTrack::CameraTrack(std::unique_ptr<CameraTrack::Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    CameraTrack::~CameraTrack() = default;
    CameraTrack::CameraTrack(CameraTrack&&) noexcept = default;
    CameraTrack& CameraTrack::operator=(CameraTrack&&) noexcept = default;

    int32_t CameraTrack::Width() const
    {
        return m_impl->width;
    }

    int32_t CameraTrack::Height() const
    {
        return m_impl->height;
    }

    CameraDevice::CameraDevice(std::unique_ptr<CameraDevice::Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    CameraDevice::~CameraDevice() = default;
    CameraDevice::CameraDevice(CameraDevice&&) noexcept = default;
    CameraDevice& CameraDevice::operator=(CameraDevice&&) noexcept = default;

    const std::vector<CameraTrack>& CameraDevice::SupportedResolutions() const
    {
        return m_impl->supportedResolutions;
    }

    const std::vector<std::unique_ptr<CameraCapability>>& CameraDevice::Capabilities() const
    {
        return m_impl->capabilities;
    }
}
