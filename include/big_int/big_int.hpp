#pragma once

#include "../util.hpp"

#include <array>
#include <stdexcept>
#include <type_traits>

// TODO : check for performance implications when substituting += (and others)
// with expanded form

namespace detail
{
template <typename T,
          typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr static T abs(T val) noexcept
{
    return val > 0 ? val : -val;
}

// template <typename T>
// constexpr static std::array<u8, sizeof(T)> bytes_of(const T value) noexcept
// {
//     static_assert(std::is_integral<T>::value, "The type has to be
//     integral!"); constexpr size_t bytes_cnt = sizeof(T);

//     using unsigned_T = typename std::make_unsigned<T>::type;

//     constexpr u8 byte_mask = u8(~0);
//     constexpr size_t bits_in_byte = 8;
//     constexpr unsigned_T type_mask = unsigned_T(byte_mask);

//     std::array<u8, bytes_cnt> res{};

//     for (size_t i = 0; i < sizeof(T); ++i)
//     {
//         res[i] = u8((value & (type_mask << (i * bits_in_byte))) >>
//                     (i * bits_in_byte));
//     }
//     return res;
// }

// template <size_t arr_sz>
// constexpr static bool is_rest_all_zeroes(const std::array<u8, arr_sz>& arr,
//                                          size_t idx) noexcept
// {
//     // assert(idx < arr_sz && "Cannot check ouside of array!");
//     for (size_t i = idx; i < arr_sz; ++i)
//     {
//         if (arr[i] != 0)
//         {
//             return false;
//         }
//     }
//     return true;
// }

// template <size_t size, size_t src_cnt>
// constexpr static void sign_extend_into(
//     std::array<u8, size>& raw,
//     const std::array<u8, src_cnt>& src) noexcept
// {
//     static_assert(size >= src_cnt,
//                   "The resulting count has to be greater than(or equal) the "
//                   "destination count!");

//     constexpr u8 full_mask = 0b11111111;
//     constexpr u8 empty_mask = 0b00000000;

//     size_t curr_idx = 0;

//     while (!is_rest_all_zeroes<src_cnt>(src, curr_idx) && curr_idx < src_cnt)
//     {
//         raw[curr_idx] = src[curr_idx];
//         ++curr_idx;
//     }

//     const bool sign = most_significant_bit(src[src_cnt - 1]) != 0;

//     for (size_t i = curr_idx; i < size; ++i)
//     {
//         raw[i] = sign ? full_mask : empty_mask;
//     }
// }
}  // namespace detail

// Integer representation in size number of bytes
// two's complement
// constexpr friendly
// Guaranteed to wrap on overlow
// TODO : template parameter to control the behaviour on overflow
template <size_t size>
struct big_int
{
    static_assert(size != 0, "Cannot have zero-sized integer!");
    constexpr big_int() noexcept = default;

    // Specifically not explicit so it can be used like regular numbers
    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value>::type>
    constexpr big_int(T a) noexcept  // NOLINT(hicpp-explicit-conversions)
    {
        big_int_init<T>(a);
    }

    template <typename T>
    constexpr big_int& operator=(T a) noexcept
    {
        big_int_init<T>(a);
        return *this;
    }

    // TODO : converting constructor from smaller big_ints
    // TODO : add feature switch for lossy conversions

#pragma region comparison_ops
    // TODO: compare with big_ints of different size

    BIG_INT_NODISCARD constexpr bool operator==(
        const big_int& other) const noexcept
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

    BIG_INT_NODISCARD constexpr bool operator!=(
        const big_int& other) const noexcept
    {
        return !(*this == other);
    }

