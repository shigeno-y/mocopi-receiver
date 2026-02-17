// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_RECEIVER_HPP)
#    define SHIGENOY_MOCOPIPARSER_RECEIVER_HPP

#    include <shigenoy/mocopi_parser/Container.hpp>

#    include "pxr/usd/usd/stage.h"
#    include "pxr/usd/usdSkel/root.h"

namespace shigenoy::mocopi_parser {
void generateSkeleton(pxr::UsdStageRefPtr stage,
                      pxr::UsdSkelRoot& skel_root,
                      const ParsedMocopiPacket& boneDefinitionPacket);
}

#endif
