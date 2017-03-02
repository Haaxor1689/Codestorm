/*

LinkedList
-Non-sequence container
-dynamic memory is used to store elements
-unique_ptrs and RAII principles are used, thus implementation should be leak-free

 */

#include <iostream>
#include <initializer_list>
#include <memory>

namespace {

}

template <typename T> class LinkedList;


namespace detail {

	template <typename B, typename T>
	class forward_iterator;

	/**
	 * Class representing inner Node of LinkedList
	 */
	template <typename T>
	class Node {
	public:
		friend class LinkedList<T>;
		friend class forward_iterator<Node<T>, T>;
		friend class forward_iterator<const Node<T>, const T>;
		friend std::unique_ptr<Node> std::make_unique<Node>(T&);
		friend std::unique_ptr<Node> std::make_unique<Node>(T&&);

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
		Node* next() noexcept {
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
		/**
		 * @brief	default ctor
		 */
		Node() = default;

		/**
		 * @brief 	parametric ctor
		 * @param 	val 		value
		 */
		Node(T val) : _value(std::move(val)) {}

		/**
		 * Copying on Nodes is not allowed!
		 */
		Node(const Node&) = delete;

		/**
		 * @brief	move ctor
		 * @param 	other 		move Node
		 */
		Node(Node&& other) : _value(std::move(other._value)),
							 _next(std::move(other._next)),
							 _prev(other._prev) {
			other._prev = nullptr;
		}

		/**
		 * @brief	value setter
		 * @param 	val
		 */
		void value(T val) noexcept {
			_value = std::move(val);
		}

		/**
		 * @brief	next setters
		 * @param 	nxt			poiner to Node
		 */
		void next(Node* nxt) noexcept {
			_next.reset(nxt);
		}
		void next(std::unique_ptr<Node> nxt) noexcept {
			_next = std::move(nxt);
		}

		/**
		 * @brief	prev setter
		 * @param 	prv			pointer to Node
		 */
		void prev(Node* prv) noexcept {
			_prev = prv;
		}

	private:
		T _value;
		std::unique_ptr<Node> _next;
		Node* _prev = nullptr;
	};  // Node

///-----------------------------------------------------------------------------------------------------

	/**
	 * Class representing iterator of LinkedList
	 */
	template <typename Base, typename Value>
	class forward_iterator {
	protected:
		Base _current = nullptr;

	public:
		using reference = Value&;
		using pointer = Value*;

		/**
		 * @brief	default ctor
		 */
		forward_iterator() = default;

		/**
		 * @brief	parametric ctor
		 * @param 	bs 			Base pointer
		 */
		forward_iterator(Base bs) : _current(bs) {}

		/**
		 * @brief	copy ctor
		 * @param 	fi
		 */
		forward_iterator(const forward_iterator& fi) : _current(fi._current) {}

		/**
		 * @brief	move ctor
		 * @param 	fi
		 */
		forward_iterator(forward_iterator&& fi) noexcept : _current(fi._current) {
			fi._current = nullptr;
		}

		/**
		 * @brief	assigment operator
		 * @param 	bs
		 * @return 	reference tot his object
		 */
		forward_iterator& operator=(Base bs) {
			_current = bs;
			return *this;
		}

		/**
		 * @brief	copy assigment operator
		 * @param 	fi
		 */
		forward_iterator& operator=(const forward_iterator& fi) {
			_current = fi._current;
			return *this;
		}

		/**
		 * @brief	move assigment operator
		 * @param 	fi
		 */
		forward_iterator& operator=(forward_iterator&& fi) noexcept {
			_current = fi._current;
			fi._current = nullptr;
			return *this;
		}

		/**
		 * @brief	dereferencing operator
		 * @return 	reference to object within Node
		 */
		reference operator*() const noexcept {
			return _current->value();
		}

		/**
		 * @brief	arrow operator
		 * @return 	pointer to object within Node
		 */
		pointer operator->() const noexcept {
			return &_current->_value;
		}

		/**
		 * @brief	incrementing prefix ++ operator
		 * @return 	reference to this updated object
		 */
		forward_iterator& operator++() noexcept {
			_current = _current->next();
			return *this;
		}

		/**
		 * @brief	incrementing postfix ++ operator
		 * @return 	copy of iterator before incrementain
		 */
		forward_iterator operator++(int) noexcept {
			forward_iterator tmp = *this;
			_current = _current->next();
			return tmp;
		}

		/**
		 * @brief	comparing != function
		 * @param fwi1 			const ref to forward_iterator
		 * @param fwi2 			const ref to forward_iterator
		 * @return	true if they current position are NOT equal, false otherwise
		 */
		friend bool operator!=(const forward_iterator& fwi1, const forward_iterator& fwi2) noexcept {
			return fwi1._current != fwi2._current;
		}

		/**
		 * @brief	comparing == function
		 * @param fwi1 			const ref to forward_iterator
		 * @param fwi2 			const ref to forward_iterator
		 * @return	true if their current positions are equal, false otherwise
		 */
		friend bool operator==(const forward_iterator& fwi1, const forward_iterator& fwi2) noexcept {
			return fwi1._current == fwi2._current;
		}
	}; // forward_iterator

} // namespace detail



/**
 * LinkedList
 */
template< typename T>
class LinkedList {
public:
	using Node = detail::Node<T>;
	using iterator = detail::forward_iterator<Node*, T>;
	using const_iterator = detail::forward_iterator<const Node*, const T>;

