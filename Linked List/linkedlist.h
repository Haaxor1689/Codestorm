/*

LinkeList

Here your task is to implement doubly linked list in C++14.
- You should use unique_ptr for memory management and avoid any explicit deletes.
  - Keep in mind that not all pointers can be unique_ptrs.
- You should try to avoid code duplication.
- There are tests provided for you.
- Solution will be provided after both seminars end.

 */

#include <iostream>
#include <memory>

template <typename T> class LinkedList;

namespace detail {
	template <typename T>
	class Node {
	public:
		using reference = T&;
		using const_reference = const T&;

		reference value() noexcept {
			return _value;
		}
		const_reference value() const noexcept {
			return _value;
		}
		void value(const T& val) noexcept {
			_value = val;
		}

		Node* next() noexcept {
			return _next.get();
		}
		const Node* next() const noexcept {
			return _next.get();
		}
		void next(Node* nod) noexcept {
			_next.reset(nod);
		}

		Node* prev() noexcept {
			return _prev;
		}
		const Node* prev() const noexcept {
			return _prev;
		}
		void prev(Node* nod) noexcept {
			_prev = nod;
		}

	protected:
		friend class LinkedList<T>;
		explicit Node() :_value(T()), _next(nullptr), _prev(nullptr) {}
		explicit Node(const T& val) : _value(val), _next(nullptr), _prev(nullptr) {}
		Node(const Node&) = delete;
	private:
		T _value;
		std::unique_ptr<Node> _next;
		Node* _prev;
	};
}

template< typename T >
class LinkedList {
public:
   	using Node = detail::Node<T>;

    LinkedList() : _last(nullptr) {}
    LinkedList(const LinkedList &o) {
		const Node* n = o.first();
		while (n) {
			push_back(n->value());
			n = n->next();
		}
	}

    bool empty() const noexcept {
		return !_first && !_last;
	}

    Node *first() noexcept {
		return _first.get();
	}
    const Node *first() const noexcept {
		return _first.get();
	}

    Node *last() noexcept {
		return _last;
	}
    const Node *last() const noexcept {
		return _last;
	}

    Node *find(const T &val) noexcept {
		Node* n = first();
		while (n) {
			if (n->value() == val) {
				return n;
			}
			n = n->next();
		}
		return nullptr;
	}
    const Node *find( const T &val ) const noexcept {
		const Node* n = first();
		while (n) {
			if (n->value() == val) {
				return n;
			}
			n = n->next();
		}
		return nullptr;
	}

    void erase(const T& val) {
		erase(find(val));
	}
    void erase(Node *n) {
		if (n) {
			if (n == _last && n == _first.get()) {
				_first.reset();
				_last = nullptr;
			} else {
				if (n == first()) {
					n->next()->prev(nullptr);
					_first.reset(n->_next.release());
				} else {
					if (n == last()) {
						_last = n->prev();
						_last->_next.reset();
					} else {
						Node 	*next = n->next(),
								*prev = n->prev();
						next->prev(n->prev());
						prev->next(n->_next.release());
					}
				}
			}
		}
	}

    void insert_before(Node *n, const T& val) {
		if (n) {
			if (n == first()) {
				push_front(val);
			} else {
				auto nod = std::unique_ptr<Node>(new detail::Node<T>(val));
				Node *pr = n->prev();

				n->prev(nod.get());
				nod->prev(pr);
				nod->next(pr->_next.release());
				pr->next(nod.release());
			}
		}
	}
    void insert_after(Node *n,const T& val) {
		if (n) {
			if (n == last()) {
				push_back(val);
			} else {
				auto nod = std::unique_ptr<Node>(new detail::Node<T>(val));
				Node* nx = n->next();

				nod->prev(n);
				nx->prev(nod.get());
				nod->next(n->_next.release());
				n->next(nod.release());
			}
		}
	}

    void push_back(const T& val) {
		auto nod = std::unique_ptr<Node>(new detail::Node<T>(val));
		if (empty()) {
			_first = std::move(nod);
			_last = first();
		} else {
			nod->prev(last());
			last()->next(nod.release());
			_last = _last->next();
		}
	}
    void push_front(const T& val) {
		auto nod = std::unique_ptr<Node>(new detail::Node<T>(val));
		if (empty()) {
			_first = std::move(nod);
			_last  = first();
		} else {
			first()->prev(nod.get());
			nod->next(_first.release());
			_first.reset(nod.release());
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const LinkedList& ll) noexcept {
		auto n = ll._first.get();
		while (n) {
			os << n->value();
			n = n->next();
			if (n)
				os << " ";
		}
		return os;
	}

private:
	std::unique_ptr<Node> _first;
	Node* _last;
};
