// SPDX-License-Identifier: Apache-2.0
#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <limits>
#include <type_traits>

#include <boxed-cpp/boxed.hpp>

// clang-format off
namespace tags { struct Length{}; struct From{}; struct To{}; }

using Length = boxed::boxed<std::size_t, tags::Length>;
using From = boxed::boxed<std::size_t, tags::From>;
using To = boxed::boxed<std::size_t, tags::To>;
struct Range { From from; To to; };
// clang-format on

constexpr Length length(Range range) noexcept
{
    auto result = static_cast<Length::inner_type>(range.to.value - range.from.value);
    ++result;
    return Length { static_cast<Length::inner_type>(result) };
}

TEST_CASE("boxed")
{
    auto constexpr r = Range { From { 2 }, To { 4 } };
    auto constexpr l = length(r);
    static_assert(l == Length { 3 });
}

TEST_CASE("boxed_cast")
{
    auto constexpr f = From { 3 };
    auto constexpr t = boxed_cast<To>(f);
    static_assert(*f == *t);
    static_assert(std::is_same_v<decltype(t), To const>);
}

// clang-format off
namespace tags { struct N {}; struct Z {}; }
// clang-format on

using N = boxed::boxed<unsigned int, tags::N>;
using Z = boxed::boxed<signed int, tags::Z>;

TEST_CASE("boxed_cast with different inner types")
{
    auto constexpr a = N { 3 };
    auto constexpr b = boxed_cast<Z>(a);
#ifndef __GNUG__
    static_assert(*a == *b);
#endif
    static_assert(std::is_same_v<decltype(b), Z const>);
}

struct Speed_tag
{
};
struct Permittivity_tag
{
};
struct Permeability_tag;
using Speed = boxed::boxed<double, Speed_tag>;
using Permittivity = boxed::boxed<double, Permittivity_tag>;
using Permeability = boxed::boxed<double, Permeability_tag>;

TEST_CASE("function with boxed variables")
{
    auto wave_speed = [](Permittivity epsilon, Permeability mu) -> Speed {
        return Speed(1.0 / std::sqrt(unbox(epsilon) * unbox(mu)));
    };
    REQUIRE(wave_speed(Permittivity(1.0), Permeability(1.0)) == Speed(1.0));
    auto speed_of_light = Speed(299792458.0);
    auto vacuum_permittivity = Permittivity(8.85418781762039e-12);
    auto pi = 3.14159265358979323846;
    auto vacuum_permeability = Permeability(4 * pi * 1e-7);
    auto value = std::abs(unbox(wave_speed(vacuum_permittivity, vacuum_permeability) - speed_of_light));
    REQUIRE(value < std::numeric_limits<double>::epsilon());
}

TEST_CASE("unbox types check")
{
    auto speed_of_light = Speed(299792458.0);
    auto speed_value_native = unbox(speed_of_light);
    static_assert(std::is_same_v<decltype(speed_value_native), double>);
    auto speed_value_float = unbox<float>(speed_of_light);
    static_assert(std::is_same_v<decltype(speed_value_float), float>);
    auto speed_value_int = unbox<int>(speed_of_light);
    static_assert(std::is_same_v<decltype(speed_value_int), int>);
}

TEST_CASE("unbox without template parameters to initial type")
{
    auto speed_of_light = Speed(299792458.0);
    REQUIRE(std::abs(unbox(speed_of_light) - 299792458.0) < std::numeric_limits<double>::epsilon());
}

TEST_CASE("cast inside rvalue")
{
    auto speed_of_light = Speed(299792458.0);

    auto distance_auto_right = speed_of_light * 2.0;
    auto distance_auto_left = 2.0 * speed_of_light;
    static_assert(std::is_same_v<decltype(distance_auto_right), Speed>);
    static_assert(std::is_same_v<decltype(distance_auto_left), Speed>);

    double distance_d_right = speed_of_light * 2.0;
    double distance_d_left = 2.0 * speed_of_light;
    REQUIRE(distance_d_right - 2.0 * 299792458.0 < std::numeric_limits<double>::epsilon());
    REQUIRE(distance_d_left - 2.0 * 299792458.0 < std::numeric_limits<double>::epsilon());
}

TEST_CASE("all options for unbox")
{
    auto speed_of_light = Speed(299792458.0);
    REQUIRE(unbox<double>(speed_of_light));
    REQUIRE(unbox<float>(speed_of_light));
    REQUIRE(unbox(speed_of_light));
}

// advanced usage test

// clang-format off
template<typename ...Ts>
    struct not_same{};

template<typename T>
    struct not_same<T,T> : virtual std::false_type {};

template<typename T,typename S>
    struct not_same<T,S> : virtual std::true_type {} ;

template <typename ...T>
struct all_different : std::false_type {};

template<typename T, typename F>
struct all_different<T,F> : not_same<T,F> {};

template<typename T, typename... Ts>
struct all_different<T,T, Ts...> : virtual std::false_type {};

template<typename T, typename F,typename... Ts>
struct all_different<T,F, Ts...> : all_different<T,Ts...>, all_different<F,Ts...> {};

namespace Tag{ struct Rho{}; struct Theta{}; struct Phi{};}

using rho_type = boxed::boxed<double,Tag::Rho>;
using theta_type = boxed::boxed<double,Tag::Theta>;
using phi_type = boxed::boxed<double,Tag::Phi>;
// clang-format on

template <typename... T>
struct Wrap
{
};

template <typename T, typename... Rest>
struct Wrap<T, Rest...>
{

    constexpr static inline std::size_t n = 1 + sizeof...(Rest);
    using fun_type = std::function<double(T)>;
    Wrap(fun_type&& first, std::function<double(Rest)>&&... rest):
        first(std::forward<fun_type>(first)), rest(std::forward<std::function<double(Rest)>>(rest)...)
    {
    }

    const fun_type first;
    Wrap<Rest...> rest;

    auto operator()(T v) { return first(v); }

    template <typename F>
        requires(!std::is_same_v<T, F>)
    decltype(auto) operator()(F v)
    {
        return rest(v);
    }

    template <typename... Args>
        requires(!std::derived_from<all_different<typename std::decay<Args>::type...>, std::false_type>)
    decltype(auto) operator()(Args&&... args)
    {
        static_assert((sizeof...(Args) == n));
        return (operator()(std::forward<Args>(args)) * ...);
    }
};

// clang-format off
auto x_coord = Wrap<rho_type, theta_type, phi_type> {
    [](rho_type rho) { return unbox(rho); },
    [](theta_type theta) { return sin(unbox(theta)); },
    [](phi_type phi) { return cos(unbox(phi)); }
};
// clang-format on

TEST_CASE("advanced usage")
{

    rho_type rho { 1.0 };
    theta_type theta { 3.14 / 3.0 };
    phi_type phi { 3.14 / 2.0 };

    REQUIRE(x_coord(rho, theta, phi) == x_coord(theta, rho, phi));
    REQUIRE(x_coord(rho, theta, phi) == x_coord(phi, rho, theta));
}
