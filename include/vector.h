#pragma once
#include <initializer_list>
#include <memory>
#include <stdexcept>

namespace leistd {
template <typename T, typename Alloc = std::allocator<T>>
class vector {
  public:
  using value_type = T;
  using allocator_type = Alloc;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using const_reference = const T&;

  class const_iterator;  // 提前声明
  // 双向随机访问迭代器
  class iterator {
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    iterator() : ptr(nullptr) {
    }

    explicit iterator(T* p) : ptr(p) {
    }

    // 隐式转换成 const_iterator
    operator const_iterator() const noexcept {
      return const_iterator(ptr);  // 假设迭代器内部就是个指针
    }

    reference operator*() const {
      return *ptr;
    }
    pointer operator->() const {
      return ptr;
    }

    iterator& operator++() {
      ++ptr;
      return *this;
    }  // 前置++
    iterator operator++(int) {
      iterator tmp = *this;
      ++ptr;
      return tmp;
    }  // 后置++

    iterator& operator--() {
      --ptr;
      return *this;
    }  // 前置--
    iterator operator--(int) {
      iterator tmp = *this;
      --ptr;
      return tmp;
    }  // 后置--

    iterator operator+(difference_type n) const {
      return iterator(ptr + n);
    }
    iterator operator-(difference_type n) const {
      return iterator(ptr - n);
    }
    difference_type operator-(const iterator& rhs) const {
      return ptr - rhs.ptr;
    }

    reference operator[](difference_type n) const {
      return ptr[n];
    }

    bool operator==(const iterator& rhs) const {
      return ptr == rhs.ptr;
    }
    bool operator!=(const iterator& rhs) const {
      return ptr != rhs.ptr;
    }
    bool operator<(const iterator& rhs) const {
      return ptr < rhs.ptr;
    }
    bool operator<=(const iterator& rhs) const {
      return ptr <= rhs.ptr;
    }
    bool operator>(const iterator& rhs) const {
      return ptr > rhs.ptr;
    }
    bool operator>=(const iterator& rhs) const {
      return ptr >= rhs.ptr;
    }

private:
    T* ptr;
  };

  class const_iterator {
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::random_access_iterator_tag;

    const_iterator() : ptr(nullptr) {
    }

    explicit const_iterator(const T* p) : ptr(p) {
    }

    reference operator*() const {
      return *ptr;
    }
    pointer operator->() const {
      return ptr;
    }

    const_iterator& operator++() {
      ++ptr;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++ptr;
      return tmp;
    }

    const_iterator& operator--() {
      --ptr;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --ptr;
      return tmp;
    }

    const_iterator operator+(difference_type n) const {
      return const_iterator(ptr + n);
    }
    const_iterator operator-(difference_type n) const {
      return const_iterator(ptr - n);
    }
    difference_type operator-(const const_iterator& rhs) const {
      return ptr - rhs.ptr;
    }

    reference operator[](difference_type n) const {
      return ptr[n];
    }

    bool operator==(const const_iterator& rhs) const {
      return ptr == rhs.ptr;
    }
    bool operator!=(const const_iterator& rhs) const {
      return ptr != rhs.ptr;
    }
    bool operator<(const const_iterator& rhs) const {
      return ptr < rhs.ptr;
    }
    bool operator<=(const const_iterator& rhs) const {
      return ptr <= rhs.ptr;
    }
    bool operator>(const const_iterator& rhs) const {
      return ptr > rhs.ptr;
    }
    bool operator>=(const const_iterator& rhs) const {
      return ptr >= rhs.ptr;
    }

private:
    const T* ptr;
  };

  // 反向迭代器
  class reverse_iterator {
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    reverse_iterator() : ptr(nullptr) {
    }

    explicit reverse_iterator(T* p) : ptr(p) {
    }

    reference operator*() const {
      return *(ptr - 1);
    }
    pointer operator->() const {
      return ptr - 1;
    }

    // ++r 反向，实际是 --ptr
    reverse_iterator& operator++() {
      --ptr;
      return *this;
    }

    reverse_iterator operator++(int) {
      reverse_iterator tmp = *this;
      --ptr;
      return tmp;
    }

    // --r 反向，实际是 ++ptr
    reverse_iterator& operator--() {
      ++ptr;
      return *this;
    }

    reverse_iterator operator--(int) {
      reverse_iterator tmp = *this;
      ++ptr;
      return tmp;
    }

    reverse_iterator operator+(difference_type n) const {
      return reverse_iterator(ptr - n);
    }
    reverse_iterator operator-(difference_type n) const {
      return reverse_iterator(ptr + n);
    }
    difference_type operator-(const reverse_iterator& rhs) const {
      return rhs.ptr - ptr;
    }

    reference operator[](difference_type n) const {
      return *(ptr - n - 1);
    }

