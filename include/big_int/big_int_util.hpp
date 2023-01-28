#pragma once

#ifdef ENABLE_BIG_INT_UTIL

#include "big_int.hpp"
#include "util.hpp"

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> gcd(
    const big_int<size>& a,
    const big_int<size>& b) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> lcm(
    const big_int<size>& a,
    const big_int<size>& b) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> midpoint(
    const big_int<size>& a,
    const big_int<size>& b) noexcept
{
    const big_int<size> half_a = a >> 2;
    const big_int<size> half_b = b >> 2;
    return half_a + half_b;
}

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> log(
    const big_int<size>& base,
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> log2(
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> log10(
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> loglp(
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> exp(
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> exp2(
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> pow(
    const big_int<size>& base,
    const big_int<size>& power) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> expml(
    const big_int<size>& number) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> rotl(
    const big_int<size>& num) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static big_int<size> rotr(
    const big_int<size>& num) noexcept;

template <size_t size>
BIG_INT_NODISCARD constexpr static size_t popcnt(
    const big_int<size>& num) noexcept;

template <size_t size>
BIG_INT_NODISCARD static constexpr big_int<size> mirror(
    const big_int<size>& num) noexcept
{
    big_int<size> res;
    for (size_t i = 0; i < size; ++i)
    {
        res.raw[i] = num.raw[size - i - 1];
    }
    return res;
}

template <size_t size>
BIG_INT_NODISCARD static constexpr const big_int<size>& clamp(
    const big_int<size>& num,
    const big_int<size>& low,
    const big_int<size>& high) noexcept
{
    assert(low < high);
    if (num < low)
    {
        return low;
    }
    else if (num > high)
    {
        return high;
    }
    else
    {
        return num;
    }
}

template <size_t size>
BIG_INT_NODISCARD static constexpr const big_int<size>& fact(
    const big_int<size>& num) noexcept;

template <size_t size>
BIG_INT_NODISCARD static constexpr const big_int<size>& fib(
    const big_int<size>& num) noexcept;

#endif  // ENABLE_BIG_INT_UTIL
