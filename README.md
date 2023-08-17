# C++ primitive type boxing

This is a small header-only library for easing primitive type boxing in C++.
Primary goal of the library is to make it easy to avoid code with easily swappable parameters [clang-tidy:bugprone-easily-swappable-parameters](https://clang.llvm.org/extra/clang-tidy/checks/bugprone/easily-swappable-parameters.html).

Overview on the topic: [C++ Weekly With Jason Turner](https://www.youtube.com/watch?v=Zq4yYPG7Erc)

library is created to aid code health in [Contour Terminal Emulator](https://github.com/christianparpart/contour/).

This header can be simply copied into a project or used via CMake builtin functions, such as `FetchContent`.

# Simple usage

Example of creation boxed structures and usage
``` c++

#include <boxed-cpp/boxed.hpp>

// Create unique structures
namespace tags { struct Speed{}; struct Permittivity{}; struct Permeability{}; }

using Speed = boxed::boxed<double, tags::Speed>;
using Permittivity = boxed::boxed<double, tags::Permittivity>;
using Permeability = boxed::boxed<double, tags::Permeability>;


int main()
{
    auto wave_speed = [](Permittivity epsilon, Permeability mu) -> Speed
    {
        return Speed(1.0 / std::sqrt(unbox(epsilon) * unbox(mu)));
    };
    auto vacuum_permittivity = Permittivity(8.85418781762039e-12);
    auto pi = 3.14159265358979323846;
    auto vacuum_permeability = Permeability(4 * pi * 1e-7);

    auto speed = wave_speed(vacuum_permittivity, vacuum_permeability);
    // speed == Speed(299792458.0);
}

```


When you need to get value from boxed type, you need to unbox it
``` c++
//unbox in declared type. double in this case
auto speed_value_native = unbox(speed_of_light);
//unbox into float type
auto speed_value_float = unbox<float>(speed_of_light);
// unbox into int type
auto speed_value_int = unbox<int>(speed_of_light);
```


You can also evaluate expressions with boxed types without the need of unboxing them if explicitly declare the resulted type
``` c++
auto speed_of_light = Speed(299792458.0);
auto value = speed_of_light * 2.0; // type of value is Speed

// boxed value will be automatically unboxed into type that was boxed, in this case double
double value_d = speed_of_light * 2.0;
```


# More advanced usage
You can forget about the order of parameters in your code

``` c++
#include <boxed-cpp/boxed.hpp>

// Create unique structures
namespace tags { struct Speed{}; struct Permittivity{}; struct Permeability{}; }

using Speed = boxed::boxed<double, tags::Speed>;
using Permittivity = boxed::boxed<double, tags::Permittivity>;
using Permeability = boxed::boxed<double, tags::Permeability>;

Speed wave_speed_inside(Permittivity epsilon, Permeability mu)
{
    return Speed(1.0 / std::sqrt(unbox(epsilon) * unbox(mu)));
};

template <typename T, typename S>
Speed wave_speed(T t, S s) // recursive variadic function
{
    if constexpr (std::is_same_v<T, Permittivity>)
    {
        return wave_speed_inside(t, s);
    }
    else
    {
        return wave_speed_inside(s, t);
    }
}


int main()
{
    constexpr auto vacuum_permittivity = Permittivity(8.85418781762039e-12);
    constexpr auto pi = 3.14159265358979323846;
    constexpr auto vacuum_permeability = Permeability(4 * pi * 1e-7);

    auto speed_one = wave_speed(vacuum_permittivity, vacuum_permeability);
    auto speed_two = wave_speed(vacuum_permeability, vacuum_permittivity);
    // speed_one == speed_two
}
```



### License

```
boxed-cpp
=========

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
