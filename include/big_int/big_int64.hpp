#pragma once

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
    constexpr big_int64(T a)  // NOLINT(hicpp-explicit-conversions)
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
        return reinterpret_cast<widest_unsigned* const>(raw.data());
    }

    constexpr const widest_unsigned* const get_widest_unsigned_pointer()
        const noexcept
    {
        return reinterpret_cast<widest_unsigned* const>(raw.data());
    }

    constexpr void init_raw_for_widest_unsigned_access() noexcept
    {
        new (raw.data()) widest_unsigned[widest_unsigned_count];
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
};

#include <type_traits>

static_assert(std::is_trivially_copyable_v<big_int64<128>>);

static_assert(std::is_literal_type_v<big_int64<128>>);
static_assert(std::has_unique_object_representations_v<big_int64<128>>);

static_assert(std::is_move_constructible_v<big_int64<128>>);
static_assert(std::is_trivially_move_constructible_v<big_int64<128>>);
static_assert(std::is_nothrow_move_constructible_v<big_int64<128>>);