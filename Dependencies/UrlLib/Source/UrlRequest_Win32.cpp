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
                    try
                    {
                        std::ifstream file{path};
                        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                        std::ostringstream ss;
                        ss << file.rdbuf();
                        m_responseString = ss.str();
                        m_statusCode = UrlStatusCode::Ok;
                    }
                    catch (std::ios_base::failure)
                    {
                        std::ostringstream msg;
                        msg << "Failed to load file '" << winrt::to_string(path) << "'";
                        throw std::runtime_error{msg.str()};
                    }
                });
            }
            case UrlResponseType::Buffer:
            {
                return arcana::make_task(arcana::threadpool_scheduler, m_cancellationSource, [this, path] {
                    try
                    {
                        std::ifstream file{path, std::ios::binary | std::ios::ate};
                        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                        m_fileResponseBuffer.resize(file.tellg());
                        file.seekg(0, std::ios::beg);
                        file.read(reinterpret_cast<char*>(m_fileResponseBuffer.data()), m_fileResponseBuffer.size());
                        m_statusCode = UrlStatusCode::Ok;
                    }
                    catch (std::ios::failure)
                    {
                        std::ostringstream msg;
                        msg << "Failed to load file '" << winrt::to_string(path) << "'";
                        throw std::runtime_error{msg.str()};
                    }
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