    BIG_INT_NODISCARD constexpr bool operator<(
        const big_int& other) const noexcept
    {
        // invert the check for the most significant bit
        if (is_negative() != other.is_negative())
        {
            return is_negative();
        }

        const u8 most_significant_byte_no_sign_this =
            most_significant_byte_no_sign();
        const u8 most_significant_byte_no_sign_other =
            other.most_significant_byte_no_sign();

        if (most_significant_byte_no_sign_this ==
            most_significant_byte_no_sign_other)
        {
            // like memcpy, but in reverse
            for (size_t i = size - 1; i < size; --i)
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

    BIG_INT_NODISCARD constexpr bool operator<=(
        const big_int& other) const noexcept
    {
        return *this < other || *this == other;
    }

    BIG_INT_NODISCARD constexpr bool operator>(
        const big_int& other) const noexcept
    {
        return !(*this <= other);
    }

    BIG_INT_NODISCARD constexpr bool operator>=(
        const big_int& other) const noexcept
    {
        return !(*this < other);
    }

    // TODO : spaceship operator
#pragma endregion

#pragma region sign_access_and_manipulation
    // Makes the number it's absolute value
    constexpr void abs() noexcept
    {
        if (is_negative())
        {
            negate();
        }
    }

    // true if the number is negative
    BIG_INT_NODISCARD constexpr bool is_negative() const noexcept
    {
        return most_significant_bit(most_significant_byte()) != 0;
    }

    constexpr void flip_sign_bit() noexcept
    {
        constexpr size_t bits_in_byte = 8;
        raw[size - 1] ^= (1U << (bits_in_byte * sizeof(u8) - 1));
    }

    // negates the number in two's complement
    constexpr void negate() noexcept
    {
        for (size_t i = 0; i < size; ++i)
        {
            raw[i] = u8(~raw[i]);
        }
        increment();
    }

#pragma endregion

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
    constexpr big_int operator++(int) noexcept  // NOLINT(cert-dcl21-cpp)
    {
        const big_int cpy = *this;
        increment();
        return cpy;
    }

    // Postfix oeprator --
    constexpr big_int operator--(int) noexcept  // NOLINT(cert-dcl21-cpp)
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
            raw[i] = u8(raw[i] + other.raw[i]);
            const bool overflowed_on_sum = raw[i] < old;

            const u8 summed = raw[i];
            // separate expression to prevent
            // overflow on operands in the first incrementation
            raw[i] = u8(raw[i] + carry);

            const bool overflowed_on_carry = raw[i] < summed;
            // assert(overflowed_on_carry != overflowed_on_sum ||
            //       !overflowed_on_carry || !overflowed_on_sum);

            carry = overflowed_on_carry || overflowed_on_sum;
        }

        // TODO : enable checks for overflow

        return *this;
    }

    constexpr big_int& operator-=(const big_int& other) noexcept
    {
        *this += -other;
        return *this;
    }

    constexpr big_int& operator<<=(const big_int& other) noexcept
    {
        // assert(other >= 0 &&
        //       "Need to shift by a positive amount, maybe you need "
        //       "to use the other shift!");

        for (big_int i = zero(); i < other; ++i)
        {
            left_shift_once();
        }
        return *this;
    }

    constexpr big_int& operator>>=(const big_int& other) noexcept
    {
        // assert(other >= 0 &&
        //       "Need to shift by a positive amount, maybe you need "
        //       "to use the other shift!");

        for (big_int i = u8(0); i < other; ++i)
        {
            right_shift_once();
        }
        return *this;
    }

    constexpr big_int& operator*=(const big_int& other) noexcept
    {
        *this = *this * other;
        return *this;
    }

    constexpr big_int& operator/=(const big_int& other);
    constexpr big_int& operator%=(const big_int& other);

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

    // Non-modifying operators

    constexpr big_int operator+() const noexcept { return *this; }

    BIG_INT_NODISCARD constexpr big_int operator-() const noexcept
    {
        big_int cpy = *this;
        cpy.negate();
        return cpy;
    }

    BIG_INT_NODISCARD constexpr big_int operator+(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res += other;
        return res;
    }

