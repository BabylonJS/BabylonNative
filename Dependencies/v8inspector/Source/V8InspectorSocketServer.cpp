// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See LICENSE_NODE for Node.js' project license details
#include "V8InspectorSocketServer.h"
#include "V8InspectorTCP.h"
#include "V8InspectorUtils.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>

namespace Babylon
{

    namespace
    {

        // We don't support protocol http end point.
        static const uint8_t PROTOCOL_JSON[] = {0};
        //    #include "v8_inspector_protocol_json.h"  // NOLINT(build/include_order)
        //};

        void Escape(std::string* string)
        {
            for (char& c : *string)
            {
                c = (c == '\"' || c == '\\') ? '_' : c;
            }
        }

        std::string FormatWsAddress(const std::string& /*host*/, int /*port*/,
            const std::string& /*target_id*/,
            bool /*include_protocol*/)
        {
            // return FormatAddress(FormatHostPort(host, port), target_id, include_protocol);
            return "formatted address!!";
        }

        std::string FormatHostPort(const std::string& host, int port)
        {
            // Host is valid (socket was bound) so colon means it's a v6 IP address
            bool v6 = host.find(':') != std::string::npos;
            std::ostringstream url;
            if (v6)
            {
                url << '[';
            }
            url << host;
            if (v6)
            {
                url << ']';
            }
            url << ':' << port;
            return url.str();
        }

        std::string FormatAddress(const std::string& host,
            const std::string& target_id,
            bool include_protocol)
        {
            std::ostringstream url;
            if (include_protocol)
                url << "ws://";
            url << host << '/' << target_id;
            return url.str();
        }

        std::string MapToString(const std::map<std::string, std::string>& object)
        {
            bool first = true;
            std::ostringstream json;
            json << "{\n";
            for (const auto& name_value : object)
            {
                if (!first)
                    json << ",\n";
                first = false;
                json << "  \"" << name_value.first << "\": \"";
                json << name_value.second << "\"";
            }
            json << "\n} ";
            return json.str();
        }

        std::string MapsToString(
            const std::vector<std::map<std::string, std::string>>& array)
        {
            bool first = true;
            std::ostringstream json;
            json << "[ ";
            for (const auto& object : array)
            {
                if (!first)
                    json << ", ";
                first = false;
                json << MapToString(object);
            }
            json << "]\n\n";
            return json.str();
        }

        const char* MatchPathSegment(const char* path, const char* expected)
        {
            size_t len = strlen(expected);
            if (utils::StringEqualNoCaseN(path, expected, len))
            {
                if (path[len] == '/')
                    return path + len + 1;
                if (path[len] == '\0')
                    return path + len;
            }
            return nullptr;
        }

        void PrintDebuggerReadyMessage(const std::string& host,
            int port,
            const std::vector<std::string>& ids,
            FILE* out)
        {
            if (out == nullptr)
            {
                return;
            }
            for (const std::string& id : ids)
            {
                fprintf(out, "Debugger listening on %s\n",
                    FormatWsAddress(host, port, id, true).c_str());
            }
            fprintf(out, "For help, see: %s\n",
                "https://nodejs.org/en/docs/inspector");
            fflush(out);
        }

        void SendHttpResponse(InspectorSocket* socket, const std::string& response)
        {
            const char HEADERS[] = "HTTP/1.0 200 OK\r\n"
                                   "Content-Type: application/json; charset=UTF-8\r\n"
                                   "Cache-Control: no-cache\r\n"
                                   "Content-Length: %zu\r\n"
                                   "\r\n";
            char header[sizeof(HEADERS) + 20];
            int header_len = snprintf(header, sizeof(header), HEADERS, response.size());
            socket->Write(header, header_len);
            socket->Write(response.data(), response.size());
        }

        void SendVersionResponse(InspectorSocket* socket)
        {
            std::map<std::string, std::string> response;
            response["Browser"] = "node.js v12.0.0";
            response["Protocol-Version"] = "1.1";
            SendHttpResponse(socket, MapToString(response));
        }

        void SendProtocolJson(InspectorSocket* socket)
        {
            SendHttpResponse(socket, "TBD");
        }
    } // namespace

