// SPDX-License-Identifier: MIT

#include "receiver.hpp"
#include "logic.hpp"
#include "tokens.hpp"

#include <string>
#include <thread>

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
    const auto& args = layer->GetFileFormatArguments();
    pxr::SdfLayerHandle handle{ layer };

    if (handle)
    {
        auto root_prim_spec =
            pxr::SdfPrimSpec::New(handle, "Root", pxr::SdfSpecifier::SdfSpecifierDef);
        root_prim_spec->SetTypeName("Scope");
        handle->SetDefaultPrim(root_prim_spec->GetNameToken());
        std::thread worker{ shigenoy::mocopi_parser::invokeWorkerThread, handle, args };
        worker.detach();
        //worker.join();
    }
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
