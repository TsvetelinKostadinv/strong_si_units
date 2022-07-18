#pragma once

#include "../util.hpp"

#include <algorithm>
#include <array>
#include <new>
#include <type_traits>

using widest_unsigned = u64;

template <size_t size>
struct wide_words_helper
{
    static constexpr size_t widest_unsigned_count =
        size / sizeof(widest_unsigned);
    std::array<widest_unsigned, widest_unsigned_count>
        raw_least_significant_wide = {0};
};

template <size_t size>
struct most_significant_bytes
{
    static constexpr size_t padding_u8_count = size % sizeof(widest_unsigned);

    std::array<u8, padding_u8_count> raw_most_significant_bytes = {0};
};

// TODO : get away from the pragma
#pragma pack(push, 1)

// Integer representation in size number of bytes
// two's complement
// constexpr friendly
// Guaranteed to wrap on overlow
// TODO : template parameter to control the behaviour on overflow
template <size_t size>
struct big_int : wide_words_helper<size>,
                 std::conditional_t<size % sizeof(widest_unsigned) != 0,
                                    most_significant_bytes<size>,
                                    void>
{
    // Stored in order from least significant to most significant
    // The least significant bytes are packaged in the widest words available
    // for less iterations in arithmetic operations

    static_assert(size != 0, "Cannot have zero-sized integer!");

    static constexpr bool contains_padding_bytes =
        size % sizeof(widest_unsigned) != 0;

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

#pragma region factory_ops

    [[nodiscard]] static constexpr big_int zero() noexcept
    {
        return big_int<size>(u8(0));
    }

    [[nodiscard]] static constexpr big_int one() noexcept
    {
        return big_int<size>(u8(1));
    }

    //[[nodiscard]] static constexpr big_int(max)() noexcept { return ~min(); }

#pragma endregion

#pragma region informational_ops

    // true if the number is negative
    [[nodiscard]] constexpr bool is_negative() const noexcept
    {
        return most_significant_bit(most_significant_byte()) != 0;
    }

#pragma region byte_ops

    [[nodiscard]] constexpr u8 most_significant_byte() const noexcept
    {
        if constexpr (contains_padding_bytes)
        {
            return raw_most_significant_bytes[padding_u8_count - 1];
        }
        else
        {
            return (raw_least_significant_wide[widest_unsigned_count - 1] >>
                    (64 - 8));
        }
    }

    [[nodiscard]] constexpr u8 most_significant_byte_no_sign() const noexcept
    {
        return most_significant_byte() & 0b01111111;
    }

#pragma endregion

#pragma region comparison_ops

    // TODO: compare with bigger/smaller big_ints

    [[nodiscard]] constexpr bool operator==(const big_int& other) const noexcept
    {
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            if (raw_least_significant_wide[i] !=
                other.raw_least_significant_wide[i])
            {
                return false;
            }
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < padding_u8_count; ++i)
            {
                if (raw_most_significant_bytes[i] !=
                    other.raw_most_significant_bytes[i])
                {
                    return false;
                }
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
            for (size_t i = widest_unsigned_count - 1;
                 i < widest_unsigned_count; --i)
            {
                if (raw_least_significant_wide[i] !=
                    other.raw_least_significant_wide[i])
                {
                    return raw_least_significant_wide[i] <
                           other.raw_least_significant_wide[i];
                }
            }

            if constexpr (contains_padding_bytes)
            {
                for (size_t i = padding_u8_count - 1; i < padding_u8_count; --i)
                {
                    if (raw_most_significant_bytes[i] !=
                        other.raw_most_significant_bytes[i])
                    {
                        return raw_most_significant_bytes[i] <
                               other.raw_most_significant_bytes[i];
                    }
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
#pragma endregion

#pragma endregion

#pragma region modifying_ops
#pragma region unary

    constexpr void flipSignBit() noexcept
    {
        constexpr size_t bitsInByte = 8;
        if constexpr (contains_padding_bytes)
        {
            raw_most_significant_bytes[padding_u8_count - 1] ^=
                (u8(1) << (bitsInByte * sizeof(u8) - 1));
        }
        else
        {
            raw_least_significant_wide[widest_unsigned_count - 1] ^=
                (u64(1) << (bitsInByte * sizeof(widest_unsigned) - 1));
        }
    }

    // negates the number in two's complement
    constexpr void negate() noexcept
    {
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            raw_least_significant_wide[i] = ~raw_least_significant_wide[i];
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < padding_u8_count; ++i)
            {
                raw_most_significant_bytes[i] = ~raw_most_significant_bytes[i];
            }
        }

        increment();
    }

    // Makes the number it's absolute value
    constexpr void abs() noexcept
    {
        if (is_negative())
        {
            negate();
        }
    }

#pragma region arithmetic

    // Increments the number by one
    constexpr void increment() noexcept
    {
        bool carry = false;
        size_t curr_idx = 0;
        do
        {
            u64 old_value = raw_least_significant_wide[curr_idx];
            ++raw_least_significant_wide[curr_idx];
            carry = old_value > raw_least_significant_wide[curr_idx];
            ++curr_idx;
        } while (carry && curr_idx < widest_unsigned_count);

        // Here if carry is true => we have a carry from the least significant
        // wide to the most significant bytes

        if constexpr (contains_padding_bytes)
        {
            curr_idx = 0;
            while (carry && curr_idx < padding_u8_count)
            {
                u8 old_value = raw_most_significant_bytes[curr_idx];
                ++raw_most_significant_bytes[curr_idx];
                carry = old_value > raw_most_significant_bytes[curr_idx];
                ++curr_idx;
            }

            // TODO: if the currIdx is padding_u8_count, then there is overlow
            // maybe make the behaviour configurable
        }
    }

    // Decrements the number by one
    constexpr void decrement() noexcept
    {
        bool take = false;
        size_t curr_idx = 0;
        do
        {
            u64 old_value = raw_least_significant_wide[curr_idx];
            --raw_least_significant_wide[curr_idx];
            take = old_value < raw_least_significant_wide[curr_idx];
            ++curr_idx;
        } while (take && curr_idx < widest_unsigned_count);

        // Here if take is true => we have a take from the most significant
        // bytes

        if constexpr (contains_padding_bytes)
        {
            curr_idx = 0;
            while (take && curr_idx < padding_u8_count)
            {
                u8 old_value = raw_most_significant_bytes[curr_idx];
                --raw_most_significant_bytes[curr_idx];
                take = old_value < raw_most_significant_bytes[curr_idx];
                ++curr_idx;
            }

            // TODO: if the curr_idx is padding_u8_count, then there is
            // maybe make the behaviour configurable
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
#pragma endregion

#pragma region bitwise

    constexpr void left_shift_once() noexcept
    {
        u8 carry = 0;
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            const u8 msb = most_significant_bit(raw_least_significant_wide[i]);
            raw_least_significant_wide[i] <<= 1;
            raw_least_significant_wide[i] |= carry;
            carry = msb;
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < padding_u8_count; ++i)
            {
                const u8 msb =
                    most_significant_bit(raw_most_significant_bytes[i]);
                raw_most_significant_bytes[i] <<= 1;
                raw_most_significant_bytes[i] |= carry;
                carry = msb;
            }
        }
    }

    constexpr void right_shift_once() noexcept
    {
        u8 carry = 0;

        for (size_t i = widest_unsigned_count - 1; i < widest_unsigned_count;
             --i)
        {
            const u8 lsb = least_significant_bit(raw_least_significant_wide[i]);
            raw_least_significant_wide[i] >>= 1;
            raw_least_significant_wide[i] |= (carry << 7);
            carry = lsb;
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = padding_u8_count - 1; i < padding_u8_count; --i)
            {
                const u8 lsb =
                    least_significant_bit(raw_most_significant_bytes[i]);
                raw_most_significant_bytes[i] >>= 1;
                raw_most_significant_bytes[i] |= (carry << 7);
                carry = lsb;
            }
        }
    }

#pragma endregion

#pragma endregion

#pragma endregion

#pragma region copy_on_write_ops
#pragma region arithmetic

    constexpr big_int operator+() const noexcept { return *this; }

    [[nodiscard]] constexpr big_int operator-() const noexcept
    {
        big_int cpy = *this;
        cpy.negate();
        return cpy;
    }
#pragma endregion

#pragma region bitwise

    [[nodiscard]] constexpr big_int operator~() const noexcept
    {
        big_int cpy = *this;
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            cpy.raw_least_significant_wide[i] =
                ~cpy.raw_least_significant_wide[i];
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < padding_u8_count; ++i)
            {
                cpy.raw_most_significant_bytes[i] =
                    ~cpy.raw_most_significant_bytes[i];
            }
        }

        return cpy;
    }

    [[nodiscard]] constexpr big_int operator<<(
        const big_int& other) const noexcept
    {
        // wildly suboptimal. Can shift going least
        // significant to most significant looking "backwards" at what should
        // end up at the target byte

        big_int res = *this;

        for (big_int i = zero(); i < other; ++i)
        {
            res.left_shift_once();
        }
        return res;
    }

    [[nodiscard]] constexpr big_int operator>>(
        const big_int& other) const noexcept
    {
        // wildly suboptimal. Can shift going most significant to least
        // significant looking "backwards" at what should end up at the target
        // byte

        big_int res = *this;

        for (big_int i = zero(); i < other; ++i)
        {
            res.right_shift_once();
        }
        return res;
    }
