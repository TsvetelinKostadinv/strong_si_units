#pragma once

#include <cstdint>

using u8 = std::uint8_t;
using i8 = std::int8_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

constexpr static bool constexpr_is_digit(char ch) noexcept
{
    return '0' <= ch && ch <= '9';
}