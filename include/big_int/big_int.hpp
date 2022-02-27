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

// enum class cmp_result
//{
//    EQ,
//    LT,
//    GT,
//};
//
// template <size_t cnt, size_t offset>
// constexpr cmp_result constexpr_memcmp(const std::array<u8, cnt>& buf1,
//                                      const std::array<u8, cnt>& buf2)
//{
//    static_assert(offset < cnt, "Offset out of bounds!");
//
//    for (size_t i = offset; i < cnt; ++i)
//    {
//        if (buf1[i] != buf2[i])
//        {
//            return buf1[i] < buf2[i] ? cmp_result::GT : cmp_result::LT;
//        }
//    }
//    return cmp_result::EQ;
//}

// Integer representation in size number of bytes
// two's complement
// constexpr friendly
// Guaranteed to wrap on overlow
// TODO : template parameter to control the behaviour on overflow
template <size_t size>
struct big_int
{
    static_assert(size != 0, "Cannot have zero-sized integer!");
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
            if (raw[i] != other.raw[i])
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr bool operator!=(const big_int& other) const noexcept
    {
        return !(*this == other);
    }

    [[nodiscard]] constexpr bool operator<(const big_int& other) const noexcept
    {
        // invert the check for the most significant bit
        if (isNegative() != other.isNegative())
        {
            return isNegative();
        }

        const u8 most_significant_byte_no_sign_this =
            raw[size - 1] & ((u8(1) << u8(7)) - 1);
        const u8 most_significant_byte_no_sign_other =
            other.raw[size - 1] & ((u8(1) << u8(7)) - 1);

        if (most_significant_byte_no_sign_this ==
            most_significant_byte_no_sign_other)
        {
            // like memcpy, but in reverse
            for (size_t i = size - 1; 0 <= i && i < size; --i)
            {
                if (raw[i] != other.raw[i])
                {
                    return raw[i] < other.raw[i];
                }
            }

            return false;
        }
        else
        {
            return most_significant_byte_no_sign_this <
                   most_significant_byte_no_sign_other;
        }
    }

    [[nodiscard]] constexpr bool operator<=(const big_int& other) const noexcept
    {
        return *this < other || *this == other;
    }

    [[nodiscard]] constexpr bool operator>(const big_int& other) const noexcept
    {
        return !(*this <= other);
    }

    [[nodiscard]] constexpr bool operator>=(const big_int& other) const noexcept
    {
        return !(*this < other);
    }

    // TODO : spaceship operator

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
            const u8 old = raw[i];
            raw[i] += other.raw[i];
            const bool overflowed_on_sum = raw[i] < old;

            const u8 summed = raw[i];
            raw[i] += carry;  // separate expression to prevent overflow on
                              // operands in the first incrementation

            const bool overflowed_on_carry = raw[i] < summed;
            assert(overflowed_on_carry != overflowed_on_sum ||
                   (overflowed_on_carry == overflowed_on_sum &&
                    overflowed_on_carry == false));

            carry = overflowed_on_carry || overflowed_on_sum;

            // carry += raw[i] < old ? carry + 1 : 0;
        }

        // TODO : enable checks for overflow

        return *this;
    }

    constexpr big_int& operator-=(const big_int& other) noexcept
    {
        *this += -other;
        return *this;
    }

    constexpr big_int& operator*=(const big_int& other) noexcept;
    constexpr big_int& operator/=(const big_int& other) noexcept;
    constexpr big_int& operator%=(const big_int& other) noexcept;
    constexpr big_int& operator&=(const big_int& other) noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            raw[i] &= other.raw[i];
        }
        return *this;
    }

    constexpr big_int& operator|=(const big_int& other) noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            raw[i] |= other.raw[i];
        }
        return *this;
    }

    constexpr big_int& operator^=(const big_int& other) noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            raw[i] ^= other.raw[i];
        }
        return *this;
    }

    constexpr big_int& operator<<=(const big_int& other) noexcept;
    constexpr big_int& operator>>=(const big_int& other) noexcept;

    // Non-modifying operators
    constexpr big_int operator+() const noexcept { return *this; }
    [[nodiscard]] constexpr big_int operator-() const noexcept
    {
        big_int cpy = *this;
        cpy.negate();
        return cpy;
    }

    [[nodiscard]] constexpr big_int log(const big_int& base) const noexcept;

    [[nodiscard]] constexpr big_int operator+(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res += other;
        return res;
    }

    [[nodiscard]] constexpr big_int operator-(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res -= other;
        return res;
    }

    [[nodiscard]] constexpr big_int operator*(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res *= other;
        return res;
    }

    [[nodiscard]] constexpr big_int operator/(
        const big_int& other) const noexcept
    {
        big_int res = *this;

        if constexpr (other == 0)
        {
            throw std::runtime_error("Cannot divide by zero!");
        }

        res /= other;
        return res;
    }

    [[nodiscard]] constexpr big_int operator%(
        const big_int& other) const noexcept
    {
        big_int res = *this;

        if constexpr (other == 0)
        {
            throw std::runtime_error("Cannot find modulo by zero!");
        }

        res /= other;
        return res;
    }

    [[nodiscard]] constexpr bool operator!() const noexcept
    {
        return !bool(*this);
    }

    [[nodiscard]] constexpr bool operator&&(const big_int& other) const noexcept
    {
        return bool(*this) && bool(other);
    }

    [[nodiscard]] constexpr bool operator||(const big_int& other) const noexcept
    {
        return bool(*this) || bool(other);
    }

    // Bitwise operations

    [[nodiscard]] constexpr big_int operator~() const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < size; ++i)
        {
            cpy.raw[i] = ~raw[i];
        }
        return cpy;
    }

    [[nodiscard]] constexpr big_int operator&(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy &= other;
        return cpy;
    }

    [[nodiscard]] constexpr big_int operator|(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy |= other;
        return cpy;
    }

    [[nodiscard]] constexpr big_int operator^(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy ^= other;
        return cpy;
    }

    constexpr big_int operator<<(const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy <<= other;
        return cpy;
    }

    constexpr big_int operator>>(const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy >>= other;
        return cpy;
    }

    // Conversions
    [[nodiscard]] explicit operator bool() const noexcept
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
        sign_extend_into(raw, bytesOf<T>(a));
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

    template <typename T>
    constexpr static std::array<u8, sizeof(T)> bytesOf(const T value)
    {
        static_assert(std::is_integral<T>::value,
                      "The type has to be integral!");
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

    template <size_t arrSz>
    constexpr static bool is_rest_all_zeroes(const std::array<u8, arrSz>& arr,
                                             size_t idx) noexcept
    {
        for (size_t i = idx; i < arrSz; ++i)
        {
            if (arr[i] != 0)
            {
                return false;
            }
        }
        return true;
    }

    template <size_t srcCnt>
    constexpr static void sign_extend_into(
        std::array<u8, size>& raw,
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

        while (!is_rest_all_zeroes<srcCnt>(src, currIdx) && currIdx < srcCnt)
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
