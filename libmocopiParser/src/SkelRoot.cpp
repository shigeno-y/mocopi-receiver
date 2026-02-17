// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi_parser/Skeleton.hpp>

#include <shigenoy/mocopi_parser/Generator.hpp>

#include <array>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "pxr/base/gf/matrix4d.h"
#include "pxr/usd/usdSkel/bindingAPI.h"

namespace {
const std::array<std::u8string, 27> bone_names{
    u8"root",      u8"torso_1",   u8"torso_2",   u8"torso_3",    u8"torso_4",  u8"torso_5",
    u8"torso_6",   u8"torso_7",   u8"neck_1",    u8"neck_2",     u8"head",     u8"l_shoulder",
    u8"l_up_arm",  u8"l_low_arm", u8"l_hand",    u8"r_shoulder", u8"r_up_arm", u8"r_low_arm",
    u8"r_hand",    u8"l_up_leg",  u8"l_low_leg", u8"l_foot",     u8"l_toes",   u8"r_up_leg",
    u8"r_low_leg", u8"r_foot",    u8"r_toes",
};
}

namespace {
class Node
{
public:
    const std::uint16_t parent_id;
    const std::uint16_t myself_id;
    const std::u8string bone_name;
    const pxr::GfMatrix4d global_to_self_transform;

    std::filesystem::path mapped_name;
    std::vector<Node> children;

public:
    Node(const std::uint16_t& mmd_parent_id,
         const std::uint16_t& mmd_myself_id,
         const std::u8string& mmd_bone_name_jp,
         const pxr::GfMatrix4d& global_to_self_transform);

    Node(const Node& n)          = default;
    Node(Node&& n)               = default;
    ~Node()                      = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&)      = default;

    bool add(const std::uint16_t& mmd_parent_id,
             const std::uint16_t& mmd_myself_id,
             const std::u8string& mmd_bone_name_jp,
             const pxr::GfMatrix4d& global_to_self_transform);
};

Node::Node(const std::uint16_t& mmd_parent_id,
           const std::uint16_t& mmd_myself_id,
           const std::u8string& mmd_bone_name_jp,
           const pxr::GfMatrix4d& global_to_self_transform)
    : parent_id(mmd_parent_id)
    , myself_id(mmd_myself_id)
    , bone_name(mmd_bone_name_jp)
    , global_to_self_transform(global_to_self_transform)
{
}

bool
Node::add(const std::uint16_t& parent,
          const std::uint16_t& idx,
          const std::u8string& mmd_bone_name_jp,
          const pxr::GfMatrix4d& global_to_self_transform)
{
    constexpr std::uint16_t INHERIT_ROTATION_FLAG{ 1 << 8 };
    constexpr std::uint16_t INHERIT_TRANSLATION_FLAG{ 1 << 9 };

    if (this->myself_id == parent)
    {
        this->children.emplace_back(parent, idx, mmd_bone_name_jp, global_to_self_transform);
        return true;
    }
    else
    {
        for (auto& p : this->children)
        {
            if (p.add(parent, idx, mmd_bone_name_jp, global_to_self_transform))
            {
                return true;
            }
        }
    }
    return false;
}

Node
buildForest(const std::set<shigenoy::mocopi_parser::BoneDefinition>& bones)
{
    const auto& root_bd = bones.begin();

    Node root(root_bd->pbid_,             //
              root_bd->bnid_,             //
              bone_names[root_bd->bnid_], //
              pxr::GfMatrix4d{ 1.,
                               0.,
                               0.,
                               0.,
                               0.,
                               1.,
                               0.,
                               0.,
                               0.,
                               0.,
                               1.,
                               0.,
                               root_bd->tran_x_,
                               root_bd->tran_y_,
                               root_bd->tran_z_,
                               1.0 });

    {
        std::set<shigenoy::mocopi_parser::BoneDefinition> rest_of_tree;
        bool begin_skip{ true };
        for (const auto& bone : bones)
        {
            if (begin_skip)
            {
                begin_skip = false;
                continue;
            }
            if (!root.add(bone.pbid_,
                          bone.bnid_,
                          bone_names[bone.bnid_],
                          pxr::GfMatrix4d{ 1.,
                                           0.,
                                           0.,
                                           0.,
                                           0.,
                                           1.,
                                           0.,
                                           0.,
                                           0.,
                                           0.,
                                           1.,
                                           0.,
                                           bone.tran_x_,
                                           bone.tran_y_,
                                           bone.tran_z_,
                                           1.0 }))
            {
                rest_of_tree.emplace(bone.pbid_);
            }
        }
        while (!rest_of_tree.empty())
        {
            std::vector<shigenoy::mocopi_parser::BoneDefinition> removal;
            for (auto& bone : rest_of_tree)
            {
                if (root.add(bone.pbid_,
                             bone.bnid_,
                             bone_names[root_bd->bnid_],
                             pxr::GfMatrix4d{ 1.,
                                              0.,
                                              0.,
                                              0.,
                                              0.,
                                              1.,
                                              0.,
                                              0.,
                                              0.,
                                              0.,
                                              1.,
                                              0.,
                                              bone.tran_x_,
                                              bone.tran_y_,
                                              bone.tran_z_,
                                              1.0 }))
                {
                    removal.emplace_back(bone);
                }
            }
            for (const auto& idx : removal)
            {
                rest_of_tree.erase(idx);
            }
        }
    }

    return root;
}

