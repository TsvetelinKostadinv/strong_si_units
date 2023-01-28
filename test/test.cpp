#include "catch2/catch_all.hpp"

#include <utility>
#include "big_int.hpp"
#include "test_util.hpp"

TEST_CASE("sanity check", "[sanity]")
{
    REQUIRE(true);
}

TEMPLATE_TEST_CASE_SIG("Default construction",
                       "[ctor]",
                       (size_t test_size, test_size),
                       7,
                       8,
                       9,
                       10,
                       11,
                       12,
                       13,
                       14,
                       15,
                       16,
                       64,
                       65,
                       66,
                       67,
                       128,
                       129,
                       130,
                       131)
{
    const big_int<test_size> def_with_init = big_int<test_size>();
    const big_int<test_size> def_with_noinit;

    REQUIRE(def_with_init == def_with_noinit);
}

template <typename integral, size_t test_size>
static void concrete_from_integral_test()
{
    // Essentially we want to be able to construct from the supplied integrals
    REQUIRE_NOTHROW(big_int<test_size>(integral(0)));

    REQUIRE_NOTHROW(big_int<test_size>(integral(1)));

    REQUIRE_NOTHROW(big_int<test_size>(integral(test_size)));

    REQUIRE_NOTHROW(big_int<test_size>(std::numeric_limits<integral>::min()));

    REQUIRE_NOTHROW(big_int<test_size>(std::numeric_limits<integral>::max()));
}

template <typename integral, size_t... test_sizes>
static void construction_from_integral_test()
{
    std::initializer_list<int> dummy = std::initializer_list<int>{
        ((void) concrete_from_integral_test<integral, test_sizes>(), 0)...};
    (void) dummy;
}

// https://www.fluentcpp.com/2019/03/05/for_each_arg-applying-a-function-to-each-argument-of-a-function-in-cpp/
template <typename... integral_types, size_t... test_sizes>
static void construction_from_integral_runner(
    std::index_sequence<test_sizes...>)
{
    std::initializer_list<int> dummy = std::initializer_list<int>{(
        (void) construction_from_integral_test<integral_types, test_sizes...>(),
        0)...};
    (void) dummy;
}

// https://www.fluentcpp.com/2019/03/05/for_each_arg-applying-a-function-to-each-argument-of-a-function-in-cpp/
template <template <typename integral, size_t... test_sizes> typename test_case,
          typename... integral_types,
          size_t... test_sizes>
static void test_case_runner(
    std::index_sequence<test_sizes...> = typename offset_index_sequence<
        8,
        std::make_index_sequence<32>>::sequence())
{
    BIG_INT_UNUSED std::initializer_list<int> dummy =
        std::initializer_list<int>{
            ((void) test_case<integral_types, test_sizes...>(), 0)...};
    (void) dummy;
}

TEST_CASE("Construction from integral", "[ctor]")
{
    using offsetted_index_sequence_test_sizes =
        typename offset_index_sequence<8,
                                       std::make_index_sequence<32>>::sequence;
    construction_from_integral_runner<ALL_INTEGRAL>(
        offsetted_index_sequence_test_sizes());
}