// SPDX-License-Identifier: MIT
#include "logic.hpp"

#include "Skeleton.hpp"
#include "tokens.hpp"

#include <shigenoy/mocopi_parser/Receiver.hpp>

#include "pxr/usd/usdGeom/scope.h"

#include <chrono>
#include <cstdint>
#include <thread>
#include <unordered_map>

namespace {
template <typename T>
void
replaceAll(T& target, const T& old_fragment, const T& new_fragment)
{
    if (!old_fragment.empty())
    {
        typename T::size_type pos = 0;
        while ((pos = target.find(old_fragment, pos)) != T::npos)
        {
            target.replace(pos, old_fragment.length(), new_fragment);
            pos += new_fragment.length();
        }
    }
}
} // namespace

void
shigenoy::mocopi_parser::invokeWorkerThread(pxr::SdfLayerHandle handle,
                                            const pxr::SdfFileFormat::FileFormatArguments& args)
{
    constexpr std::chrono::milliseconds WAIT{ 10 };
    std::string host{ "127.0.0.1" };
    std::uint16_t port{ 12351 };
    bool honor_frame_number{ false };

    oneapi::tbb::concurrent_unordered_map<std::string,
                                          oneapi::tbb::concurrent_queue<ParsedMocopiPacket>>
        queue;
    {
        const auto& itr = args.find(shigenoy::mocopi_parser::tokens->listen_host);
        if (itr != args.cend())
        {
            host = itr->second;
        }
    }
    {
        const auto& itr = args.find(shigenoy::mocopi_parser::tokens->listen_port);
        if (itr != args.cend())
        {
            port = static_cast<std::uint16_t>(std::stoul(itr->second));
        }
    }
    {
        const auto& itr = args.find(shigenoy::mocopi_parser::tokens->honor_frame_number);
        if (itr != args.cend())
        {
            honor_frame_number = static_cast<int>(std::stoul(itr->second)) != 0;
        }
    }
    shigenoy::mocopi_parser::receiveMocopiUdp(host, port, queue);

    std::unordered_map<std::string, std::vector<pxr::TfToken>> client_joints{};
    ParsedMocopiPacket packet{};

    auto stage = pxr::UsdStage::CreateInMemory();
    auto root_prim =
        pxr::UsdGeomScope::Define(stage, pxr::SdfPath{ "/" }.AppendChild(tokens->skels));
    stage->SetDefaultPrim(root_prim.GetPrim());
    for (;;)
    {
        for (auto& mocopi : queue)
        {
            std::string client{ mocopi.first };
            replaceAll<std::string>(client, ".", "_");
            replaceAll<std::string>(client, ":", "p");

            auto skel_root = pxr::UsdSkelRoot::Define(
                stage, root_prim.GetPath().AppendChild(pxr::TfToken{ "mocopi" + client }));

            while (mocopi.second.try_pop(packet))
            {
                if (!client_joints.contains(client) && packet.hasBoneDefinition())
                {
                    // just pre-define for anim source
                    pxr::UsdSkelAnimation::Define(stage,
                                                  skel_root.GetPath().AppendChild(tokens->motion));
                    generateSkelRoot(stage, skel_root, client_joints[client], packet);
                }
                else if (client_joints.contains(client) && packet.hasFrameData())
                {
                    generateSkelAnim(
                        stage, skel_root, honor_frame_number, client_joints.at(client), packet);
                }
            }
        }
        handle->TransferContent(stage->GetRootLayer());
        std::this_thread::sleep_for(WAIT);
    }
}
