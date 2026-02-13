// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_PARSER_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_PARSER_HPP

#    include <shigenoy/mocopi-receiver/Data.hpp>

#    include <vector>

namespace shigenoy::mocopi_receiver {
bool isBoneDefinition(const std::vector<char>& raw);
bool isFrameData(const std::vector<char>& raw);

BoneDefinition parseBoneDefinition(const std::vector<char>& raw);
FrameData parseFrameData(const std::vector<char>& raw);
} // namespace shigenoy::mocopi_receiver

#endif
