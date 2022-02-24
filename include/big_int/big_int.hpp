#pragma once

#include <any>
#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

// utils, to be extracted to another file
using u8 = std::uint8_t;
using i8 = std::int8_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

// slower, less efficient
// but constexpr memcpy
template <size_t destCnt, size_t cnt>
constexpr std::array<u8, destCnt>& constexpr_memcpy(
    std::array<u8, destCnt>& dest,
    const std::array<u8, cnt>& src) noexcept
{
    static_assert(destCnt >= cnt,
                  "The destination count must be able to hold cnt values");

    for (size_t i = 0; i < cnt; ++i)
    {
        dest[i] = src[i];
    }

    return dest;
}

template <typename T>
constexpr std::array<u8, sizeof(T)> bytesOf(const T value)
{
    static_assert(std::is_integral<T>::value, "The type has to be integral!");
    constexpr size_t bytesCnt = sizeof(T);

    constexpr u8 byteMask = 0b11111111;
    constexpr size_t bitsInByte = 8;
    constexpr T typeMask = T(byteMask);

    std::array<u8, bytesCnt> res{};

    for (size_t i = 0; i < sizeof(T); ++i)
    {
        res[i] = value & (typeMask << (i * bitsInByte));
    }
    return res;
}

template <size_t size>
constexpr bool is_rest_all_zeroes(const std::array<u8, size>& arr,
                                  size_t idx) noexcept
{
    for (size_t i = idx; i < size; ++i)
    {
        if (arr[i] != 0)
        {
            return false;
        }
    }
    return true;
}

template <size_t dstCnt, size_t srcCnt>
constexpr std::array<u8, dstCnt> sign_extend(
    const std::array<u8, srcCnt>& src) noexcept
{
    static_assert(dstCnt >= srcCnt,
                  "The resulting count has to be greater than(or equal) the "
                  "destination count!");

    constexpr u8 fillMask = 0b11111111;
    constexpr u8 emptyMask = 0;
    constexpr size_t bitsInByte = 8;

    size_t currIdx = 0;
    std::array<u8, dstCnt> res{};

    bool sign = false;

    while (!is_rest_all_zeroes(src, currIdx) && currIdx < dstCnt)
    {
        sign = src[currIdx] & (u8(1) << (bitsInByte - 1));
        res[currIdx] = src[currIdx];
        ++currIdx;
    }

    for (size_t i = srcCnt; i < dstCnt; ++i)
    {
        res[i] = sign ? fillMask : emptyMask;
    }
    return res;
}

// Integer representation in size number of bytes
// two's complement
// constexpr friendly
// Guaranteed to wrap on overlow
// TODO : template parameter to control the behaviour on overflow
template <size_t size>
struct big_int
{
    // no constructors or operator=
    // the compiler can deduce
    constexpr big_int() noexcept = default;

    // Specifically not explicit so it can be used like regular numbers
    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value>::type>
    constexpr big_int(T a)  // NOLINT(hicpp-explicit-conversions)
    {
        big_int_init<T>(a);
    }

    template <typename T>
    constexpr big_int& operator=(T a)
    {
        big_int_init<T>(a);
        return *this;
    }

    // Comparison functions
    // TODO: compare with smaller big_ints

