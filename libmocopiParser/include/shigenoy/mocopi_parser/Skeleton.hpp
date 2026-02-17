// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_SKELETON_HPP)
#    define SHIGENOY_MOCOPIPARSER_SKELETON_HPP

#    include <shigenoy/mocopi_parser/Container.hpp>

#    include "pxr/usd/usd/stage.h"
#    include "pxr/usd/usdSkel/root.h"

namespace shigenoy::mocopi_parser {
void generateSkelRoot(pxr::UsdStageRefPtr stage,
                      pxr::UsdSkelRoot& skel_root,
                      std::vector<pxr::TfToken>& joints,
                      const ParsedMocopiPacket& boneDefinitionPacket);

void generateSkelAnim(pxr::UsdStageRefPtr stage,
                      pxr::UsdSkelRoot& skel_root,
                      const std::vector<pxr::TfToken>& joints,
                      const ParsedMocopiPacket& boneTransformPacket);
} // namespace shigenoy::mocopi_parser
#endif