#pragma endregion

#pragma endregion

#pragma region arithmetic_ops

    [[nodiscard]] constexpr big_int operator+(
        const big_int& other) const noexcept
    {
        big_int res;
        bool carry = false;
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            res.raw_least_significant_wide[i] =
                raw_least_significant_wide[i] +
                other.raw_least_significant_wide[i];
            const bool overflowed_on_sum =
                res.raw_least_significant_wide[i] <
                    raw_least_significant_wide[i] ||
                res.raw_least_significant_wide[i] <
                    other.raw_least_significant_wide[i];

            const u64 summed = res.raw_least_significant_wide[i];
            // separate expression to prevent overflow on
            // operands in the first incrementation
            res.raw_least_significant_wide[i] += carry;

            const bool overflowed_on_carry =
                res.raw_least_significant_wide[i] < summed;
            // assert(overflowed_on_carry != overflowed_on_sum ||
            //       !overflowed_on_carry || !overflowed_on_sum);

            carry = overflowed_on_carry || overflowed_on_sum;
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < padding_u8_count; ++i)
            {
                res.raw_most_significant_bytes[i] =
                    raw_most_significant_bytes[i] +
                    other.raw_most_significant_bytes[i];
                const bool overflowed_on_sum =
                    res.raw_most_significant_bytes[i] <
                        raw_most_significant_bytes[i] ||
                    res.raw_most_significant_bytes[i] <
                        other.raw_most_significant_bytes[i];

                const u8 summed = res.raw_most_significant_bytes[i];
                // separate expression to prevent overflow on
                // operands in the first incrementation
                res.raw_most_significant_bytes[i] += carry;

                const bool overflowed_on_carry =
                    res.raw_most_significant_bytes[i] < summed;
                // assert(overflowed_on_carry != overflowed_on_sum ||
                //       !overflowed_on_carry || !overflowed_on_sum);

                carry = overflowed_on_carry || overflowed_on_sum;
            }
        }
        return res;
    }

    [[nodiscard]] constexpr big_int operator-(
        const big_int& other) const noexcept
    {
        return *this + (-other);
    }

