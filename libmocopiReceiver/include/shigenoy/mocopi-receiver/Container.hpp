// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_DATA_Container_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_DATA_Container_HPP

#    include <bit>
#    include <cstdint>
#    include <span>
#    include <unordered_map>

namespace shigenoy::mocopi_receiver {
template <size_t Length>
struct CCCode
{
public:
    char buf_[Length] = {};
    consteval CCCode(const char (&code)[Length + 1])
    {
        for (auto i = 0; i < Length; i++)
        {
            this->buf_[i] = code[i];
        }
    }
    consteval operator std::uint32_t() const
    {
        return (this->buf_[0] << 0) | (this->buf_[1] << 8) | (this->buf_[2] << 16) |
               (this->buf_[3] << 24);
    }
};

template <size_t Length>
CCCode(const char (&)[Length]) -> CCCode<Length - 1>;

class Container
{
public:
    std::span<const std::byte, 4> length;
    std::span<const std::byte, 4> type;
    std::span<const std::byte> payload;
    std::unordered_multimap<std::uint32_t, Container> children_;

public:
    Container(const std::span<const std::byte, std::dynamic_extent>& raw);
    Container(const Container&) = default;
    Container(Container&&)      = default;
    ~Container()                = default;

    Container& operator=(const Container&) = default;
    Container& operator=(Container&&)      = default;

public:
    std::uint32_t getLength() const;
    std::uint32_t getType() const;
    size_t getSize() const;

    template <typename T>
    const T readAs(const size_t offset) const
    {
        return std::bit_cast<T>(*(reinterpret_cast<const T*>(
            reinterpret_cast<const void*>(this->payload.data() + offset))));
    }
};

class ParsedMocopiPacket
{
public:
    std::unordered_multimap<std::uint32_t, Container> parsed_;

public:
    ParsedMocopiPacket()                                     = default;
    ParsedMocopiPacket(const ParsedMocopiPacket&)            = default;
    ParsedMocopiPacket(ParsedMocopiPacket&&)                 = default;
    ~ParsedMocopiPacket()                                    = default;
    ParsedMocopiPacket& operator=(const ParsedMocopiPacket&) = default;
    ParsedMocopiPacket& operator=(ParsedMocopiPacket&&)      = default;
};
} // namespace shigenoy::mocopi_receiver

#endif
