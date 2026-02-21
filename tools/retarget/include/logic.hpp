// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_TOOLS_RETARGET_LOGIC_HPP)
#    define SHIGENOY_MOCOPIPARSER_TOOLS_RETARGET_LOGIC_HPP

#    include <filesystem>
#    include <string>

namespace shigenoy::mocopi_parser::retarget {
bool phase1(const std::filesystem::path& source_anim_file_path_,
            const std::filesystem::path& target_model_file_path_,
            const std::filesystem::path& output_anim_file_path_,
            std::string& source_anim_skel_root_path_,
            std::string& target_model_skel_root_path_);
} // namespace shigenoy::mocopi_parser::retarget

#endif
