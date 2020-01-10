#include "XMLHttpRequest/XMLHttpRequest.h"

#include <robuffer.h>
#include <winrt/Windows.Storage.Streams.h>
#include <ppltasks.h>

// Included after the WinRT headers because they enable non-WinRT interfaces. If this were included before
// the WinRT headers, we'd have to explicitly include unknwn.h, or build would fail with C2338.
//#include <arcana/threading/task_conversions.h>
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Storage;

namespace Babylon
{
    class XMLHttpRequestUWP : protected XMLHttpRequest
    {
        void XMLHttpRequestUWP::SendAsync()
        {
            // clang-format off
            /*return SendAsyncImpl()
                .then(arcana::inline_scheduler, m_runtimeImpl.Cancellation(), [url = m_url, responseType = m_responseType, this](arcana::expected<void, std::exception_ptr> result)
            {
                if (result.has_error())
                {
                    winrt::Windows::Foundation::Uri uri{ winrt::to_hstring(url) };

                    std::wstring_view path{ uri.Path() };
                    if (path[0] != L'/')
                    {
                        throw std::runtime_error("Invalid file url");
                    }

                    std::wstring localPath{ path.substr(1) };
                    std::replace(localPath.begin(), localPath.end(), '/', '\\');
                    // TODO: decode escaped url characters

                    // TODO: handle errors
                    return arcana::create_task<std::exception_ptr>(winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(localPath))
                        .then(arcana::inline_scheduler, m_runtimeImpl.Cancellation(), [responseType = std::move(responseType), this](const winrt::Windows::Storage::StorageFile& file)
                    {
                        if (responseType.empty() || responseType == XMLHttpRequestTypes::ResponseType::Text)
                        {
                            return arcana::create_task<std::exception_ptr>(winrt::Windows::Storage::FileIO::ReadTextAsync(file))
                                .then(m_runtimeImpl.Dispatcher(), m_runtimeImpl.Cancellation(), [this](const winrt::hstring& text)
                            {
                                m_responseText = winrt::to_string(text);
                            });
                        }
                        else if (responseType == XMLHttpRequestTypes::ResponseType::ArrayBuffer)
                        {
                            return arcana::create_task<std::exception_ptr>(winrt::Windows::Storage::FileIO::ReadBufferAsync(file))
                                .then(m_runtimeImpl.Dispatcher(), m_runtimeImpl.Cancellation(), [this](const winrt::Windows::Storage::Streams::IBuffer& buffer)
                            {
                                std::byte* bytes;
                                auto bufferByteAccess = buffer.as<::Windows::Storage::Streams::IBufferByteAccess>();
                                winrt::check_hresult(bufferByteAccess->Buffer(reinterpret_cast<byte**>(&bytes)));

                                m_response = Napi::Persistent(Napi::ArrayBuffer::New(Env(), buffer.Length()));
                                memcpy(m_response.Value().Data(), bytes, buffer.Length());
                            });
                        }
                        else
                        {
                            throw std::logic_error("Unexpected response type.");
                        }
                    }).then(m_runtimeImpl.Dispatcher(), m_runtimeImpl.Cancellation(), [this, url = std::move(url)]
                    {
                        m_responseURL = url;
                        m_status = HTTPStatusCode::Ok;

                        SetReadyState(ReadyState::Done);
                    });
                }
                else
                {
                    return arcana::task_from_result<std::exception_ptr>();
                }
            });
            */
            // clang-format off
            winrt::Windows::Foundation::Uri uri{ winrt::to_hstring(m_url) };
            
            std::wstring_view path{ uri.Path() };
            if (path[0] != L'/')
            {
                throw std::runtime_error("Invalid file url");
            }

            std::wstring localPath{ path.substr(1) };
            std::replace(localPath.begin(), localPath.end(), '/', '\\');

            /*
            create_task(StorageFile::GetFileFromPathAsync(L"")).then([](StorageFile^ storageFileSample) -> IAsyncAction^ {
                return storageFileSample->DeleteAsync();
                }).then([](void) {
                    OutputDebugString(L"File deleted.");
                    });
                    */
                    
            // TODO: decode escaped url characters
            Platform::String^ strLocalPath = ref new Platform::String(localPath.c_str());

            create_task(StorageFile::GetFileFromPathAsync(strLocalPath)).then([url = m_url, responseType = m_responseType, this](StorageFile^ file) -> IAsyncAction^ {
                if (responseType.empty() || responseType == XMLHttpRequestTypes::ResponseType::Text)
                {
                    create_task(FileIO::ReadTextAsync(file)).then([responseType = std::move(responseType), this](Platform::String^ text) {
                            std::wstring responseWString(text->Data());
                            int bytes = ::WideCharToMultiByte(CP_UTF8, 0, responseWString.data(), -1, NULL, 0, NULL, NULL);
                            m_responseText.resize(bytes);
                            ::WideCharToMultiByte(CP_UTF8, 0, responseWString.data(), bytes, m_responseText.data(), bytes, nullptr, nullptr);;
                        });
                }
                else if (responseType == XMLHttpRequestTypes::ResponseType::ArrayBuffer)
                {
                    create_task(FileIO::ReadBufferAsync(file)).then([this](Streams::IBuffer^ buffer)
                            {
                            auto reader = ::Windows::Storage::Streams::DataReader::FromBuffer(buffer);
                            const auto length = reader->UnconsumedBufferLength;
                            if (length)
                            {
                                m_response = Napi::Persistent(Napi::ArrayBuffer::New(Env(), length));
                                reader->ReadBytes(::Platform::ArrayReference<unsigned char>((unsigned char*)m_response.Value().Data(), length));
                            }
                        });
                }
                else
                {
                    throw std::logic_error("Unexpected response type.");
                }
            });
            
        }
    };
}
