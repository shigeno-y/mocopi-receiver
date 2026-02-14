// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_PARSER_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_PARSER_HPP

#    include <shigenoy/mocopi-receiver/Container.hpp>

#    include <vector>

namespace shigenoy::mocopi_receiver {
ParsedMocopiPacket parse(const std::vector<std::byte>& raw);
} // namespace shigenoy::mocopi_receiver

#endif
