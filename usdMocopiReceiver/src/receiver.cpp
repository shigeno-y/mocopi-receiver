// SPDX-License-Identifier: MIT

#include "receiver.hpp"
#include "tokens.hpp"

#include <chrono>
#include <thread>

#include "pxr/usd/pcp/dynamicFileFormatContext.h"
#include "pxr/usd/usdGeom/scope.h"

namespace {
void
invokeWorkerThread(pxr::SdfLayer* layer)
{
    std::this_thread::sleep_for(std::chrono::seconds{ 30 });
    pxr::SdfLayerHandle handle{ layer };

    auto stage = pxr::UsdStage::CreateInMemory();
    auto root  = pxr::UsdGeomScope::Define(stage, pxr::SdfPath{ "/hogehgoe" });
    stage->SetDefaultPrim(root.GetPrim());

    layer->TransferContent(stage->GetRootLayer());
}
} // namespace

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdMocopiReceiverFileFormatTokens, USDMOCOPIRECEIVER_FILE_FORMAT_TOKENS);

TF_REGISTRY_FUNCTION(TfType)
{
    SDF_DEFINE_FILE_FORMAT(UsdMocopiReceiverFileFormat, SdfFileFormat);
}

UsdMocopiReceiverFileFormat::UsdMocopiReceiverFileFormat()
    : SdfFileFormat(UsdMocopiReceiverFileFormatTokens->Id,
                    UsdMocopiReceiverFileFormatTokens->Version,
                    UsdMocopiReceiverFileFormatTokens->Target,
                    UsdMocopiReceiverFileFormatTokens->Id)
{
}

UsdMocopiReceiverFileFormat::~UsdMocopiReceiverFileFormat()
{
}

bool
UsdMocopiReceiverFileFormat::CanRead(const std::string& file_path) const
{
    return true;
}

bool
UsdMocopiReceiverFileFormat::Read(SdfLayer* layer,
                                  const std::string& resolved_path,
                                  bool metadata_only) const
{
    std::thread worker{ invokeWorkerThread, layer };
    worker.detach();
    return true;
}

void
UsdMocopiReceiverFileFormat::ComposeFieldsForFileFormatArguments(
    const std::string& asset_path,
    const pxr::PcpDynamicFileFormatContext& context,
    pxr::SdfFileFormat::FileFormatArguments* args,
    pxr::VtValue* context_dependency_data) const
{
    // for listenHost
    {
        pxr::VtValue val;
        if (context.ComposeValue(shigenoy::mocopi_parser::tokens->listen_host, &val) &&
            val.IsHolding<std::string>())
        {
            args->operator[](shigenoy::mocopi_parser::tokens->listen_host) =
                pxr::TfStringify(val.Get<std::string>());
        }
    }
    // for listenPort
    {
        pxr::VtValue val;
        if (context.ComposeValue(shigenoy::mocopi_parser::tokens->listen_port, &val) &&
            val.IsHolding<unsigned int>())
        {
            args->operator[](shigenoy::mocopi_parser::tokens->listen_port) =
                pxr::TfStringify(val.Get<unsigned int>());
        }
    }
}

bool
UsdMocopiReceiverFileFormat::CanFieldChangeAffectFileFormatArguments(
    const pxr::TfToken& field,
    const pxr::VtValue& old_value,
    const pxr::VtValue& new_value,
    const pxr::VtValue& context_dependency_data) const
{
    // each metadata or SDF args affects loaded (or `baked') data
    return true;
}

PXR_NAMESPACE_CLOSE_SCOPE
