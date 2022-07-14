#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string_view>

#include <iostream>
#include <numeric>

#include "big_int.hpp"
#include "big_int64.hpp"

struct Foo
{
    int num;
};

constexpr size_t test_max_size = 128;

int main()
{
    big_int64<128> bi64;

    {  // valid constexpr
        [[maybe_unused]] constexpr big_int<test_max_size> def;
        static_assert(sizeof(def) == test_max_size);

        [[maybe_unused]] constexpr big_int<test_max_size> eqInt = 3;
        [[maybe_unused]] constexpr big_int<test_max_size> ctorInt(3);
        [[maybe_unused]] constexpr big_int<test_max_size> eqLong = 3L;
        [[maybe_unused]] constexpr big_int<test_max_size> ctorLong(3L);
        [[maybe_unused]] constexpr big_int<test_max_size> eqLongLong = 3LL;
        [[maybe_unused]] constexpr big_int<test_max_size> ctorLongLong(3LL);

        [[maybe_unused]] constexpr big_int<test_max_size> ctorUInt(3U);
        [[maybe_unused]] constexpr big_int<test_max_size> eqULong = 3UL;
        [[maybe_unused]] constexpr big_int<test_max_size> ctorULong(3UL);
        [[maybe_unused]] constexpr big_int<test_max_size> eqULongLong = 3ULL;
        [[maybe_unused]] constexpr big_int<test_max_size> ctorULongLong(3ULL);
        static_assert(eqInt == ctorInt && ctorInt == eqLong);

        [[maybe_unused]] constexpr big_int<test_max_size> copy = def;

        [[maybe_unused]] constexpr big_int<test_max_size> minimal =
            big_int<test_max_size>::min();
        [[maybe_unused]] constexpr big_int<test_max_size> maximal =
            big_int<test_max_size>::max();

        static_assert(minimal + maximal == -1);
        static_assert(minimal - maximal == 1);
        static_assert(maximal + 1 == minimal);
        static_assert(maximal - minimal == -1);
        static_assert(minimal - 1 == maximal);
        static_assert(big_int<test_max_size>(-3) - big_int<test_max_size>(-2) ==
                      -1);
        constexpr big_int<test_max_size> one = 1;
        constexpr big_int<test_max_size> two = 2;
        constexpr big_int<test_max_size> three = 3;

        static_assert(minimal < maximal);
        static_assert(!(maximal < maximal));
        static_assert(!(maximal < minimal));
        static_assert(two < maximal);
        static_assert(two < three);
        static_assert(-two < three);
        static_assert(-three < two);
        static_assert(-three < -two);

        static_assert((one << 3) == (1 << 3));
        static_assert((one >> 0) == (1 >> 0));
        static_assert((one >> 3) == (1 >> 3));

        // constexpr auto gcd = std::gcd(two, two * three);

        // static_assert(gcd == 2);
        static_assert(std::clamp(three, one, two) == 2);

        // static_assert(123_bi == 123);

        static_assert(std::max(3_bi, 4_bi) == 4_bi);
        static_assert(std::min(-3_bi, 4_bi) == -3_bi);
    }

    {  // valid runtime
        [[maybe_unused]] const big_int<test_max_size> def;

        [[maybe_unused]] big_int<test_max_size> two = 2;
        [[maybe_unused]] big_int<test_max_size> three = 3;
        [[maybe_unused]] big_int<test_max_size> minusOne = -1;
        [[maybe_unused]] big_int<test_max_size> twoOoops = 4;
        twoOoops = 2;

        [[maybe_unused]] big_int<test_max_size> copy;
        copy = def;

        [[maybe_unused]] big_int<test_max_size> minimal =
            big_int<test_max_size>::min();
        [[maybe_unused]] big_int<test_max_size> maximal =
            big_int<test_max_size>::max();

        big_int<test_max_size> increment;
        ++increment;
        ++increment;
        assert(increment == two);

        big_int<test_max_size> overflow = big_int<test_max_size>::max();
        ++overflow;
        assert(overflow == minimal);

        assert(minimal < maximal);
        assert(!(maximal < maximal));
        assert(!(maximal < minimal));
        assert(two < maximal);
        assert(two < three);
        assert(-two < three);
        assert(-three < two);
        assert(-three < -two);

        big_int<test_max_size> underflow = big_int<test_max_size>::min();
        --underflow;
        assert(underflow == maximal);

        big_int<test_max_size> decremented = 0;
        --decremented;
        assert(decremented == minusOne);

        big_int<test_max_size> negated = -1;
        assert(-negated == 1);  // test out the auto conversion

        big_int<test_max_size> absoluted = -1;
        absoluted.abs();
        assert(absoluted == 1);
        assert(-absoluted == -1);
        assert(+absoluted == +1);
        assert(absoluted++ == 1);
        assert(absoluted == 2);
        assert(++absoluted == 3);

        assert(!big_int<test_max_size>());
        assert(big_int<test_max_size>(1));
        assert(bool(big_int<test_max_size>(1)));
        assert(std::min(big_int<test_max_size>(1), big_int<test_max_size>(0)) ==
               0);

        big_int<test_max_size> five = 3;
        five += 2;
        assert(five == 5);

        big_int<test_max_size> wrap_test_on_plus_eq =
            big_int<test_max_size>::max();
        wrap_test_on_plus_eq += 1;
        assert(wrap_test_on_plus_eq == big_int<test_max_size>::min());

        big_int<test_max_size> seven = 10;
        seven -= 3;
        assert(seven == 7);

        big_int<test_max_size> wrap_test_on_minus_eq =
            big_int<test_max_size>::min();
        wrap_test_on_minus_eq -= 1;
        assert(wrap_test_on_minus_eq == big_int<test_max_size>::max());

        // big_int<sizeof(long long)> i_bi = 0;
        // for (long long i = 0; i < 1024; ++i, ++i_bi)
        //{
        //    big_int<sizeof(long long)> j_bi = long long(0);
        //    for (long long j = 0; j < 1024; ++j, ++j_bi)
        //    {
        //        if (i_bi + j_bi != long long(i + j))
        //        {
        //            printf("Different!");
        //        }
        //        if (i_bi * j_bi != long long(i * j))
        //        {
        //            printf("Different mult!");
        //        }
        //    }
        //}

        {  // custom literal testing
            big_int<128> correct = 123;
            big_int<128> correct_neg = -123;
            // big_int avogadro = 602'214'076'000'000'000'000'000_bi;

            auto one_two_three = 123_bi;
            auto minus_one_two_three = -123_bi;

            assert(one_two_three == correct);
            assert(minus_one_two_three == correct_neg);
            assert(+0_bi == -0_bi);
        }
    }

    {
        // error constexpr
        [[maybe_unused]] constexpr Foo f{3};
        static_assert(!std::is_integral<Foo>::value);
        //[[maybe_unused]] constexpr big_int<32> err(f);
        // [[maybe_unused]] constexpr big_int<32> err(3.14);
        // [[maybe_unused]] constexpr big_int<2> err(2ull);
        //[[maybe_unused]] constexpr big_int<0> err;

        static_assert(
            !std::is_constructible<big_int<test_max_size>, Foo>::value);
    }

    {  // error runtime
       // [[maybe_unused]] big_int<32> err;
       // err = 3.14;
    }

    return 0;
}