	/**
	 * @brief	default ctor
	 */
	LinkedList() = default;

	/**
	 * @brief 	parametric ctor
	 * 			constructs LinkedList containing element count times
	 * @param 	count 	number of elements inserted
	 * @param 	elem	element
	 * @return
	 */
	LinkedList(std::size_t count, const T& elem) {
		for (std::size_t i = 0; i < count; ++i)
			push_back(elem);
	}


	/**
	 * @brief	parametric ctor
	 * 			constructs LinkedList from iterators, until last is met
	 * 			doesn't check validity of iterators in any way
	 * 			can be constructed from any container using iterators
	 * @param 	node
	 */
	template <typename Iter>
	LinkedList(Iter first, Iter last) {
		for (; first != last; ++first) {
			push_back(*first);
		}
	}

	/**
	 * @brief	copy ctor
	 * @param 	ll		copied LinkedList
	 */
	LinkedList(const LinkedList &ll) {
		const auto* ptr = ll.first();
		while (ptr) {
			push_back(ptr->value());
			ptr = ptr->next();
		}
	}

	/**
	 * @brief	move ctor
	 * @param 	ll 		moved LinkedList
	 */
	LinkedList(LinkedList&& ll) noexcept : _first(std::move(ll._first)),
										   _last(ll._last), _size(ll._size) {
		ll._size = 0;
		ll._last = nullptr;
	}

	/**
	 * @brief	initializer_list ctor
	 * @param 	init 	initializer_list
	 */
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
		_size = ll._size;
		ll._size = 0;
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
	 * @brief	replaces content with count number of coppies of param elem
	 * @param 	count
	 * @param 	elem	element
	 */
	void assign(std::size_t count, const T& elem) {
		clear();
		for (std::size_t i = 0; i < count; ++i)
			push_back(elem);
	}

	/**
	 * @brief	replaces elements with contents of initializer_list
	 * @param 	init
	 */
	void assign(std::initializer_list<T> init) {
		clear();
		for (auto elem : init)
			push_back(std::move(elem));
	}

	/**
	 * @brief	empty state getter
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
	void erase(const T& val) noexcept {
		erase(find(val));
	}

	/**
	 * @brief	erases Node from LL
	 * @param 	n
	 */
	void erase(Node* n) noexcept {
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
			--_size;
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
				++_size;
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
		++_size;
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
		++_size;
	}

	/**
	 * @brief	pops element from back
	 * 			logic_error throw if the contaner is empty
	 * @return 	element from back
	 */
	T pop_back() {
		if (empty()) {
			throw std::logic_error(std::string("LinkedList<")
								   + typeid(T).name()
								   + ">: pop_back called on empty container");
		}

		--_size;
		if (first() == last()) {
			auto nod = std::move(_first);
			_last = nullptr;
			return std::move(nod->_value);
		}
		auto nod = std::move(_last->prev()->_next);
		_last = nod->prev();
		return std::move(nod->_value);
	}

	/**
	 * @brief	pops element from front
	 * 			logic error throw if the container is empty
	 * @return
	 */
	T pop_front() {
		if (empty()) {
			throw std::logic_error(std::string("LinkedList<")
								   + typeid(T).name()
								   + ">: pop_front called on empty container");
		}
		--_size;
		if (first() == last()) {
			auto nod = std::move(_first);
			_last = nullptr;
			return std::move(nod->_value);
		}
		auto nod = std::move(_first);
		_first = std::move(nod->_next);
		_first->prev(nullptr);
		return std::move(nod->_value);
	}

	/**
	 * @brief	clears container
	 */
	void clear() noexcept {
		_first.reset();
		_last = nullptr;
		_size = 0;
	}

	/**
	 * @brief	size getter
	 * @return 	size of the container
	 */
	std::size_t size() const noexcept {
		return _size;
	}

	////////////////////////////////////
	///		ITERATOR SECTION		///
	///////////////////////////////////

	/**
	 * @return 	iterator to first element
	 */
	iterator begin() {
		return iterator(first());
	}


	/**
	 * @return 	const_iterator to first element
	 */
	const_iterator begin() const {
		return const_iterator(first());
	}

	/**
	 * @return	const-iterator to first element
	 */
	const_iterator cbegin() const {
		return const_iterator(first());
	}

	/**
	 * @return 	iterator to nullptr (element after last element in container)
	 */
	iterator end() {
		return iterator(nullptr);
	}

	/**
	 * @return 	const_iterator to nullptr (element after last element in container)
	 */
	const_iterator end() const {
		return const_iterator(nullptr);
	}

	/**
	 * @return	const_iterator to nullptr (element after last element in container)
	 */
	const_iterator cend() const {
		return const_iterator(nullptr);
	}

private:

	std::unique_ptr<Node> _first;
	Node* _last = nullptr;
	std::size_t _size = 0;
};



/**
 * @brief	prints elements of LL into ostre
 * 			expects T is printable in any way
 * @param 	os		instance od ostream
 * @param 	ll		instance of LinkedList
 * @return	reference to ostream
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const LinkedList<T>& ll) noexcept {
	for (const auto& o : ll)
		os << o << " ";
	os << std::endl;
	return os;
}