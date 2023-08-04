#include "WebSocket_Base.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <arcana/threading/task_conversions.h>

namespace UrlLib
{
    using namespace winrt;

    class WebSocket::Impl : public ImplBase
    {
    public:
        Impl(const std::string& url, std::function<void()> onOpen, std::function<void(int, const std::string&)> onClose, std::function<void(const std::string&)> onMessage, std::function<void(const std::string&)> onError)
        : ImplBase{url, onOpen, onClose, onMessage, onError}
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
        {
        }

        ~Impl()
        {
            m_cancellationSource->cancel();
        }
        
        void Send(std::string message)
        {
            try
            {
                Windows::Storage::Streams::DataWriter dataWriter{m_webSocket.OutputStream()};
                dataWriter.WriteString(winrt::to_hstring(message));

                arcana::create_task<std::exception_ptr>(dataWriter.StoreAsync())
                    .then(arcana::inline_scheduler, *m_cancellationSource, [this, dataWriter](int)
                    {
                        dataWriter.DetachStream();
                    })
                    .then(arcana::inline_scheduler, *m_cancellationSource, [this, cancellationSource{m_cancellationSource}](const arcana::expected<void, std::exception_ptr>& result)
                    {
                        if (cancellationSource->cancelled())
                        {
                            return;
                        }
                        if (result.has_error())
                        {
                            try
                            {
                                std::rethrow_exception(result.error());
                            }
                            catch (const std::exception& ex)
                            {
                                m_onError(ex.what());
                            }
                        }
                    });
            }
            catch (winrt::hresult_error const& ex)
            {
                std::string errorMessage = winrt::to_string(ex.message());
                m_onError(errorMessage);
            }
        }

        void Open()
        {
            m_webSocket.Control().MessageType(Windows::Networking::Sockets::SocketMessageType::Utf8);
            m_messageReceivedEventToken = m_webSocket.MessageReceived({this, &WebSocket::Impl::OnMessageReceived});
            m_closedEventToken = m_webSocket.Closed({this, &WebSocket::Impl::OnWebSocketClosed});

            try
            {
                hstring hURL = to_hstring(m_url);

                arcana::create_task<std::exception_ptr>(m_webSocket.ConnectAsync(Windows::Foundation::Uri{hURL}))
                    .then(arcana::inline_scheduler, *m_cancellationSource, [this]()
                    {
                        m_onOpen();
                    })
                    .then(arcana::inline_scheduler, *m_cancellationSource, [this, cancellationSource{m_cancellationSource}](const arcana::expected<void, std::exception_ptr>& result)
                    {
                        if (cancellationSource->cancelled())
                        {
                            return;
                        }
                        if (result.has_error())
                        {
                            try
                            {
                                std::rethrow_exception(result.error());
                            }
                            catch (const std::exception& ex)
                            {
                                m_onError(ex.what());
                            }
                        }
                    });
            }
            catch (hresult_error const& ex)
            {
                std::string errorMessage = winrt::to_string(ex.message());
                m_onError(errorMessage);
            }
        }

        void Close()
        {
            m_webSocket.Close();
        }

    private:
        void OnWebSocketClosed(Windows::Networking::Sockets::IWebSocket const& /* sender */, Windows::Networking::Sockets::WebSocketClosedEventArgs const& args)
        {
            if (args.Code() != 1000)
            {
                m_onError(winrt::to_string(args.Reason()));
            }
            m_onClose(args.Code(), winrt::to_string(args.Reason()));
        }

        void OnMessageReceived(Windows::Networking::Sockets::MessageWebSocket const& /* sender */, Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs const& args)
        {
            try
            {
                Windows::Storage::Streams::DataReader dataReader{args.GetDataReader()};
                dataReader.UnicodeEncoding(Windows::Storage::Streams::UnicodeEncoding::Utf8);
                std::string message = winrt::to_string(dataReader.ReadString(dataReader.UnconsumedBufferLength()));

                m_onMessage(message);
            }
            catch (winrt::hresult_error const& ex)
            {
                std::string errorMessage = winrt::to_string(ex.message());
                m_onError(errorMessage);
            }
        }
       
        Windows::Networking::Sockets::MessageWebSocket m_webSocket;
        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        event_token m_messageReceivedEventToken;
        event_token m_closedEventToken;
    };
}

#include "WebSocket_Shared.h"
