/*

LinkeList

Here your task is to implement doubly linked list in C++14.
- You should use unique_ptr for memory management and avoid any explicit deletes.
  - Keep in mind that not all pointers can be unique_ptrs.
- You should try to avoid code duplication.
- There are tests provided for you.
- Solution will be provided after both seminars end.

 */

#include <memory>

namespace {
    template <typename T>
    class Node {
    public:
        Node() = default;
        Node(const T& val ) : _data(std::make_unique<T>(val)) {}

        T& value() noexcept {
            return *_data;
        }

        const T& value() const noexcept {
            return *_data;
        }

        Node* next() noexcept {
            return _next.get();
        }

        void next(std::unique_ptr<Node> n) noexcept {
            _next = std::move(n);
        }

        const Node* next() const noexcept {
            return _next.get();
        }

        Node* prev() noexcept {
            return _prev;
        }
        void prev(Node* n) noexcept {
            _prev = n;
        }
        const Node* prev() const noexcept {
            return _prev;
        }

        std::unique_ptr<T> _data;
        std::unique_ptr<Node> _next;
        Node* _prev;
    };

    template <typename Nod>
    class fw_iterator {
    public:
        using reference = Nod&;
        using pointer   = Nod*;
    private:
        Nod* _current;
    public:
        fw_iterator() : _current(nullptr) {}
        fw_iterator(Nod* ptr) : _current(ptr) {}
        fw_iterator(const fw_iterator& it)
                : _current(it._current) {}

        reference operator*() const noexcept {
            return *_current;
        }

        pointer operator->() const noexcept {
            return _current;
        }

        fw_iterator& operator++() noexcept {
            _current = _current->next();
            return *this;
        }

        fw_iterator operator++(int) noexcept {
            fw_iterator cpy = *this;
            _current = _current->next();
            return cpy;
        }

        fw_iterator& operator--() noexcept {
            _current = _current->prev();
            return *this;
        }

        fw_iterator operator--(int) noexcept {
            fw_iterator cpy = *this;
            _current = _current->prev();
            return cpy;
        }

        friend bool operator==(const fw_iterator& i1, const fw_iterator& i2) noexcept {
            return i1._current == i2._current;
        }

        friend bool operator!=(const fw_iterator& i1, const fw_iterator& i2) noexcept {
            return i1._current != i2._current;
        }
    };
}

template< typename T >
class LinkedList {
public:
    using Node = ::Node<T>;
    using iterator = ::fw_iterator<Node>;
    using const_iterator = ::fw_iterator<const Node>;

    LinkedList() = default;
    LinkedList(const LinkedList& other) {
        for (auto& nod : other) {
            push_back(nod.value());
        }
    }

    bool empty() const noexcept {
        return _first.get() == nullptr;
    }

    Node* first() noexcept {
        return _first.get();
    }
    const Node* first() const noexcept {
        return _first.get();
    }

    iterator begin() noexcept {
        return iterator(_first.get());
    }
    const_iterator begin() const noexcept {
        return const_iterator(_first.get());
    }

    Node* last() noexcept{
        return _last;
    }
    const Node* last() const {
        return _last;
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }
    const_iterator end() const {
        return const_iterator(nullptr);
    }

    Node* find( const T &val ) {
        auto it = begin();
        while (it != end()) {
            if (it->value() == val) {
                return it.operator->();
            }
            ++it;
        }
        return nullptr;
    }
    const Node* find( const T &val ) const {
        auto it = begin();
        while (it != end()) {
            if (it->value() == val) {
                return it.operator->();
            }
            ++it;
        }
        return nullptr;
    }

    void erase(T val) {
        Node* ptr = find(val);
        if (ptr) {
            erase(ptr);
        }
    }
    void erase(Node *n) {
        if (n == _last) {
            _last = n->prev();
        }

        if (n == _first.get()) {
            _first = std::move(n->_next);
        }

        if (n->next()) {
            n->next()->prev(n->prev());
        }

        if (n->prev()) {
            n->prev()->next(std::move(n->_next));
        }
    }

    void insert_before( Node *n, T val );
    void insert_after( Node *n, T val );

    void push_back(T val) {
		auto n = std::make_unique<Node>(val);
		if (empty()) {
			_first = std::move(n);
			_last = _first.get();
		} else {
			_last->next(std::move(n));
			_last->next()->prev(_last);
			_last = _last->next();
		}
	}
    void push_front( T val ) {
		auto n = std::make_unique<Node>(val);
		if (empty()) {
			_first = std::move(n);
			_last = _first.get();
		} else {
			_first->prev(n.get());
			_first->prev()->next(std::move(_first));
			_first = std::move(n);
		}
	}

private:
    std::unique_ptr<Node> _first;
    Node* _last;
};