    BIG_INT_NODISCARD constexpr big_int operator-(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res -= other;
        return res;
    }

    BIG_INT_NODISCARD constexpr big_int operator*(
        const big_int& other) const noexcept
    {
        // check out
        // 1. https://en.wikipedia.org/wiki/Karatsuba_algorithm
        // 2. https://en.wikipedia.org/wiki/Toom%E2%80%93Cook_multiplication
        // 3.
        // https://en.wikipedia.org/wiki/Sch%C3%B6nhage%E2%80%93Strassen_algorithm
        // 4. https://en.wikipedia.org/wiki/F%C3%BCrer%27s_algorithm
        // 5.
        // https://projecteuclid.org/journals/annals-of-mathematics/volume-193/issue-2/Integer-multiplication-in-time-Onmathrmlog-n/10.4007/annals.2021.193.2.4.short

        // big_int res = 0;

        // big_int summand = other;

        // for (size_t byte_idx = 0; byte_idx < size; ++byte_idx)
        //{
        //    for (u8 bit_idx = 0; bit_idx < 8;
        //         ++bit_idx, summand.left_shift_once())
        //    {
        //        if (is_bit_set(byte_idx, bit_idx))
        //        {
        //            res += summand;
        //        }
        //    }
        //}

        big_int res = 0;

        const bool should_negate = other.is_negative();

        const big_int abs_other = other.is_negative() ? -other : other;

        for (big_int i = 0; i < abs_other; ++i)
        {
            res += *this;
        }

        if (should_negate)
        {
            res.negate();
        }
        return res;
    }

    BIG_INT_NODISCARD constexpr big_int operator/(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res /= other;
        return res;
    }

    BIG_INT_NODISCARD constexpr big_int operator%(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        res %= other;
        return res;
    }

    BIG_INT_NODISCARD constexpr bool operator!() const noexcept
    {
        return !bool(*this);
    }

    BIG_INT_NODISCARD constexpr bool operator&&(
        const big_int& other) const noexcept
    {
        return bool(*this) && bool(other);
    }

    BIG_INT_NODISCARD constexpr bool operator||(
        const big_int& other) const noexcept
    {
        return bool(*this) || bool(other);
    }

    // Bitwise operations

    BIG_INT_NODISCARD constexpr big_int operator~() const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < size; ++i)
        {
            cpy.raw[i] = u8(~raw[i]);
        }
        return cpy;
    }

    BIG_INT_NODISCARD constexpr big_int operator&(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy &= other;
        return cpy;
    }

    BIG_INT_NODISCARD constexpr big_int operator|(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy |= other;
        return cpy;
    }

    BIG_INT_NODISCARD constexpr big_int operator^(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy ^= other;
        return cpy;
    }

    BIG_INT_NODISCARD constexpr big_int operator<<(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy <<= other;
        return cpy;
    }

    BIG_INT_NODISCARD constexpr big_int operator>>(
        const big_int& other) const noexcept
    {
        big_int cpy = *this;
        cpy >>= other;
        return cpy;
    }

    // Conversions
    BIG_INT_NODISCARD constexpr explicit operator bool() const noexcept
    {
        for (const u8 byte : raw)
        {
            if (byte != 0)
            {
                return true;
            }
        }
        return false;
    }

    // TODO : finish implementation of the conversion to integral
    // template <typename targ>
    // BIG_INT_NODISCARD constexpr explicit
    // operator typename std::enable_if<size == sizeof(targ), targ>::type()
    // const
    //{
    //    return targ();
    //}

    // Static factory methods
    BIG_INT_NODISCARD constexpr static big_int zero() noexcept
    {
        return big_int<size>(u8(0));
    }

    BIG_INT_NODISCARD constexpr static big_int one() noexcept
    {
        return big_int<size>(u8(1));
    }

    // stored in order from least significant to most significant
    // 0 -> least significant
    // size - 1 -> most significant
    std::array<u8, size> raw = {0};

