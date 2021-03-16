// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See LICENSE_NODE for Node.js' project license details
#include "V8InspectorTCP.h"
#include "V8InspectorSocket.h"
#include "V8InspectorUtils.h"

#include <llhttp.h>
#include <cstddef>
#include <algorithm>
#include <cstdint>
#include <cstring>

//#include <http.h>

typedef llhttp_type_t parser_type_t;
typedef llhttp_errno_t parser_errno_t;
typedef llhttp_settings_t parser_settings_t;
typedef llhttp_t parser_t;

// The helper is for doing safe downcasts from base types to derived types.
template <typename Inner, typename Outer>
class ContainerOfHelper {
public:
  inline ContainerOfHelper(Inner Outer::*field, Inner* pointer);
  template <typename TypeName>
  inline operator TypeName*() const;
private:
  Outer* const pointer_;
};

template <typename Inner, typename Outer>
constexpr uintptr_t OffsetOf(Inner Outer::*field) {
  return reinterpret_cast<uintptr_t>(&(static_cast<Outer*>(0)->*field));
}

template <typename Inner, typename Outer>
ContainerOfHelper<Inner, Outer>::ContainerOfHelper(Inner Outer::*field,
  Inner* pointer)
  : pointer_(
    reinterpret_cast<Outer*>(
      reinterpret_cast<uintptr_t>(pointer) - OffsetOf(field))) {}

template <typename Inner, typename Outer>
template <typename TypeName>
ContainerOfHelper<Inner, Outer>::operator TypeName*() const {
  return static_cast<TypeName*>(pointer_);
}

template <typename Inner, typename Outer>
constexpr ContainerOfHelper<Inner, Outer> ContainerOf(Inner Outer::*field,
  Inner* pointer) {
  return ContainerOfHelper<Inner, Outer>(field, pointer);
}

#include <map>
#include <string.h>

#define ACCEPT_KEY_LENGTH Babylon::utils::base64_encoded_size(20)
#define BUFFER_GROWTH_CHUNK_SIZE 1024

#define DUMP_READS 0
#define DUMP_WRITES 0

namespace Babylon {

class TcpHolder : public std::enable_shared_from_this<TcpHolder> {
 public:
  void SetHandler(ProtocolHandler* handler);
  int WriteRaw(const std::vector<char>& buffer/*, uv_write_cb write_cb*/);

  void read_loop();

  std::shared_ptr<tcp_connection> connection() { return connection_; };
  InspectorSocket::Delegate* delegate();

  TcpHolder(std::shared_ptr<tcp_connection> connection, std::unique_ptr<InspectorSocket::Delegate> delegate);
  ~TcpHolder();

 private:
  static void OnDataReceivedCb(std::vector<char>&, bool iseof, void*data);

  std::shared_ptr<tcp_connection> connection_;

  const std::unique_ptr<InspectorSocket::Delegate> delegate_;
  ProtocolHandler* handler_;
  std::vector<char> buffer_;
};


class ProtocolHandler {
 public:
  ProtocolHandler(InspectorSocket* inspector, std::shared_ptr<TcpHolder>&& tcp);
  virtual ~ProtocolHandler() = default;

  virtual void AcceptUpgrade(const std::string& accept_key) = 0;
  virtual void OnData(std::vector<char>* data) = 0;
  virtual void OnEof() = 0;
  virtual void Write(const std::vector<char> data) = 0;
  virtual void CancelHandshake() = 0;

  std::string GetHost() const;

  InspectorSocket* inspector() {
    return inspector_;
  }

 protected:
  int WriteRaw(const std::vector<char>& buffer/*, uv_write_cb write_cb*/);
  InspectorSocket::Delegate* delegate();

