// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_USDMOCOPIRECEIVER_RECEIVER_HPP)
#    define SHIGENOY_MOCOPIPARSER_USDMOCOPIRECEIVER_RECEIVER_HPP

#    include "pxr/base/tf/staticTokens.h"
#    include "pxr/pxr.h"
#    include "pxr/usd/pcp/dynamicFileFormatContext.h"
#    include "pxr/usd/pcp/dynamicFileFormatInterface.h"
#    include "pxr/usd/sdf/fileFormat.h"
#    include "pxr/usd/usdGeom/scope.h"

#    include <string>

PXR_NAMESPACE_OPEN_SCOPE

#    define USDMOCOPIRECEIVER_FILE_FORMAT_TOKENS ((Id, "mocopi"))((Version, "1.0"))((Target, "usd"))

TF_DECLARE_PUBLIC_TOKENS(UsdMocopiReceiverFileFormatTokens, USDMOCOPIRECEIVER_FILE_FORMAT_TOKENS);

TF_DECLARE_WEAK_AND_REF_PTRS(UsdMocopiReceiverFileFormat);

class UsdMocopiReceiverFileFormat : public SdfFileFormat, public PcpDynamicFileFormatInterface
{
public:
    // SdfFileFormat overrides.
    bool CanRead(const std::string& file) const override;
    bool Read(SdfLayer* layer, const std::string& resolved_path, bool metadata_only) const override;
    /*
    virtual bool ReadFromString(SdfLayer* layer, const std::string& str) const override;
    // */

    /*
    virtual bool WriteToString(const SdfLayer& layer,
                               std::string* str,
                               const std::string& comment = std::string()) const override;
    // */
    /*virtual bool
    WriteToStream(const SdfSpecHandle& spec, std::ostream& out, size_t indent) const override;
    */

    void ComposeFieldsForFileFormatArguments(const std::string& asset_path,
                                             const pxr::PcpDynamicFileFormatContext& context,
                                             pxr::SdfFileFormat::FileFormatArguments* args,
                                             pxr::VtValue* context_dependency_data) const override;

    bool CanFieldChangeAffectFileFormatArguments(
        const pxr::TfToken& field,
        const pxr::VtValue& old_value,
        const pxr::VtValue& new_value,
        const pxr::VtValue& context_dependency_data) const override;

    /*
    bool CanAttributeDefaultValueChangeAffectFileFormatArguments(
        const pxr::TfToken& field,
        const pxr::VtValue& oldValue,
        const pxr::VtValue& newValue,
        const pxr::VtValue& contextDependencyData) const override;
    // */

protected:
    SDF_FILE_FORMAT_FACTORY_ACCESS;

    ~UsdMocopiReceiverFileFormat() override;
    UsdMocopiReceiverFileFormat();
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
