#pragma once

#include <utility>

#define ALL_INTEGRAL                                                       \
    char, signed char, unsigned char, short, signed short, unsigned short, \
        int, signed int, unsigned int, long, signed long, unsigned long,   \
        long long, signed long long, unsigned long long

template <std::size_t t_offset, typename Seq>
struct offset_index_sequence;

template <std::size_t t_offset, std::size_t... idxs>
struct offset_index_sequence<t_offset, std::index_sequence<idxs...>>
{
    using sequence = std::index_sequence<idxs + t_offset...>;
};