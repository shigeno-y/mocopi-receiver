// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_DATA_Container_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_DATA_Container_HPP

#    include <array>
#    include <bit>
#    include <cstdint>

namespace shigenoy::mocopi_receiver {
template <size_t payload_size>
class Container
{
public:
    std::uint32_t length;
    std::array<char, 4> type;
    std::array<char, payload_size> payload;

public:
    static constexpr size_t Size{ sizeof(decltype(length)) + sizeof(decltype(type)) +
                                  sizeof(decltype(payload)) };
    static constexpr size_t PayloadLength{ payload_size };

public:
    template <typename T>
    const T parseAs(const size_t offset) const
    {
        return std::bit_cast<T>(*(reinterpret_cast<const T*>(
            reinterpret_cast<const void*>(this->payload.data() + offset))));
    }
};
} // namespace shigenoy::mocopi_receiver

#endif