    bool operator==(const reverse_iterator& rhs) const {
      return ptr == rhs.ptr;
    }
    bool operator!=(const reverse_iterator& rhs) const {
      return ptr != rhs.ptr;
    }
    bool operator<(const reverse_iterator& rhs) const {
      return ptr > rhs.ptr;
    }
    bool operator<=(const reverse_iterator& rhs) const {
      return ptr >= rhs.ptr;
    }
    bool operator>(const reverse_iterator& rhs) const {
      return ptr < rhs.ptr;
    }
    bool operator>=(const reverse_iterator& rhs) const {
      return ptr <= rhs.ptr;
    }

private:
    T* ptr;  // 内部保存原始迭代器
  };

  vector() : _start(nullptr), _end(nullptr), _cap(nullptr) {
  }

  vector(std::initializer_list<T> init)
      : _start(nullptr), _end(nullptr), _cap(nullptr) {
    reserve(init.size());
    for (auto& val : init) {
      std::allocator_traits<Alloc>::construct(_alloc, _end, val);
      ++_end;
    }
  }

  vector(const vector& other);

  vector(vector&& other) noexcept;

  ~vector();

  void push_back(T&& value);

  void push_back(const T& value);

  void pop_back();

  void reserve(size_type n);

  void clear();

  bool empty() const;

  size_type size() const;

  size_type capacity() const;

  reference operator[](size_type index);

  const_reference operator[](size_type index) const;

  const_reference at(size_type index) const;

  vector& operator=(const vector& rhs);

  vector& operator=(vector&& other) noexcept;

  bool operator==(const vector& other) const;

  bool operator!=(const vector& other) const;

  // 迭代器接口
  iterator begin() {
    return iterator(_start);
  }
  iterator end() {
    return iterator(_end);
  }
  const_iterator begin() const {
    return const_iterator(_start);
  }
  const_iterator end() const {
    return const_iterator(_end);
  }
  const_iterator cbegin() const {
    return const_iterator(_start);
  }
  const_iterator cend() const {
    return const_iterator(_end);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(_end);
  }
  reverse_iterator rend() {
    return reverse_iterator(_start);
  }

  reverse_iterator rbegin() const {
    return reverse_iterator(_end);
  }
  reverse_iterator rend() const {
    return reverse_iterator(_start);
  }

  reverse_iterator crbegin() const {
    return reverse_iterator(_end);
  }
  reverse_iterator crend() const {
    return reverse_iterator(_start);
  }

  /*支持insert*/
  // 插入单个元素
  iterator insert(const_iterator pos, const T& value) {
    size_type idx = pos - cbegin();
    if (_end == _cap) {
      reserve(capacity() == 0 ? 1 : capacity() * 2);
    }

    // 移动元素（从 idx 到尾部，往后挪一位）
    std::move_backward(_start + idx, _end, _end + 1);
    // 插入元素
    std::allocator_traits<Alloc>::construct(_alloc, _start + idx, value);
    ++_end;
    return iterator(_start + idx);
  }

  iterator insert(const_iterator pos, T&& value) {
    size_type idx = pos - cbegin();
    if (_end == _cap) {
      reserve(capacity() == 0 ? 1 : capacity() * 2);
    }

    // 移动元素（从 idx 到尾部，往后挪一位）
    std::move_backward(_start + idx, _end, _end + 1);
    // 插入元素
    std::allocator_traits<Alloc>::construct(_alloc, _start + idx,
                                            std::move(value));
    ++_end;
    return iterator(_start + idx);
  }

  // 插入 count 个 value
  iterator insert(const_iterator pos, size_type count, const T& value) {
    size_type idx = pos - cbegin();

    if (size() + count > capacity()) {
      reserve(std::max(capacity() * 2, size() + count));
    }
    std::move_backward(_start + idx, _end, _end + count);

    for (size_type i = 0; i < count; ++i) {
      std::allocator_traits<Alloc>::construct(_alloc, _start + idx + i, value);
    }
    _end += count;
    return iterator(_start + idx);
  }

  // 插入迭代器区间 [first, last)
  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    size_type n = std::distance(first, last);
    if (n == 0)
      return begin() + (pos - cbegin());

    size_type idx = pos - cbegin();
    if (size() + n > capacity()) {
      reserve(std::max(capacity() * 2, size() + n));
    }

    pointer insert_ptr = _start + idx;               // 原始指针
    std::move_backward(insert_ptr, _end, _end + n);  // 使用指针，不是 iterator
    std::copy(first, last, insert_ptr);

    _end += n;
    return iterator(insert_ptr);
  }

  iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  /* 删除功能 */
  // 删除单个元素
  iterator erase(const_iterator pos) {
    difference_type n = pos - cbegin();

    // 把 [pos+1, end) 搬到 [pos, end-1)
    std::move(_start + n + 1, _end, _start + n);

    // destroy 最后一个元素
    --_end;
    std::allocator_traits<Alloc>::destroy(_alloc, _end);

    return begin() + n;
  }