  InspectorSocket* const inspector_;
  std::shared_ptr<TcpHolder> tcp_;
};

namespace {

#if DUMP_READS || DUMP_WRITES
static void dump_hex(const char* buf, size_t len) {
  const char* ptr = buf;
  const char* end = ptr + len;
  const char* cptr;
  char c;
  int i;

  while (ptr < end) {
    cptr = ptr;
    for (i = 0; i < 16 && ptr < end; i++) {
      printf("%2.2X  ", static_cast<unsigned char>(*(ptr++)));
    }
    for (i = 72 - (i * 4); i > 0; i--) {
      printf(" ");
    }
    for (i = 0; i < 16 && cptr < end; i++) {
      c = *(cptr++);
      printf("%c", (c > 0x19) ? c : '.');
    }
    printf("\n");
  }
  printf("\n\n");
}
#endif

static void remove_from_beginning(std::vector<char>* buffer, size_t count) {
  buffer->erase(buffer->begin(), buffer->begin() + count);
}

static const char CLOSE_FRAME[] = {'\x88', '\x00'};

enum ws_decode_result {
  FRAME_OK, FRAME_INCOMPLETE, FRAME_CLOSE, FRAME_ERROR
};

static void generate_accept_string(const std::string& client_key,
                                   char (*buffer)[ACCEPT_KEY_LENGTH]) {
  // Magic string from websockets spec.
  static const char ws_magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  std::string input(client_key + ws_magic);

  Babylon::utils::sha1_context ctx;
  init(ctx);
  update(ctx, input.data(), input.size());
  char digest[Babylon::utils::sha1_context::digest_size];
  finish(ctx, &digest[0]);
  
  Babylon::utils::base64_encode(digest, Babylon::utils::sha1_context::digest_size, *buffer, sizeof(*buffer));
}

static std::string TrimPort(const std::string& host) {
  size_t last_colon_pos = host.rfind(":");
  if (last_colon_pos == std::string::npos)
    return host;
  size_t bracket = host.rfind("]");
  if (bracket == std::string::npos || last_colon_pos > bracket)
    return host.substr(0, last_colon_pos);
  return host;
}

static bool IsIPAddress(const std::string& host) {
  if (host.length() >= 4 && host.front() == '[' && host.back() == ']')
    return true;
  int quads = 0;
  for (char c : host) {
    if (c == '.')
      quads++;
    else if (!isdigit(c))
      return false;
  }
  return quads == 3;
}

// Constants for hybi-10 frame format.

typedef unsigned char OpCode;

const OpCode kOpCodeContinuation = 0x0;
const OpCode kOpCodeText = 0x1;
const OpCode kOpCodeBinary = 0x2;
const OpCode kOpCodeClose = 0x8;
const OpCode kOpCodePing = 0x9;
const OpCode kOpCodePong = 0xA;

const unsigned char kFinalBit = 0x80;
const unsigned char kReserved1Bit = 0x40;
const unsigned char kReserved2Bit = 0x20;
const unsigned char kReserved3Bit = 0x10;
const unsigned char kOpCodeMask = 0xF;
const unsigned char kMaskBit = 0x80;
const unsigned char kPayloadLengthMask = 0x7F;

const size_t kMaxSingleBytePayloadLength = 125;
const size_t kTwoBytePayloadLengthField = 126;
const size_t kEightBytePayloadLengthField = 127;
const size_t kMaskingKeyWidthInBytes = 4;

static std::vector<char> encode_frame_hybi17(const std::vector<char>& message) {
  std::vector<char> frame;
  OpCode op_code = kOpCodeText;
  frame.push_back(kFinalBit | op_code);
  const size_t data_length = message.size();
  if (data_length <= kMaxSingleBytePayloadLength) {
    frame.push_back(static_cast<char>(data_length));
  } else if (data_length <= 0xFFFF) {
    frame.push_back(kTwoBytePayloadLengthField);
    frame.push_back((data_length & 0xFF00) >> 8);
    frame.push_back(data_length & 0xFF);
  } else {
    frame.push_back(kEightBytePayloadLengthField);
    char extended_payload_length[8];
    size_t remaining = data_length;
    // Fill the length into extended_payload_length in the network byte order.
    for (int i = 0; i < 8; ++i) {
      extended_payload_length[7 - i] = remaining & 0xFF;
      remaining >>= 8;
    }
    frame.insert(frame.end(), extended_payload_length,
                 extended_payload_length + 8);
    CHECK_EQ(0, remaining);
  }
  frame.insert(frame.end(), message.begin(), message.end());
  return frame;
}

static ws_decode_result decode_frame_hybi17(const std::vector<char>& buffer,
                                            bool client_frame,
                                            size_t* bytes_consumed,
                                            std::vector<char>* output,
                                            bool* compressed) {
  *bytes_consumed = 0;
  if (buffer.size() < 2)
    return FRAME_INCOMPLETE;

  auto it = buffer.begin();

  unsigned char first_byte = *it++;
  unsigned char second_byte = *it++;

  bool final = (first_byte & kFinalBit) != 0;
  bool reserved1 = (first_byte & kReserved1Bit) != 0;
  bool reserved2 = (first_byte & kReserved2Bit) != 0;
  bool reserved3 = (first_byte & kReserved3Bit) != 0;
  int op_code = first_byte & kOpCodeMask;
  bool masked = (second_byte & kMaskBit) != 0;
  *compressed = reserved1;
  if (!final || reserved2 || reserved3)
    return FRAME_ERROR;  // Only compression extension is supported.

  bool closed = false;
  switch (op_code) {
    case kOpCodeClose:
      closed = true;
      break;
    case kOpCodeText:
      break;
    case kOpCodeBinary:        // We don't support binary frames yet.
    case kOpCodeContinuation:  // We don't support binary frames yet.
    case kOpCodePing:          // We don't support binary frames yet.
    case kOpCodePong:          // We don't support binary frames yet.
    default:
      return FRAME_ERROR;
  }

  // In Hybi-17 spec client MUST mask its frame.
  if (client_frame && !masked) {
    return FRAME_ERROR;
  }

  uint64_t payload_length64 = second_byte & kPayloadLengthMask;
  if (payload_length64 > kMaxSingleBytePayloadLength) {
    int extended_payload_length_size;
    if (payload_length64 == kTwoBytePayloadLengthField) {
      extended_payload_length_size = 2;
    } else if (payload_length64 == kEightBytePayloadLengthField) {
      extended_payload_length_size = 8;
    } else {
      return FRAME_ERROR;
    }
    if ((buffer.end() - it) < extended_payload_length_size)
      return FRAME_INCOMPLETE;
    payload_length64 = 0;
    for (int i = 0; i < extended_payload_length_size; ++i) {
      payload_length64 <<= 8;
      payload_length64 |= static_cast<unsigned char>(*it++);
    }
  }

  static const uint64_t max_payload_length = 0x7FFFFFFFFFFFFFFFull;
  static const size_t max_length = SIZE_MAX;
  if (payload_length64 > max_payload_length ||
      payload_length64 > max_length - kMaskingKeyWidthInBytes) {
    // WebSocket frame length too large.
    return FRAME_ERROR;
  }
  size_t payload_length = static_cast<size_t>(payload_length64);

  if (buffer.size() - kMaskingKeyWidthInBytes < payload_length)
    return FRAME_INCOMPLETE;

  std::vector<char>::const_iterator masking_key = it;
  std::vector<char>::const_iterator payload = it + kMaskingKeyWidthInBytes;
  for (size_t i = 0; i < payload_length; ++i)  // Unmask the payload.
    output->insert(output->end(),
                   payload[i] ^ masking_key[i % kMaskingKeyWidthInBytes]);

  size_t pos = it + kMaskingKeyWidthInBytes + payload_length - buffer.begin();
  *bytes_consumed = pos;
  return closed ? FRAME_CLOSE : FRAME_OK;
}

// WS protocol
class WsHandler : public ProtocolHandler {
 public:
  WsHandler(InspectorSocket* inspector, std::shared_ptr<TcpHolder> tcp)
            : ProtocolHandler(inspector, std::move(tcp)),
              OnCloseSent(&WsHandler::WaitForCloseReply),
              OnCloseRecieved(&WsHandler::CloseFrameReceived) { }

