// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi-receiver/Container.hpp>

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
