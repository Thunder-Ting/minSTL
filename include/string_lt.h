#pragma once
#include <cstddef>   // size_t, ptrdiff_t
#include <memory>    // std::allocator, allocator_traits
#include <stdexcept> // out_of_range

namespace leistd {
    template<typename CharT>
    struct char_traits {
        using char_type = CharT;

        static void assign(CharT &c1, const CharT &c2) {
            c1 = c2;
        }

        static bool eq(CharT c1, CharT c2) {
            return c1 == c2;
        }

        static std::size_t length(const CharT *s) {
            std::size_t len = 0;
            while (!eq(s[len], CharT())) ++len;
            return len;
        }

        static CharT *copy(CharT *dest, const CharT *src, std::size_t n) {
            return static_cast<CharT *>(std::memcpy(dest, src, n * sizeof(CharT)));
        }

        static CharT *move(CharT *dest, const CharT *src, std::size_t n) {
            return static_cast<CharT *>(std::memmove(dest, src, n * sizeof(CharT)));
        }
    };

    template<typename CharT, typename Traits = char_traits<CharT>, typename Alloc = std::allocator<CharT> >
    class basic_string {
    public:
        static constexpr size_t npos = -1;

        // 迭代器
        class iterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = CharT;
            using difference_type = std::ptrdiff_t;
            using pointer = CharT *;
            using reference = CharT &;

            iterator(pointer ptr = nullptr) : _ptr(ptr) {
            }

            reference operator*() const { return *_ptr; }
            pointer operator->() { return _ptr; }

            iterator &operator++() {
                ++_ptr;
                return *this;
            }

            iterator operator++(int) {
                iterator tmp(*this);
                ++_ptr;
                return tmp;
            }

            iterator &operator--() {
                --_ptr;
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(*this);
                --_ptr;
                return tmp;
            }

            iterator operator+(difference_type n) { return iterator(_ptr + n); }
            iterator operator-(difference_type n) { return iterator(_ptr - n); }
            difference_type operator-(const iterator &other) const { return _ptr - other._ptr; }

            bool operator==(const iterator &other) const { return _ptr == other._ptr; }
            bool operator!=(const iterator &other) const { return _ptr != other._ptr; }
            bool operator<(const iterator &other) const { return _ptr < other._ptr; }
            bool operator>(const iterator &other) const { return _ptr > other._ptr; }

        private:
            pointer _ptr;
        };

        class const_iterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = const CharT;
            using difference_type = std::ptrdiff_t;
            using pointer = const CharT *;
            using reference = const CharT &;

            const_iterator(pointer ptr = nullptr) : _ptr(ptr) {
            }

            reference operator*() const { return *_ptr; }
            pointer operator->() const { return _ptr; }

            const_iterator &operator++() {
                ++_ptr;
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tmp(*this);
                ++_ptr;
                return tmp;
            }

            const_iterator &operator--() {
                --_ptr;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                --_ptr;
                return tmp;
            }

            const_iterator operator+(difference_type n) const { return const_iterator(_ptr + n); }
            const_iterator operator-(difference_type n) const { return const_iterator(_ptr - n); }
            difference_type operator-(const const_iterator &other) const { return _ptr - other._ptr; }

            bool operator==(const const_iterator &other) const { return _ptr == other._ptr; }
            bool operator!=(const const_iterator &other) const { return _ptr != other._ptr; }
            bool operator<(const const_iterator &other) const { return _ptr < other._ptr; }
            bool operator>(const const_iterator &other) const { return _ptr > other._ptr; }

