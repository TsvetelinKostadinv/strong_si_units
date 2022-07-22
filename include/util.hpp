#pragma once

#include <cstdint>

using u8 = std::uint8_t;
using i8 = std::int8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

constexpr static bool constexpr_is_digit(char ch) noexcept
{
    return '0' <= ch && ch <= '9';
}

constexpr static u8 most_significant_bit(u8 byte) noexcept
{
    return u8(byte >> 7);
}

constexpr static u8 most_significant_bit(u64 wide) noexcept
{
    return u8((wide >> 24) >> 7);
}

constexpr static u8 least_significant_bit(u8 byte) noexcept
{
    return u8(byte & 1);
}

constexpr static u8 least_significant_bit(u64 wide) noexcept
{
    return u8(wide & 1);
}

#include <utility>
#include <version>

#if __has_cpp_attribute(nodiscard)
#define BIG_INT_NODISCARD [[nodiscard]]
#else
#define BIG_INT_NODISCARD
#endif  // __has_cpp_attribute(nodiscard)

#if __has_cpp_attribute(maybe_unused)
#define BIG_INT_UNUSED [[maybe_unused]]
#else
#define BIG_INT_UNUSED
#endif

#if __cpp_lib_unreachable
#define BIG_INT_UNREACHABLE \
    {                       \
        assert(false);      \
        std::unreachable(); \
    }
#else
#define BIG_INT_UNREACHABLE \
    {                       \
    }
#endif
