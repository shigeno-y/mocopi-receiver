// SPDX-License-Identifier: MIT

#include "logic.hpp"

#include <filesystem>
#include <string>

#include <CLI/CLI.hpp>

int
main(int argc, char* argv[])
{
    {
        std::system("chcp.com 65001");
    }
    CLI::App app;

    std::filesystem::path source_anim_file_path{};
    std::filesystem::path target_model_file_path{};
    std::filesystem::path output_anim_file_path{};

    std::string source_anim_skel_root_path{};
    std::string target_model_skel_root_path{};

    app.add_option("source_file_path", source_anim_file_path, "source animation file path");
    app.add_option("target_file_path", target_model_file_path, "target model file path");
    app.add_option("output_file_path", output_anim_file_path, "outpu animation file path");

    app.add_option("--source-skelroot", source_anim_skel_root_path, "target model SkelRoot path");
    app.add_option("--target-skelroot", target_model_skel_root_path, "target model SkelRoot path");
    app.allow_windows_style_options();

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
    }

    shigenoy::mocopi_parser::retarget::phase1(source_anim_file_path,
                                              target_model_file_path,
                                              output_anim_file_path,
                                              source_anim_skel_root_path,
                                              target_model_skel_root_path);
    return 0;
}
