// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi_parser/Receiver.hpp>

#include <shigenoy/mocopi_parser/Container.hpp>
#include <shigenoy/mocopi_parser/Generator.hpp>
#include <shigenoy/mocopi_parser/Skeleton.hpp>

#include "sockpp/udp_socket.h"

#include <atomic>
#include <thread>
#include <vector>

namespace {
void
run_echo(sockpp::udp_socket sock, pxr::UsdStageRefPtr stage, pxr::UsdSkelRoot& skel_root)
{
    std::atomic<bool> skeleton_defined{ false };
    std::vector<std::byte> buf;
    buf.resize(2048);

    // Each UDP socket type knows its address type as `addr_t`
    typename sockpp::udp_socket::addr_t srcAddr;

    // Read some data, also getting the address of the sender,
    // then just send it back.
    while (true)
    {
        auto res = sock.recv_from(buf.data(), buf.size(), &srcAddr);
        if (!res || res == 0)
        {
            break;
        }

        const ParsedMocopiPacket p{ buf };
        std::vector<pxr::TfToken> joints;

        if (p.hasFrameData())
        {
            // Frame Data
            generateSkelAnim(stage, skel_root, joints, p);
        }
        else if (!skeleton_defined && p.hasBoneDefinition())
        {
            // Bone Definition
            skeleton_defined = true;
            generateSkelRoot(stage, skel_root, joints, p);
        }
    }
}
} // namespace

void
shigenoy::mocopi_parser::receiveMocopiUdp(pxr::UsdStageRefPtr stage,
                                          pxr::UsdSkelRoot& skel_root,
                                          const std::string& host,
                                          std::uint16_t port)
{
    sockpp::initialize();
    sockpp::udp_socket udpsock;

    int timeout{ 10000 };
    udpsock.set_option(SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    auto res = udpsock.bind(sockpp::inet_address(host, port));
    if (!res)
    {
        return;
    }

    std::thread t{ run_echo, std::move(udpsock), stage, skel_root };
    t.detach();
}
