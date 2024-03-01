// SPDX-License-Identifier: Apache-2.0
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <functional>
#include <limits>
#include <type_traits>

#include <boxed-cpp/boxed.hpp>

// clang-format off
namespace tags { struct Length{}; struct From{}; struct To{}; }

using Length = boxed::boxed<std::size_t, tags::Length>;
using From = boxed::boxed<std::size_t, tags::From>;
using To = boxed::boxed<std::size_t, tags::To>;
using BoxedDouble = boxed::boxed<double>;
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

TEST_CASE("boxed cout")
{
    auto constexpr r = Range { From { 2 }, To { 4 } };
    auto constexpr l = length(r);
    std::cout << l << std::endl;
}

TEST_CASE("boxed comparison")
{
    auto constexpr l1 = Length { 1 };
    auto constexpr l2 = Length { 2 };
    auto constexpr l3 = Length { 3 };
    REQUIRE(l3 > l1);
    REQUIRE(l2 < l3);
    REQUIRE(l2 != l1);
    REQUIRE(l1 == l1);
}

TEST_CASE("boxed_as")
{
    auto constexpr f = From { 3 };
    To t = f.as<To>();
    auto constexpr tint = f.as<int>();
    REQUIRE(t.as<int>() == tint);

    auto constexpr bd = BoxedDouble { 3.14 };
    auto constexpr bdint = bd.as<int>();
    // bd.as<To>(); // muse be compile error
    REQUIRE(bdint == unbox<int>(bd));

    auto constexpr t2 = To::cast_from(f);
    REQUIRE(t2 == t);
}

TEST_CASE("boxed_get")
{
    auto constexpr cbd = BoxedDouble { 3.14 };
    REQUIRE(cbd.get() == 3.14);

    auto bd = BoxedDouble { 2.781 };
    REQUIRE(bd.get() == 2.781);

    auto& bdp = bd.get();
    bdp += 1.0;
    REQUIRE(bd.get() == 3.781);
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

//clang-format off
struct Speed_tag;
struct Permittivity_tag;
struct Permeability_tag;
//clang-format on
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

TEST_CASE("Without tags")
{
    using rho_type = boxed::boxed<double>;
    rho_type rho { 1.0 };

    REQUIRE(unbox(rho) == 1.0);
}

template <typename... F>
struct overload: F...
{
    using F::operator()...;
};

template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

template <typename... Ts>
struct Wrap
{
    overload<Ts...> func_wrap;

    Wrap(Ts... funcs): func_wrap(funcs...) {}

    template <typename... Args>
    auto operator()(Args... args)
    {
        return (func_wrap(args) * ...);
    }
};

TEST_CASE("advanced")
{
    using rho_type = boxed::boxed<double>;
    using theta_type = boxed::boxed<double>;
    using phi_type = boxed::boxed<double>;

    auto x_coord = Wrap([](rho_type rho) { return unbox(rho); },
                        [](theta_type theta) { return sin(unbox(theta)); },
                        [](phi_type phi) { return cos(unbox(phi)); });

    rho_type rho { 1.0 };
    theta_type theta { 3.14 / 3.0 };
    phi_type phi { 3.14 / 2.0 };

    REQUIRE(x_coord(rho, theta, phi) == x_coord(theta, phi, rho));
    REQUIRE(x_coord(phi, theta, rho) == x_coord(phi, theta, rho));
}