  ~WsHandler() {
    if (tcp_) {
      SendClose();
    }
  }

  void AcceptUpgrade(const std::string& /*accept_key*/) override { }
  void CancelHandshake() override {}

  void OnEof() override {
    if (tcp_)
    {
      tcp_.reset();
    }
  }

  void OnData(std::vector<char>* data) override {
    // 1. Parse.
    size_t processed = 0;
    do {
      processed = ParseWsFrames(*data);
      // 2. Fix the data size & length
      if (processed > 0) {
        remove_from_beginning(data, processed);
      }
    } while (processed > 0 && !data->empty());
  }

  void Write(const std::vector<char> data) override {
    std::vector<char> output = encode_frame_hybi17(data);
    WriteRaw(output/*, WriteRequest::Cleanup*/);
  }

 private:
  using Callback = void (WsHandler::*)(void);

  static void OnCloseFrameWritten(/*uv_write_t* req, */int /*status*/) {
    /*WriteRequest* wr = WriteRequest::from_write_req(req);
    WsHandler* handler = static_cast<WsHandler*>(wr->handler);
    delete wr;*/
    /*Callback cb = handler->OnCloseSent;
    (handler->*cb)();*/
  }

  void WaitForCloseReply() {
    OnCloseRecieved = &WsHandler::OnEof;
  }

