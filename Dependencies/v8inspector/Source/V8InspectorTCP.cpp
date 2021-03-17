// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See NOTICE.md for Node.js' project license details
#include "V8InspectorTCP.h"

#include <memory>

namespace Babylon
{

    tcp_server::tcp_server(unsigned short port, ConnectionCallback callback, void* data)
        : io_service_()
        , acceptor_(io_service_)
        , socket_(io_service_)
        , connectioncallback_(callback)
        , callbackData_(data)
    {
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        do_accept();
    }

    void tcp_server::run()
    {
        io_service_.run();
    }

    void tcp_server::stop()
    {
        asio::error_code ec;
        acceptor_.close(ec);
        socket_.close(ec);

        io_service_.stop();
    }

    void tcp_server::do_accept()
    {
        std::shared_ptr<tcp_server> self;
        acceptor_.async_accept(socket_,
            [this, self](asio::error_code ec) {
                if (!ec)
                {
                    connectioncallback_(std::make_shared<tcp_connection>(std::move(socket_)), callbackData_);
                }

                do_accept();
            });
    }

    asio::ip::tcp::socket& tcp_connection::socket()
    {
        return socket_;
    }

    void tcp_connection::read_loop_async()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(buffer_),
            [this, self](asio::error_code ec, std::size_t bytes_transferred) {
                if (!ec)
                {
                    std::vector<char> vc;
                    //const char* start = boost::asio::buffer_cast<const char*>(input_buffer_.data());
                    vc.reserve(bytes_transferred);
                    for (size_t c = 0; c < bytes_transferred; c++)
                    {
                        vc.push_back(buffer_.data()[c]);
                    }

                    std::string s((buffer_.data()), bytes_transferred);

                    readcallback_(vc, false, callbackData_);

                    self->read_loop_async();
                }
                else if (ec == asio::error::eof)
                {
                    std::vector<char> vc;
                    readcallback_(vc, true, callbackData_);
                }
                else if (ec == asio::error::operation_aborted)
                {
                    std::abort();
                }
                else
                {
                    return;
                }
            });
    }

    // !!COPY
    void tcp_connection::write_async(std::vector<char> message)
    {

        {
            std::lock_guard<std::mutex> guard(queueAccessMutex);
            outQueue.push(std::move(message));
        }

        do_write(false);
    }

    void tcp_connection::do_write(bool cont)
    {

        std::vector<char> message;
        {
            std::lock_guard<std::mutex> guard(queueAccessMutex);

            // New message but the last write is going on.
            if (!cont && writing_)
                return;

            if (outQueue.empty())
            {
                writing_ = false;
                return;
            }
            message = outQueue.front();
            outQueue.pop();

            writing_ = true;
        }

        auto self(shared_from_this());

        messageToWrite_ = std::move(message);

        std::string str;
        std::transform(messageToWrite_.begin(), messageToWrite_.end(), std::back_inserter(str), [](char c) { return c; });

        socket_.async_send(asio::buffer(messageToWrite_),
            [this, self](asio::error_code ec, std::size_t bytes_transferred) {
                if (!ec)
                {
                    std::ostringstream stream;
                    stream << "Writing completed .. " << bytes_transferred << " bytes.";

                    self->do_write(true);
                }

                if (ec == asio::error::operation_aborted)
                {
                    std::abort();
                }
                else
                {
                    // TODO
                }
            });
    }

    void tcp_connection::close()
    {
        asio::error_code ec;
        socket_.close(ec);
        if (ec)
            std::abort();
    }

}