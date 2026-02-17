// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_PLUGIN_LOGIC_HPP)
#    define SHIGENOY_MOCOPIPARSER_PLUGIN_LOGIC_HPP

#    include "pxr/usd/pcp/dynamicFileFormatInterface.h"
#    include "pxr/usd/sdf/layer.h"

namespace shigenoy::mocopi_parser {
void
invokeWorkerThread(pxr::SdfLayerHandle handle, const pxr::SdfFileFormat::FileFormatArguments& args);
} // namespace shigenoy::mocopi_parser

#endif