  void SendClose() {
    WriteRaw(std::vector<char>(CLOSE_FRAME, CLOSE_FRAME + sizeof(CLOSE_FRAME))/*,
             OnCloseFrameWritten*/);
  }

  void CloseFrameReceived() {
    OnCloseSent = &WsHandler::OnEof;
    SendClose();
  }

  size_t ParseWsFrames(const std::vector<char>& buffer) {
    size_t bytes_consumed = 0;
    std::vector<char> output;
    bool compressed = false;

    ws_decode_result r =  decode_frame_hybi17(buffer,
                                              true /* client_frame */,
                                              &bytes_consumed, &output,
                                              &compressed);
    // Compressed frame means client is ignoring the headers and misbehaves
    if (compressed || r == FRAME_ERROR) {
      OnEof();
      bytes_consumed = 0;
    } else if (r == FRAME_CLOSE) {
      (this->*OnCloseRecieved)();
      bytes_consumed = 0;
    } else if (r == FRAME_OK) {
      delegate()->OnWsFrame(output);
    }
    return bytes_consumed;
  }


  Callback OnCloseSent;
  Callback OnCloseRecieved;
};

// HTTP protocol
class HttpEvent {
 public:
  HttpEvent(const std::string& path, bool upgrade, bool isGET,
            const std::string& ws_key, const std::string& host)
            : path(path), upgrade(upgrade), isGET(isGET), ws_key(ws_key),
              host(host) { }

  std::string path;
  bool upgrade;
  bool isGET;
  std::string ws_key;
  std::string host;
};

class HttpHandler : public ProtocolHandler {
 public:
  explicit HttpHandler(InspectorSocket* inspector, std::shared_ptr<TcpHolder>&& tcp)
                       : ProtocolHandler(inspector, std::move(tcp)),
                         parsing_value_(false) {

      llhttp_init(&parser_, HTTP_REQUEST, &parser_settings);
      llhttp_settings_init(&parser_settings);
      parser_settings.on_header_field = OnHeaderField;
      parser_settings.on_header_value = OnHeaderValue;
      parser_settings.on_message_complete = OnMessageComplete;
      parser_settings.on_url = OnPath;
  }

  void AcceptUpgrade(const std::string& accept_key) override {
    char accept_string[ACCEPT_KEY_LENGTH];
    generate_accept_string(accept_key, &accept_string);
    const char accept_ws_prefix[] = "HTTP/1.1 101 Switching Protocols\r\n"
                                    "Upgrade: websocket\r\n"
                                    "Connection: Upgrade\r\n"
                                    "Sec-WebSocket-Accept: ";
    const char accept_ws_suffix[] = "\r\n\r\n";
    std::vector<char> reply(accept_ws_prefix,
                            accept_ws_prefix + sizeof(accept_ws_prefix) - 1);
    reply.insert(reply.end(), accept_string,
                 accept_string + sizeof(accept_string));
    reply.insert(reply.end(), accept_ws_suffix,
                 accept_ws_suffix + sizeof(accept_ws_suffix) - 1);
    if (WriteRaw(reply/*, WriteRequest::Cleanup*/) >= 0) {
      inspector_->SwitchProtocol(std::make_unique<WsHandler>(inspector_, std::move(tcp_)));
    } else {
      tcp_.reset();
    }
  }

