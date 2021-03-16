// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See LICENSE_NODE for Node.js' project license details
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "V8InspectorTCP.h"

namespace Babylon {

class ProtocolHandler;

// HTTP Wrapper around a uv_tcp_t
class InspectorSocket {
 public:
  InspectorSocket() = default;
  ~InspectorSocket();

  class Delegate {
   public:
    virtual void OnHttpGet(const std::string& host,
                           const std::string& path) = 0;
    virtual void OnSocketUpgrade(const std::string& host,
                                 const std::string& path,
                                 const std::string& accept_key) = 0;
    virtual void OnWsFrame(const std::vector<char>& frame) = 0;
    virtual ~Delegate() {}
  };

  static std::unique_ptr<InspectorSocket> Accept(std::shared_ptr<tcp_connection> connection, std::unique_ptr<Delegate>&& delegate);

  void AcceptUpgrade(const std::string& accept_key);
  void CancelHandshake();
  void Write(const char* data, size_t len);
  void SwitchProtocol(std::unique_ptr<ProtocolHandler>&& handler);
  std::string GetHost();

 private:
  std::unique_ptr<ProtocolHandler> protocol_handler_;

  InspectorSocket(const InspectorSocket&) = delete;
  InspectorSocket& operator=(const InspectorSocket&) = delete;
};

}  // namespace inspector