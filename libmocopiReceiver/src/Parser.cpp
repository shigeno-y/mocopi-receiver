// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi-receiver/Parser.hpp>

#include <span>
#include <unordered_map>
#include <unordered_set>

namespace {
static const std::unordered_set<std::uint32_t> parents{
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "head" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "sndf" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "skdf" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "bons" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "bndt" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "fram" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "btrs" }),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::CCCode{ "btdt" }),
};

void
parseChildren(std::unordered_multimap<std::uint32_t, shigenoy::mocopi_receiver::Container>& ret,
              shigenoy::mocopi_receiver::Container& parent)
{
    if (parents.contains(parent.getType()))
    {
        size_t offset{ 0 };
        const auto& buffer = parent.payload;
        while (offset < buffer.size())
        {
            shigenoy::mocopi_receiver::Container c{ buffer.subspan(offset,
                                                                   buffer.size() - offset) };
            parseChildren(ret, c);
            parent.children_.emplace(c.getType(), c);
            ret.emplace(c.getType(), c);
            offset += c.getSize();
        }
    }
}
} // namespace

shigenoy::mocopi_receiver::ParsedMocopiPacket
shigenoy::mocopi_receiver::parse(const std::vector<std::byte>& raw)
{
    ParsedMocopiPacket ret;
    std::span buffer{ raw.cbegin(), raw.cend() };
    size_t offset{ 0 };

    while (offset < raw.size())
    {
        Container c{ buffer.subspan(offset, raw.size() - offset) };
        parseChildren(ret.parsed_, c);
        ret.parsed_.emplace(c.getType(), c);
        offset += c.getSize();
    }

    return ret;
}