    class SocketSession
    {
    public:
        SocketSession(InspectorSocketServer* server, int id, int server_port);
        void Close()
        {
            ws_socket_.reset();
        }
        void Send(const std::string& message);
        void Own(std::unique_ptr<InspectorSocket> ws_socket)
        {
            ws_socket_ = std::move(ws_socket);
        }
        int id() const { return id_; }
        int server_port()
        {
            return server_port_;
        }
        InspectorSocket* ws_socket()
        {
            return ws_socket_.get();
        }
        void Accept(const std::string& ws_key)
        {
            ws_socket_->AcceptUpgrade(ws_key);
        }
        void Decline()
        {
            ws_socket_->CancelHandshake();
        }

        class Delegate : public InspectorSocket::Delegate
        {
        public:
            Delegate(InspectorSocketServer* server, int session_id)
                : server_(server)
                , session_id_(session_id)
            {
            }
            ~Delegate()
            {
                server_->SessionTerminated(session_id_);
            }
            void OnHttpGet(const std::string& host, const std::string& path) override;
            void OnSocketUpgrade(const std::string& host, const std::string& path,
                const std::string& ws_key) override;
            void OnWsFrame(const std::vector<char>& data) override;

        private:
            SocketSession* Session()
            {
                return server_->Session(session_id_);
            }

            InspectorSocketServer* server_;
            int session_id_;
        };

    private:
        const int id_;
        std::unique_ptr<InspectorSocket> ws_socket_;
        const int server_port_;
    };

    InspectorSocketServer::InspectorSocketServer(
        std::unique_ptr<InspectorAgentDelegate>&& delegate, unsigned short port, FILE* out)
        : delegate_(std::move(delegate))
        , port_(port)
        , next_session_id_(0)
        , out_(out)
    {
        state_ = ServerState::kNew;
    }

    InspectorSocketServer::~InspectorSocketServer() = default;

    SocketSession* InspectorSocketServer::Session(int session_id)
    {
        auto it = connected_sessions_.find(session_id);
        return it == connected_sessions_.end() ? nullptr : it->second.second.get();
    }

    void InspectorSocketServer::SessionStarted(int session_id,
        const std::string& id,
        const std::string& ws_key)
    {
        SocketSession* session = Session(session_id);

        //TODODO
        //if (!TargetExists(id)) {
        //  session->Decline();
        //  return;
        // }

        //TODODO
        // if (connected_session_) std::abort();
        //connected_session_.reset(session);
        connected_sessions_[session_id].first = id;
        session->Accept(ws_key);
        delegate_->StartSession(session_id, id);
    }

    void InspectorSocketServer::SessionTerminated(int session_id)
    {
        if (Session(session_id) == nullptr)
        {
            return;
        }
        bool was_attached = connected_sessions_[session_id].first != "";
        // bool was_attached = connected_session_ != nullptr;
        if (was_attached)
        {
            delegate_->EndSession(session_id);
        }
        connected_sessions_.erase(session_id);
        if (connected_sessions_.empty())
        {
            if (was_attached && state_ == ServerState::kRunning)
            {
            }
            if (state_ == ServerState::kStopped)
            {
                delegate_.reset();
            }
        }
    }

    bool InspectorSocketServer::HandleGetRequest(int session_id,
        const std::string& host,
        const std::string& path)
    {
        SocketSession* session = Session(session_id);
        InspectorSocket* socket = session->ws_socket();
        const char* command = MatchPathSegment(path.c_str(), "/json");
        if (command == nullptr)
            return false;

        if (MatchPathSegment(command, "list") || command[0] == '\0')
        {
            SendListResponse(socket, host, session);
            return true;
        }
        else if (MatchPathSegment(command, "protocol"))
        {
            SendProtocolJson(socket);
            return true;
        }
        else if (MatchPathSegment(command, "version"))
        {
            SendVersionResponse(socket);
            return true;
        }
        return false;
    }

    void InspectorSocketServer::SendListResponse(InspectorSocket* socket,
        const std::string& host,
        SocketSession* session)
    {
        std::vector<std::map<std::string, std::string>> response;
        for (const std::string& id : delegate_->GetTargetIds())
        {
            response.push_back(std::map<std::string, std::string>());
            std::map<std::string, std::string>& target_map = response.back();
            target_map["description"] = "node.js instance";
            target_map["faviconUrl"] = "https://nodejs.org/static/favicon.ico";
            target_map["id"] = id;
            target_map["title"] = delegate_->GetTargetTitle(id);
            Escape(&target_map["title"]);
            target_map["type"] = "node";
            // This attribute value is a "best effort" URL that is passed as a JSON
            // string. It is not guaranteed to resolve to a valid resource.
            target_map["url"] = delegate_->GetTargetUrl(id);
            Escape(&target_map["url"]);

            std::string detected_host = host;
            if (detected_host.empty())
            {
                detected_host = FormatHostPort(socket->GetHost(),
                    session->server_port());
            }
            std::string formatted_address = FormatAddress(detected_host, id, false);
            target_map["devtoolsFrontendUrl"] = GetFrontendURL(false,
                formatted_address);
            // The compat URL is for Chrome browsers older than 66.0.3345.0
            target_map["devtoolsFrontendUrlCompat"] = GetFrontendURL(true,
                formatted_address);
            target_map["webSocketDebuggerUrl"] = FormatAddress(detected_host, id, true);
        }
        SendHttpResponse(socket, MapsToString(response));
    }

