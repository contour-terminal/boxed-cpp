// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <iostream>
#include <limits>
#include <type_traits>

namespace boxed
{
// {{{ forward decls
template <typename T, typename Tag>
struct boxed;

namespace helper
{
    template <typename A>
    struct is_boxed;
    template <typename A, typename B>
    struct is_boxed<boxed<A, B>>;
} // namespace helper
// }}}

template <typename T>
constexpr bool is_boxed = helper::is_boxed<T>::value;

/**
 * Wrapper to provide strong typing on primitive types.
 *
 * You must provide a unique tag (an empty struct) to each boxed
 * type to ensure uniqueness of this type.
 *
 * @code
 * namespace tags { struct Length{}; }
 * using Length = boxed<std::size_t, tags::Length>;
 * @endcode
 */
template <typename T, typename Tag>
struct boxed
{
    static_assert(std::is_enum_v<T> || std::is_integral_v<T> || std::is_floating_point_v<T>,
                  "Boxing is only useful on integral & floating point types.");

    using inner_type = T;
    using element_type = T;

    constexpr boxed(): value {} {}
    constexpr explicit boxed(T value) noexcept: value { value } {}
    constexpr boxed(boxed const&) = default;
    constexpr boxed& operator=(boxed const&) = default;
    constexpr boxed(boxed&&) noexcept = default;
    constexpr boxed& operator=(boxed&&) noexcept = default;
    ~boxed() = default;

    T value;

    [[nodiscard]] constexpr T& get() noexcept { return value; }
    [[nodiscard]] constexpr T const& get() const noexcept { return value; }
    constexpr operator T() const&& { return value; }

    template <typename To>
    [[nodiscard]] constexpr auto as() const noexcept
    {
        if constexpr (is_boxed<To>)
            return To { static_cast<typename To::element_type>(value) };
        else
            return static_cast<To>(value);
    }

    template <typename Source>
    // NOLINTNEXTLINE(readability-identifier-naming)
    [[nodiscard]] constexpr static boxed<T, Tag> cast_from(Source value)
    {
        if constexpr (is_boxed<Source>)
            return boxed<T, Tag> { static_cast<T>(value.value) };
        else
            return boxed<T, Tag>(static_cast<T>(value));
    }
};

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, boxed<T, U> value)
{
    os << value.value;
    return os;
}

