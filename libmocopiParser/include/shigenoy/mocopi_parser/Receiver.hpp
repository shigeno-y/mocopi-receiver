// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_RECEIVER_HPP)
#    define SHIGENOY_MOCOPIPARSER_RECEIVER_HPP

#    include "pxr/usd/usd/stage.h"
#    include "pxr/usd/usdSkel/root.h"

#    include <cstdint>
#    include <string>

namespace shigenoy::mocopi_parser {
void receiveMocopiUdp(pxr::UsdStageRefPtr stage,
                      pxr::UsdSkelRoot& skel_root,
                      const std::string& host,
                      std::uint16_t port);
} // namespace shigenoy::mocopi_parser
#endif
