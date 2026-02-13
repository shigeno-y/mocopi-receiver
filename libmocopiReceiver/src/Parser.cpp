// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi-receiver/Parser.hpp>

#include <bit>
#include <string_view>

namespace {
using TypeChecker = shigenoy::mocopi_receiver::Container<0>;

template <typename T>
T
parseAs(const char* ptr, const size_t offset)
{
    return std::bit_cast<T>(*(reinterpret_cast<const T*>(ptr + offset)));
}

bool
canParseAs(const char* ptr, const size_t offset, const char* cccode)
{
    const auto& test = parseAs<TypeChecker>(ptr, offset);
    return std::string_view{ test.type.data(), test.type.size() } == std::string_view{ cccode };
}
} // namespace

bool
shigenoy::mocopi_receiver::isBoneDefinition(const std::vector<char>& raw)
{
    constexpr size_t offset{ shigenoy::mocopi_receiver::Head::Size +
                             shigenoy::mocopi_receiver::Sndf::Size };

    const auto& test =
        std::bit_cast<TypeChecker>(*(reinterpret_cast<const TypeChecker*>(raw.data() + offset)));

    return std::string_view{ test.type.data(), test.type.size() } == std::string_view{ "skdf" };
}

bool
shigenoy::mocopi_receiver::isFrameData(const std::vector<char>& raw)
{
    constexpr size_t offset{ shigenoy::mocopi_receiver::Head::Size +
                             shigenoy::mocopi_receiver::Sndf::Size };

    const auto& test =
        std::bit_cast<TypeChecker>(*(reinterpret_cast<const TypeChecker*>(raw.data() + offset)));

    return std::string_view{ test.type.data(), test.type.size() } == std::string_view{ "fram" };
}

shigenoy::mocopi_receiver::BoneDefinition
shigenoy::mocopi_receiver::parseBoneDefinition(const std::vector<char>& raw)
{
    BoneDefinition ret{};
    size_t offset{ 0 };

    ret.head_ = parseAs<decltype(ret.head_)>(raw.data(), offset);
    offset += decltype(ret.head_)::Size;
    {
        ret.ftyp_ = ret.head_.parseAs<decltype(ret.ftyp_)>(0);
        ret.vrsn_ = ret.head_.parseAs<decltype(ret.vrsn_)>(decltype(ret.ftyp_)::Size);
    }

    ret.sndf_ = parseAs<decltype(ret.sndf_)>(raw.data(), offset);
    offset += decltype(ret.sndf_)::Size;
    {
        ret.ipad_ = ret.sndf_.parseAs<decltype(ret.ipad_)>(0);
        ret.rcvp_ = ret.sndf_.parseAs<decltype(ret.rcvp_)>(decltype(ret.ipad_)::Size);
    }

    ret.skdf_ = parseAs<decltype(ret.skdf_)>(raw.data(), offset);
    offset += decltype(ret.skdf_)::Size;
    {
        ret.bons_ = ret.skdf_.parseAs<decltype(ret.bons_)>(0);
        for (auto idx = 0; idx < ret.bndt_.size(); ++idx)
        {
            ret.bndt_[idx] = ret.bons_.parseAs<Bndt>(Bndt::Size * idx);
        }
    }

    return ret;
}

shigenoy::mocopi_receiver::FrameData
shigenoy::mocopi_receiver::parseFrameData(const std::vector<char>& raw)
{
    FrameData ret{};
    size_t offset{ 0 };

    ret.head_ = parseAs<decltype(ret.head_)>(raw.data(), offset);
    offset += decltype(ret.head_)::Size;
    {
        ret.ftyp_ = ret.head_.parseAs<decltype(ret.ftyp_)>(0);
        ret.vrsn_ = ret.head_.parseAs<decltype(ret.vrsn_)>(decltype(ret.ftyp_)::Size);
    }
    ret.sndf_ = parseAs<decltype(ret.sndf_)>(raw.data(), offset);
    offset += decltype(ret.sndf_)::Size;
    {
        ret.ipad_ = ret.sndf_.parseAs<decltype(ret.ipad_)>(0);
        ret.rcvp_ = ret.sndf_.parseAs<decltype(ret.rcvp_)>(decltype(ret.ipad_)::Size);
    }

    ret.fram_ = parseAs<decltype(ret.fram_)>(raw.data(), offset);
    offset += decltype(ret.fram_)::Size;
    {
        size_t i_offset{ 0 };
        ret.fnum_ = ret.fram_.parseAs<decltype(ret.fnum_)>(i_offset);
        i_offset += decltype(ret.fnum_)::Size;

        ret.time_ = ret.fram_.parseAs<decltype(ret.time_)>(i_offset);
        i_offset += decltype(ret.time_)::Size;

        ret.uttm_ = ret.fram_.parseAs<decltype(ret.uttm_)>(i_offset);
        i_offset += decltype(ret.uttm_)::Size;

        if (canParseAs(ret.fram_.payload.data(), i_offset, "tmcd"))
        {
            ret.tmcd_ = ret.fram_.parseAs<decltype(ret.tmcd_)>(i_offset);
            i_offset += decltype(ret.tmcd_)::Size;
        }
        ret.btrs_ = ret.fram_.parseAs<decltype(ret.btrs_)>(i_offset);
        i_offset += decltype(ret.btrs_)::Size;

        for (auto idx = 0; idx < ret.btdt_.size(); ++idx)
        {
            ret.btdt_[idx] = ret.btrs_.parseAs<Btdt>(Btdt::Size * idx);
        }
    }

    return ret;
}