// clang-format off
template <typename T, typename U> constexpr boxed<T, U>& operator++(boxed<T, U>& a) noexcept { ++a.value; return a; }
template <typename T, typename U> constexpr boxed<T, U>& operator--(boxed<T, U>& a) noexcept { --a.value; return a; }
template <typename T, typename U> constexpr boxed<T, U> operator++(boxed<T, U>& a, int) noexcept { auto old = a; a.value++; return old; }
template <typename T, typename U> constexpr boxed<T, U> operator--(boxed<T, U>& a, int) noexcept { auto old = a; a.value--; return old; }
template <typename T, typename U> constexpr T const& operator*(boxed<T, U> const& a) noexcept { return a.value; }
template <typename T, typename U> constexpr bool operator<(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return a.value < b.value; }
template <typename T, typename U> constexpr bool operator>(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return a.value > b.value; }
template <typename T, typename U> constexpr bool operator<=(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return a.value <= b.value; }
template <typename T, typename U> constexpr bool operator>=(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return a.value >= b.value; }
template <typename T, typename U> constexpr bool operator==(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return a.value == b.value; }
template <typename T, typename U> constexpr bool operator!=(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return a.value != b.value; }
template <typename T, typename U> constexpr bool operator!(boxed<T, U> const& a) noexcept { return !a.value; }
template <typename T, typename U> constexpr boxed<T, U> operator+(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return boxed<T, U>{a.value + b.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator-(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return boxed<T, U>{a.value - b.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator*(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return boxed<T, U>{a.value * b.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator/(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return boxed<T, U>{a.value / b.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator%(boxed<T, U> const& a, boxed<T, U> const& b) noexcept { return boxed<T, U>{a.value % b.value}; }

template <typename T, typename U> constexpr boxed<T, U> operator-(boxed<T, U> const& a) noexcept { return boxed<T, U>{-a.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator+(boxed<T, U> const& a) noexcept { return boxed<T, U>{+a.value}; }

template <typename T, typename U> constexpr boxed<T, U> operator+(boxed<T, U> const& a, T b) noexcept { return boxed<T, U>{a.value + b}; }
template <typename T, typename U> constexpr boxed<T, U> operator-(boxed<T, U> const& a, T b) noexcept { return boxed<T, U>{a.value - b}; }
template <typename T, typename U> constexpr boxed<T, U> operator*(boxed<T, U> const& a, T b) noexcept { return boxed<T, U>{a.value * b}; }
template <typename T, typename U> constexpr boxed<T, U> operator/(boxed<T, U> const& a, T b) noexcept { return boxed<T, U>{a.value / b}; }

template <typename T, typename U> constexpr boxed<T, U> operator+(T b, boxed<T, U> const& a) noexcept { return boxed<T, U>{b - a.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator-(T b, boxed<T, U> const& a) noexcept { return boxed<T, U>{b - a.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator*(T b, boxed<T, U> const& a) noexcept { return boxed<T, U>{b * a.value}; }
template <typename T, typename U> constexpr boxed<T, U> operator/(T b, boxed<T, U> const& a) noexcept { return boxed<T, U>{b / a.value}; }

template <typename T, typename U> constexpr boxed<T, U>& operator+=(boxed<T, U>& a, boxed<T, U> const& b) noexcept { a.value += b.value; return a; }
template <typename T, typename U> constexpr boxed<T, U>& operator-=(boxed<T, U>& a, boxed<T, U> const& b) noexcept { a.value -= b.value; return a; }
template <typename T, typename U> constexpr boxed<T, U>& operator*=(boxed<T, U>& a, boxed<T, U> const& b) noexcept { a.value *= b.value; return a; }
template <typename T, typename U> constexpr boxed<T, U>& operator/=(boxed<T, U>& a, boxed<T, U> const& b) noexcept { a.value /= b.value; return a; }
template <typename T, typename U> constexpr boxed<T, U>& operator%=(boxed<T, U>& a, boxed<T, U> const& b) noexcept { a.value %= b.value; return a; }

template <typename T, typename U> std::ostream& operator<<(std::ostream& os, boxed<T, U> const& v) { return os << v.value; }
// clang-format on

namespace helper
{
    template <typename A>
    struct is_boxed
    {
        constexpr static bool value = false; // NOLINT(readability-identifier-naming)
    };

    template <typename A, typename B>
    struct is_boxed<boxed<A, B>>
    {
        constexpr static bool value = true; // NOLINT(readability-identifier-naming)
    };
} // namespace helper

} // end namespace boxed

// Casts from one boxed type to another boxed type.
template <typename To, typename From, typename FromTag>
constexpr auto boxed_cast(boxed::boxed<From, FromTag> const& from) noexcept
{
    return To { static_cast<typename To::inner_type>(from.value) };
}

// Casting a boxed type out of the box.
template <typename To, typename From, typename FromTag>
constexpr auto unbox(boxed::boxed<From, FromTag> const& from) noexcept
{
    return static_cast<To>(from.value);
}

template <typename T>
concept con_boxed = requires(T t) { typename T::inner_type; };

// Casting a boxed type out of the box.
template <con_boxed T>
constexpr auto unbox(T from) noexcept
{
    return unbox<typename T::inner_type>(from);
}

namespace std
{
template <typename A, typename B>
struct numeric_limits<boxed::boxed<A, B>>
{
    using value_type = A;
    using Boxed = boxed::boxed<A, B>;

    // clang-format off
    // NOLINTBEGIN(readability-identifier-naming)
    static Boxed min() noexcept { return Boxed { std::numeric_limits<A>::min() }; }
    static Boxed max() noexcept { return Boxed { std::numeric_limits<A>::max() }; }
    static Boxed lowest() noexcept { return Boxed { std::numeric_limits<A>::lowest() }; }
    static Boxed epsilon() noexcept { return Boxed { std::numeric_limits<A>::epsilon() }; }
    static Boxed round_error() noexcept { return Boxed { std::numeric_limits<A>::round_error() }; }
    static Boxed infinity() noexcept { return Boxed { std::numeric_limits<A>::infinity() }; }
    static Boxed quiet_NaN() noexcept { return Boxed { std::numeric_limits<A>::quiet_NaN() }; }
    static Boxed signaling_NaN() noexcept { return Boxed { std::numeric_limits<A>::signaling_NaNinfinity() }; }
    static Boxed denorm_min() noexcept { return Boxed { std::numeric_limits<A>::denorm_min() }; }
    // NOLINTEND(readability-identifier-naming)
    // clang-format on
};
} // namespace std

namespace std // {{{
{
template <typename T, typename U>
struct hash<boxed::boxed<T, U>>
{
    constexpr size_t operator()(boxed::boxed<T, U> v) const noexcept { return std::hash<T> {}(v.value); }
};
} // namespace std
// {{{ fmtlib integration
// clang-format off
#if __has_include(<fmt/format.h>)

#include <fmt/format.h>
// clang-format on

namespace fmt
{

template <typename A, typename B>
struct formatter<boxed::boxed<A, B>>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }
    template <typename FormatContext>
    auto format(const boxed::boxed<A, B> _value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{}", _value.value);
    }
};

} // namespace fmt
#endif
// }}}
