// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_DATA_Container_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_DATA_Container_HPP

#    include <bit>
#    include <cstdint>
#    include <functional>
#    include <span>
#    include <unordered_map>
#    include <vector>

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

inline namespace wellknown_code {
// /head
constexpr shigenoy::mocopi_receiver::CCCode HEAD{ "head" };
// /head/ftyp
constexpr shigenoy::mocopi_receiver::CCCode FTYP{ "ftyp" };
// /head/vrsn
constexpr shigenoy::mocopi_receiver::CCCode VRSN{ "vrsn" };
// /sndf
constexpr shigenoy::mocopi_receiver::CCCode SNDF{ "sndf" };
// /sndf/ipad
constexpr shigenoy::mocopi_receiver::CCCode IPAD{ "ipad" };
// /sndf/rcvp
constexpr shigenoy::mocopi_receiver::CCCode RCVP{ "rcvp" };
// /skdf/bons/bndt/bnid
// /fram/btrs/btdt/bnid
constexpr shigenoy::mocopi_receiver::CCCode BNID{ "bnid" };
// /skdf/bons/bndt/tran
// /fram/btrs/btdt/tran
constexpr shigenoy::mocopi_receiver::CCCode TRAN{ "tran" };
// /skdf
constexpr shigenoy::mocopi_receiver::CCCode SKDF{ "skdf" };
// /skdf/bons
constexpr shigenoy::mocopi_receiver::CCCode BONS{ "bons" };
// /skdf/bons/bndt
constexpr shigenoy::mocopi_receiver::CCCode BNDT{ "bndt" };
// /skdf/bons/bndt/pbid
constexpr shigenoy::mocopi_receiver::CCCode PBID{ "pbid" };
// /fram
constexpr shigenoy::mocopi_receiver::CCCode FRAM{ "fram" };
// /fram/fnum
constexpr shigenoy::mocopi_receiver::CCCode FNUM{ "fnum" };
// /fram/time
constexpr shigenoy::mocopi_receiver::CCCode TIME{ "time" };
// /fram/uttm
constexpr shigenoy::mocopi_receiver::CCCode UTTM{ "uttm" };
// /fram/tmcd
constexpr shigenoy::mocopi_receiver::CCCode TMCD{ "tmcd" };
// /fram/btrs
constexpr shigenoy::mocopi_receiver::CCCode BTRS{ "btrs" };
// /fram/btrs/btdt
constexpr shigenoy::mocopi_receiver::CCCode BTDT{ "btdt" };
} // namespace wellknown_code

class Container
{
public:
    std::span<const std::byte, 4> length;
    std::span<const std::byte, 4> type;
    std::span<const std::byte> payload;
    std::unordered_multimap<std::uint32_t, std::reference_wrapper<Container>> children_;

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
    const std::vector<std::byte>& raw_;
    std::unordered_multimap<std::uint32_t, Container> parsed_;

public:
    ParsedMocopiPacket(const std::vector<std::byte>& raw);
    ParsedMocopiPacket(const ParsedMocopiPacket&)            = default;
    ParsedMocopiPacket(ParsedMocopiPacket&&)                 = default;
    ~ParsedMocopiPacket()                                    = default;
    ParsedMocopiPacket& operator=(const ParsedMocopiPacket&) = default;
    ParsedMocopiPacket& operator=(ParsedMocopiPacket&&)      = default;
};
} // namespace shigenoy::mocopi_receiver

#endif
