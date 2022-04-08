#pragma once
#include <type_traits>
#include <utility>

template <auto Start, auto End, auto Inc, class F>
constexpr void constexpr_for(F &&f) {
  if constexpr (Start < End) {
    f(std::integral_constant<decltype(Start), Start>());
    constexpr_for<Start + Inc, End, Inc>(f);
  }
}

template <class F, class... Args>
constexpr void constexpr_for(F &&f, Args &&...args) {
  (f(std::forward<Args>(args)), ...);
}
