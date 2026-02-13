// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIRECEIVER_DATA_HPP)
#    define SHIGENOY_MOCOPIRECEIVER_DATA_HPP

#    include <shigenoy/mocopi-receiver/Data/Common.hpp>
#    include <shigenoy/mocopi-receiver/Data/Container.hpp>
#    include <shigenoy/mocopi-receiver/Data/FRAM.hpp>
#    include <shigenoy/mocopi-receiver/Data/HEAD.hpp>
#    include <shigenoy/mocopi-receiver/Data/SKDF.hpp>
#    include <shigenoy/mocopi-receiver/Data/SNDF.hpp>
#    include <shigenoy/mocopi-receiver/Data/SNDT.hpp>

#    include <array>

namespace shigenoy::mocopi_receiver {
class BoneDefinition
{
public:
    Head head_;
    Ftyp ftyp_;
    Vrsn vrsn_;
    //
    Sndf sndf_;
    Ipad ipad_;
    Rcvp rcvp_;
    //
    Skdf skdf_;
    Bons bons_;
    std::array<Bndt, 27> bndt_;
};

class FrameData
{
public:
    Head head_;
    Ftyp ftyp_;
    Vrsn vrsn_;
    //
    Sndf sndf_;
    Ipad ipad_;
    Rcvp rcvp_;
    //
    Fram fram_;
    Fnum fnum_;
    Time time_;
    Uttm uttm_;
    Tmcd tmcd_;
    Btrs btrs_;
    std::array<Btdt, 27> btdt_;
};
} // namespace shigenoy::mocopi_receiver

#endif