void
generateSkelData(const Node& root,
                 std::vector<pxr::GfMatrix4d>& bind_transforms,
                 std::vector<pxr::TfToken>& joint_names,
                 std::vector<pxr::TfToken>& joints,
                 std::vector<pxr::GfMatrix4d>& rest_transforms)
{
    std::u8string current_prefix{ u8"/" };
    try
    {
        bind_transforms.emplace_back(root.global_to_self_transform);
        rest_transforms.emplace_back(root.global_to_self_transform);
        // * global_to_pnt_local_transform.GetInverse());

        {
            const auto& path = root.mapped_name.filename().generic_u8string();
            joint_names.emplace_back(pxr::TfToken{ std::string{ path.cbegin(), path.cend() } });
        }
        {
            const auto& path = root.mapped_name.generic_u8string();
            joints.emplace_back(pxr::TfToken{ std::string{ path.cbegin(), path.cend() } });
        }

        for (const auto& c : root.children)
        {
            generateSkelData(c, bind_transforms, joint_names, joints, rest_transforms);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
}

void
resolveInnerMappedName(const std::filesystem::path& prefix, Node& root)
{
    root.mapped_name = prefix / root.bone_name;

    for (auto& c : root.children)
    {
        resolveInnerMappedName(root.mapped_name, c);
    }
}

void
buildMappedName(Node& root)
{
    root.mapped_name = std::filesystem::path{ root.bone_name };
    for (auto& c : root.children)
    {
        resolveInnerMappedName(root.mapped_name, c);
    }
}
} // namespace

void
shigenoy::mocopi_parser::generateSkelRoot(pxr::UsdStageRefPtr stage,
                                          pxr::UsdSkelRoot& skel_root,
                                          std::vector<pxr::TfToken>& joints,
                                          const ParsedMocopiPacket& bone_definition_packet)
{
    std::vector<pxr::GfMatrix4d> bind_transforms;
    std::vector<pxr::TfToken> joint_names;
    // std::vector<pxr::TfToken> joints;
    std::vector<pxr::GfMatrix4d> rest_transforms;

    std::set<BoneDefinition> bones;
    for (const auto& b : readBoneDefinitions(bone_definition_packet))
    {
        bones.emplace(b);
    }

    Node root = buildForest(bones);
    buildMappedName(root);
    generateSkelData(root, bind_transforms, joint_names, joints, rest_transforms);

    {
        auto skeleton = pxr::UsdSkelSkeleton::Define(
            stage, skel_root.GetPath().AppendChild(pxr::TfToken{ "Skeleton" }));

        skeleton.GetPurposeAttr().Set(pxr::UsdGeomTokens->guide);
        skeleton.GetPrim()
            .GetRelationship(pxr::TfToken{ "skel:animationSource" })
            .SetTargets({ skel_root.GetPath().AppendChild(pxr::TfToken{ "Motion" }) });

        {
            const auto& arr = bind_transforms;
            skeleton.GetBindTransformsAttr().Set(
                pxr::VtArray<pxr::GfMatrix4d>(arr.cbegin(), arr.cend()));
        }
        {
            const auto& arr = joint_names;
            skeleton.GetJointNamesAttr().Set(pxr::VtArray<pxr::TfToken>(arr.cbegin(), arr.cend()));
        }
        {
            const auto& arr = joints;
            skeleton.GetJointsAttr().Set(pxr::VtArray<pxr::TfToken>(arr.cbegin(), arr.cend()));
        }
        {
            const auto& arr = rest_transforms;
            skeleton.GetRestTransformsAttr().Set(
                pxr::VtArray<pxr::GfMatrix4d>(arr.cbegin(), arr.cend()));
        }

        auto skel_binding = pxr::UsdSkelBindingAPI::Apply(skel_root.GetPrim());
        skel_binding.GetSkeletonRel().AddTarget(skeleton.GetPath());
    }
}
