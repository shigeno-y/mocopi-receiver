// SPDX-License-Identifier: MIT

#include <shigenoy/mocopi_parser/Container.hpp>
#include <shigenoy/mocopi_parser/Generator.hpp>
#include <shigenoy/mocopi_parser/Skeleton.hpp>

#include <filesystem>
#include <iostream>
#include <vector>

#include <CLI/CLI.hpp>

int
main(int argc, char* argv[])
{
    try
    {
        CLI::App app;

        std::filesystem::path packet_file_path{};
        app.add_option("packet", packet_file_path, "raw packet file path");
        app.allow_windows_style_options();

        try
        {
            app.parse(argc, argv);
        }
        catch (const CLI::ParseError& e)
        {
            return app.exit(e);
        }

        if (std::filesystem::exists(packet_file_path))
        {
            std::ifstream ifs{ packet_file_path, std::ios::in | std::ios::binary };
            std::vector<std::byte> buf;

            ifs.seekg(0, std::ios::end);
            buf.resize(ifs.tellg());
            ifs.seekg(0, std::ios::beg);

            ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
            if (ifs)
            {
                const shigenoy::mocopi_parser::ParsedMocopiPacket p{ buf };
                std::vector<pxr::TfToken> joints;

                if (p.hasBoneDefinition())
                {
                    // Bone Definition
                    for (const auto& b : shigenoy::mocopi_parser::readBoneDefinitions(p))
                    {
                        std::cout << "Bone#" << b.bnid_;
                        std::cout << "\tParent#" << b.pbid_;
                        std::cout << "\t(quat{X, Y, Z, W}, tran{X, Y, Z})=({";
                        std::cout << b.quat_x_ << ", ";
                        std::cout << b.quat_y_ << ", ";
                        std::cout << b.quat_z_ << ", ";
                        std::cout << b.quat_w_ << "}, {";
                        std::cout << b.tran_x_ << ", ";
                        std::cout << b.tran_y_ << ", ";
                        std::cout << b.tran_z_ << "})\n";
                    }
                    {
                        auto stage     = pxr::UsdStage::CreateInMemory();
                        auto skel_root = pxr::UsdSkelRoot::Define(stage, pxr::SdfPath{ "/Skel" });
                        stage->SetDefaultPrim(skel_root.GetPrim());

                        shigenoy::mocopi_parser::generateSkelRoot(stage, skel_root, joints, p);
                        stage->Flatten()->Export(
                            std::filesystem::path{ "T:/hogehoge.usda" }.generic_string());
                    }
                }
                else if (p.hasFrameData())
                {
                    // Frame Data
                    for (const auto& b : shigenoy::mocopi_parser::readBoneTransforms(p))
                    {
                        std::cout << "Bone#" << b.bnid_;
                        std::cout << "\t(quat{X, Y, Z, W}, tran{X, Y, Z})=({";
                        std::cout << b.quat_x_ << ", ";
                        std::cout << b.quat_y_ << ", ";
                        std::cout << b.quat_z_ << ", ";
                        std::cout << b.quat_w_ << "}, {";
                        std::cout << b.tran_x_ << ", ";
                        std::cout << b.tran_y_ << ", ";
                        std::cout << b.tran_z_ << "})\n";
                    }
                    {
                        auto stage     = pxr::UsdStage::CreateInMemory();
                        auto skel_root = pxr::UsdSkelRoot::Define(stage, pxr::SdfPath{ "/Skel" });
                        stage->SetDefaultPrim(skel_root.GetPrim());

                        shigenoy::mocopi_parser::generateSkelAnim(stage, skel_root, joints, p);
                        stage->Flatten()->Export(
                            std::filesystem::path{ "T:/hogehoge.usda" }.generic_string());
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
