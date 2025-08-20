#pragma once
#include <initializer_list>
#include <iterator>
#include <memory>
#include <utility>

namespace leistd {

template <typename T, typename Alloc = std::allocator<T>>
class list {
private:
  /*============ 节点结构 ============*/
  struct Node {
    Node* prev;
    Node* next;
    T data;  // 哨兵节点不会真正使用 data
  };

  /*============ 迭代器定义 ============*/
  template <typename U, typename Ref, typename Ptr>
  struct list_iterator {
    using value_type = U;
    using reference = Ref;
    using pointer = Ptr;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    Node* node;

    list_iterator(Node* n = nullptr) : node(n) {}
    list_iterator(const list_iterator<T, T&, T*>& it) : node(it.node) {}  // 非 const -> const 迭代器转换

    reference operator*() const { return node->data; }
    pointer operator->() const { return &(node->data); }

    list_iterator& operator++() {
      node = node->next;
      return *this;
    }
    list_iterator operator++(int) {
      list_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    list_iterator& operator--() {
      node = node->prev;
      return *this;
    }
    list_iterator operator--(int) {
      list_iterator tmp(*this);
      --(*this);
      return tmp;
    }

    bool operator==(const list_iterator& rhs) const { return node == rhs.node; }
    bool operator!=(const list_iterator& rhs) const { return node != rhs.node; }
  };

public:
  /*============ 类型定义 ============*/
  using value_type = T;
  using allocator_type = Alloc;
  using size_type = std::size_t;
  using iterator = list_iterator<T, T&, T*>;
  using const_iterator = list_iterator<T, const T&, const T*>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  friend void swap(list& lhs, list& rhs) noexcept {
    using std::swap;
    swap(lhs._M_node, rhs._M_node);
    swap(lhs._M_size, rhs._M_size);
    swap(lhs._M_alloc, rhs._M_alloc);
  }

  /*============ 构造/析构 ============*/
  list() : _M_size(0) {
    _M_node = _M_create_node();  // 哨兵节点
    _M_node->next = _M_node;
    _M_node->prev = _M_node;
  }

  list(std::initializer_list<T> ilist) : list() {
    for (auto& x : ilist) push_back(x);
  }

  list(const list& other) : list() {
    for (auto& x : other) push_back(x);
  }

  list(list&& other) noexcept : _M_node(other._M_node), _M_size(other._M_size), _M_alloc(std::move(other._M_alloc)) {
    other._M_node = _M_create_node();
    other._M_node->next = other._M_node;
    other._M_node->prev = other._M_node;
    other._M_size = 0;
  }

  list& operator=(list other) {  // copy-and-swap
    swap(*this, other);
    return *this;
  }

  ~list() {
    clear();
    _M_destroy_node(_M_node);
  }

  /*============ 容量接口 ============*/
  bool empty() const { return _M_size == 0; }
  size_type size() const { return _M_size; }

  /*============ 迭代器接口 ============*/
  iterator begin() { return iterator(_M_node->next); }
  iterator end() { return iterator(_M_node); }

  const_iterator begin() const { return const_iterator(_M_node->next); }
  const_iterator end() const { return const_iterator(_M_node); }
  const_iterator cbegin() const { return const_iterator(_M_node->next); }
  const_iterator cend() const { return const_iterator(_M_node); }

  /*============ 访问接口 ============*/
  T& front() { return _M_node->next->data; }
  const T& front() const { return _M_node->next->data; }

  T& back() { return _M_node->prev->data; }
  const T& back() const { return _M_node->prev->data; }

  /*============ 插入接口 ============*/
  void push_back(const T& value) { _M_insert_node(_M_node, value); }
  void push_back(T&& value) { _M_insert_node(_M_node, std::move(value)); }

  void push_front(const T& value) { _M_insert_node(_M_node->next, value); }
  void push_front(T&& value) { _M_insert_node(_M_node->next, std::move(value)); }

  iterator insert(const_iterator pos, const T& value) { return _M_insert_node(pos.node, value); }
  iterator insert(const_iterator pos, T&& value) { return _M_insert_node(pos.node, std::move(value)); }

  void splice(const_iterator pos, list& other) {
    if (other.empty()) return;
    if (this == &other) return;  // 自身 splice 要特判

    Node* first = other._M_node->next;
    Node* last = other._M_node->prev;

    Node* cur = pos.node;
    Node* prev = cur->prev;

    // 1. 从 other 脱链
    other._M_node->next = other._M_node;
    other._M_node->prev = other._M_node;

    // 2. 拼接到 this
    prev->next = first;
    first->prev = prev;

    last->next = cur;
    cur->prev = last;

    // 3. 调整 size
    this->_M_size += other._M_size;
    other._M_size = 0;
  }

  void splice(const_iterator pos, list& other, const_iterator it) {
    if (pos.node == it.node || pos.node == it.node->next) return;  // 自己 splice 到自己原地，啥都不做

    Node* n = it.node;
    Node* nprev = n->prev;
    Node* nnext = n->next;

    // 从 other 脱链
    nprev->next = nnext;
    nnext->prev = nprev;
    --other._M_size;

    // 插入到 this 的 pos 前
    Node* cur = pos.node;
    Node* prev = cur->prev;
    prev->next = n;
    n->prev = prev;
    n->next = cur;
    cur->prev = n;
    ++this->_M_size;
  }

