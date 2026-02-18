// SPDX-License-Identifier: MIT
#include "logic.hpp"

#include "Skeleton.hpp"
#include "tokens.hpp"

#include <shigenoy/mocopi_parser/Receiver.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

void
shigenoy::mocopi_parser::invokeWorkerThread(pxr::SdfLayerHandle handle,
                                            const pxr::SdfFileFormat::FileFormatArguments& args)
{
    constexpr std::chrono::milliseconds WAIT{ 8 };
    std::string host{ "127.0.0.1" };
    std::uint16_t port{ 12351 };

    oneapi::tbb::concurrent_unordered_map<std::string,
                                          oneapi::tbb::concurrent_queue<ParsedMocopiPacket>>
        queue;
    {
        const auto& listen_host = args.find(shigenoy::mocopi_parser::tokens->listen_host);
        if (listen_host != args.cend())
        {
            host = listen_host->second;
        }
    }
    {
        const auto& listen_port = args.find(shigenoy::mocopi_parser::tokens->listen_port);
        if (listen_port != args.cend())
        {
            port = static_cast<std::uint16_t>(std::stoul(listen_port->second));
        }
    }

    std::vector<pxr::TfToken> joints;
    ParsedMocopiPacket packet{};

    auto stage = pxr::UsdStage::CreateInMemory();

    auto skel_root = pxr::UsdSkelRoot::Define(stage, pxr::SdfPath{ "/Skel" });
    stage->SetDefaultPrim(skel_root.GetPrim());

    shigenoy::mocopi_parser::receiveMocopiUdp(host, port, queue);

    auto mocopi = queue.begin();
    while (mocopi == queue.end())
    {
        std::this_thread::sleep_for(WAIT);
        mocopi = queue.begin();
    }

    std::cerr << mocopi->first << "\n";

    for (;;)
    {
        while (!mocopi->second.try_pop(packet))
        {
            std::this_thread::sleep_for(WAIT);
        }
        if (packet.hasBoneDefinition())
        {
            generateSkelRoot(stage, skel_root, joints, packet);
            break;
        }
    }

    for (;;)
    {
        while (!mocopi->second.try_pop(packet))
        {
            std::this_thread::sleep_for(WAIT);
        }
        if (packet.hasFrameData())
        {
            generateSkelAnim(stage, skel_root, joints, packet);
            handle->TransferContent(stage->GetRootLayer());
        }
    }
}
