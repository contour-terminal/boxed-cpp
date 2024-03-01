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

using Speed = boxed::boxed<double>;
using Permittivity = boxed::boxed<double>;
using Permeability = boxed::boxed<double>;

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

    // Wrong order of parameters will result in compilation error
    // wave_speed(vacuum_permeability, vacuum_permittivity);
}

```

You can create boxed types in the following way:
``` c++
using boxed_type = boxed::boxed<int>;

struct Tag{};
using boxed_type_with_custom_tag = boxed::boxed<int,Tag>;
```

When you need to get value from a boxed type, you need to `unbox` it, use `get` method, or cast it into another type with `as`.
``` c++
//unbox in declared type. double in this case
auto speed_value_native = unbox(speed_of_light); // identical to speed_of_light.get();
//unbox into float type
auto speed_value_float = unbox<float>(speed_of_light); // identical to speed_of_light.as<float>();
// unbox into int type
auto speed_value_int = unbox<int>(speed_of_light); // identical to speed_of_light.as<int>();
```


You can also evaluate expressions with boxed types without the need of unboxing them
``` c++
auto speed_of_light = Speed(299792458.0);
auto value = speed_of_light * 2.0; // type of value is Speed

// boxed value will be automatically unboxed into type that was boxed, in this case double
double value_d = speed_of_light * 2.0;
```


# Another examples
You can create functions that will automatically adjust order of parameters. [godbolt](https://godbolt.org/z/n8Ez5K6vq)

``` c++
using rho_type = boxed::boxed<double>;
using theta_type = boxed::boxed<double>;
using phi_type = boxed::boxed<double>;

template <typename... F> struct overload : F... {
  using F::operator()...;
};

template <typename... Ts> overload(Ts...) -> overload<Ts...>;

template <typename... Ts> struct Wrap {
  overload<Ts...> func_wrap;

  Wrap(Ts... funcs) : func_wrap(funcs...) {}

  template <typename... Args> auto operator()(Args... args) {
    return (func_wrap(args) * ...);
  }
};

auto x_coord = Wrap([](rho_type rho) { return unbox(rho); },
                    [](theta_type theta) { return sin(unbox(theta)); },
                    [](phi_type phi) { return cos(unbox(phi)); });

int main() {
  rho_type rho{1.0};
  theta_type theta{3.14 / 3.0};
  phi_type phi{3.14 / 2.0};

  std::cout << x_coord(rho, theta, phi) << std::endl;
  std::cout << x_coord(phi, theta, rho) << std::endl;
  std::cout << x_coord(rho, phi, theta) << std::endl;
}
```

Or using another approach: [godbolt](https://godbolt.org/z/fjhaaT5hh)

``` c++
using rho_type = boxed::boxed<double>;
using theta_type = boxed::boxed<double>;
using phi_type = boxed::boxed<double>;

template <typename Func, typename... Tuple> struct Wrap_with_tuple {
  using type_order = std::tuple<Tuple...>;

  Wrap_with_tuple(Func f, type_order s) : _func(f), _order(s) {};

  template <typename... F> decltype(auto) operator()(F... args) {
    auto arg_tuple = std::make_tuple(args...);
    auto ints = std::make_index_sequence<sizeof...(args)>{};
    return make_call(arg_tuple, ints);
  }

  template <typename call_tuple, typename T, T... ints>
  decltype(auto) make_call(call_tuple arg_tuple,
                           std::integer_sequence<T, ints...> int_seq) {
    return _func(
        std::get<std::decay_t<decltype(std::get<ints>(_order))>>(arg_tuple)...);
  }

  Func _func;
  type_order _order;
};

auto x_coord = Wrap_with_tuple(
    [](rho_type rho, theta_type theta, phi_type phi) {
      return unbox(rho) * sin(unbox(theta)) * cos(unbox(phi));
    },
    std::make_tuple(rho_type{}, theta_type{}, phi_type{}));

int main() {
  rho_type rho{1.0};
  theta_type theta{3.14 / 3.0};
  phi_type phi{3.14 / 2.0};

  std::cout << x_coord(rho, theta, phi) << std::endl;
  std::cout << x_coord(phi, theta, rho) << std::endl;
  std::cout << x_coord(rho, phi, theta) << std::endl;
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