  void CancelHandshake() override {
    const char HANDSHAKE_FAILED_RESPONSE[] =
        "HTTP/1.0 400 Bad Request\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
        "WebSockets request was expected\r\n";
    WriteRaw(std::vector<char>(HANDSHAKE_FAILED_RESPONSE,
             HANDSHAKE_FAILED_RESPONSE + sizeof(HANDSHAKE_FAILED_RESPONSE) - 1)/*,
             ThenCloseAndReportFailure*/);
  }


  void OnEof() override {
    tcp_.reset();
  }

  void OnData(std::vector<char>* data) override {
    parser_errno_t err;
    err = llhttp_execute(&parser_, data->data(), data->size());

    if (err == HPE_PAUSED_UPGRADE) {
      err = HPE_OK;
      llhttp_resume_after_upgrade(&parser_);
    }
    data->clear();
    if (err != HPE_OK) {
      CancelHandshake();
    }
    // Event handling may delete *this
    std::vector<HttpEvent> events;
    std::swap(events, events_);
    for (const HttpEvent& event : events) {
      if (!IsAllowedHost(event.host) || !event.isGET) {
        CancelHandshake();
        return;
      } else if (!event.upgrade) {
        delegate()->OnHttpGet(event.host, event.path);
      } else if (event.ws_key.empty()) {
        CancelHandshake();
        return;
      } else {
        delegate()->OnSocketUpgrade(event.host, event.path, event.ws_key);
      }
    }
  }

  void Write(const std::vector<char> data) override {
    WriteRaw(data/*, WriteRequest::Cleanup*/);
  }

 private:
  static void ThenCloseAndReportFailure(/*uv_write_t* req, */int /*status*/) {
    /*ProtocolHandler* handler = WriteRequest::from_write_req(req)->handler;
    WriteRequest::Cleanup(req, status);
    handler->inspector()->SwitchProtocol(nullptr);*/
  }

  static int OnHeaderValue(parser_t* parser, const char* at, size_t length) {
    HttpHandler* handler = From(parser);
    handler->parsing_value_ = true;
    handler->headers_[handler->current_header_].append(at, length);
    return 0;
  }

  static int OnHeaderField(parser_t* parser, const char* at, size_t length) {
    HttpHandler* handler = From(parser);
    if (handler->parsing_value_) {
      handler->parsing_value_ = false;
      handler->current_header_.clear();
    }
    handler->current_header_.append(at, length);
    return 0;
  }

  static int OnPath(parser_t* parser, const char* at, size_t length) {
    HttpHandler* handler = From(parser);
    handler->path_.append(at, length);
    return 0;
  }

  static HttpHandler* From(parser_t* parser) {
    return ContainerOf(&HttpHandler::parser_, parser);
  }

  static int OnMessageComplete(parser_t* parser) {
    // Event needs to be fired after the parser is done.
    HttpHandler* handler = From(parser);
    handler->events_.push_back(
        HttpEvent(handler->path_, parser->upgrade, parser->method == HTTP_GET,
                  handler->HeaderValue("Sec-WebSocket-Key"),
                  handler->HeaderValue("Host")));
    handler->path_ = "";
    handler->parsing_value_ = false;
    handler->headers_.clear();
    handler->current_header_ = "";
    return 0;
  }

  std::string HeaderValue(const std::string& header) const {
    bool header_found = false;
    std::string value;
    for (const auto& header_value : headers_) {
      if (utils::StringEqualNoCaseN(header_value.first.data(), header.data(),
                                   header.length())) {
        if (header_found)
          return "";
        value = header_value.second;
        header_found = true;
      }
    }
    return value;
  }

