#pragma once

#define ALL_INTEGRAL                                                       \
    char, signed char, unsigned char, short, signed short, unsigned short, \
        int, signed int, unsigned int, long, signed long, unsigned long,   \
        long long, signed long long, unsigned long long

//template <typename seq, size_t t_offset>
//struct offset_index_sequence;
//
//template <std::size_t t_offset, std::size_t... Ints>
//struct offset_index_sequence<std::index_sequence<Ints...>, t_offset>
//{
//    constexpr static size_t offset = t_offset;
//    using type = std::index_sequence<Ints + N...>;
//};

template <std::size_t N, typename Seq>
struct offset_index_sequence;

template <std::size_t N, std::size_t... Ints>
struct offset_index_sequence<N, std::index_sequence<Ints...>>
{
    using type = std::index_sequence<Ints + N...>;
};