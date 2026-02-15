// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi-receiver/Container.hpp>

#include <unordered_set>

shigenoy::mocopi_receiver::Container::Container(const std::span<const std::byte>& raw)
    : length(raw.subspan(0, 4))
    , type(raw.subspan(4, 4))
    , payload(raw.subspan(
          8, std::bit_cast<std::uint32_t>(*reinterpret_cast<const std::uint32_t*>(length.data()))))
{
}

std::uint32_t
shigenoy::mocopi_receiver::Container::getLength() const
{
    return std::bit_cast<std::uint32_t>(*reinterpret_cast<const std::uint32_t*>(length.data()));
}

std::uint32_t
shigenoy::mocopi_receiver::Container::getType() const
{
    return std::bit_cast<std::uint32_t>(*reinterpret_cast<const std::uint32_t*>(type.data()));
}

size_t
shigenoy::mocopi_receiver::Container::getSize() const
{
    return this->getLength() + 8;
}

namespace {
static const std::unordered_set<std::uint32_t> parents{
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::HEAD),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::SNDF),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::SKDF),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::BONS),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::BNDT),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::FRAM),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::BTRS),
    static_cast<std::uint32_t>(shigenoy::mocopi_receiver::wellknown_code::BTDT),
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
            const auto& emp = ret.emplace(c.getType(), c);
            parent.children_.emplace(emp->second.getType(), emp->second);

            offset += c.getSize();
        }
    }
}
} // namespace

shigenoy::mocopi_receiver::ParsedMocopiPacket::ParsedMocopiPacket(const std::vector<std::byte>& raw)
    : raw_(raw)
{
    std::span buffer{ this->raw_.cbegin(), this->raw_.cend() };
    size_t offset{ 0 };

    while (offset < this->raw_.size())
    {
        Container c{ buffer.subspan(offset, this->raw_.size() - offset) };
        parseChildren(this->parsed_, c);
        this->parsed_.emplace(c.getType(), c);
        offset += c.getSize();
    }
}