    [[nodiscard]] constexpr bool operator==(const big_int& other) const noexcept
    {
        // for (size_t i = size - 1; i >= 0 && i < size; --i)
        for (size_t i = 0; i < size; ++i)
        {
            if ((raw[i] != other.raw[i]))
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr bool operator!=(const big_int& other) const noexcept
    {
        // code is repeated, but more precise optimizations can be made this way
        // for (size_t i = size - 1; i >= 0 && i < size; --i)
        for (size_t i = 0; i < size; ++i)
        {
            if ((raw[i] == other.raw[i]))
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr bool operator<(const big_int& other) const noexcept;
    [[nodiscard]] constexpr bool operator<=(
        const big_int& other) const noexcept;

    [[nodiscard]] constexpr bool operator>(const big_int& other) const noexcept;
    [[nodiscard]] constexpr bool operator>=(
        const big_int& other) const noexcept;

    // Modifying operations

    // Increments the number by one
    constexpr void increment() noexcept
    {
        bool carry = false;
        size_t currIdx = 0;
        do
        {
            u8 oldValue = raw[currIdx];
            ++raw[currIdx];
            carry = oldValue > raw[currIdx];
            ++currIdx;
        } while (carry && currIdx < size);

        // TODO: if the currIdx is size, then there is overlow
        // maybe make it configurable so it throws
    }

    // Decrements the number by one
    constexpr void decrement() noexcept
    {
        bool take = false;
        size_t currIdx = 0;
        do
        {
            u8 oldValue = raw[currIdx];
            --raw[currIdx];
            take = oldValue < raw[currIdx];
            ++currIdx;
        } while (take && currIdx < size);

        // TODO: if the currIdx is size, then there is underflow
        // maybe make it configurable so it throws
    }

    // negates the number in two's complement
    constexpr void negate() noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            raw[i] = ~raw[i];
        }
        increment();
    }

    // Makes the number it's absolute value
    constexpr void abs() noexcept
    {
        if (isNegative())
        {
            negate();
        }
    }

    // Prefix oeprator ++
    constexpr big_int& operator++() noexcept
    {
        increment();
        return *this;
    }

    // Prefix oeprator --
    constexpr big_int& operator--() noexcept
    {
        decrement();
        return *this;
    }

    // Postfix oeprator ++
    constexpr big_int operator++(int) noexcept
    {
        const big_int cpy = *this;
        increment();
        return cpy;
    }

    // Postfix oeprator --
    constexpr big_int operator--(int) noexcept
    {
        const big_int cpy = *this;
        decrement();
        return cpy;
    }

    constexpr big_int& operator+=(const big_int& other) noexcept
    {
        bool carry = false;
        for (size_t i = 0; i < size; ++i)
        {
            u8 old = raw[i];
            raw[i] += other.raw[i];
            raw[i] += carry;       // separate expression to prevent overflow on
                                   // operands in the above row
            carry = raw[i] < old;  // detect overflow
        }

        // TODO : enable checks for overflow

        return *this;
    }

    constexpr big_int& operator-=(const big_int& other) noexcept
    {
        bool take = false;
        for (size_t i = 0; i < size; ++i)
        {
            u8 old = raw[i];
            raw[i] -= other.raw[i];
            raw[i] -= take;       // separate expression to prevent overflow on
                                  // operands in the above row
            take = raw[i] > old;  // detect overflow
        }

        return *this;
    }

    constexpr big_int& operator*=(const big_int& other) noexcept;
    constexpr big_int& operator/=(const big_int& other) noexcept;
    constexpr big_int& operator%=(const big_int& other) noexcept;
    constexpr big_int& operator&=(const big_int& other) noexcept;
    constexpr big_int& operator|=(const big_int& other) noexcept;
    constexpr big_int& operator^=(const big_int& other) noexcept;
    constexpr big_int& operator<<=(const big_int& other) noexcept;
    constexpr big_int& operator>>=(const big_int& other) noexcept;

    // Non-modifying operators
    constexpr big_int operator+() const noexcept { return *this; }
    constexpr big_int operator-() const noexcept
    {
        big_int cpy = *this;
        cpy.negate();
        return cpy;
    }

    constexpr big_int log(const big_int& base) const noexcept;

    constexpr big_int operator+(const big_int& other) const noexcept
    {
        big_int res = *this;
        res += other;
        return res;
    }

    constexpr big_int operator-(const big_int& other) const noexcept
    {
        big_int res = *this;
        res -= other;
        return res;
    }

    constexpr big_int operator*(const big_int& other) const noexcept;
    constexpr big_int operator/(const big_int& other) const noexcept;
    constexpr big_int operator%(const big_int& other) const noexcept;

    constexpr bool operator!() const noexcept { return !bool(*this); }

    constexpr bool operator&&(const big_int& other) const noexcept
    {
        return bool(*this) && bool(other);
    }

    constexpr bool operator||(const big_int& other) const noexcept
    {
        return bool(*this) || bool(other);
    }

    constexpr big_int operator~() const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < size; ++i)
        {
            cpy.raw[i] = ~raw[i];
        }
        return cpy;
    }

