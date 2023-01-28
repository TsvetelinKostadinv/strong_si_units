#pragma once

#ifdef ENABLE_BIG_INT_STD_INTEGRATION

#include "big_int.hpp"
#include "util.hpp"

#include <type_traits>

template <size_t sz_a, size_t sz_b>
struct std::common_type<big_int<sz_a>, big_int<sz_b>>
{
    using type = big_int<std::max(sz_a, sz_b)>;
};

#include <string>

template <size_t size>
BIG_INT_NODISCARD static std::string to_string(
    const big_int<size>& num) noexcept;

template <size_t size>
BIG_INT_NODISCARD static big_int<size> from_string(
    const std::string& str) noexcept;

//#include <charconv>
//
// template <size_t bi_size>
// std::to_chars_result to_chars(char* first,
//                              char* last,
//                              const big_int<bi_size>& value,
//                              int base = 10);
//
// template <size_t bi_size>
// std::to_chars_result to_chars(char* first,
//                              char* last,
//                              const big_int<bi_size>& value,
//                              std::chars_format fmt);
//
// template <size_t bi_size>
// std::from_chars_result from_chars(const char* first,
//                                  const char* last,
//                                  big_int<bi_size>& value,
//                                  int base = 10);
//
// template <size_t bi_size>
// std::from_chars_result from_chars(
//    const char* first,
//    const char* last,
//    big_int<bi_size>& value,
//    std::chars_format fmt = std::chars_format::general);

#include <ostream>

template <size_t size>
BIG_INT_NODISCARD static std::ostream& operator<<(std::ostream& os,
                                                  const big_int<size>& a)
{
    // TODO : Check base
    return os << to_string(a);
}

#include <limits>

template <size_t bi_size>
class std::numeric_limits<big_int<bi_size>>
{
public:
    BIG_INT_NODISCARD static constexpr big_int<bi_size>(min)() noexcept
    {
        return cached_min;
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size>(max)() noexcept
    {
        return cached_max;
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> lowest() noexcept
    {
        return cached_min;
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> epsilon() noexcept
    {
        return big_int<bi_size>();
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> round_error() noexcept
    {
        return big_int<bi_size>();
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> denorm_min() noexcept
    {
        return big_int<bi_size>();
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> infinity() noexcept
    {
        return big_int<bi_size>();
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> quiet_NaN() noexcept
    {
        return big_int<bi_size>();
    }

    BIG_INT_NODISCARD static constexpr big_int<bi_size> signaling_NaN() noexcept
    {
        return big_int<bi_size>();
    }

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr auto has_denorm = float_denorm_style::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr auto round_style = float_round_style::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    // the big_int wraps around on overflow
    static constexpr bool is_modulo = true;
    static constexpr int digits = int(bi_size);
    // static constexpr int digits10 = ?;
    // static constexpr int max_digits10 = ?;

    static constexpr int radix = 256;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;

private:
    static constexpr big_int<bi_size> cached_min = []() noexcept
    {
        big_int<bi_size> tmp;
        tmp.flip_sign_bit();
        return tmp;
    }();

    static constexpr big_int<bi_size> cached_max = []() noexcept
    {
        big_int<bi_size> tmp;
        tmp.flip_sign_bit();
        return ~tmp;
    }();
};

#include <functional>

template <size_t bi_size>
class std::hash<big_int<bi_size>>
{
};

#endif  // ENABLE_BIG_INT_STD_INTEGRATION
