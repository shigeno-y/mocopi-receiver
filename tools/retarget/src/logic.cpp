// SPDX-License-Identifier: MIT
#include "logic.hpp"

#include "tokens.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h> //NOLINT
#include <pxr/usd/usd/relationship.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdSkel/animQuery.h>
#include <pxr/usd/usdSkel/animation.h>
#include <pxr/usd/usdSkel/bindingAPI.h>
#include <pxr/usd/usdSkel/root.h>
#include <pxr/usd/usdSkel/skeleton.h>

bool
shigenoy::mocopi_parser::retarget::phase1(const std::filesystem::path& source_anim_file_path,
                                          const std::filesystem::path& target_model_file_path,
                                          const std::filesystem::path& output_anim_file_path,
                                          std::string& source_anim_skel_root_path,
                                          std::string& target_model_skel_root_path)
{
    try
    {
        auto source_anim_stage  = pxr::UsdStage::Open(source_anim_file_path.generic_string());
        auto target_model_stage = pxr::UsdStage::Open(target_model_file_path.generic_string());
        auto output_anim_stage  = pxr::UsdStage::CreateInMemory();

        if (source_anim_skel_root_path.length() == 0)
        {
            for (const auto& prim : source_anim_stage->TraverseAll())
            {
                if (prim.IsA<pxr::UsdSkelRoot>() && prim.HasAPI<pxr::UsdSkelBindingAPI>())
                {
                    source_anim_skel_root_path = prim.GetPath().GetAsString();
                    break;
                }
            }
        }
        if (target_model_skel_root_path.length() == 0)
        {
            for (const auto& prim : target_model_stage->TraverseAll())
            {
                if (prim.IsA<pxr::UsdSkelRoot>() && prim.HasAPI<pxr::UsdSkelBindingAPI>())
                {
                    target_model_skel_root_path = prim.GetPath().GetAsString();
                    break;
                }
            }
        }

        // auto sourceSkelRoot    = pxr::UsdSkelRoot{ source_anim_stage->GetPrimAtPath(
        //     pxr::SdfPath{ source_anim_skelRoot_path }) };
        auto source_skel_binding = pxr::UsdSkelBindingAPI{ source_anim_stage->GetPrimAtPath(
            pxr::SdfPath{ source_anim_skel_root_path }) };
        auto target_skel_binding = pxr::UsdSkelBindingAPI{ target_model_stage->GetPrimAtPath(
            pxr::SdfPath{ target_model_skel_root_path }) };

        pxr::UsdPrim source_skel_anim_source;
        pxr::UsdSkelSkeleton source_skel_skeleton;
        if (!source_skel_binding.GetAnimationSource(&source_skel_anim_source))
        {
            return false;
        }
        if (!source_skel_binding.GetSkeleton(&source_skel_skeleton))
        {
            return false;
        }

        pxr::UsdSkelSkeleton target_skel_skeleton;
        if (!target_skel_binding.GetSkeleton(&target_skel_skeleton))
        {
            return false;
        }

        {
            std::map<size_t, size_t> filtering_rule;
            std::unordered_map<std::u8string, pxr::TfToken> source_joints;
            std::unordered_map<std::u8string, pxr::TfToken> target_joints;

            std::unordered_map<std::u8string, std::u8string> mocopi_mapping_rule;
            {
                mocopi_mapping_rule.emplace(u8"root", u8"センター");
                mocopi_mapping_rule.emplace(u8"torso_7", u8"上半身");

                mocopi_mapping_rule.emplace(u8"neck_1", u8"首");
                mocopi_mapping_rule.emplace(u8"head", u8"頭");

                mocopi_mapping_rule.emplace(u8"l_shoulder", u8"左肩");
                mocopi_mapping_rule.emplace(u8"l_up_arm", u8"左腕");
                mocopi_mapping_rule.emplace(u8"l_low_arm", u8"左ひじ");
                mocopi_mapping_rule.emplace(u8"l_hand", u8"左手首");

                mocopi_mapping_rule.emplace(u8"r_shoulder", u8"右肩");
                mocopi_mapping_rule.emplace(u8"r_up_arm", u8"右腕");
                mocopi_mapping_rule.emplace(u8"r_low_arm", u8"右ひじ");
                mocopi_mapping_rule.emplace(u8"r_hand", u8"右手首");

                mocopi_mapping_rule.emplace(u8"l_up_leg", u8"左足");
                mocopi_mapping_rule.emplace(u8"l_low_leg", u8"左ひざ");
                mocopi_mapping_rule.emplace(u8"l_foot", u8"左足首");
                mocopi_mapping_rule.emplace(u8"l_toes", u8"左つま先");

                mocopi_mapping_rule.emplace(u8"r_up_leg", u8"右足");
                mocopi_mapping_rule.emplace(u8"r_low_leg", u8"右ひざ");
                mocopi_mapping_rule.emplace(u8"r_foot", u8"右足首");
                mocopi_mapping_rule.emplace(u8"r_toes", u8"右つま先");
            }
            //
            pxr::VtValue src_joints;
            pxr::VtValue dst_joints;
            {

                source_skel_skeleton.GetJointsAttr().Get(&src_joints);
                for (const auto& v : src_joints.Get<pxr::VtArray<pxr::TfToken>>())
                {
                    const std::string ts{ v.GetString() };
                    std::filesystem::path frag{ std::u8string{ ts.cbegin(), ts.cend() } };
                    if (mocopi_mapping_rule.contains(frag.filename().u8string()))
                    {
                        source_joints.emplace(frag.filename().u8string(), v);
                    }
                }
            }
            {
                target_skel_skeleton.GetJointsAttr().Get(&dst_joints);
                for (const auto& v : dst_joints.Get<pxr::VtArray<pxr::TfToken>>())
                {
                    const std::string ts{ v.GetString() };
                    std::filesystem::path frag{ std::u8string{ ts.cbegin(), ts.cend() } };
                    for (const auto& [m, t] : mocopi_mapping_rule)
                    {
                        if (t == frag.filename().u8string())
                        {
                            target_joints.emplace(frag.filename().u8string(), v);
                            break;
                        }
                    }
                }
            }
            if (source_joints.size() == mocopi_mapping_rule.size() &&
                target_joints.size() == mocopi_mapping_rule.size())
            {
                const auto& src = src_joints.Get<pxr::VtArray<pxr::TfToken>>();
                const auto& dst = dst_joints.Get<pxr::VtArray<pxr::TfToken>>();
                for (const auto& [from, to] : mocopi_mapping_rule)
                {
                    filtering_rule.emplace(
                        std::distance(src.cbegin(),
                                      std::find(src.cbegin(), src.cend(), source_joints[from])),
                        std::distance(dst.cbegin(),
                                      std::find(dst.cbegin(), dst.cend(), target_joints[to])));
                }
            }
            if (filtering_rule.size() == mocopi_mapping_rule.size())
            { // ok to copy

                // output_anim_stage
                auto root_prim = pxr::UsdGeomScope::Define(
                    output_anim_stage,
                    pxr::SdfPath{ "/" }.AppendChild(
                        shigenoy::mocopi_parser::retarget::tokens->skels));
                output_anim_stage->SetDefaultPrim(root_prim.GetPrim());

                auto skel_root = pxr::UsdSkelRoot::Define(
                    output_anim_stage,
                    root_prim.GetPath().AppendChild(pxr::TfToken{ "retargeted" }));

                auto motion = pxr::UsdSkelAnimation::Define(
                    output_anim_stage,
                    skel_root.GetPath().AppendChild(
                        shigenoy::mocopi_parser::retarget::tokens->motion));

                auto skeleton = pxr::UsdSkelSkeleton::Define(
                    output_anim_stage,
                    skel_root.GetPath().AppendChild(
                        shigenoy::mocopi_parser::retarget::tokens->skeleton));

                skeleton.GetPurposeAttr().Set(pxr::UsdGeomTokens->guide);

                pxr::UsdSkelBindingAPI{ skel_root }.GetAnimationSourceRel().AddTarget(
                    motion.GetPath());

                auto refs = skel_root.GetPrim().GetReferences();
                refs.AddReference(target_model_file_path.generic_string(),
                                  pxr::SdfPath{ target_model_skel_root_path });

                // just joints
                {
                    //std::vector<pxr::TfToken> joints;
                    //for (const auto& [from, to] : mocopiMappingRule)
                    //{
                    //    // sourceJoints.at(from)
                    //    // targetJoints.at(to)
                    //    joints.emplace_back(targetJoints.at(to));
                    //}

                    ////motion.GetJointsAttr().Set(pxr::VtArray<pxr::TfToken>(joints.cbegin(), joints.cend()));
                    ////motion.GetScalesAttr().Set(pxr::VtArray<pxr::GfVec3h>(joints.size(), { 1, 1, 1 }));

                    motion.GetJointsAttr().Set(dst_joints.Get<pxr::VtArray<pxr::TfToken>>());
                    motion.GetScalesAttr().Set(
                        pxr::VtArray<pxr::GfVec3h>(dst_joints.GetArraySize(), { 1, 1, 1 }));
                }
                pxr::UsdSkelAnimation ssas{ source_skel_anim_source };
                // rotations
                {
                    std::vector<double> timecodes;
                    ssas.GetRotationsAttr().GetTimeSamples(&timecodes);
                    for (const auto& timecode : timecodes)
                    {
                        pxr::VtValue v;
                        ssas.GetRotationsAttr().Get(&v, timecode);

                        const auto& arr = v.Get<pxr::VtArray<pxr::GfQuatf>>();
                        pxr::VtArray<pxr::GfQuatf> filtered{ dst_joints.GetArraySize(),
                                                             pxr::GfQuatf::GetIdentity() };
                        // filtered.reserve(dst_joints.GetArraySize());
                        for (const auto& [srcIdx, dstIdx] : filtering_rule)
                        {
                            filtered[dstIdx] = arr[srcIdx];
                        }
                        motion.GetRotationsAttr().Set(filtered, timecode);
                    }
                }
                // translations
                {
                    pxr::VtArray<pxr::GfMatrix4d> rest_transforms;
                    pxr::VtArray<pxr::GfVec3f> base;

                    target_skel_skeleton.GetRestTransformsAttr().Get(&rest_transforms);
                    for (const auto& mat : rest_transforms)
                    {
                        base.emplace_back(mat.ExtractTranslation());
                    }

                    std::vector<double> timecodes;
                    ssas.GetTranslationsAttr().GetTimeSamples(&timecodes);
                    for (const auto& timecode : timecodes)
                    {
                        pxr::VtValue v;
                        ssas.GetTranslationsAttr().Get(&v, timecode);

                        const auto& arr = v.Get<pxr::VtArray<pxr::GfVec3f>>();
                        pxr::VtArray<pxr::GfVec3f> filtered{ base };
                        // filtered.reserve(dst_joints.GetArraySize());
                        // for (const auto& [srcIdx, dstIdx] : filteringRule)
                        // {
                        //     filtered[dstIdx] = arr[srcIdx];
                        // }
                        filtered[filtering_rule.begin()->second] =
                            arr[filtering_rule.begin()->first];
                        motion.GetTranslationsAttr().Set(filtered, timecode);
                    }
                }

                output_anim_stage->Export(output_anim_file_path.generic_string());
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}
