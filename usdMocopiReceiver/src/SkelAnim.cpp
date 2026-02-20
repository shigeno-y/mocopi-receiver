// SPDX-License-Identifier: MIT
#include "Skeleton.hpp"

#include "tokens.hpp"

#include <shigenoy/mocopi_parser/Generator.hpp>

#include "pxr/usd/usdSkel/animation.h"

#include <vector>

void
shigenoy::mocopi_parser::generateSkelAnim(pxr::UsdStageRefPtr stage,
                                          pxr::UsdSkelRoot& skel_root,
                                          bool honorFrameNumber,
                                          const std::vector<pxr::TfToken>& joints,
                                          const ParsedMocopiPacket& bone_transform_packet)
{
    auto motion =
        pxr::UsdSkelAnimation::Define(stage, skel_root.GetPath().AppendChild(tokens->motion));

    motion.GetJointsAttr().Set(pxr::VtArray<pxr::TfToken>(joints.cbegin(), joints.cend()));
    motion.GetScalesAttr().Set(pxr::VtArray<pxr::GfVec3h>(joints.size(), { 1, 1, 1 }));

    try
    {
        const auto change_block = pxr::SdfChangeBlock();
        auto rotation_timesamples_attr_path =
            motion.GetPath().AppendProperty(pxr::UsdSkelTokens->rotations);
        auto translation_timesamples_attr_path =
            motion.GetPath().AppendProperty(pxr::UsdSkelTokens->translations);

        std::vector<pxr::GfQuatf> initial_rotations;
        std::vector<pxr::GfVec3f> initial_translations;

        for (const auto& bone : readBoneTransforms(bone_transform_packet))
        {
            initial_rotations.emplace_back(bone.quat_w_, bone.quat_x_, bone.quat_y_, bone.quat_z_);
            initial_translations.emplace_back(bone.tran_x_, bone.tran_y_, bone.tran_z_);
        }

        if (honorFrameNumber)
        {
            const auto& fnum =
                bone_transform_packet.parsed_.equal_range(wellknown_code::FNUM).first->second;
            double timecode = fnum.readAs<std::uint32_t>(0);
            stage->SetStartTimeCode(std::min(timecode, stage->GetStartTimeCode()));
            stage->SetEndTimeCode(std::max(timecode, stage->GetEndTimeCode()));

            motion.GetRotationsAttr().Set(
                pxr::VtArray<decltype(initial_rotations)::value_type>{ initial_rotations.cbegin(),
                                                                       initial_rotations.cend() },
                timecode);
            motion.GetTranslationsAttr().Set(
                pxr::VtArray<decltype(initial_translations)::value_type>{
                    initial_translations.cbegin(), initial_translations.cend() },
                timecode);
        }
        else
        {
            motion.GetRotationsAttr().Set(pxr::VtArray<decltype(initial_rotations)::value_type>{
                initial_rotations.cbegin(), initial_rotations.cend() });
            motion.GetTranslationsAttr().Set(
                pxr::VtArray<decltype(initial_translations)::value_type>{
                    initial_translations.cbegin(), initial_translations.cend() });
        }
    }
    catch (...)
    {
    }
}
