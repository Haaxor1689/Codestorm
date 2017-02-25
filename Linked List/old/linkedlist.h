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
#include <initializer_list>
#include <memory>

template <typename T> class LinkedList;


namespace detail {

	/**
	 * Class representing inner Node of LinkedList
	 */
	template <typename T>
	class Node {
	public:
		friend class LinkedList<T>;
		friend std::unique_ptr<Node> std::make_unique<Node>(T&);
		friend std::unique_ptr<Node> std::make_unique<Node>(T&&);

		using value_type = T;

		/**
		 * @brief	value getter
		 * @return 	value
		 */
		T& value() noexcept {
			return _value;
		}
		const T& value() const noexcept {
			return _value;
		}

		/**
		 * @brief	next raw ptr getter
		 * @return  raw ptr to next
		 */
		Node*  next() noexcept {
			return _next.get();
		}
		const Node*  next() const noexcept {
			return _next.get();
		}

		/**
		 * @brief	prev raw ptr getter
		 * @return 	raw ptr to prev
		 */
		Node*  prev() noexcept {
			return _prev;
		}
		const Node* prev() const noexcept {
			return _prev;
		}

	protected:

		Node() : _prev(nullptr) {}
		Node(T val) : _value(std::move(val)), _prev(nullptr) {}
		Node(const Node&) = delete;
		Node(Node&& other) : _value(std::move(other._value)),
							 _next(std::move(other._next)),
							 _prev(other._prev) {}

		/**
		 * @brief	value setter
		 * @param 	val
		 */
		void value(T val) noexcept {
			_value = std::move(val);
		}

		/**
		 * @brief	next setters
		 * @param 	nxt
		 */
		void next(Node* nxt) noexcept {
			_next.reset(nxt);
		}
		void next(std::unique_ptr<Node> nxt) noexcept {
			_next = std::move(nxt);
		}

		/**
		 * @brief	prev stter
		 * @param 	prv
		 */
		void prev(Node* prv) noexcept {
			_prev = prv;
		}

	private:
		T _value;
		std::unique_ptr<Node> _next;
		Node*  _prev;
	};

} // namespace detail

template< typename T >
class LinkedList {
public:
    using Node = detail::Node<T>;

	/**
	 * @brief	default ctor
	 */
    LinkedList() = default;

	/**
	 * @brief	copy ctor
	 * @param 	ll		copied LinkedList
	 */
    LinkedList(const LinkedList &ll) {
		const auto* ptr = ll.first();
		while (ptr != nullptr) {
			push_back(ptr->value());
			ptr = ptr->next();
		}
	}

	/**
	 * @brief	move ctor
	 * @param 	ll 		moved LinkedList
	 */
	LinkedList(LinkedList&& ll) noexcept : _first(std::move(ll._first)),
										   _last(ll._last) {
		ll._last = nullptr;
	}

	LinkedList(std::initializer_list<T> init) {
		for (auto elem : init)
			push_back(std::move(elem));
	}

	/**
	 * @brief	copy assigment operator
	 * @param 	ll 		copied LinkedList
	 * @return 	reference to this instance of LL
	 */
	LinkedList& operator=(const LinkedList& ll) {
		clear();
		const auto* ptr = ll.first();
		while (ptr != nullptr) {
			push_back(ptr->value());
			ptr = ptr->next();
		}
		return *this;
	}

	/**
	 * @brief	move assigment operator
	 * @param 	ll		moved LinkedList
	 * @return 	reference to this instance of LL
	 */
	LinkedList& operator=(LinkedList&& ll) noexcept {
		_first = std::move(ll._first);
		_last = ll._last;
		ll._last = nullptr;
		return *this;
	}

	/**
	 * @brief	initializer_list assigment operator
	 * @param 	init
	 * @return 	reference to this instance of LL
	 */
	LinkedList& operator=(std::initializer_list<T> init) {
		this->operator=(std::move(LinkedList(init)));
		return *this;
	}

	/**
	 * @brief
	 * @return	true if LinkedList is empty, false otherwise
	 */
    bool empty() const noexcept {
		return !_first && !_last;
	}

