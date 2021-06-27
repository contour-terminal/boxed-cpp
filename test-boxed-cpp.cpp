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
    auto constexpr t = boxed::boxed_cast<To>(f);
    static_assert(*f == *t);
    static_assert(std::is_same_v<decltype(t), To const>);
}

namespace tags { struct N{}; struct Z{}; }
using N = boxed::boxed<unsigned int, tags::N>;
using Z = boxed::boxed<signed int, tags::Z>;

TEST_CASE("boxed_cast with different inner types")
{
    auto constexpr a = N{3};
    auto constexpr b = boxed::boxed_cast<Z>(a);

    static_assert(*a == *b);
    static_assert(std::is_same_v<decltype(b), Z const>);
}
