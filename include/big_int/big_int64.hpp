#pragma once

// Unfinished implementation

/*

#include "../util.hpp"

#include <array>
#include <new>

template <size_t size>
struct big_int64
{
    static_assert(size != 0, "Cannot have zero-sized integer!");

    constexpr big_int64() noexcept : raw{0} {};

    // Specifically not explicit so it can be used like regular numbers
    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value>::type>
    constexpr big_int64(T a) noexcept  // NOLINT(hicpp-explicit-conversions)
    {
        big_int_init<T>(a);
    }

    // stored in order from least significant to most significant
    // 0 -> least significant
    // size - 1 -> most significant
    // The first `widest_unsigned_count` can be accessed as u64's
    // The remaining `padding_u8_count` MUST be accessed as u8's
    std::array<u8, size> raw;

    using widest_unsigned = u64;
    static constexpr size_t widest_unsigned_count =
        size / sizeof(widest_unsigned);
    static constexpr size_t padding_u8_count = size % sizeof(widest_unsigned);

    constexpr widest_unsigned* const get_widest_unsigned_pointer() noexcept
    {
        return std::launder(
            reinterpret_cast<widest_unsigned* const>(raw.data()));
    }

    [[nodiscard]] constexpr const widest_unsigned* const
    get_widest_unsigned_pointer() const noexcept
    {
        // TODO : reinterpret cast not constexpr friendly
        return std::launder(
            reinterpret_cast<const widest_unsigned* const>(raw.data()));
    }

    constexpr widest_unsigned touch() const noexcept
    {
        return get_widest_unsigned_pointer()[0];
    }

    constexpr void init_raw_for_widest_unsigned_access() noexcept
    {
        // no constexpr placement new
        // new (raw.data()) widest_unsigned[widest_unsigned_count];
    }

#pragma region initialization_from_arithmetic

    template <typename T>
    constexpr static std::array<u8, sizeof(T)> bytesOf(const T value) noexcept
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
            res[i] =
                (value & (typeMask << (i * bitsInByte))) >> (i * bitsInByte);
        }
        return res;
    }

    template <size_t arrSz>
    constexpr static bool is_rest_all_zeroes(const std::array<u8, arrSz>& arr,
                                             size_t idx) noexcept
    {
        // assert(idx < arrSz && "Cannot check ouside of array!");
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
        constexpr u8 emptyMask = 0b00000000;

        size_t currIdx = 0;

        while (!is_rest_all_zeroes<srcCnt>(src, currIdx) && currIdx < srcCnt)
        {
            raw[currIdx] = src[currIdx];
            ++currIdx;
        }

        const bool sign = most_significant_bit(src[srcCnt - 1]) != 0;

        for (size_t i = currIdx; i < size; ++i)
        {
            raw[i] = sign ? fillMask : emptyMask;
        }
    }

    template <typename T>
    constexpr void big_int_init(T a) noexcept
    {
        static_assert(std::is_integral<T>::value,
                      "The type has to be integral!");
        static_assert(size >= sizeof(T),
                      "The size of the big int must be greater than the size "
                      "of the source type");

        init_raw_for_widest_unsigned_access();
    }
#pragma endregion
};

#include <type_traits>

static_assert(std::is_trivially_copyable_v<big_int64<128>>);

static_assert(std::is_literal_type_v<big_int64<128>>);
static_assert(std::has_unique_object_representations_v<big_int64<128>>);

static_assert(std::is_move_constructible_v<big_int64<128>>);
static_assert(std::is_trivially_move_constructible_v<big_int64<128>>);
static_assert(std::is_nothrow_move_constructible_v<big_int64<128>>);

*/