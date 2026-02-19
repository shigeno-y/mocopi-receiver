// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_RECEIVER_HPP)
#    define SHIGENOY_MOCOPIPARSER_RECEIVER_HPP

#    include <shigenoy/mocopi_parser/Container.hpp>

#    include "oneapi/tbb/concurrent_queue.h"
#    include "oneapi/tbb/concurrent_unordered_map.h"

#    include <cstdint>
#    include <string>

namespace shigenoy::mocopi_parser {
void receiveMocopiUdp(
    const std::string& listenAddr,
    std::uint16_t listenPort,
    oneapi::tbb::concurrent_unordered_map<
        std::string,
        oneapi::tbb::concurrent_queue<shigenoy::mocopi_parser::ParsedMocopiPacket>>& queues);
} // namespace shigenoy::mocopi_parser
#endif