  // 删除一个区间 [first, last)
  iterator erase(const_iterator first, const_iterator last) {
    if (first == last)
      return begin() + (first - cbegin());

    difference_type start = first - cbegin();
    difference_type count = last - first;

    // 把 [last, end) 搬到 [first, end-(last-first))
    std::move(_start + start + count, _end, _start + start);

    // destroy 尾部 count 个元素
    pointer new_end = _end - count;
    for (pointer p = new_end; p != _end; ++p) {
      std::allocator_traits<Alloc>::destroy(_alloc, p);
    }
    _end = new_end;

    return begin() + start;
  }

  private:
  Alloc _alloc;
  pointer _start;
  pointer _end;
  pointer _cap;
};
}  // namespace leistd

/**
 * Implement vector
 */
namespace leistd {
template <typename T, typename Alloc>
void vector<T, Alloc>::push_back(T&& value) {
  if (_end == _cap)
    reserve(capacity() == 0 ? 1 : capacity() * 2);
  std::allocator_traits<Alloc>::construct(_alloc, _end, std::move(value));
  ++_end;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::push_back(const T& value) {
  if (_end == _cap)
    reserve(capacity() == 0 ? 1 : capacity() * 2);
  std::allocator_traits<Alloc>::construct(_alloc, _end, value);
}

template <typename T, typename Alloc>
void vector<T, Alloc>::pop_back() {
  if (!empty()) {
    --_end;
    std::allocator_traits<Alloc>::destroy(_alloc, _end);
  }
}

template <typename T, typename Alloc>
vector<T, Alloc>::~vector() {
  clear();
  if (_start)
    std::allocator_traits<Alloc>::deallocate(_alloc, _start, capacity());
}

template <typename T, typename Alloc>
void vector<T, Alloc>::reserve(size_type new_cap) {
  if (new_cap <= capacity())
    return;

  pointer new_start = _alloc.allocate(new_cap);
  pointer new_end = new_start;

  for (pointer p = _start; p != _end; ++p, ++new_end) {
    std::allocator_traits<Alloc>::construct(_alloc, new_end, std::move(*p));
    std::allocator_traits<Alloc>::destroy(_alloc, p);
  }

  if (_start)
    std::allocator_traits<Alloc>::deallocate(_alloc, _start, capacity());

  _start = new_start;
  _end = new_end;
  _cap = _start + new_cap;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::clear() {
  while (_start != _end) {
    --_end;
    std::allocator_traits<Alloc>::destroy(_alloc, _end);
  }
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::size_type vector<T, Alloc>::capacity() const {
  return _cap - _start;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::size_type vector<T, Alloc>::size() const {
  return _end - _start;
}

template <typename T, typename Alloc>
bool vector<T, Alloc>::empty() const {
  return _start == _end;
}

// 操作符重载
template <typename T, typename Alloc>
typename vector<T, Alloc>::reference
vector<T, Alloc>::operator[](size_type index) {
  // 不做越界检查
  return _start[index];
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::const_reference
vector<T, Alloc>::operator[](size_type index) const {
  return _start[index];
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::const_reference
vector<T, Alloc>::at(size_type index) const {
  if (index >= size())
    throw std::out_of_range("vector");
  return _start[index];
}

// 拷贝赋值
template <typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& other) {
  if (this != &other) {
    clear();
    reserve(other.size());
    for (pointer p = other._start; p != other._end; ++p) {
      std::allocator_traits<Alloc>::construct(_alloc, _end, *p);
      ++_end;
    }
  }
  return *this;
}

// 移动赋值
template <typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& other) noexcept {
  if (this != &other) {
    clear();
    if (_start)
      std::allocator_traits<Alloc>::deallocate(_alloc, _start, capacity());

    _start = other._start;
    _end = other._end;
    _cap = other._cap;
    _alloc = std::move(other._alloc);

    other._start = other._end = other._cap = nullptr;
  }
  return *this;
}

template <typename T, typename Alloc>
bool vector<T, Alloc>::operator==(const vector& other) const {
  if (size() != other.size())
    return false;
  for (size_type i = 0; i < size(); ++i)
    if (_start[i] != other._start[i])
      return false;
  return true;
}

template <typename T, typename Alloc>
bool vector<T, Alloc>::operator!=(const vector& other) const {
  return !(*this == other);
}

// 拷贝构造
template <typename T, typename Alloc>
vector<T, Alloc>::vector(const vector& other)
    : _start(nullptr), _end(nullptr), _cap(nullptr) {
  reserve(other.size());
  for (pointer p = other._start; p != other._end; ++p) {
    std::allocator_traits<Alloc>::construct(_alloc, _end, *p);
    ++_end;
  }
}

// 移动构造
template <typename T, typename Alloc>
vector<T, Alloc>::vector(vector&& other) noexcept
    : _alloc(std::move(other._alloc)), _start(other._start), _end(other._end),
      _cap(other._cap) {
  other._start = other._end = other._cap = nullptr;
}
}  // namespace leistd