#pragma endregion

#pragma region bitwise_ops
    [[nodiscard]] constexpr big_int operator&(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            res.raw_least_significant_wide[i] &=
                other.raw_least_significant_wide[i];
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < paddin_u8_bytes; ++i)
            {
                res.raw_most_significant_bytes[i] &=
                    other.raw_most_significant_bytes[i];
            }
        }

        return res;
    }

    [[nodiscard]] constexpr big_int operator|(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            res.raw_least_significant_wide[i] |=
                other.raw_least_significant_wide[i];
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < paddin_u8_bytes; ++i)
            {
                res.raw_most_significant_bytes[i] |=
                    other.raw_most_significant_bytes[i];
            }
        }

        return res;
    }

    [[nodiscard]] constexpr big_int operator^(
        const big_int& other) const noexcept
    {
        big_int res = *this;
        for (size_t i = 0; i < widest_unsigned_count; ++i)
        {
            res.raw_least_significant_wide[i] ^=
                other.raw_least_significant_wide[i];
        }

        if constexpr (contains_padding_bytes)
        {
            for (size_t i = 0; i < paddin_u8_bytes; ++i)
            {
                res.raw_most_significant_bytes[i] ^=
                    other.raw_most_significant_bytes[i];
            }
        }

        return res;
    }

#pragma endregion

