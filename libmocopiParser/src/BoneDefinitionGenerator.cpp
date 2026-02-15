// SPDX-License-Identifier: MIT
#include <shigenoy/mocopi_parser/Generator.hpp>

auto
shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type::get_return_object()
{
    return BoneDefinitionGenerator{ *this };
}

auto
shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type::initial_suspend()
{
    return std::suspend_always{};
}

auto
shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type::final_suspend() noexcept
{
    return std::suspend_always{};
}

auto
shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type::yield_value(BoneDefinition v)
{
    value_ = v;
    return std::suspend_always{};
}

void
shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type::return_void()
{
}

void
shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type::unhandled_exception()
{
    std::terminate();
}

shigenoy::mocopi_parser::BoneDefinitionGenerator::iterator&
shigenoy::mocopi_parser::BoneDefinitionGenerator::iterator::operator++()
{
    coro_.resume();
    done_ = coro_.done();
    return *this;
}

bool
shigenoy::mocopi_parser::BoneDefinitionGenerator::iterator::operator!=(const iterator& rhs) const
{
    return done_ != rhs.done_;
}

shigenoy::mocopi_parser::BoneDefinition
shigenoy::mocopi_parser::BoneDefinitionGenerator::iterator::operator*() const
{
    return coro_.promise().value_;
}

shigenoy::mocopi_parser::BoneDefinitionGenerator::BoneDefinitionGenerator(
    shigenoy::mocopi_parser::BoneDefinitionGenerator::promise_type& p)
    : coro_(coro_handle::from_promise(p))
{
}

shigenoy::mocopi_parser::BoneDefinitionGenerator::BoneDefinitionGenerator(
    BoneDefinitionGenerator&& rhs) noexcept
    : coro_(std::exchange(rhs.coro_, nullptr))
{
}

shigenoy::mocopi_parser::BoneDefinitionGenerator::~BoneDefinitionGenerator()
{
    if (coro_)
    {
        coro_.destroy();
    }
}

shigenoy::mocopi_parser::BoneDefinitionGenerator::iterator
shigenoy::mocopi_parser::BoneDefinitionGenerator::begin()
{
    coro_.resume();
    return { coro_, coro_.done() };
}

shigenoy::mocopi_parser::BoneDefinitionGenerator::iterator
shigenoy::mocopi_parser::BoneDefinitionGenerator::end()
{
    return { {}, true };
}

shigenoy::mocopi_parser::BoneDefinitionGenerator
shigenoy::mocopi_parser::readBoneDefinitions(const ParsedMocopiPacket& bone_definition_packet)
{
    if (!bone_definition_packet.parsed_.contains(wellknown_code::BNDT))
    {
        co_return;
    }

    const auto& [begin, end] = bone_definition_packet.parsed_.equal_range(wellknown_code::BNDT);
    for (auto itr = begin; itr != end; ++itr)
    {
        const auto& bndt = itr->second;
        const auto& bnid = bndt.children_.equal_range(wellknown_code::BNID).first->second;
        const auto& pbid = bndt.children_.equal_range(wellknown_code::PBID).first->second;
        const auto& tran = bndt.children_.equal_range(wellknown_code::TRAN).first->second;

        co_yield BoneDefinition{
            .bnid_   = bnid.get().readAs<std::uint16_t>(0),
            .pbid_   = pbid.get().readAs<std::uint16_t>(0),
            .quat_x_ = tran.get().readAs<float>(0),
            .quat_y_ = tran.get().readAs<float>(4),
            .quat_z_ = tran.get().readAs<float>(8),
            .quat_w_ = tran.get().readAs<float>(12),
            .tran_x_ = tran.get().readAs<float>(16),
            .tran_y_ = tran.get().readAs<float>(20),
            .tran_z_ = tran.get().readAs<float>(24),
        };
    }
}
