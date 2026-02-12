#if !defined(SHIGENOY_MOCOPIRECEIVER_DATA_BLOCK_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_DATA_BLOCK_HPP

#    include <array>
#    include <cstdint>
#    include <vector>

namespace shigenoy::mocopi_receiver {
class Block
{
protected:
    // from parent
    const std::vector<std::uint8_t>& original_buffer_;
    size_t offset_;

    // from packet
    std::uint32_t length_;
    std::array<char, 4> type_;
    std::vector<Block> children_;

public:
    Block(const std::vector<std::uint8_t>& raw, size_t offset);
    Block(const Block&) = delete;
    Block(Block&&)      = default;
    virtual ~Block()    = default;

    Block& operator=(const Block&) &     = delete;
    Block& operator=(Block&&) & noexcept = default;

public:
    size_t length() const;

public:
    virtual void parse();
};
} // namespace shigenoy::mocopi_receiver

#endif