  bool IsAllowedHost(const std::string& host_with_port) const {
    std::string host = TrimPort(host_with_port);
    return host.empty() || IsIPAddress(host)
           || utils::StringEqualNoCase(host.data(), "localhost")
           || utils::StringEqualNoCase(host.data(), "localhost6");
  }

  bool parsing_value_;
  parser_t parser_;
  parser_settings_t parser_settings;
  std::vector<HttpEvent> events_;
  std::string current_header_;
  std::map<std::string, std::string> headers_;
  std::string path_;
};

}  // namespace

// Any protocol
ProtocolHandler::ProtocolHandler(InspectorSocket* inspector,
                                 std::shared_ptr<TcpHolder>&& tcp)
                                 : inspector_(inspector), tcp_(std::move(tcp)) {
  CHECK_NOT_NULL(tcp_);
  tcp_->SetHandler(this);
}

int ProtocolHandler::WriteRaw(const std::vector<char>& buffer/*,
                              uv_write_cb write_cb*/) {
  return tcp_->WriteRaw(buffer/*, write_cb*/);
}

InspectorSocket::Delegate* ProtocolHandler::delegate() {
  return tcp_->delegate();
}

std::string ProtocolHandler::GetHost() const {
  // TBD
  return "";
}

// RAII uv_tcp_t wrapper
TcpHolder::TcpHolder(std::shared_ptr<tcp_connection> connection, std::unique_ptr<InspectorSocket::Delegate> delegate)
                     : delegate_(std::move(delegate)),
                       connection_(connection), handler_(nullptr) {
  connection_->registerReadCallback(TcpHolder::OnDataReceivedCb, this);
  connection_->read_loop_async();
}

TcpHolder::~TcpHolder() {
  connection_->close();
}

void TcpHolder::read_loop() {
  connection_->read_loop_async();
}

void TcpHolder::SetHandler(ProtocolHandler* handler) {
  handler_ = handler;
}

int TcpHolder::WriteRaw(const std::vector<char>& buffer/*, uv_write_cb write_cb*/) {
#if DUMP_WRITES
  printf("%s (%ld bytes):\n", __FUNCTION__, buffer.size());
  dump_hex(buffer.data(), buffer.size());
  printf("\n");
#endif

  // Freed in write_request_cleanup
  /*WriteRequest* wr = new WriteRequest(handler_, buffer);
  uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(&tcp_);
  int err = uv_write(&wr->req, stream, &wr->buf, 1, write_cb);
  if (err < 0)
    delete wr;
  return err < 0;*/
  connection_->write_async(buffer);
  return 0;
}

InspectorSocket::Delegate* TcpHolder::delegate() {
  return delegate_.get();
}

void TcpHolder::OnDataReceivedCb(std::vector<char>& wiredata, bool iseof, void*data) {
  TcpHolder* holder = reinterpret_cast<TcpHolder*>(data);
  
  if (iseof) {
    holder->handler_->OnEof();
  }
  else {
    holder->handler_->OnData(&wiredata);
  }
}

InspectorSocket::~InspectorSocket() = default;

// static
std::unique_ptr<InspectorSocket> InspectorSocket::Accept(std::shared_ptr<tcp_connection> connection, std::unique_ptr<InspectorSocket::Delegate>&& delegate) {
  auto tcp = std::make_shared<TcpHolder>(std::move(connection), std::move(delegate));

  auto inspector = std::make_unique<InspectorSocket>();
  inspector->SwitchProtocol(std::make_unique<HttpHandler>(inspector.get(), std::move(tcp)));
  return inspector;
}

void InspectorSocket::AcceptUpgrade(const std::string& ws_key) {
  protocol_handler_->AcceptUpgrade(ws_key);
}

void InspectorSocket::CancelHandshake() {
  protocol_handler_->CancelHandshake();
}

std::string InspectorSocket::GetHost() {
  return protocol_handler_->GetHost();
}

void InspectorSocket::SwitchProtocol(std::unique_ptr<ProtocolHandler>&& handler) {
  protocol_handler_ = std::move(handler);
}

void InspectorSocket::Write(const char* data, size_t len) {
  protocol_handler_->Write(std::vector<char>(data, data + len));
}

}  // namespace inspector