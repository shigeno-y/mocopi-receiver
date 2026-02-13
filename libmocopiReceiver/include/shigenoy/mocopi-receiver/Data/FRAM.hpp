// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_DATA_FRAM_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_DATA_FRAM_HPP

#    include <shigenoy/mocopi-receiver/Data/Common.hpp>
#    include <shigenoy/mocopi-receiver/Data/Container.hpp>

namespace shigenoy::mocopi_receiver {
using Fram = Container<1520>;
using Fnum = Container<4>;
using Time = Container<4>;
using Uttm = Container<8>;
using Tmcd = Container<6>;
using Btrs = Container<1458>;
using Btdt = Container<46>;
} // namespace shigenoy::mocopi_receiver

#endif
