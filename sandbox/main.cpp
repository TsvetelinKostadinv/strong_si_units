#include <algorithm>
#include <cassert>

//#include "big_int64.hpp"
#include "big_int.hpp"

struct Foo
{
    int num;
};

struct Empty
{
};

template <size_t test_size>
void constexpr_and_runtime_test();

int main()
{
    constexpr_and_runtime_test<64>();
    //constexpr_and_runtime_test<65>();
    // cosntexpr_and_runtime_test<66>();
    // cosntexpr_and_runtime_test<67>();

    //constexpr_and_runtime_test<128>();
    //constexpr_and_runtime_test<129>();
    // cosntexpr_and_runtime_test<130>();
    // cosntexpr_and_runtime_test<131>();

    return 0;
}

template <size_t test_size>
void constexpr_and_runtime_test()
{
    {  // valid constexpr
        [[maybe_unused]] constexpr big_int<test_size> def =
            big_int<test_size>();
        static_assert(sizeof(def) == test_size);

        [[maybe_unused]] constexpr big_int<test_size> eqInt = 3;
        [[maybe_unused]] constexpr big_int<test_size> ctorInt(3);
        [[maybe_unused]] constexpr big_int<test_size> eqLong = 3L;
        [[maybe_unused]] constexpr big_int<test_size> ctorLong(3L);
        [[maybe_unused]] constexpr big_int<test_size> eqLongLong = 3LL;
        [[maybe_unused]] constexpr big_int<test_size> ctorLongLong(3LL);

        [[maybe_unused]] constexpr big_int<test_size> ctorUInt(3U);
        [[maybe_unused]] constexpr big_int<test_size> eqULong = 3UL;
        [[maybe_unused]] constexpr big_int<test_size> ctorULong(3UL);
        [[maybe_unused]] constexpr big_int<test_size> eqULongLong = 3ULL;
        [[maybe_unused]] constexpr big_int<test_size> ctorULongLong(3ULL);
        static_assert(eqInt == ctorInt && ctorInt == eqLong);

        [[maybe_unused]] constexpr big_int<test_size> copy = def;

        [[maybe_unused]] constexpr big_int<test_size> minimal =
            std::numeric_limits<big_int<test_size>>::min();
        [[maybe_unused]] constexpr big_int<test_size> maximal =
            std::numeric_limits<big_int<test_size>>::max();

        static_assert(minimal + maximal == -1);
        static_assert(minimal - maximal == 1);

        static_assert(maximal + 1 == minimal);
        static_assert(maximal - minimal == -1);
        static_assert(minimal - 1 == maximal);
        static_assert(big_int<test_size>(-3) - big_int<test_size>(-2) == -1);
        constexpr big_int<test_size> one = 1;
        constexpr big_int<test_size> two = 2;
        constexpr big_int<test_size> three = 3;

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

        // TODO : make std::gcd work
        // constexpr auto gcd = std::gcd(two, two * three);

        // static_assert(gcd == 2);
        static_assert(std::clamp(three, one, two) == 2);

        static_assert(123_bi == 123);

        static_assert(std::max(3_bi, 4_bi) == 4_bi);
        static_assert(std::min(-3_bi, 4_bi) == -3_bi);
    }

    {  // valid runtime
        [[maybe_unused]] const big_int<test_size> def;

        [[maybe_unused]] big_int<test_size> two = 2;
        [[maybe_unused]] big_int<test_size> three = 3;
        [[maybe_unused]] big_int<test_size> minusOne = -1;
        [[maybe_unused]] big_int<test_size> twoOoops = 4;
        twoOoops = 2;

        [[maybe_unused]] big_int<test_size> copy;
        copy = def;

        [[maybe_unused]] const big_int<test_size> minimal =
            std::numeric_limits<big_int<test_size>>::min();
        [[maybe_unused]] const big_int<test_size> maximal =
            std::numeric_limits<big_int<test_size>>::max();

        big_int<test_size> increment;
        ++increment;
        ++increment;
        assert(increment == two);

        big_int<test_size> overflow = maximal;
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

        big_int<test_size> underflow = minimal;
        --underflow;
        assert(underflow == maximal);

        big_int<test_size> decremented = 0;
        --decremented;
        assert(decremented == minusOne);

        big_int<test_size> negated = -1;
        assert(-negated == 1);  // test out the auto conversion

        big_int<test_size> absoluted = -1;
        absoluted.abs();
        assert(absoluted == 1);
        assert(-absoluted == -1);
        assert(+absoluted == +1);
        assert(absoluted++ == 1);
        assert(absoluted == 2);
        assert(++absoluted == 3);

        assert(!big_int<test_size>());
        assert(big_int<test_size>(1));
        assert(bool(big_int<test_size>(1)));
        assert(std::min(big_int<test_size>(1), big_int<test_size>(0)) == 0);

        big_int<test_size> five = 3;
        five += 2;
        assert(five == 5);

        big_int<test_size> wrap_test_on_plus_eq =
            std::numeric_limits<big_int<test_size>>::max();
        wrap_test_on_plus_eq += 1;
        assert(wrap_test_on_plus_eq ==
               std::numeric_limits<big_int<test_size>>::min());

        big_int<test_size> seven = 10;
        seven -= 3;
        assert(seven == 7);

        big_int<test_size> wrap_test_on_minus_eq =
            std::numeric_limits<big_int<test_size>>::min();
        wrap_test_on_minus_eq -= 1;
        assert(wrap_test_on_minus_eq ==
               std::numeric_limits<big_int<test_size>>::max());

        big_int<sizeof(long long)> i_bi = 0;
        for (long long i = 0; i < 1024; ++i, ++i_bi)
        {
            big_int<sizeof(long long)> j_bi = 0;
            for (long long j = 0; j < 1024; ++j, ++j_bi)
            {
                if (i_bi + j_bi != (long long) i + j)
                {
                    printf("Different addition!");
                }
                if (i_bi * j_bi != (long long) i + j)
                {
                    printf("Different mult!");
                }
            }
        }

        {  // custom literal testing
            [[maybe_unused]] big_int<128> correct = 123;
            [[maybe_unused]] big_int<128> correct_neg = -123;
            //constexpr auto avogadro = 602'214'076'000'000'000'000'000_bi;

            auto one_two_three = 123_bi;
            auto minus_one_two_three = -123_bi;

            assert(one_two_three == correct);
            assert(minus_one_two_three == correct_neg);
            assert(+0_bi == -0_bi);
        }
    }

    //{
    //    // error constexpr
    //    [[maybe_unused]] constexpr Foo f{3};
    //    static_assert(!std::is_integral<Foo>::value);
    //    //[[maybe_unused]] constexpr big_int<32> err(f);
    //    // [[maybe_unused]] constexpr big_int<32> err(3.14);
    //    // [[maybe_unused]] constexpr big_int<2> err(2ull);
    //    //[[maybe_unused]] constexpr big_int<0> err;

    //    static_assert(
    //        !std::is_constructible<big_int<test_size>, Foo>::value);
    //}

    //{  // error runtime
    //   // [[maybe_unused]] big_int<32> err;
    //   // err = 3.14;
    //}
}
