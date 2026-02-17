// SPDX-License-Identifier: MIT

#include "logic.hpp"
#include "tokens.hpp"

#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/scope.h"

#include <chrono>
#include <cstdint>
#include <random>
#include <thread>

void
shigenoy::mocopi_parser::invokeWorkerThread(pxr::SdfLayerHandle handle,
                                            const pxr::SdfFileFormat::FileFormatArguments& args)
{
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_real_distribution<double> dist(.1, 2.1);

    std::string host;
    std::uint16_t port = 0;

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

    auto stage = pxr::UsdStage::CreateInMemory();

    auto root_prim = pxr::UsdGeomScope::Define(stage, pxr::SdfPath{ "/Root" });
    stage->SetDefaultPrim(root_prim.GetPrim());

    auto box =
        pxr::UsdGeomCube::Define(stage, root_prim.GetPath().AppendChild(pxr::TfToken{ "box" }));

    for (int ctr = 0; ctr < 1024; ++ctr)
    {
        box.GetSizeAttr().Set(dist(engine));

        handle->TransferContent(stage->GetRootLayer());
        std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
    }
}
