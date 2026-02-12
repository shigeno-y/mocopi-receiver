#include <shigenoy/mocopi-receiver/Data/Block.hpp>

shigenoy::mocopi_receiver::Block::Block(const std::vector<std::uint8_t>& raw, size_t offset)
    : original_buffer_(raw)
    , offset_(offset)
    , length_(
          (this->original_buffer_[offset + 0] << 0) | (this->original_buffer_[offset + 1] << 8) |
          (this->original_buffer_[offset + 2] << 16) | (this->original_buffer_[offset + 3] << 24))
{
    this->type_[0] = static_cast<char>(this->original_buffer_[offset + 4]);
    this->type_[1] = static_cast<char>(this->original_buffer_[offset + 5]);
    this->type_[2] = static_cast<char>(this->original_buffer_[offset + 6]);
    this->type_[3] = static_cast<char>(this->original_buffer_[offset + 7]);
}

size_t
shigenoy::mocopi_receiver::Block::length() const
{
    return 8 + this->length_;
}

void
shigenoy::mocopi_receiver::Block::parse()
{
}