	/**
	 * @brief 	function getting raw ptr to first elem
	 * @return 	raw ptr to first elem, nullptr if empty
	 */
    Node* first() noexcept {
		return _first.get();
	}
    const Node* first() const noexcept {
		return _first.get();
	}

	/**
	 * @brief	function getting raw ptr to last elem
	 * @return 	raw ptr to last elem, nullptr if empty
	 */
    Node* last() noexcept {
		return _last;
	}
    const Node* last() const noexcept {
		return _last;
	}

	/**
	 * @brief	function looking up for value
	 * @param 	val
	 * @return	pointer to Node containing value, nullptr if value is not present
	 */
    Node* find(const T& val) noexcept {
		auto* p = first();
		while (p) {
			if (p->value() == val)
				return p;
			p = p->next();
		}
		return nullptr;
	}
    const Node* find(const T& val) const noexcept {
		return const_cast<const Node*>(const_cast<LinkedList*>(this)->find(val));
	}

	/**
	 * @brief	erases value from LL
	 * @param 	val
	 */
    void erase(const T& val) {
		erase(find(val));
	}

	/**
	 * @brief	erases Node from LL
	 * @param 	n
	 */
	void erase(Node* n) {
		if (n) {
			if (n == first() && n == last()) {
				clear();
			} else {
				if (n ==first()) {
					n->next()->prev(nullptr);
					_first.reset(n->_next.release());
				} else {
					if (n == last()) {
						_last = _last->prev();
						_last->_next.reset();
					} else {
						n->next()->prev(n->prev());
						n->prev()->next(n->_next.release());
					}
				}
			}
		}
	}

	/**
	 * @brief	inserts value before element specified in params
	 * @param 	n 		Node before which insertion will take place
	 * @param 	val		value to be inserted
	 */
    void insert_before(Node* const n, T val) {
		if (n) {
			if (n == first()) {
				push_front(std::move(val));
			} else {
				auto nod = std::make_unique<Node>(std::move(val));
				auto* prv = n->prev();
				nod->prev(n->prev());
				n->prev(nod.get());
				nod->next(prv->_next.release());
				prv->next(std::move(nod));
			}
		}
	}

	/**
	 * @brief	inserts value after element specified in params
	 * @param 	n		Node after which insertion will take place
	 * @param val 		value to be inserted
	 */
    void insert_after(Node* const n, T val) {
		if (n) {
			if (n == last()) {
				push_back(std::move(val));
			} else {
				insert_before(n->next(), std::move(val));
			}
		}
	}

	/**
	 * @brief	inserts value at the end of container
	 * @param 	val
	 */
    void push_back(T val) {
		auto nod = std::make_unique<Node>(std::move(val));
		if (empty()) {
			_first = std::move(nod);
			_last = _first.get();
		} else {
			nod->prev(_last);
			_last->next(std::move(nod));
			_last = _last->next();
		}
	}

	/**
	 * @brief	inserts value at the beginning of the container
	 * @param 	val
	 */
    void push_front(T val) {
		auto nod = std::make_unique<Node>(std::move(val));
		if (empty()) {
			_first = std::move(nod);
			_last  = _first.get();
		} else {
			_first->prev(nod.get());
			nod->next(std::move(_first));
			_first = std::move(nod);
		}
	}

	/**
	 * @brief	clears container
	 */
	void clear() noexcept {
		_first.reset();
		_last = nullptr;
	}

	friend std::ostream& operator<<(std::ostream& os, const LinkedList& ll) noexcept {
		auto* p = ll.first();
		while (p) {
			os << p->value() << " ";
			p = p->next();
		}
		os << std::endl;
		return os;
	}

	friend std::istream& operator>>(std::istream& is, LinkedList<T>& ll) {
		while (!is.eof()) {
			std::string input;
			std::getline(is, input);
			ll.push_back(std::move(func(std::move(input))));
		}
		return is;
	}

private:
	
	std::unique_ptr<Node> _first;
	Node* _last = nullptr;
};
