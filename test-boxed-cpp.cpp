/**
 * This file is part of the "libterminal" project
 *   Copyright (c) 2019-2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <boxed-cpp/boxed.hpp>
#include <catch2/catch.hpp>
#include <limits>

namespace tags { struct Length{}; struct From{}; struct To{}; }

using Length = boxed::boxed<std::size_t, tags::Length>;
using From = boxed::boxed<std::size_t, tags::From>;
using To = boxed::boxed<std::size_t, tags::To>;
struct Range { From from; To to; };

constexpr Length length(Range range) noexcept
{
    auto result = static_cast<Length::inner_type>(range.to.value - range.from.value);
    ++result;
    return Length{static_cast<Length::inner_type>(result)};
}

TEST_CASE("boxed")
{
    auto constexpr r = Range{ From{2}, To{4} };
    auto constexpr l = length(r);
    static_assert(l == Length{3});
}

TEST_CASE("boxed_cast")
{
    auto constexpr f = From{3};
    auto constexpr t = boxed_cast<To>(f);
    static_assert(*f == *t);
    static_assert(std::is_same_v<decltype(t), To const>);
}

namespace tags { struct N{}; struct Z{}; }
using N = boxed::boxed<unsigned int, tags::N>;
using Z = boxed::boxed<signed int, tags::Z>;

TEST_CASE("boxed_cast with different inner types")
{
    auto constexpr a = N{3};
    auto constexpr b = boxed_cast<Z>(a);
#ifndef __GNUG__
    static_assert(*a == *b);
#endif
    static_assert(std::is_same_v<decltype(b), Z const>);
}

struct Speed_tag{};
struct Permittivity_tag{};
struct Permeability_tag;
using Speed = boxed::boxed<double,Speed_tag>;
using Permittivity = boxed::boxed<double,Permittivity_tag>;
using Permeability = boxed::boxed<double,Permeability_tag>;

TEST_CASE("function with boxed variables")
{
    auto wave_speed = [](Permittivity epsilon, Permeability mu) -> Speed
    {
        return Speed(1.0 / std::sqrt(unbox(epsilon) * unbox(mu)));
    };
    REQUIRE(wave_speed(Permittivity(1.0),Permeability(1.0)) == Speed(1.0));
    auto speed_of_light = Speed(299792458.0);
    auto vacuum_permittivity = Permittivity(8.85418781762039e-12);
    auto pi = 3.14159265358979323846;
    auto vacuum_permeability = Permeability(4 * pi * 1e-7);
    auto value = std::abs(unbox( wave_speed(vacuum_permittivity, vacuum_permeability) - speed_of_light ));
    REQUIRE(value < std::numeric_limits<double>::epsilon());
}