  // [first, last)
  void splice(const_iterator pos, list& other, const_iterator first, const_iterator last) {
    if (first == last) return;  // 空区间

    // 1. 取出区间 [first, last)
    Node* firstNode = first.node;
    Node* lastNode = last.node;  // 注意 last 不包含
    Node* beforeFirst = firstNode->prev;
    Node* beforeLast = lastNode->prev;

    // 2. 从 other 脱链
    beforeFirst->next = lastNode;
    lastNode->prev = beforeFirst;

    // 计算区间大小（只能 O(n) 遍历）
    size_type count = 0;
    for (Node* cur = firstNode; cur != lastNode; cur = cur->next) {
      ++count;
    }
    other._M_size -= count;

    // 3. 插入到 this 的 pos 前
    Node* cur = pos.node;
    Node* prev = cur->prev;

    prev->next = firstNode;
    firstNode->prev = prev;

    beforeLast->next = cur;
    cur->prev = beforeLast;

    this->_M_size += count;
  }

  // 插入多个相同元素
  iterator insert(const_iterator pos, size_type count, const T& value) {
    Node* curPos = pos.node;
    iterator firstInserted;
    for (size_type i = 0; i < count; ++i) {
      iterator it = _M_insert_node(curPos, value);
      if (i == 0) firstInserted = it;
    }
    return firstInserted;
  }

  // 插入区间 [first, last)
  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    Node* curPos = pos.node;
    iterator firstInserted;
    bool firstFlag = true;
    for (auto it = first; it != last; ++it) {
      iterator inserted = _M_insert_node(curPos, *it);
      if (firstFlag) {
        firstInserted = inserted;
        firstFlag = false;
      }
    }
    return firstInserted;
  }

  // 插入 initializer_list
  iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }
  /*============ 删除接口 ============*/
  void pop_back() {
    if (empty()) return;
    Node* tail = _M_node->prev;
    tail->prev->next = _M_node;
    _M_node->prev = tail->prev;
    _M_destroy_node(tail);
    --_M_size;
  }

  void pop_front() {
    if (empty()) return;
    Node* head = _M_node->next;
    head->next->prev = _M_node;
    _M_node->next = head->next;
    _M_destroy_node(head);
    --_M_size;
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    Node* cur = pos.node;
    Node* prev = cur->prev;

    // 1. 创建节点，完美转发参数直接构造 data
    Node* node = NodeTraits::allocate(_M_alloc, 1);
    NodeTraits::construct(_M_alloc, &node->data, std::forward<Args>(args)...);

    // 2. 插入节点
    node->prev = prev;
    node->next = cur;
    prev->next = node;
    cur->prev = node;

    ++_M_size;
    return iterator(node);
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    emplace(end(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  void emplace_front(Args&&... args) {
    emplace(begin(), std::forward<Args>(args)...);
  }

  // 合并两个有序链表
  void merge(list& other) {
    if (this == &other || other.empty()) return;

    Node* thisCur = _M_node->next;
    Node* otherCur = other._M_node->next;

    while (thisCur != _M_node && otherCur != other._M_node) {
      if (otherCur->data < thisCur->data) {
        Node* nextOther = otherCur->next;

        // 将 otherCur 插入 thisCur 前面
        otherCur->prev->next = otherCur->next;
        otherCur->next->prev = otherCur->prev;

        Node* thisPrev = thisCur->prev;
        thisPrev->next = otherCur;
        otherCur->prev = thisPrev;
        otherCur->next = thisCur;
        thisCur->prev = otherCur;

        otherCur = nextOther;
        ++_M_size;
        --other._M_size;
      } else {
        thisCur = thisCur->next;
      }
    }

    // 如果 this 链表到尾部了，直接接上剩余的 other
    if (otherCur != other._M_node) {
      Node* tail = _M_node->prev;
      tail->next = otherCur;
      otherCur->prev = tail;

      _M_node->prev = other._M_node->prev;
      other._M_node->prev->next = _M_node;

      _M_size += other._M_size;
      other._M_size = 0;

      // 重置 other 链表为空
      other._M_node->next = other._M_node;
      other._M_node->prev = other._M_node;
    }
  }

  void clear() {
    Node* cur = _M_node->next;
    while (cur != _M_node) {
      Node* tmp = cur;
      cur = cur->next;
      _M_destroy_node(tmp);
    }
    _M_node->next = _M_node;
    _M_node->prev = _M_node;
    _M_size = 0;
  }

private:
  /*============ 内存管理 ============*/
  using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using NodeTraits = std::allocator_traits<NodeAlloc>;

  Node* _M_create_node() {
    Node* node = NodeTraits::allocate(_M_alloc, 1);
    return node;  // 哨兵节点不构造 data
  }

  template <typename U>
  Node* _M_create_node(U&& value) {
    Node* node = NodeTraits::allocate(_M_alloc, 1);
    NodeTraits::construct(_M_alloc, &(node->data), std::forward<U>(value));
    return node;
  }

  template <typename U>
  iterator _M_insert_node(Node* pos, U&& value) {
    Node* node = _M_create_node(std::forward<U>(value));
    Node* prev = pos->prev;

    node->next = pos;
    node->prev = prev;
    prev->next = node;
    pos->prev = node;

    ++_M_size;
    return iterator(node);
  }

  void _M_destroy_node(Node* node) {
    if (node == _M_node) return;                   // 哨兵节点可单独析构
    NodeTraits::destroy(_M_alloc, &(node->data));  // 调用 data 的析构函数
    NodeTraits::deallocate(_M_alloc, node, 1);     // 释放节点内存
  }

private:
  Node* _M_node;  // 哨兵节点
  size_type _M_size;
  NodeAlloc _M_alloc;
};

}  // namespace leistd
