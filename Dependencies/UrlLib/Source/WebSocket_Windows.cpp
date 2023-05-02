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
        Impl(std::string url, std::function<void()> onOpen, std::function<void()> onClose, std::function<void(std::string)> onMessage, std::function<void()> onError)
        : ImplBase{url, onOpen, onClose, onMessage, onError}
        {
        }

        ~Impl()
        {
            m_cancellationSource.cancel();
        }
        
        void Send(std::string message)
        {
            try
            {
                Windows::Storage::Streams::DataWriter dataWriter{m_webSocket.OutputStream()};
                dataWriter.WriteString(winrt::to_hstring(message));

                arcana::create_task<std::exception_ptr>(dataWriter.StoreAsync())
                    .then(arcana::inline_scheduler, m_cancellationSource, [this, dataWriter](int)
                    {
                        dataWriter.DetachStream();
                    })
                    .then(arcana::inline_scheduler, m_cancellationSource, [this](const arcana::expected<void, std::exception_ptr>& result)
                    {
                        if (result.has_error())
                        {
                            m_onError();
                        }
                    });
            }
            catch (winrt::hresult_error const&)
            {
                m_onError();
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
                    .then(arcana::inline_scheduler, m_cancellationSource, [this]()
                    {
                        m_onOpen();
                    })
                    .then(arcana::inline_scheduler, m_cancellationSource, [this](const arcana::expected<void, std::exception_ptr>& result)
                    {
                        if (result.has_error())
                        {
                            m_onError();
                        }
                    });
            }
            catch (hresult_error const&)
            {
                m_onError();
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
                m_onError();
            }
            m_onClose();
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
            catch (winrt::hresult_error const& )
            {
                m_onError();
            }
        }
       
        Windows::Networking::Sockets::MessageWebSocket m_webSocket;
        arcana::cancellation_source m_cancellationSource{};

        event_token m_messageReceivedEventToken;
        event_token m_closedEventToken;
    };
}

#include "WebSocket_Shared.h"