        private:
            pointer _ptr;
        };

        class reverse_iterator {
        public:
            using iterator_type = iterator;
            using difference_type = typename iterator::difference_type;
            using value_type = typename iterator::value_type;
            using pointer = typename iterator::pointer;
            using reference = typename iterator::reference;

            reverse_iterator(iterator it) : _it(it) {
            }

            reference operator*() const {
                iterator tmp = _it;
                return *--tmp; // 反向迭代器解引用访问前一个元素
            }

            pointer operator->() const { return &(operator*()); }

            reverse_iterator &operator++() {
                --_it;
                return *this;
            }

            reverse_iterator operator++(int) {
                reverse_iterator tmp(*this);
                --_it;
                return tmp;
            }

            reverse_iterator &operator--() {
                ++_it;
                return *this;
            }

            reverse_iterator operator--(int) {
                reverse_iterator tmp(*this);
                ++_it;
                return tmp;
            }

            bool operator==(const reverse_iterator &other) const { return _it == other._it; }
            bool operator!=(const reverse_iterator &other) const { return _it != other._it; }

        private:
            iterator _it;
        };

        class const_reverse_iterator {
        public:
            using iterator_type = const_iterator;
            using difference_type = typename iterator_type::difference_type;
            using value_type = typename iterator_type::value_type;
            using pointer = typename iterator_type::pointer;
            using reference = typename iterator_type::reference;

            const_reverse_iterator(iterator_type it) : _it(it) {
            }

            reference operator*() const {
                iterator_type tmp = _it;
                return *--tmp;
            }

            pointer operator->() const { return &(operator*()); }

            const_reverse_iterator &operator++() {
                --_it;
                return *this;
            }

            const_reverse_iterator operator++(int) {
                const_reverse_iterator tmp(*this);
                --_it;
                return tmp;
            }

            const_reverse_iterator &operator--() {
                ++_it;
                return *this;
            }

            const_reverse_iterator operator--(int) {
                const_reverse_iterator tmp(*this);
                ++_it;
                return tmp;
            }

            bool operator==(const const_reverse_iterator &other) const { return _it == other._it; }
            bool operator!=(const const_reverse_iterator &other) const { return _it != other._it; }

        private:
            iterator_type _it;
        };

        basic_string() : _alloc(), _size(0), _cap(0) {
            _data = std::allocator_traits<Alloc>::allocate(_alloc, 1); // 分配 1 个元素
            Traits::assign(_data[0], CharT()); // 放 '\0'
        }

        basic_string(const CharT *s) : _data(nullptr), _size(0), _cap(0) {
            size_t len = Traits::length(s);
            ensure_capacity(len + 1);
            Traits::copy(_data, s, len);
            _size = len;
            _data[_size] = CharT();
        }

        ~basic_string() {
            if (_data) {
                std::allocator_traits<Alloc>::deallocate(_alloc, _data, _cap);
            }
        }

        // 拷贝构造函数
        basic_string(const basic_string &other)
            : _alloc(other._alloc), _size(other._size), _cap(other._size) {
            // 分配空间
            _data = std::allocator_traits<Alloc>::allocate(_alloc, _size + 1);

            // 拷贝内容
            Traits::copy(_data, other._data, _size);

            // null 终止符
            _data[_size] = CharT();
        }

        // 移动构造函数
        basic_string(basic_string &&other) noexcept
            : _alloc(std::move(other._alloc)),
              _data(other._data),
              _size(other._size),
              _cap(other._cap) {
            other._data = nullptr;
            other._size = 0;
            other._cap = 0;
        }

        // 观察器
        size_t size() const noexcept { return _size; }
        bool empty() const noexcept { return _size == 0; }
        size_t capacity() const noexcept { return _cap; }

        const CharT *c_str() const noexcept {
            _data[_size] = '\0';
            return _data;
        }

        // 访问
        /* 不做越界检查 */
        CharT &operator[](size_t pos) noexcept {
            return _data[pos];
        }

        const CharT &operator[](size_t pos) const noexcept {
            return _data[pos];
        }

        CharT &at(size_t pos) {
            if (pos >= _size) {
                throw std::out_of_range("basic_string::at: index out of range");
            }
            return _data[pos];
        }

        const CharT &at(size_t pos) const {
            if (pos >= _size) {
                throw std::out_of_range("basic_string::at: index out of range");
            }
            return _data[pos];
        }

        CharT back() const {
            return _data[_size - 1];
        }

        // 修改
        void push_back(CharT ch) {
            ensure_capacity(_size + 1); // 至少保证一个空位
            _data[_size++] = ch;
        }

        basic_string &append(const CharT *s) {
            if (!s) return *this;

            size_t len = Traits::length(s); //1. 获取s长度
            size_t need = _size + len;
            ensure_capacity(need);

            Traits::copy(_data + _size, s, len);
            _size += len;
            _data[_size] = CharT();

            return *this; //支持链式调用
        }

        void pop_back() {
            if (_size == 0) return;
            --_size;
        }

        // 用默认字符（'\0'）填充新元素
        void resize(size_t n) {
            if (n > _size) {
                if (n > _cap) ensure_capacity(n + 1);
                for (size_t i = _size; i < n; i++) {
                    _data[i] = '\0';
                }
                _size = n;
            } else if (n < _size) {
                _size = n;
            }
        }

        // 用指定字符 c 填充新元素
        void resize(size_t n, char c) {
            if (n > _size) {
                if (n > _cap) ensure_capacity(n + 1);
                for (size_t i = _size; i < n; i++) {
                    _data[i] = c;
                }
                _size = n;
            } else if (n < _size) {
                _size = n;
            }
        }

        /* ===操作符重载=== */
        template<typename CharT, typename Traits, typename Alloc>
        friend std::ostream &operator<<(std::ostream &os, const basic_string<CharT, Traits, Alloc> &str) {
            return os.write(str._data, str._size);
        }

        // 拷贝赋值运算符
        basic_string &operator=(const basic_string &other) {
            if (this == &other) return *this;

            if (_cap < other._size) {
                // 重新分配内存
                CharT *new_data = std::allocator_traits<Alloc>::allocate(_alloc, other._size + 1);
                Traits::copy(new_data, other._data, other._size);

                // 释放原有内存
                std::allocator_traits<Alloc>::deallocate(_alloc, _data, _cap);

                _data = new_data;
                _cap = other._size;
            } else {
                // 直接覆盖
                Traits::copy(_data, other._data, other._size);
            }

            _size = other._size;
            _data[_size] = CharT(); // null 终止符

            return *this;
        }

        // 移动赋值运算符
        basic_string &operator=(basic_string &&other) noexcept {
            if (this == &other) return *this;

            // 释放自身内存
            std::allocator_traits<Alloc>::deallocate(_alloc, _data, _cap);

            // 窃取右值对象数据
            _data = other._data;
            _size = other._size;
            _cap = other._cap;
            _alloc = std::move(other._alloc);

            // 重置右值对象
            other._data = nullptr;
            other._size = 0;
            other._cap = 0;

            return *this;
        }

        basic_string operator+(const basic_string &other) const {
            basic_string result(*this); // 复制当前对象
            result.append(other._data); // append other
            return result; // 返回新对象
        }

        basic_string operator+(const CharT *other) const {
            basic_string result(*this);
            result.append(other);
            return result;
        }

        basic_string &operator+=(const CharT *s) {
            append(s); // 在当前对象末尾追加
            return *this; // 返回 *this 的引用
        }

        basic_string &operator+=(const basic_string &other) {
            append(other._data);
            return *this;
        }

        /* 支持迭代器 */
        iterator begin() { return iterator(_data); }
        iterator end() { return iterator(_data + _size); }
        const_iterator cbegin() const noexcept { return const_iterator(_data); }
        const_iterator cend() const noexcept { return const_iterator(_data + _size); }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }

        const_reverse_iterator rbegin() const { return const_reverse_iterator(cend()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(cbegin()); }

        const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
        const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

        /* 删除字符 */
        // 按位置删除单个字符，依靠迭代器
        iterator erase(iterator it) {
            size_t pos = it - begin();
            if (pos >= _size) return iterator(_data + _size); // 越界，返回末尾

            for (size_t i = pos; i < _size; i++) {
                _data[i] = _data[i + 1];
            }
            --_size;

            return iterator(_data + pos);
        }

        // 按位置删除单&多个字符
        iterator erase(size_t pos, size_t count) {
            if (pos >= _size) return iterator(_data + _size); // 越界，返回末尾
            if (pos + count > _size) count = _size - pos; // 调整删除长度

            if (count == 0) return iterator(_data + pos); // 无操作

            // 搬移剩余字符
            Traits::move(_data + pos, _data + pos + count, _size - pos - count);

            _size -= count;
            _data[_size] = CharT(); // 可选：保证 c_str() 安全

            return iterator(_data + pos);
        }

        // 用迭代器删除区间，不包括 [first,last)
        iterator erase(iterator first, iterator last) {
            size_t beg = first - begin();
            size_t end = last - begin();
            size_t del_cnt = end - beg;

            if (del_cnt == 0) return iterator(_data + beg); // 空区间

            // 搬移剩余元素
            Traits::move(_data + beg, _data + end, _size - end);

            _size -= del_cnt;
            _data[_size] = CharT(); // 保证 c_str() 安全

            return iterator(_data + beg);
        }

        basic_string &erase() {
            clear();
            return *this;
        }

        void clear() noexcept { _size = 0; }

        /* 查找 */
        CharT &front() {
            if (_size == 0) throw std::out_of_range("basic_string::front() called on empty string");
            return _data[0];
        }

        const CharT &front() const {
            if (_size == 0) throw std::out_of_range("basic_string::front() called on empty string");
            return _data[0];
        }

        // 查找单个字符
        size_t find(CharT ch, size_t pos = 0) const noexcept {
            for (size_t i = pos; i < _size; ++i) {
                if (Traits::eq(_data[i], ch)) return i;
            }
            return npos;
        }

        // 查找 C 字符串
        size_t find(const CharT *s, size_t pos = 0) const {
            if (!s || _size == 0 || pos >= _size) return npos;

            size_t s_len = Traits::length(s); // 要查找的字符串长度
            if (s_len == 0) return pos; // 空字符串，约定返回 pos

            // 遍历每个可能的起点
            for (size_t i = pos; i <= _size - s_len; ++i) {
                // 对比 _data[i..i+s_len-1] 和 s[0..s_len-1]
                bool match = true;
                for (size_t j = 0; j < s_len; ++j) {
                    if (!Traits::eq(_data[i + j], s[j])) {
                        match = false;
                        break;
                    }
                }
                if (match) return i; // 找到，返回起点
            }

            return npos; // 没找到
        }

        // 查找另一个 string
        size_t find(const basic_string &str, size_t pos = 0) const {
            if (str.empty() || _size == 0 || pos >= _size) return npos;

            size_t s_len = str.size(); // 子串长度
            if (s_len == 0) return pos; // 空子串，返回 pos

            // 遍历每个可能的起点
            for (size_t i = pos; i <= _size - s_len; ++i) {
                bool match = true;
                for (size_t j = 0; j < s_len; ++j) {
                    if (!Traits::eq(_data[i + j], str._data[j])) {
                        match = false;
                        break;
                    }
                }
                if (match) return i; // 找到，返回起点
            }

            return npos; // 没找到
        }

        // 从指定位置 pos 开始，查找 第一个在 chars 中出现的字符，注意不是把chars作为子串查找
        size_t find_first_of(const basic_string &chars, size_t pos = 0) const {
            if (chars.empty() || _size == 0 || pos >= _size) return npos;

            for (size_t i = pos; i < _size; ++i) {
                for (size_t j = 0; j < chars.size(); ++j) {
                    if (Traits::eq(_data[i], chars[j])) {
                        return i; // 找到
                    }
                }
            }
            return npos; // 没找到
        }

        // 从 pos 开始，查找 第一个不在 chars 中的字符
        size_t find_first_not_of(const basic_string &chars, size_t pos = 0) const {
            if (chars.empty() || _size == 0 || pos >= _size) return npos;

            for (size_t i = pos; i < _size; ++i) {
                bool found = false;
                for (size_t j = 0; j < chars.size(); ++j) {
                    if (Traits::eq(_data[i], chars[j])) {
                        found = true;
                        break;
                    }
                }
                if (!found) return i; // 第一个不属于 chars 的字符
            }
            return npos; // 没找到
        }

        // 从指定位置 pos 向前（从右到左）查找 最后一个在 chars 中出现的字符
        size_t find_last_of(const basic_string &chars, size_t pos = npos) const {
            if (_size == 0) return npos;

            pos = (pos == npos ? _size - 1 : pos);

            for (size_t i = pos + 1; i-- > 0;) {
                for (size_t j = 0; j < chars.size(); j++) {
                    if (Traits::eq(chars[j], _data[i])) {
                        return i;
                    }
                }
            }
            return npos;
        }

        // 从 pos 向前查找 最后一个不在 chars 中的字符
        size_t find_last_not_of(const basic_string &chars, size_t pos = npos) const {
            if (_size == 0) return npos;

            pos = (pos == npos ? _size - 1 : pos);

            for (size_t i = pos + 1; i-- > 0;) {
                bool found = false;
                for (size_t j = 0; j < chars.size(); j++) {
                    if (Traits::eq(chars[j], _data[i])) {
                        found = true;
                        break;
                    }
                }
                if (!found) return i;
            }
            return npos;
        }

        basic_string substr(size_t pos = 0, size_t count = npos) const {
            if (pos > _size) {
                throw std::out_of_range("basic_string::substr: pos out of range");
            }

            size_t len = std::min(count, _size - pos);
            basic_string result;
            result.ensure_capacity(len + 1); // 分配空间
            Traits::copy(result._data, _data + pos, len);
            result._size = len;
            return result;
        }

        /* 插入 */
        // 在 pos 位置插入单个字符
        void insert(size_t pos, CharT ch) {
            if (pos > _size) return;
            if (_size + 1 > _cap) ensure_capacity(_size + 1);

            for (size_t i = _size - 1; i >= pos; i--) {
                _data[i + 1] = _data[i];
            }
            _data[pos] = ch;
            _size++;
        }

        // 在 pos 位置插入 n 个相同字符
        void insert(size_t pos, size_t n, CharT ch) {
            if (pos > _size) return; // 允许 pos == _size 插入到末尾
            ensure_capacity(_size + n);

            for (size_t i = _size; i > pos; --i) {
                _data[i + n - 1] = _data[i - 1]; // 腾出位置
            }

            for (size_t i = pos; i < pos + n; ++i) {
                _data[i] = ch;
            }

            _size += n;
        }

        // 在 pos 位置插入 C 字符串
        void insert(size_t pos, const CharT *s) {
            if (pos > _size) return;
            size_t len_s = Traits::length(s);
            if (_size + len_s > _cap) ensure_capacity(_size + len_s);


            for (size_t i = _size; i > pos; --i) {
                _data[i + len_s - 1] = _data[i - 1]; // 腾出位置
            }

            Traits::copy(_data + pos, s, len_s);
            _size += len_s;
        }

        // 在 pos 位置插入 C 字符串前 n 个字符
        void insert(size_t pos, const CharT *s, size_t n) {
            if (pos > _size) return;
            size_t len_s = Traits::length(s);
            size_t inst_len = std::min(len_s, n);

            if (_size + inst_len > _cap) ensure_capacity(_size + inst_len);


            for (size_t i = _size; i > pos; --i) {
                _data[i + inst_len - 1] = _data[i - 1]; // 腾出位置
            }

            Traits::copy(_data + pos, s, inst_len);
            _size += inst_len;
        }

        // 在 pos 位置插入另一个 basic_string
        void insert(size_t pos, const basic_string &str) {
            if (pos > _size) return;

            if (_size + str._size > _cap) ensure_capacity(_size + str._size);

            for (size_t i = _size; i > pos; --i) {
                _data[i + str._size - 1] = _data[i - 1]; // 腾出位置
            }

            Traits::copy(_data + pos, str._data, str._size);
            _size += str._size;
        }

        // 插入 str 的子串 [subpos, subpos + sublen)
        void insert(size_t pos, const basic_string &str, size_t subpos, size_t sublen) {
            if (pos > _size) return;

            if (_size + sublen > _cap) ensure_capacity(_size + sublen);

            for (size_t i = _size; i > pos; --i) {
                _data[i + sublen - 1] = _data[i - 1]; // 腾出位置
            }

            Traits::copy(_data + pos, str._data + subpos, sublen);
            _size += sublen;
        }

        // 迭代器版本：在迭代器 pos 插入区间 [first, last)
        template<typename InputIt>
        void insert(iterator pos, InputIt first, InputIt last) {
            size_t n = std::distance(first, last);
            if (n == 0) return;

            size_t pos_index = pos - begin();
            ensure_capacity(_size + n);

            // 腾出位置
            for (size_t i = _size; i > pos_index; --i) {
                _data[i + n - 1] = _data[i - 1];
            }

            // 拷贝元素
            size_t idx = pos_index;
            for (auto it = first; it != last; ++it) {
                _data[idx++] = *it;
            }

            _size += n;
        }

        // 迭代器版本：在 pos 迭代器插入 n 个相同字符
        void insert(iterator pos, size_t n, CharT ch) {
            size_t idx = pos - begin();
            if (idx > _size) return;

            if (_size + n > _cap) ensure_capacity(_size + n);

            for (size_t i = _size; i > idx; --i) {
                _data[i + n - 1] = _data[i - 1];
            }

            for (size_t i = idx; i < idx + n; i++) {
                _data[i] = ch;
            }

            _size += n;
        }

    private:
        /* 私有方法 */
        void ensure_capacity(size_t need) {
            if (need <= _cap) return; // 已经够用，不做任何操作

            size_t new_cap = grow_recommend(need); // 计算新容量
            CharT *new_data = std::allocator_traits<Alloc>::allocate(_alloc, new_cap + 1); // +1 给 null terminator

            if (_data) {
                Traits::move(new_data, _data, _size); // 安全搬移原字符
                std::allocator_traits<Alloc>::deallocate(_alloc, _data, _cap);
            }

            _data = new_data;
            _cap = new_cap;
            _data[_size] = CharT(); // 保证 c_str() 可用
        }

        size_t grow_recommend(size_t need) const {
            // 翻倍策略：如果当前有容量就 *2，否则从 8 开始
            size_t new_cap = _cap ? _cap * 2 : 8;
            // 确保至少能满足 need
            if (new_cap < need) new_cap = need;
            return new_cap;
        }

        /* 成员变量 */
        CharT *_data;
        size_t _size;
        size_t _cap;
        Alloc _alloc; // 分配器实例
    };

    using string = basic_string<char>;
}