    std::string InspectorSocketServer::GetFrontendURL(bool is_compat,
        const std::string& formatted_address)
    {
        std::ostringstream frontend_url;
        frontend_url << "chrome-devtools://devtools/bundled/";
        frontend_url << (is_compat ? "inspector" : "js_app");
        frontend_url << ".html?experiments=true&v8only=true&ws=";
        frontend_url << formatted_address;
        return frontend_url.str();
    }

    /*static */ void InspectorSocketServer::SocketConnectedCallback(std::shared_ptr<tcp_connection> connection, void* callbackData_)
    {
        InspectorSocketServer* server = reinterpret_cast<InspectorSocketServer*>(callbackData_);
        server->Accept(connection, server->port_);
    }

    /*static */ void InspectorSocketServer::SocketClosedCallback(void* callbackData_)
    {
        InspectorSocketServer* server = reinterpret_cast<InspectorSocketServer*>(callbackData_);
        server->Stop();
    }

    bool InspectorSocketServer::Start()
    {
        tcp_server_ = std::make_shared<tcp_server>(port_, InspectorSocketServer::SocketConnectedCallback, this);
        state_ = ServerState::kRunning;
        tcp_server_->run();
        return true;
    }

    void InspectorSocketServer::Stop()
    {
        if (state_ == ServerState::kStopped)
            return;
        CHECK_EQ(state_, ServerState::kRunning);

        state_ = ServerState::kStopped;

        tcp_server_->stop();

        if (state_ == ServerState::kStopped)
        {
            delegate_.reset();
        }
    }

    void InspectorSocketServer::TerminateConnections()
    {
        for (const auto& key_value : connected_sessions_)
            key_value.second.second->Close();
    }

    bool InspectorSocketServer::TargetExists(const std::string& id)
    {
        const std::vector<std::string>& target_ids = delegate_->GetTargetIds();
        const auto& found = std::find(target_ids.begin(), target_ids.end(), id);
        return found != target_ids.end();
    }

    int InspectorSocketServer::Port() const
    {
        // return socket_->port();
        return port_;
    }

    void InspectorSocketServer::Accept(std::shared_ptr<tcp_connection> connection, int server_port)
    {
        std::unique_ptr<SocketSession> session(
            new SocketSession(this, next_session_id_++, server_port));

        auto delegate = std::make_unique<SocketSession::Delegate>(this, session->id());

        std::unique_ptr<InspectorSocket> inspector =
            InspectorSocket::Accept(connection, std::move(delegate));
        if (inspector)
        {
            session->Own(std::move(inspector));
            connected_sessions_[session->id()].second = std::move(session);
        }
    }

    void InspectorSocketServer::Send(int session_id, const std::string& message)
    {
        SocketSession* session = Session(session_id);
        if (session != nullptr)
        {
            session->Send(message);
        }
    }

    // InspectorSession tracking
    SocketSession::SocketSession(InspectorSocketServer* /*server*/, int id,
        int server_port)
        : id_(id)
        , server_port_(server_port)
    {
    }

    void SocketSession::Send(const std::string& message)
    {
        ws_socket_->Write(message.data(), message.length());
    }

    void SocketSession::Delegate::OnHttpGet(const std::string& host,
        const std::string& path)
    {
        if (!server_->HandleGetRequest(session_id_, host, path))
            Session()->ws_socket()->CancelHandshake();
    }

    void SocketSession::Delegate::OnSocketUpgrade(const std::string& /*host*/,
        const std::string& path,
        const std::string& ws_key)
    {
        std::string id = path.empty() ? path : path.substr(1);
        server_->SessionStarted(session_id_, id, ws_key);
    }

    void SocketSession::Delegate::OnWsFrame(const std::vector<char>& data)
    {
        server_->MessageReceived(session_id_,
            std::string(data.data(), data.size()));
    }

} // namespace inspector