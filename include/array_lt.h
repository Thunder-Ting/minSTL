#pragma once

#include <cstddef>      // size_t
#include <iterator>     // reverse_iterator
#include <stdexcept>    // out_of_range
#include <type_traits>  // integral_constant
#include <utility>      // std::swap

namespace leistd {

template <typename T, std::size_t N>
class array {
public:
  // 类型别名
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // 构造
  constexpr array() = default;

  // 元素访问
  constexpr reference operator[](size_type i) noexcept { return _elems[i]; }
  constexpr const_reference operator[](size_type i) const noexcept { return _elems[i]; }

  constexpr reference at(size_type i) {
    if (i >= N) throw std::out_of_range("array::at out of range");
    return _elems[i];
  }
  constexpr const_reference at(size_type i) const {
    if (i >= N) throw std::out_of_range("array::at out of range");
    return _elems[i];
  }

  constexpr reference front() noexcept { return _elems[0]; }
  constexpr const_reference front() const noexcept { return _elems[0]; }
  constexpr reference back() noexcept { return _elems[N - 1]; }
  constexpr const_reference back() const noexcept { return _elems[N - 1]; }

  constexpr pointer data() noexcept { return _elems; }
  constexpr const_pointer data() const noexcept { return _elems; }

  // 容量
  [[nodiscard]] constexpr size_type size() const noexcept { return N; }
  [[nodiscard]] constexpr size_type max_size() const noexcept { return N; }
  [[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }

  // 迭代器
  constexpr iterator begin() noexcept { return _elems; }
  constexpr const_iterator begin() const noexcept { return _elems; }
  constexpr const_iterator cbegin() const noexcept { return _elems; }

  constexpr iterator end() noexcept { return _elems + N; }
  constexpr const_iterator end() const noexcept { return _elems + N; }
  constexpr const_iterator cend() const noexcept { return _elems + N; }

  constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

  constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

  // 修改器
  void fill(const T& value) {
    for (size_type i = 0; i < N; ++i) {
      _elems[i] = value;
    }
  }

  void swap(array& other) noexcept {
    for (size_type i = 0; i < N; ++i) {
      std::swap(_elems[i], other._elems[i]);
    }
  }

private:
  T _elems[N > 0 ? N : 1];  // 保证 N=0 时数组也能编译
};

// 全局 swap
template <typename T, std::size_t N>
void swap(array<T, N>& a, array<T, N>& b) noexcept {
  a.swap(b);
}

// 关系运算符
template <typename T, std::size_t N>
constexpr bool operator==(const array<T, N>& lhs, const array<T, N>& rhs) {
  for (std::size_t i = 0; i < N; ++i) {
    if (!(lhs[i] == rhs[i])) return false;
  }
  return true;
}

template <typename T, std::size_t N>
constexpr bool operator!=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(lhs == rhs);
}

template <typename T, std::size_t N>
constexpr bool operator<(const array<T, N>& lhs, const array<T, N>& rhs) {
  for (std::size_t i = 0; i < N; ++i) {
    if (lhs[i] < rhs[i]) return true;
    if (rhs[i] < lhs[i]) return false;
  }
  return false;
}

template <typename T, std::size_t N>
constexpr bool operator>(const array<T, N>& lhs, const array<T, N>& rhs) {
  return rhs < lhs;
}

template <typename T, std::size_t N>
constexpr bool operator<=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(rhs < lhs);
}

template <typename T, std::size_t N>
constexpr bool operator>=(const array<T, N>& lhs, const array<T, N>& rhs) {
  return !(lhs < rhs);
}

}  // namespace leistd

// tuple_size / tuple_element 特化，支持结构化绑定
namespace std {
template <typename T, size_t N>
struct tuple_size<leistd::array<T, N>> : std::integral_constant<size_t, N> {};

template <size_t I, typename T, size_t N>
struct tuple_element<I, leistd::array<T, N>> {
  using type = T;
};
}  // namespace std
