// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_GENERATOR_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_GENERATOR_HPP

#    include <shigenoy/mocopi-receiver/Container.hpp>

#    include <coroutine>

namespace shigenoy::mocopi_receiver {
class BoneDefinition
{
public:
    std::uint16_t bnid_;
    std::uint16_t pbid_;
    float quat_x_;
    float quat_y_;
    float quat_z_;
    float quat_w_;
    float tran_x_;
    float tran_y_;
    float tran_z_;
};

struct BoneDefinitionGenerator
{
public:
    struct promise_type // NOLINT
    {
        BoneDefinition value_;

        auto get_return_object();           //NOLINT
        auto initial_suspend();             //NOLINT
        auto final_suspend() noexcept;      //NOLINT
        auto yield_value(BoneDefinition v); //NOLINT
        void return_void();                 //NOLINT
        void unhandled_exception();         //NOLINT
    };
    using coro_handle = std::coroutine_handle<promise_type>; //NOLINT

    struct iterator // NOLINT
    {
        coro_handle coro_;
        bool done_;

        iterator& operator++();
        bool operator!=(const iterator& rhs) const;
        BoneDefinition operator*() const;
    };

private:
    coro_handle coro_;

private:
    explicit BoneDefinitionGenerator(promise_type& p);

public:
    BoneDefinitionGenerator(BoneDefinitionGenerator const&) = delete;
    BoneDefinitionGenerator(BoneDefinitionGenerator&& rhs) noexcept;
    ~BoneDefinitionGenerator();

public:
    iterator begin(); //NOLINT
    iterator end();   //NOLINT
};

BoneDefinitionGenerator readBoneDefinitions(const ParsedMocopiPacket& boneDefinitionPacket);

class BoneTransform
{
public:
    std::uint16_t bnid_;
    float quat_x_;
    float quat_y_;
    float quat_z_;
    float quat_w_;
    float tran_x_;
    float tran_y_;
    float tran_z_;
};

struct BoneTransformGenerator
{
public:
    struct promise_type // NOLINT
    {
        BoneTransform value_;

        auto get_return_object();          //NOLINT
        auto initial_suspend();            //NOLINT
        auto final_suspend() noexcept;     //NOLINT
        auto yield_value(BoneTransform v); //NOLINT
        void return_void();                //NOLINT
        void unhandled_exception();        //NOLINT
    };
    using coro_handle = std::coroutine_handle<promise_type>; //NOLINT

    struct iterator // NOLINT
    {
        coro_handle coro_;
        bool done_;

        iterator& operator++();
        bool operator!=(const iterator& rhs) const;
        BoneTransform operator*() const;
    };

private:
    coro_handle coro_;

private:
    explicit BoneTransformGenerator(promise_type& p);

public:
    BoneTransformGenerator(BoneTransformGenerator const&) = delete;
    BoneTransformGenerator(BoneTransformGenerator&& rhs) noexcept;
    ~BoneTransformGenerator();

public:
    iterator begin(); //NOLINT
    iterator end();   //NOLINT
};

BoneTransformGenerator readBoneTransforms(const ParsedMocopiPacket& boneTransformPacket);

} // namespace shigenoy::mocopi_receiver

#endif