#pragma region conversion_ops
    [[nodiscard]] explicit operator bool() const noexcept
    {
        const bool least_significant_nz =
            std::any_of(raw_least_significant_wide.begin(),
                        raw_least_significant_wide.end(),
                        [](const u64 byte) { return byte != 0; });

        if constexpr (contains_padding_bytes)
        {
            return least_significant_nz ||
                   std::any_of(raw_most_significant_bytes.begin(),
                               raw_most_significant_bytes.end(),
                               [](const u8 byte) { return byte != 0; });
        }

        return least_significant_nz;
    }
#pragma endregion

#pragma region initialization_from_arithmetic

    template <typename T>
    constexpr void big_int_init(T a) noexcept
    {
        static_assert(std::is_integral<T>::value,
                      "The type has to be integral!");
        static_assert(size >= sizeof(T),
                      "The size of the big int must be greater than the size "
                      "of the source type");
        // T is integral and for sure is less than u64
        raw_least_significant_wide[0] = a;
        const bool should_fill_with_sign_bit = a < 0;
        constexpr u64 full_mask64 = ~u64(0);

        if (should_fill_with_sign_bit)
        {
            for (size_t i = 1; i < widest_unsigned_count; ++i)
            {
                raw_least_significant_wide[i] = full_mask64;
            }

            if constexpr (contains_padding_bytes)
            {
                constexpr u8 full_mask = ~u8(0);
                for (size_t i = 0; i < padding_u8_count; ++i)
                {
                    raw_most_significant_bytes[i] = full_mask;
                }
            }
        }
    }

#pragma endregion
};
#pragma pack(pop)

static_assert(std::is_trivially_copyable_v<big_int<128>>);

static_assert(std::is_literal_type_v<big_int<128>>);
// TODO : fix assertion without the pragma
static_assert(std::has_unique_object_representations_v<big_int<128>>);
static_assert(std::has_unique_object_representations_v<big_int<129>>);
static_assert(std::has_unique_object_representations_v<big_int<130>>);
static_assert(sizeof(big_int<128>) == 128);
static_assert(sizeof(big_int<129>) == 129);
static_assert(sizeof(big_int<130>) == 130);

static_assert(std::is_move_constructible_v<big_int<128>>);
static_assert(std::is_trivially_move_constructible_v<big_int<128>>);
static_assert(std::is_nothrow_move_constructible_v<big_int<128>>);

// Assuring common_type can be specialized for the big_int
static_assert(std::is_same_v<std::decay_t<big_int<128>>, big_int<128>>);
static_assert(std::is_same_v<std::decay_t<big_int<129>>, big_int<129>>);
template <size_t sz_a, size_t sz_b>
struct std::common_type<big_int<sz_a>, big_int<sz_b>>
{
    using type = big_int<std::max(sz_a, sz_b)>;
};

#include <limits>

template <size_t size>
class std::numeric_limits<big_int<size>>
{
public:
    [[nodiscard]] static constexpr big_int<size>(min)() noexcept
    {
        big_int<size> tmp;
        tmp.flipSignBit();
        return tmp;
        // return cached_min;
    }

    [[nodiscard]] static constexpr big_int<size>(max)() noexcept
    {
        return cached_max;
    }

    [[nodiscard]] static constexpr big_int<size> lowest() noexcept
    {
        return cached_min;
    }

    [[nodiscard]] static constexpr big_int<size> epsilon() noexcept
    {
        return big_int<size>();
    }

    [[nodiscard]] static constexpr big_int<size> round_error() noexcept
    {
        return big_int<size>();
    }

    [[nodiscard]] static constexpr big_int<size> denorm_min() noexcept
    {
        return big_int<size>();
    }

    [[nodiscard]] static constexpr big_int<size> infinity() noexcept
    {
        return big_int<size>();
    }

    [[nodiscard]] static constexpr big_int<size> quiet_NaN() noexcept
    {
        return big_int<size>();
    }

    [[nodiscard]] static constexpr big_int<size> signaling_NaN() noexcept
    {
        return big_int<size>();
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
    static constexpr int digits = size;
    // static constexpr int digits10 = ?;
    // static constexpr int max_digits10 = ?;

    // The lower bytes are combined into 2^64 base
    // The upper bytes are coded in 2^8 base
    // At the end it's all in 256 base, so we say that's the radix
    static constexpr int radix = 256;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;

private:
    static constexpr big_int<size> cached_min = []() noexcept
    {
        big_int<size> tmp;
        tmp.flipSignBit();
        return tmp;
    }();

    static constexpr big_int<size> cached_max = []() noexcept
    {
        big_int<size> tmp;
        tmp.flipSignBit();
        return ~tmp;
    }();
};