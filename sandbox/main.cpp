#include <algorithm>
#include <stdexcept>
#include <string_view>

#include <iostream>

#include "big_int.hpp"
struct Foo
{
    int num;
};

constexpr size_t test_max_size = 16;

int main()
{
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
        increment.increment();
        increment.increment();
        assert(increment == two);

        big_int<test_max_size> overflow = big_int<test_max_size>::max();
        overflow.increment();
        assert(overflow == minimal);

        // assert(minimal < maximal);
        // assert(!(maximal < maximal));

        big_int<test_max_size> underflow = big_int<test_max_size>::min();
        underflow.decrement();
        assert(underflow == maximal);

        big_int<test_max_size> decremented = 0;
        decremented.decrement();
        assert(decremented == minusOne);

        big_int<test_max_size> negated = -1;
        negated.negate();
        assert(negated == 1);  // test out the auto conversion

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
        // assert(std::min(big_int<test_max_size>(1), big_int<test_max_size>(0))
        // ==
        //       0);

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
    }

    {
        // error constexpr
        [[maybe_unused]] constexpr Foo f{3};
        static_assert(!std::is_integral<Foo>::value);
        //[[maybe_unused]] constexpr big_int<32> err(f);
        // [[maybe_unused]] constexpr big_int<32> err(3.14);
        // [[maybe_unused]] constexpr big_int<2> err(2ull);

        static_assert(
            !std::is_constructible<big_int<test_max_size>, Foo>::value);
    }

    {  // error runtime
       // [[maybe_unused]] big_int<32> err;
       // err = 3.14;
    }

    return 0;
}
