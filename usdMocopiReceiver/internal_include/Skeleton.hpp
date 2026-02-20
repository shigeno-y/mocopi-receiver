// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_SKELETON_HPP)
#    define SHIGENOY_MOCOPIPARSER_SKELETON_HPP

#    include <shigenoy/mocopi_parser/Container.hpp>

#    include "pxr/usd/usd/stage.h"
#    include "pxr/usd/usdSkel/animation.h"
#    include "pxr/usd/usdSkel/bindingAPI.h"
#    include "pxr/usd/usdSkel/root.h"
#    include "pxr/usd/usdSkel/skeleton.h"

namespace shigenoy::mocopi_parser {
void generateSkelRoot(pxr::UsdStageRefPtr stage,
                      pxr::UsdSkelRoot& skel_root,
                      std::vector<pxr::TfToken>& joints,
                      const ParsedMocopiPacket& boneDefinitionPacket);

void generateSkelAnim(pxr::UsdStageRefPtr stage,
                      pxr::UsdSkelRoot& skel_root,
                      bool honorFrameNumber,
                      const std::vector<pxr::TfToken>& joints,
                      const ParsedMocopiPacket& bone_transform_packet);
} // namespace shigenoy::mocopi_parser
#endif
