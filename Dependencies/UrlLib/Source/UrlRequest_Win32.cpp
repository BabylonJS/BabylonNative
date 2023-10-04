#include "UrlRequest_Windows_Shared.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace UrlLib
{
    using namespace winrt::Windows;

    namespace
    {
        winrt::hstring GetInstalledLocation()
        {
            WCHAR modulePath[4096];
            DWORD result{::GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath))};
            winrt::check_bool(result != 0 && result != std::size(modulePath));
            winrt::check_hresult(PathCchRemoveFileSpec(modulePath, ARRAYSIZE(modulePath)));
            return modulePath;
        }
    }

    arcana::task<void, std::exception_ptr> UrlRequest::Impl::SendAsync()
    {
        try
        {
            if (m_uri.SchemeName() == L"app" || m_uri.SchemeName() == L"file")
            {
                auto path = GetLocalPath(m_uri);
                if (m_uri.SchemeName() == L"app")
                {
                    path = std::wstring(GetInstalledLocation()) + L'\\' + path;
                }
                return LoadFileAsync(path);
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
        if (!m_fileResponseBuffer.empty())
        {
            return {m_fileResponseBuffer.data(), m_fileResponseBuffer.size()};
        }
        else if (m_responseBuffer)
        {
            std::byte* bytes;
            auto bufferByteAccess = m_responseBuffer.as<::Windows::Storage::Streams::IBufferByteAccess>();
            winrt::check_hresult(bufferByteAccess->Buffer(reinterpret_cast<byte**>(&bytes)));
            return {bytes, gsl::narrow_cast<std::size_t>(m_responseBuffer.Length())};
        }
        return {};
    }

    arcana::task<void, std::exception_ptr> UrlRequest::Impl::LoadFileAsync(const std::wstring& path)
    {
        switch (m_responseType)
        {
            case UrlResponseType::String:
            {
                return arcana::make_task(arcana::threadpool_scheduler, m_cancellationSource, [this, path] {
                    std::ifstream file(path);
                    if (!file.good())
                    {
                        std::stringstream msg;
                        msg << "Failed to read file " << path.c_str();
                        throw std::runtime_error{msg.str()};
                    }

                    std::stringstream ss;
                    ss << file.rdbuf();

                    m_responseString = ss.str();
                    m_statusCode = UrlStatusCode::Ok;
                });
            }
            case UrlResponseType::Buffer:
            {
                return arcana::make_task(arcana::threadpool_scheduler, m_cancellationSource, [this, path] {
                    std::ifstream file(path, std::ios::binary);
                    if (!file.good())
                    {
                        std::stringstream msg;
                        msg << "Failed to read file " << path.c_str();
                        throw std::runtime_error{msg.str()};
                    }

                    const auto fileSize = std::filesystem::file_size(path);
                    m_fileResponseBuffer.resize(fileSize);
                    file.read(reinterpret_cast<char*>(m_fileResponseBuffer.data()), fileSize);
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
