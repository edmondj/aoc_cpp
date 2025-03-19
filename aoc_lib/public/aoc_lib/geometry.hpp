#pragma once

#include <algorithm>
#include <format>
#include <ranges>

namespace aoc {

// point2d
template <typename T> struct point2d {
  T x, y;

  constexpr auto operator<=>(const point2d &) const = default;
};

template <typename T> constexpr bool is_point = false;
template <typename T> constexpr bool is_point<point2d<T>> = true;

template <typename T> struct point_value;
template <typename T> struct point_value<point2d<T>> {
  using type = T;
};
template <typename T> using point_value_t = point_value<T>::type;

template <typename T> struct std::hash<point2d<T>> {
  size_t operator()(const aoc::point2d<T> &p) const noexcept {
    std::hash<T> hasher;
    return hasher(p.x) ^ (hasher(p.y) << 1);
  }
};

// vec2d
template <typename T> struct vec2d {
  T dx, dy;
};

template <typename T> auto amplitude(const vec2d<T> &v) {
  return std::sqrt(v.dx * v.dx + v.dy * v.dy);
}

// point / point operators
template <typename T, typename U>
auto operator-(const point2d<T> &l, const point2d<U> &r)
    -> vec2d<decltype(l.x - r.x)> {
  return {.dx = l.x - r.x, .dy = l.y - r.y};
}

// vector / scalar operators
template <typename T, typename U>
auto operator*(const vec2d<T> &l, const U &r) -> vec2d<decltype(l.dx * r)> {
  return {.dx = l.dx * r, .dy = l.dy * r};
}

template <typename T, typename U>
auto operator*(const T &l, const vec2d<U> &r) -> vec2d<decltype(l * r.dx)> {
  return {.dx = l * r.dx, .dy = l * r.dy};
}

// point / vector operators
template <typename T, typename U>
auto operator+(const point2d<T> &l, const vec2d<U> &r)
    -> point2d<decltype(l.x + r.dx)> {
  return {.x = l.x + r.dx, .y = l.y + r.dy};
}

template <typename T, typename U>
auto operator+(const vec2d<T> &l, const point2d<U> &r)
    -> point2d<decltype(l.dx + r.x)> {
  return {.x = l.dx + r.x, .y = l.dy + r.y};
}

template <typename T, typename U>
auto operator-(const point2d<T> &l, const vec2d<U> &r)
    -> point2d<decltype(l.x - r.dx)> {
  return {.x = l.x - r.dx, .y = l.y - r.dy};
}

template <typename T, typename U>
auto operator-(const vec2d<T> &l, const point2d<U> &r)
    -> point2d<decltype(l.dx - r.x)> {
  return {.x = l.dx - r.x, .y = l.dy - r.y};
}

template <typename T, typename U>
point2d<T> &operator-=(point2d<T> &l, const vec2d<U> &r) {
  l.x -= r.dx;
  l.y -= r.dy;
  return l;
}

// algorithms
template <std::ranges::input_range Points>
auto centroid(Points &&points)
    -> std::optional<std::ranges::range_value_t<Points>>
  requires is_point<std::ranges::range_value_t<Points>>
{
  using Point = std::ranges::range_value_t<Points>;
  point_value_t<Point> x = {}, y = {}, size = {};
  for (const auto &point : points) {
    x += point.x;
    y += point.y;
    size += 1;
  }
  if (size == 0) {
    return std::nullopt;
  }
  return Point{.x = x / size, .y = y / size};
}

template <std::ranges::input_range Values>
auto rms(Values &&values) -> std::optional<std::ranges::range_value_t<Values>> {
  if (std::ranges::empty(values)) {
    return std::nullopt;
  }
  std::ranges::range_value_t<Values> rms = {}, size = {};
  for (const auto &v : values) {
    rms += v * v;
    size += 1;
  }
  return std::sqrt(rms / size);
}

// format
template <typename T, typename Char>
struct std::formatter<aoc::point2d<T>, Char> {
  std::formatter<T, Char> data_formatter;

  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext &ctx) {
    return data_formatter.parse(ctx);
  }

  template <typename FmtContext>
  constexpr FmtContext::iterator format(const aoc::point2d<T> &point,
                                        FmtContext &ctx) const {
    ctx.advance_to(data_formatter.format(point.x, ctx));
    ctx.advance_to(std::format_to(ctx.out(), ", "));
    return data_formatter.format(point.y, ctx);
  }
};

} // namespace aoc