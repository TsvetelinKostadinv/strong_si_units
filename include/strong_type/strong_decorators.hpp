#pragma once

template <typename underlying_t, template <typename> typename current>
struct crtp
{
    constexpr const underlying_t& underlying() const
    {
        return static_cast<const underlying_t&>(*this);
    }
};

template <typename a, typename b>
struct units_added;

template <typename a>
struct units_added<a, a>
{
    using type = a;
};

// uses the predefined + operator of the strong type's base type
template <typename underlying_t>
struct addable : crtp<underlying_t, addable>
{
    using add_res_t = typename units_added<underlying_t, underlying_t>::type;

    constexpr add_res_t add(const underlying_t& other) const
    {
        return add_res_t(this->underlying().value + other.value);
    }

    constexpr add_res_t operator+(const underlying_t& other) const
    {
        return add_res_t(this->underlying().value + other.value);
    }
};

template <typename a, typename b>
struct units_multiplied;

template <typename underlying_t>
struct multipliable : crtp<underlying_t, multipliable>
{
    using multiplication_res_t =
        typename units_multiplied<underlying_t, underlying_t>::type;

    constexpr multiplication_res_t multiply(const underlying_t& other) const
    {
        return multiplication_res_t(this->underlying().value * other.value);
    }

    constexpr multiplication_res_t operator*(const underlying_t& other) const
    {
        return multiplication_res_t(this->underlying().value * other.value);
    }
};