    constexpr big_int operator&(const big_int& other) const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < size; ++i)
        {
            cpy.raw[i] = raw[i] & other.raw[i];
        }
        return cpy;
    }

    constexpr big_int operator|(const big_int& other) const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < size; ++i)
        {
            cpy.raw[i] = raw[i] | other.raw[i];
        }
        return cpy;
    }

    constexpr big_int operator^(const big_int& other) const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < size; ++i)
        {
            cpy.raw[i] = raw[i] ^ other.raw[i];
        }
        return cpy;
    }

    constexpr big_int operator<<(const big_int& other) const noexcept;
    constexpr big_int operator>>(const big_int& other) const noexcept;

    // Conversions
    explicit operator bool() const noexcept
    {
        for (const u8& byte : raw)
        {
            if (byte != 0)
            {
                return true;
            }
        }
        return false;
    }

    // Utilities
    [[nodiscard]] constexpr const char* to_string() const noexcept;

    // Static factory methods
    [[nodiscard]] constexpr static big_int zero() noexcept
    {
        return big_int<size>(0);
    }

    [[nodiscard]] constexpr static big_int one() noexcept
    {
        return big_int<size>(1);
    }

    [[nodiscard]] constexpr static big_int(min)() noexcept
    {
        big_int<size> tmp;
        tmp.flipSignBit();
        return tmp;
    }

    [[nodiscard]] constexpr static big_int(max)() noexcept
    {
        constexpr u8 byteMask = 0b11111111;

        big_int<size> tmp;
        for (size_t i = 0; i < size; ++i)
        {
            tmp.raw[i] = byteMask;
        }
        tmp.flipSignBit();

        return tmp;
    }

    // static_assert(min() < max(), "The min should be less than the max!");

private:
    template <typename T>
    constexpr void big_int_init(T a)
    {
        static_assert(std::is_integral<T>::value,
                      "The type has to be integral!");
        static_assert(size >= sizeof(T),
                      "The size of the big int must be greater than the size "
                      "of the source type");
        sign_extend_into_raw(bytesOf<T>(a));
    }

    std::array<u8, size> raw = {0};

    // true if the number is negative
    [[nodiscard]] constexpr bool isNegative() const
    {
        constexpr size_t bitsInByte = 8;
        return raw[size - 1] & (1U << (bitsInByte * sizeof(u8) - 1));
    }

    constexpr void flipSignBit()
    {
        constexpr size_t bitsInByte = 8;
        raw[size - 1] ^= (1U << (bitsInByte * sizeof(u8) - 1));
    }

    template <size_t srcCnt>
    constexpr void sign_extend_into_raw(
        const std::array<u8, srcCnt>& src) noexcept
    {
        static_assert(
            size >= srcCnt,
            "The resulting count has to be greater than(or equal) the "
            "destination count!");

        constexpr u8 fillMask = 0b11111111;
        constexpr u8 emptyMask = 0;
        constexpr size_t bitsInByte = 8;

        size_t currIdx = 0;

        bool sign = false;

        while (!is_rest_all_zeroes(src, currIdx) && currIdx < srcCnt)
        {
            sign = src[currIdx] & (u8(1) << (bitsInByte - 1));
            raw[currIdx] = src[currIdx];
            ++currIdx;
        }

        for (size_t i = currIdx; i < size; ++i)
        {
            raw[i] = sign ? fillMask : emptyMask;
        }
    }
};

// template <char... c>
// struct size_to_fit : std::integral_constant<size_t, 2>
//{
//};
//
// template <char... c>
// constexpr big_int<size_to_fit<c...>::value> operator""_bi() noexcept
//{
//}
