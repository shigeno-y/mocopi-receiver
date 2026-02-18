// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi_parser/Receiver.hpp>

#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Thread.h"

#include <thread>
#include <vector>

namespace {
class MyUDPServer : public Poco::Runnable
/// A simple sequential UDP echo server.
{
private:
    oneapi::tbb::concurrent_unordered_map<
        std::string,
        oneapi::tbb::concurrent_queue<shigenoy::mocopi_parser::ParsedMocopiPacket>>& queues_;

public:
    MyUDPServer(
        const Poco::Net::SocketAddress& sa,
        oneapi::tbb::concurrent_unordered_map<
            std::string,
            oneapi::tbb::concurrent_queue<shigenoy::mocopi_parser::ParsedMocopiPacket>>& queues);
    /// Creates the UDPEchoServer and binds it to
    /// the given address.

    ~MyUDPServer() override;
    /// Destroys the UDPEchoServer.

    Poco::UInt16 port() const;
    /// Returns the port the echo server is
    /// listening on.

    Poco::Net::SocketAddress address() const;
    /// Returns the address of the server.

    void run() override;
    /// Does the work.

private:
    Poco::Net::DatagramSocket socket_;
    Poco::Thread thread_;
    Poco::Event ready_;
    std::atomic<bool> stop_;
};

MyUDPServer::MyUDPServer(
    const Poco::Net::SocketAddress& sa,
    oneapi::tbb::concurrent_unordered_map<
        std::string,
        oneapi::tbb::concurrent_queue<shigenoy::mocopi_parser::ParsedMocopiPacket>>& queues)
    : thread_("UDPEchoServer"), stop_(false), queues_(queues)
{
    socket_.bind(sa, true);
    thread_.start(*this);
    ready_.wait();
}

MyUDPServer::~MyUDPServer()
{
    stop_ = true;
    thread_.join();
}

Poco::UInt16
MyUDPServer::port() const
{
    return socket_.address().port();
}

void
MyUDPServer::run()
{
    std::vector<std::byte> buffer;
    Poco::Timespan span(250000);
    Poco::Net::SocketAddress sender;

    buffer.resize(2048);
    while (!stop_)
    {
        ready_.set();
        if (socket_.poll(span, Poco::Net::Socket::SELECT_READ))
        {
            try
            {
                int n = socket_.receiveFrom(buffer.data(), buffer.size(), sender);
                this->queues_[sender.toString()].push(
                    shigenoy::mocopi_parser::ParsedMocopiPacket{ buffer });
            }
            catch (...)
            {
            }
        }
    }
}

Poco::Net::SocketAddress
MyUDPServer::address() const
{
    return socket_.address();
}
} // namespace

void
shigenoy::mocopi_parser::receiveMocopiUdp(
    const std::string& listen_addr,
    std::uint16_t listen_port,
    oneapi::tbb::concurrent_unordered_map<std::string,
                                          oneapi::tbb::concurrent_queue<ParsedMocopiPacket>>&
        queues)
{
    std::thread th{ [=, &queues]() {
        Poco::Net::SocketAddress sa{ listen_addr, listen_port };
        MyUDPServer server{ sa, queues };
        server.run();
    } };
    th.detach();
}