private:
    template <typename T>
    constexpr void big_int_init(T a) noexcept
    {
        static_assert(std::is_integral<T>::value,
                      "The type has to be integral!");
        static_assert(size >= sizeof(T),
                      "The size of the big int must be greater than the size "
                      "of the source type");
        // This relies too much on representation
        // TODO : Detect representation
        // detail::sign_extend_into(raw, detail::bytes_of<T>(a));

        if (a == 0)
        {
            return;
        }

        const bool should_negate = a < 0;
        a = detail::abs<T>(a);

        if ((a % T(2)) == 0)
        {
            increment();
            increment();
            a = T(a - T(2));
        }
        else
        {
            increment();
            a = T(a - T(1));
        }

        while (a != 0)
        {
            if ((a % T(2)) == 0)
            {
                left_shift_once();
                a /= T(a / T(2));
            }
            else
            {
                increment();
                --a;
            }
        }

        if (should_negate)
        {
            negate();
        }
    }

#pragma region arithmetic_helpers
    // Increments the number by one
    constexpr void increment() noexcept
    {
        bool carry = false;
        size_t curr_idx = 0;
        do
        {
            const u8 old_value = raw[curr_idx];
            ++raw[curr_idx];
            carry = old_value > raw[curr_idx];
            ++curr_idx;
        } while (carry && curr_idx < size);

        // TODO: if the curr_idx is size, then there is overlow
        // maybe make it configurable so it throws
    }

    // Decrements the number by one
    constexpr void decrement() noexcept
    {
        bool take = false;
        size_t curr_idx = 0;
        do
        {
            const u8 old_value = raw[curr_idx];
            --raw[curr_idx];
            take = old_value < raw[curr_idx];
            ++curr_idx;
        } while (take && curr_idx < size);

        // TODO: if the curr_idx is size, then there is underflow
        // maybe make it configurable so it throws
    }
#pragma endregion

#pragma region bitwise_helpers
    constexpr void left_shift_once() noexcept
    {
        u8 carry = 0;
        for (size_t i = 0; i < size; ++i)
        {
            const u8 msb = most_significant_bit(raw[i]);
            raw[i] = u8(raw[i] << 1);
            raw[i] = u8(raw[i] | carry);
            carry = msb;
        }
    }

    constexpr void right_shift_once() noexcept
    {
        u8 carry = 0;

        for (size_t i = size - 1; i < size; --i)
        {
            const u8 lsb = least_significant_bit(raw[i]);
            raw[i] = u8(raw[i] >> 1);
            raw[i] = u8(raw[i] | (carry << 7));
            carry = lsb;
        }
    }

    BIG_INT_NODISCARD constexpr bool is_bit_set(size_t byte_idx,
                                                u8 bit_idx) const noexcept
    {
        return raw[byte_idx] & (1 << (bit_idx));
    }

    BIG_INT_NODISCARD constexpr u8 most_significant_byte() const noexcept
    {
        return raw[size - 1];
    }

    BIG_INT_NODISCARD constexpr u8 most_significant_byte_no_sign()
        const noexcept
    {
        return u8(most_significant_byte() & 0b01111111);
    }

#pragma endregion
};

#ifdef ENABLE_BIG_INT_LITERAL
namespace detail
{
// TODO : user defined literals
template <char... c>
struct size_to_fit : std::integral_constant<size_t, 128>
{
};

// TODO : disallow consecutive separator chars - '
// TODO : parse binary, octal and hex numbers
template <size_t arr_sz, size_t bi_size>
BIG_INT_NODISCARD static constexpr big_int<bi_size> from_fixed_char_array(
    const std::array<char, arr_sz>& arr) /*noexcept*/
{
    static_assert(arr_sz >= 1, "Cannot have zero length integers!");

    constexpr big_int<bi_size> base = 10;
    big_int<bi_size> res = 0;

    const bool should_negate = arr[0] == '-';
    const bool should_skip_first = should_negate || (arr[0] == '+');

    for (size_t i = should_skip_first ? 1 : 0; i < arr_sz; ++i)
    {
        if (arr[i] == '\'')
        {
            continue;
        }

        if (!constexpr_is_digit(arr[i]))
        {
            throw std::invalid_argument("Illegal character in number!");
        }

        res *= base;
        res += arr[i] - '0';
    }

    if (should_negate)
    {
        res.negate();
    }
    return res;
}

}  // namespace detail

