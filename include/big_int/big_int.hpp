//#pragma once
//
//#include "../util.hpp"
//
//#include <array>
////#include <cassert>
//#include <stdexcept>
//
//// enum class cmp_result
////{
////    EQ,
////    LT,
////    GT,
////};
////
//// template <size_t cnt, size_t offset>
//// constexpr cmp_result constexpr_memcmp(const std::array<u8, cnt>& buf1,
////                                      const std::array<u8, cnt>& buf2)
////{
////    static_assert(offset < cnt, "Offset out of bounds!");
////
////    for (size_t i = offset; i < cnt; ++i)
////    {
////        if (buf1[i] != buf2[i])
////        {
////            return buf1[i] < buf2[i] ? cmp_result::GT : cmp_result::LT;
////        }
////    }
////    return cmp_result::EQ;
////}
//
//// Integer representation in size number of bytes
//// two's complement
//// constexpr friendly
//// Guaranteed to wrap on overlow
//// TODO : template parameter to control the behaviour on overflow
//template <size_t size>
//struct big_int
//{
//    static_assert(size != 0, "Cannot have zero-sized integer!");
//    constexpr big_int() noexcept = default;
//
//    // Specifically not explicit so it can be used like regular numbers
//    template <
//        typename T,
//        typename = typename std::enable_if<std::is_integral<T>::value>::type>
//    constexpr big_int(T a) noexcept  // NOLINT(hicpp-explicit-conversions)
//    {
//        big_int_init<T>(a);
//    }
//
//    template <typename T>
//    constexpr big_int& operator=(T a) noexcept
//    {
//        big_int_init<T>(a);
//        return *this;
//    }
//
//    // Comparison functions
//    // TODO: compare with smaller big_ints
//
//    [[nodiscard]] constexpr bool operator==(const big_int& other) const noexcept
//    {
//        // for (size_t i = size - 1; i >= 0 && i < size; --i)
//        for (size_t i = 0; i < size; ++i)
//        {
//            if (raw[i] != other.raw[i])
//            {
//                return false;
//            }
//        }
//        return true;
//    }
//
//    [[nodiscard]] constexpr bool operator!=(const big_int& other) const noexcept
//    {
//        return !(*this == other);
//    }
//
//    [[nodiscard]] constexpr bool operator<(const big_int& other) const noexcept
//    {
//        // invert the check for the most significant bit
//        if (isNegative() != other.isNegative())
//        {
//            return isNegative();
//        }
//
//        const u8 most_significant_byte_no_sign_this =
//            raw[size - 1] & ((u8(1) << u8(7)) - 1);
//        const u8 most_significant_byte_no_sign_other =
//            other.raw[size - 1] & ((u8(1) << u8(7)) - 1);
//
//        if (most_significant_byte_no_sign_this ==
//            most_significant_byte_no_sign_other)
//        {
//            // like memcpy, but in reverse
//            for (size_t i = size - 1; i < size; --i)
//            {
//                if (raw[i] != other.raw[i])
//                {
//                    return raw[i] < other.raw[i];
//                }
//            }
//
//            return false;
//        }
//        else
//        {
//            return most_significant_byte_no_sign_this <
//                   most_significant_byte_no_sign_other;
//        }
//    }
//
//    [[nodiscard]] constexpr bool operator<=(const big_int& other) const noexcept
//    {
//        return *this < other || *this == other;
//    }
//
//    [[nodiscard]] constexpr bool operator>(const big_int& other) const noexcept
//    {
//        return !(*this <= other);
//    }
//
//    [[nodiscard]] constexpr bool operator>=(const big_int& other) const noexcept
//    {
//        return !(*this < other);
//    }
//
//    // TODO : spaceship operator
//
//    // Modifying operations
//
//    // Makes the number it's absolute value
//    constexpr void abs() noexcept
//    {
//        if (isNegative())
//        {
//            negate();
//        }
//    }
//
//    // Prefix oeprator ++
//    constexpr big_int& operator++() noexcept
//    {
//        increment();
//        return *this;
//    }
//
//    // Prefix oeprator --
//    constexpr big_int& operator--() noexcept
//    {
//        decrement();
//        return *this;
//    }
//
//    // Postfix oeprator ++
//    constexpr big_int operator++(int) noexcept  // NOLINT(cert-dcl21-cpp)
//    {
//        const big_int cpy = *this;
//        increment();
//        return cpy;
//    }
//
//    // Postfix oeprator --
//    constexpr big_int operator--(int) noexcept  // NOLINT(cert-dcl21-cpp)
//    {
//        const big_int cpy = *this;
//        decrement();
//        return cpy;
//    }
//
//    constexpr big_int& operator+=(const big_int& other) noexcept
//    {
//        bool carry = false;
//        for (size_t i = 0; i < size; ++i)
//        {
//            const u8 old = raw[i];
//            raw[i] += other.raw[i];
//            const bool overflowed_on_sum = raw[i] < old;
//
//            const u8 summed = raw[i];
//            raw[i] += carry;  // separate expression to prevent overflow on
//                              // operands in the first incrementation
//
//            const bool overflowed_on_carry = raw[i] < summed;
//            // assert(overflowed_on_carry != overflowed_on_sum ||
//            //       !overflowed_on_carry || !overflowed_on_sum);
//
//            carry = overflowed_on_carry || overflowed_on_sum;
//        }
//
//        // TODO : enable checks for overflow
//
//        return *this;
//    }
//
//    constexpr big_int& operator-=(const big_int& other) noexcept
//    {
//        *this += -other;
//        return *this;
//    }
//
//    constexpr big_int& operator<<=(const big_int& other) noexcept
//    {
//        // assert(other >= 0 &&
//        //       "Need to shift by a positive amount, maybe you need "
//        //       "to use the other shift!");
//
//        for (big_int i = zero(); i < other; ++i)
//        {
//            left_shift_once();
//        }
//        return *this;
//    }
//
//    constexpr big_int& operator>>=(const big_int& other) noexcept
//    {
//        // assert(other >= 0 &&
//        //       "Need to shift by a positive amount, maybe you need "
//        //       "to use the other shift!");
//
//        for (big_int i = u8(0); i < other; ++i)
//        {
//            right_shift_once();
//        }
//        return *this;
//    }
//
//    constexpr big_int& operator*=(const big_int& other) noexcept
//    {
//        *this = *this * other;
//        return *this;
//    }
//
//    constexpr big_int& operator/=(const big_int& other);
//    constexpr big_int& operator%=(const big_int& other);
//
//    constexpr big_int& operator&=(const big_int& other) noexcept
//    {
//        for (size_t i = 0; i < size; ++i)
//        {
//            raw[i] &= other.raw[i];
//        }
//        return *this;
//    }
//
//    constexpr big_int& operator|=(const big_int& other) noexcept
//    {
//        for (size_t i = 0; i < size; ++i)
//        {
//            raw[i] |= other.raw[i];
//        }
//        return *this;
//    }
//
//    constexpr big_int& operator^=(const big_int& other) noexcept
//    {
//        for (size_t i = 0; i < size; ++i)
//        {
//            raw[i] ^= other.raw[i];
//        }
//        return *this;
//    }
//
//    // Non-modifying operators
//
//    constexpr big_int operator+() const noexcept { return *this; }
//
//    [[nodiscard]] constexpr big_int operator-() const noexcept
//    {
//        big_int cpy = *this;
//        cpy.negate();
//        return cpy;
//    }
//
//    [[nodiscard]] constexpr big_int operator+(
//        const big_int& other) const noexcept
//    {
//        big_int res = *this;
//        res += other;
//        return res;
//    }
//
//    [[nodiscard]] constexpr big_int operator-(
//        const big_int& other) const noexcept
//    {
//        big_int res = *this;
//        res -= other;
//        return res;
//    }
//
//    [[nodiscard]] constexpr big_int operator*(
//        const big_int& other) const noexcept
//    {
//        // check out
//        // 1. https://en.wikipedia.org/wiki/Karatsuba_algorithm
//        // 2. https://en.wikipedia.org/wiki/Toom%E2%80%93Cook_multiplication
//        // 3.
//        // https://en.wikipedia.org/wiki/Sch%C3%B6nhage%E2%80%93Strassen_algorithm
//        // 4. https://en.wikipedia.org/wiki/F%C3%BCrer%27s_algorithm
//        // 5.
//        // https://projecteuclid.org/journals/annals-of-mathematics/volume-193/issue-2/Integer-multiplication-in-time-Onmathrmlog-n/10.4007/annals.2021.193.2.4.short
//
//        // big_int res = 0;
//
//        // big_int summand = other;
//
//        // for (size_t byte_idx = 0; byte_idx < size; ++byte_idx)
//        //{
//        //    for (u8 bit_idx = 0; bit_idx < 8;
//        //         ++bit_idx, summand.left_shift_once())
//        //    {
//        //        if (is_bit_set(byte_idx, bit_idx))
//        //        {
//        //            res += summand;
//        //        }
//        //    }
//        //}
//
//        big_int res = 0;
//
//        const bool should_negate = other.isNegative();
//
//        const big_int abs_other = other.isNegative() ? -other : other;
//
//        for (big_int i = 0; i < abs_other; ++i)
//        {
//            res += *this;
//        }
//
//        if (should_negate)
//        {
//            res.negate();
//        }
//        return res;
//    }
//
//    [[nodiscard]] constexpr big_int operator/(
//        const big_int& other) const noexcept
//    {
//        big_int res = *this;
//        res /= other;
//        return res;
//    }
//
//    [[nodiscard]] constexpr big_int operator%(
//        const big_int& other) const noexcept
//    {
//        big_int res = *this;
//        res %= other;
//        return res;
//    }
//
//    [[nodiscard]] constexpr bool operator!() const noexcept
//    {
//        return !bool(*this);
//    }
//
//    [[nodiscard]] constexpr bool operator&&(const big_int& other) const noexcept
//    {
//        return bool(*this) && bool(other);
//    }
//
//    [[nodiscard]] constexpr bool operator||(const big_int& other) const noexcept
//    {
//        return bool(*this) || bool(other);
//    }
//
//    // Bitwise operations
//
//    [[nodiscard]] constexpr big_int operator~() const noexcept
//    {
//        big_int cpy = *this;
//        for (size_t i = 0; i < size; ++i)
//        {
//            cpy.raw[i] = ~raw[i];
//        }
//        return cpy;
//    }
//
//    [[nodiscard]] constexpr big_int operator&(
//        const big_int& other) const noexcept
//    {
//        big_int cpy = *this;
//        cpy &= other;
//        return cpy;
//    }
//
//    [[nodiscard]] constexpr big_int operator|(
//        const big_int& other) const noexcept
//    {
//        big_int cpy = *this;
//        cpy |= other;
//        return cpy;
//    }
//
//    [[nodiscard]] constexpr big_int operator^(
//        const big_int& other) const noexcept
//    {
//        big_int cpy = *this;
//        cpy ^= other;
//        return cpy;
//    }
//
//    [[nodiscard]] constexpr big_int operator<<(
//        const big_int& other) const noexcept
//    {
//        big_int cpy = *this;
//        cpy <<= other;
//        return cpy;
//    }
//
//    [[nodiscard]] constexpr big_int operator>>(
//        const big_int& other) const noexcept
//    {
//        big_int cpy = *this;
//        cpy >>= other;
//        return cpy;
//    }
//
//    // Conversions
//    [[nodiscard]] explicit operator bool() const noexcept
//    {
//        for (const u8 byte : raw)
//        {
//            if (byte != 0)
//            {
//                return true;
//            }
//        }
//        return false;
//    }
//
//    // Static factory methods
//    [[nodiscard]] constexpr static big_int zero() noexcept
//    {
//        return big_int<size>(u8(0));
//    }
//
//    [[nodiscard]] constexpr static big_int one() noexcept
//    {
//        return big_int<size>(u8(1));
//    }
//
//    [[nodiscard]] constexpr static big_int(min)() noexcept
//    {
//        big_int<size> tmp;
//        tmp.flipSignBit();
//        return tmp;
//    }
//
//    [[nodiscard]] constexpr static big_int(max)() noexcept { return ~min(); }
//
//    // TODO : disallow consecutive separator chars - '
//    // TODO : parse octal and hex numbers
//    template <size_t arrSz>
//    [[nodiscard]] constexpr static big_int from_fixed_char_array(
//        const std::array<char, arrSz>& arr) /*noexcept*/
//    {
//        static_assert(arrSz > 1, "Cannot have zero length integers!");
//
//        const big_int base = 10;
//        big_int res = 0;
//
//        const bool should_negate = arr[0] == '-';
//        const bool should_skip_first = should_negate || (arr[0] == '+');
//
//        for (size_t i = should_skip_first ? 1 : 0; i < arrSz && arr[i] != '\0';
//             ++i)
//        {
//            if (arr[i] == '\'')
//            {
//                continue;
//            }
//
//            if (!constexpr_is_digit(arr[i]))
//            {
//                throw std::invalid_argument("Illegal character in number!");
//            }
//
//            res *= base;
//            res += arr[i] - '0';
//        }
//
//        if (should_negate)
//        {
//            res.negate();
//        }
//        return res;
//    }
//
//    // stored in order from least significant to most significant
//    // 0 -> least significant
//    // size - 1 -> most significant
//    std::array<u8, size> raw = {0};
//
//private:
//    // TODO : reinterpret the raw as an array of the
//    // fastest unsigned integer and make calculations in that
//
//#pragma region initialization_from_arithmetic
//
//    template <typename T>
//    constexpr static std::array<u8, sizeof(T)> bytesOf(const T value) noexcept
//    {
//        static_assert(std::is_integral<T>::value,
//                      "The type has to be integral!");
//        constexpr size_t bytesCnt = sizeof(T);
//
//        constexpr u8 byteMask = 0b11111111;
//        constexpr size_t bitsInByte = 8;
//        constexpr T typeMask = T(byteMask);
//
//        std::array<u8, bytesCnt> res{};
//
//        for (size_t i = 0; i < sizeof(T); ++i)
//        {
//            res[i] =
//                (value & (typeMask << (i * bitsInByte))) >> (i * bitsInByte);
//        }
//        return res;
//    }
//
//    template <size_t arrSz>
//    constexpr static bool is_rest_all_zeroes(const std::array<u8, arrSz>& arr,
//                                             size_t idx) noexcept
//    {
//        // assert(idx < arrSz && "Cannot check ouside of array!");
//        for (size_t i = idx; i < arrSz; ++i)
//        {
//            if (arr[i] != 0)
//            {
//                return false;
//            }
//        }
//        return true;
//    }
//
//    template <size_t srcCnt>
//    constexpr static void sign_extend_into(
//        std::array<u8, size>& raw,
//        const std::array<u8, srcCnt>& src) noexcept
//    {
//        static_assert(
//            size >= srcCnt,
//            "The resulting count has to be greater than(or equal) the "
//            "destination count!");
//
//        constexpr u8 fillMask = 0b11111111;
//        constexpr u8 emptyMask = 0b00000000;
//
//        size_t currIdx = 0;
//
//        while (!is_rest_all_zeroes<srcCnt>(src, currIdx) && currIdx < srcCnt)
//        {
//            raw[currIdx] = src[currIdx];
//            ++currIdx;
//        }
//
//        const bool sign = most_significant_bit(src[srcCnt - 1]) != 0;
//
//        for (size_t i = currIdx; i < size; ++i)
//        {
//            raw[i] = sign ? fillMask : emptyMask;
//        }
//    }
//
//    template <typename T>
//    constexpr void big_int_init(T a) noexcept
//    {
//        static_assert(std::is_integral<T>::value,
//                      "The type has to be integral!");
//        static_assert(size >= sizeof(T),
//                      "The size of the big int must be greater than the size "
//                      "of the source type");
//
//        sign_extend_into(raw, bytesOf<T>(a));
//    }
//#pragma endregion
//
//#pragma region sign_access_and_manipulation
//    // true if the number is negative
//    [[nodiscard]] constexpr bool isNegative() const noexcept
//    {
//        constexpr size_t bitsInByte = 8;
//        return raw[size - 1] & (1U << (bitsInByte * sizeof(u8) - 1));
//    }
//
//    constexpr void flipSignBit() noexcept
//    {
//        constexpr size_t bitsInByte = 8;
//        raw[size - 1] ^= (1U << (bitsInByte * sizeof(u8) - 1));
//    }
//
//    // negates the number in two's complement
//    constexpr void negate() noexcept
//    {
//        for (size_t i = 0; i < size; ++i)
//        {
//            raw[i] = ~raw[i];
//        }
//        increment();
//    }
//
//#pragma endregion
//
//#pragma region arithmetic_helpers
//    // Increments the number by one
//    constexpr void increment() noexcept
//    {
//        bool carry = false;
//        size_t currIdx = 0;
//        do
//        {
//            u8 oldValue = raw[currIdx];
//            ++raw[currIdx];
//            carry = oldValue > raw[currIdx];
//            ++currIdx;
//        } while (carry && currIdx < size);
//
//        // TODO: if the currIdx is size, then there is overlow
//        // maybe make it configurable so it throws
//    }
//
//    // Decrements the number by one
//    constexpr void decrement() noexcept
//    {
//        bool take = false;
//        size_t currIdx = 0;
//        do
//        {
//            u8 oldValue = raw[currIdx];
//            --raw[currIdx];
//            take = oldValue < raw[currIdx];
//            ++currIdx;
//        } while (take && currIdx < size);
//
//        // TODO: if the currIdx is size, then there is underflow
//        // maybe make it configurable so it throws
//    }
//#pragma endregion
//
//#pragma region bitwise_helpers
//    constexpr void left_shift_once() noexcept
//    {
//        u8 carry = 0;
//        for (size_t i = 0; i < size; ++i)
//        {
//            const u8 msb = most_significant_bit(raw[i]);
//            raw[i] <<= 1;
//            raw[i] |= carry;
//            carry = msb;
//        }
//    }
//
//    constexpr void right_shift_once() noexcept
//    {
//        u8 carry = 0;
//
//        for (size_t i = size - 1; i < size; --i)
//        {
//            const u8 lsb = least_significant_bit(raw[i]);
//            raw[i] >>= 1;
//            raw[i] |= (carry << 7);
//            carry = lsb;
//        }
//    }
//
//    constexpr bool is_bit_set(size_t byte_idx, u8 bit_idx) const noexcept
//    {
//        assert(byte_idx < size);
//        assert(bit_idx < 8);
//        return raw[byte_idx] & (1 << (bit_idx));
//    }
//#pragma endregion
//};
//
//// TODO : user defined literals
//template <char... c>
//struct size_to_fit : std::integral_constant<size_t, 128>
//{
//};
//
//template <char... c>
//constexpr big_int<size_to_fit<c...>::value> operator""_bi() noexcept
//{
//    constexpr std::array<char, sizeof...(c) + 1> str = {c...};
//
//    return big_int<size_to_fit<c...>::value>::template from_fixed_char_array(
//        str);
//}
//
//// TODO : Specialize numeric_limits for bigint
//#include <limits>
//
//#include <type_traits>
//
//// constexpr auto avogardo_number = 602'214'076'000'000'000'000'000_bi;
//// constexpr auto speed_of_light = 299'792'458_bi;
//// constexpr auto hyperfine_transition_freq_cs = 9'192'631'770_bi;
//// constexpr auto monochromatic_radiation_freq_candela = 540'000'000'000'000_bi;
//
//// static_assert(std::is_pod_v<big_int<128>>);
//// static_assert(std::is_trivial_v<big_int<128>>);
//
//static_assert(std::is_nothrow_constructible_v<big_int<128>>);
//static_assert(std::is_default_constructible_v<big_int<128>>);
//static_assert(std::is_nothrow_default_constructible_v<big_int<128>>);
//
//static_assert(std::is_trivially_copyable_v<big_int<128>>);
//
//static_assert(std::is_literal_type_v<big_int<128>>);
//static_assert(std::has_unique_object_representations_v<big_int<128>>);
//
//static_assert(std::is_move_constructible_v<big_int<128>>);
//static_assert(std::is_trivially_move_constructible_v<big_int<128>>);
//static_assert(std::is_nothrow_move_constructible_v<big_int<128>>);
//
//// static_assert(std::is_assignable_v<big_int<128>,>);
//// static_assert(std::is_trivially_assignable_v<big_int<128>>);
//// static_assert(std::is_nothrow_assignable_v<big_int<128>>);
//
//// template <size_t size>
////[[nodiscard]] constexpr
////    typename std::common_type_t<big_int<size>, big_int<size>>
////        std::gcd<big_int<size>, big_int<size>>(big_int<size>, big_int<size>)
////{
////}
//
////#include <numeric>
//#include <string>
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> gcd(
//    const big_int<size>& a,
//    const big_int<size>& b) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> lcm(
//    const big_int<size>& a,
//    const big_int<size>& b) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> midpoint(
//    const big_int<size>& a,
//    const big_int<size>& b) noexcept
//{
//    const big_int<size> half_a = a >> 2;
//    const big_int<size> half_b = b >> 2;
//    return half_a + half_b;
//}
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> log(
//    const big_int<size>& base,
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> log2(
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> log10(
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> loglp(
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> exp(
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> exp2(
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> pow(
//    const big_int<size>& base,
//    const big_int<size>& power) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> expml(
//    const big_int<size>& number) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static std::string to_string(
//    const big_int<size>& num) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static big_int<size> from_string(
//    const std::string& str) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static std::string rotl(
//    const big_int<size>& num) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static std::string rotr(
//    const big_int<size>& num) noexcept;
//
//template <size_t size>
//[[nodiscard]] constexpr static std::string popcnt(
//    const big_int<size>& num) noexcept;
//
//template <size_t size>
//[[nodiscard]] static constexpr big_int<size> mirror(
//    const big_int<size>& num) noexcept
//{
//    big_int res;
//    for (size_t i = 0; i < size; ++i)
//    {
//        res.raw[i] = raw[size - i - 1];
//    }
//    return res;
//}
//
//template <size_t size>
//[[nodiscard]] static constexpr const big_int<size>& clamp(
//    const big_int<size>& num,
//    const big_int<size>& low,
//    const big_int<size>& high) noexcept
//{
//    assert(low < high);
//    if (num < low)
//    {
//        return low;
//    }
//    else if (num > high)
//    {
//        return high;
//    }
//    else
//    {
//        return num;
//    }
//}
//
//#include <ostream>
//
//template <size_t size>
//[[nodiscard]] static std::ostream& operator<<(std::ostream& os,
//                                              const big_int<size>& a)
//{
//    return os << to_string(a);
//}