# C++ primitive type boxing

This is a small header-only library for easing primitive type boxing in C++.

library is created to aid code health in [Contour Terminal Emulator](https://github.com/christianparpart/contour/).

This header can be simply copied into a project or used via CMake builtin functions, such as `FetchContent`.

# Simple usage

``` c++

#include <boxed-cpp/boxed.hpp>

// Create unique structures
namespace tags { struct Speed{}; struct Permittivity{}; struct Permeability{}; }

using Speed = boxed::boxed<double,tags::Speed>;
using Permittivity = boxed::boxed<double,tags::Permittivity>;
using Permeability = boxed::boxed<double,tags::Permeability>;


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

`


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
