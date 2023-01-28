#pragma once

#include "big_int/big_int.hpp"

namespace si_constants
{
// TODO : need to remove the big integer custom literal
#ifdef DEFINE_SI_CONSTANTS
constexpr auto avogardo_number = 602'214'076'000'000'000'000'000_bi;
constexpr auto speed_of_light = 299'792'458_bi;
constexpr auto hyperfine_transition_freq_cs = 9'192'631'770_bi;
constexpr auto monochromatic_radiation_freq_candela = 540'000'000'000'000_bi;
#endif  // DEFINE_SI_CONSTANTS
}  // namespace si_constants
