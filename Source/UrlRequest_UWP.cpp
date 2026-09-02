#include "UrlRequest_Windows_Shared.h"

namespace UrlLib
{
    using namespace winrt::Windows;

    namespace
    {
        winrt::hstring GetInstalledLocation()
        {
            return ApplicationModel::Package::Current().InstalledLocation().Path();
        }
    }

    arcana::task<void, std::exception_ptr> UrlRequest::Impl::SendAsync()
    {
        try
        {
            if (m_uri.SchemeName() == L"app")
            {
                return arcana::create_task<std::exception_ptr>(Storage::StorageFolder::GetFolderFromPathAsync(GetInstalledLocation()))
                    .then(arcana::inline_scheduler, m_cancellationSource, [this, m_uri{m_uri}](Storage::StorageFolder folder) {
                        return arcana::create_task<std::exception_ptr>(folder.GetFileAsync(GetLocalPath(m_uri)));
                    })
                    .then(arcana::inline_scheduler, m_cancellationSource, [this](Storage::StorageFile file) {
                        return LoadFileAsync(file);
                    });
            }
            else if (m_uri.SchemeName() == L"file")
            {
                return arcana::create_task<std::exception_ptr>(Storage::StorageFile::GetFileFromPathAsync(GetLocalPath(m_uri)))
                    .then(arcana::inline_scheduler, m_cancellationSource, [this](Storage::StorageFile file) {
                        return LoadFileAsync(file);
                    });
            }
            else
            {
                return LoadHttpAsync();
            }
        }
        catch (winrt::hresult_error)
        {
            // Catch WinRT exceptions, but retain the default status code of 0 to indicate a client side error.
            return arcana::task_from_result<std::exception_ptr>();
        }
    }

    gsl::span<const std::byte> UrlRequest::Impl::ResponseBuffer() const
    {
        if (!m_responseBuffer)
        {
            return {};
        }

        std::byte* bytes;
        auto bufferByteAccess = m_responseBuffer.as<::Windows::Storage::Streams::IBufferByteAccess>();
        winrt::check_hresult(bufferByteAccess->Buffer(reinterpret_cast<byte**>(&bytes)));
        return {bytes, gsl::narrow_cast<std::size_t>(m_responseBuffer.Length())};
    }

    arcana::task<void, std::exception_ptr> UrlRequest::Impl::LoadFileAsync(Storage::StorageFile file)
    {
        switch (m_responseType)
        {
            case UrlResponseType::String:
            {
                return arcana::create_task<std::exception_ptr>(Storage::FileIO::ReadTextAsync(file))
                    .then(arcana::inline_scheduler, m_cancellationSource, [this](winrt::hstring text) {
                        m_responseString = winrt::to_string(text);
                        m_statusCode = UrlStatusCode::Ok;
                    });
            }
            case UrlResponseType::Buffer:
            {
                return arcana::create_task<std::exception_ptr>(Storage::FileIO::ReadBufferAsync(file))
                    .then(arcana::inline_scheduler, m_cancellationSource, [this](Storage::Streams::IBuffer buffer) {
                        m_responseBuffer = std::move(buffer);
                        m_statusCode = UrlStatusCode::Ok;
                    });
            }
            default:
            {
                throw std::runtime_error{"Invalid response type"};
            }
        }
    }
}

#include "UrlRequest_Shared.h"