template <char... c>
constexpr static big_int<detail::size_to_fit<c...>::value>
operator""_bi() noexcept
{
    return detail::from_fixed_char_array<sizeof...(c),
                                         detail::size_to_fit<c...>::value>(
        std::array{c...});
}
#endif  // ENABLE_BIG_INT_LITERAL

static_assert(std::is_nothrow_constructible<big_int<128>>::value);
static_assert(std::is_default_constructible<big_int<128>>::value);
static_assert(std::is_nothrow_default_constructible<big_int<128>>::value);

static_assert(std::is_copy_constructible<big_int<128>>::value);
static_assert(std::is_trivially_copyable<big_int<128>>::value);
static_assert(std::is_nothrow_copy_constructible<big_int<128>>::value);

static_assert(std::is_move_constructible<big_int<128>>::value);
static_assert(std::is_trivially_move_constructible<big_int<128>>::value);
static_assert(std::is_nothrow_move_constructible<big_int<128>>::value);

// TODO : Abstract this to a type with a list of sizes and a list of approved
// types. This could be curated in order for big_int<2> to not accept bigger
// than short, while big_int<128> would accept all the integral types
static_assert(std::is_assignable<big_int<128>, char>::value);
static_assert(std::is_assignable<big_int<128>, unsigned char>::value);
static_assert(std::is_assignable<big_int<128>, short>::value);
static_assert(std::is_assignable<big_int<128>, unsigned short>::value);
static_assert(std::is_assignable<big_int<128>, int>::value);
static_assert(std::is_assignable<big_int<128>, unsigned int>::value);
static_assert(std::is_assignable<big_int<128>, long>::value);
static_assert(std::is_assignable<big_int<128>, unsigned long>::value);
static_assert(std::is_assignable<big_int<128>, long long>::value);
static_assert(std::is_assignable<big_int<128>, unsigned long long>::value);

// TODO : The conversion to bigger big_ints
// static_assert(std::is_assignable<big_int<129>, big_int<128>>::value);

static_assert(std::is_copy_assignable<big_int<128>>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, char>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, unsigned char>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, short>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, unsigned short>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, int>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, unsigned int>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, long>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, unsigned long>::value);
static_assert(std::is_nothrow_assignable<big_int<128>, long long>::value);
static_assert(
    std::is_nothrow_assignable<big_int<128>, unsigned long long>::value);

static_assert(std::is_move_assignable<big_int<128>>::value);
static_assert(std::is_trivially_move_assignable<big_int<128>>::value);
static_assert(std::is_nothrow_move_assignable<big_int<128>>::value);

static_assert(std::is_destructible<big_int<128>>::value);
static_assert(std::is_trivially_destructible<big_int<128>>::value);
static_assert(std::is_nothrow_destructible<big_int<128>>::value);

// Deprecated
// static_assert(std::is_literal_type<big_int<128>>::value);

static_assert(std::has_unique_object_representations<big_int<128>>::value);
static_assert(std::has_unique_object_representations<big_int<129>>::value);
static_assert(std::has_unique_object_representations<big_int<130>>::value);

static_assert(std::is_standard_layout<big_int<128>>::value);

// Assuring common_type can be specialized for the big_int
static_assert(std::is_same<std::decay_t<big_int<128>>, big_int<128>>::value);
static_assert(std::is_same<std::decay_t<big_int<129>>, big_int<129>>::value);
