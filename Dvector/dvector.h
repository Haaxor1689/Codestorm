#pragma once

#include <iterator>
#include <initializer_list>
#include <memory>
#include <type_traits>

// Several nice utilities for using uninitialised storage are available in C++17
// but not yet in most clang/gcc implementations (they are in LLVM 4.0 on FI
// computers), so they are provided also by this header.
#include "cxx17memory"

/**
 *  implementation of bidirectional vector:
 * 		provides fast insertion both ways
 * 		works similarily as std::dequeue
 */
template< typename T, typename Allocator = std::allocator< T > >
class DVector {
	enum class at {
		front,
		back
	}; // enum class at

	template <typename _T>
	class Iterator;
public:
	using value_type = T;
	using pointer = T*;
	using reference = T&;
	using const_reference = const T&;
	using iterator = Iterator<T*>;
	using const_iterator = Iterator<const T*>;


    /**
     * @brief	constructs default Dvector
     * 			does not allocate any space
     */
    DVector() = default;

	/**
	 * @brief	constructs Dvector from initializer_list
	 * 			allocates necessary space only once (does not reallocate)
	 * @param 	ilist 		initializer_list of elements to be inserted
	 */
    DVector(std::initializer_list<T> ilist) {
        reserve(ilist.size());
        for (const auto& elem : ilist) {
            push_back(std::move(elem));
        }
    }

	/**
	 * @brief	constructs Dvector from range
	 * 			should work with any iterator type
	 * 			doesn't check whether iterators are valid in any way
	 * 			might reallocate (reserves only for capacity of 10)
	 * @param 	begin		iterator to beginning of range
	 * @param 	end			iterator to end of range
	 */
    template< typename It >
    DVector(It begin, It end) {
        reserve(10);
        for (; begin != end; ++begin) {
            push_back(*begin);
        }
    }

	/**
	 * @brief	destructs Dvector
	 */
    ~DVector() {
		_deallocate();
	}

	/**
	 * @brief	constructs Dvector as copy of other Dvector
	 * @param 	o 			the other Dvector
	 */
    DVector(const DVector& o) {
        reserve(o.size());
        for (const auto& elem : o)
            push_back(elem);
    }

	/**
	 * @brief	constructs Dvector by moving Buffer of the other Dvector
	 * 			after move iterators remain valid
	 * @param 	o 			the other Dvector
	 */
    DVector(DVector&& o) noexcept
			: DVector() {
		swap(o);
	}

	/**
	 * @brief	copy-assignment operator
	 * 			uses copy and swap idiom
	 * 			does not reallocate storage in the process
	 * @param 	o			the other Dvector
	 * @return 	reference to this instance
	 */
    DVector& operator=(const DVector& o) {
		DVector tmp = o;
		swap(tmp);
		return *this;
	}

	/**
	 * @brief	move-assignment operator
	 * 			moves Buffer of the other Dvector
	 * 			this move deallocates memory owned by this instance
	 * @param 	o			the other Dvector
	 * @return	reference to this instance
	 */
    DVector& operator=(DVector&& o) noexcept {
		_deallocate();
		_storage_begin = o._storage_begin;
		_storage_end = o._storage_end;
		_begin = o._begin;
		_end = o._end;
		_capacity = o._capacity;
		_size = o._size;
		o._storage_begin = nullptr;
		o._storage_end = nullptr;
		o._begin = nullptr;
		o._end = nullptr;
		o._capacity = 0;
		o._size = 0;
		return *this;
	}

	/**
	 * @brief	Allocator getetr
	 * @return 	instance of the allocator type used by this class
	 */
    Allocator get_allocator() {
        return Allocator();
    }

	/**
	 * @brief	pushes value to the back of Dvector
	 * 			can invladite all iterators if reallocation takes place
	 * @param 	val			value
	 */
    void push_back(const_reference val) {
        _push(val, at::back);
    }

	/**
	 * @brief	move overload of push_back(const_reference)
	 */
    void push_back(T&& val){
        _push(std::move(val), at::back);
    }

	/**
	 * @brief	pushes value to thefront of Dvector
	 * 			can invladite all iterators if reallocation takes place
	 * @param 	val
	 */
    void push_front(const_reference val) {
        _push(val, at::front);
    }
	/**
	 * @brief	move overload of push_front(const_reference)
	 */
    void push_front(T&& val) {
        _push(std::move(val), at::front);
    }

	/**
	 * @brief	gets first element within the Dvector
	 * 			pmly callable on non-empty Dvector
	 * @throw	std::runtime_error if Dvector is empty
	 * @return	reference to the first element
	 */
    reference front() {
		_check();
        return *(begin());
    }

	/**
	 * @brief	const overload of front()
	 * 			only callable on non-empty Dvector
	 * @throw	std::runtime_error if Dvector is empty
	 * @return	const_reference to the first element
	 */
    const_reference front() const {
		_check();
        return *(begin());
    }

	/**
	 * @brief	gets last element within the Dvector
	 * 			only callable on non=empty Dvector
	 * @throw	std::runtime_error if Dvector is empty
	 * @return 	reference to the last element
	 */
    reference back() {
		_check();
        return *(--end());
    }

	/**
	 * @brief	const overload of back()
	 * 			only callable on non-empty Dvector
	 * @throw	std::runtime_error if Dvector is empty
	 * @return 	const_reference to the last element
	 */
    const_reference back() const {
		_check();
        return *(--end());
    }

	/**
	 * @brief	removes element from front
	 * 			invalidates beginning iterators
	 * 			other iterators remain valid
	 * @throw	std::runtime_error if Dvector is empty
	 */
    void pop_front() {
		_check();
        _pop(at::front);
    }

	/**
	 * @brief	removes element from the back
	 * 			invalidates past-the-end and (former) last element iterator
	 * 			other iterators remain valid
	 * @throw	std::runtime_error if Dvector is empty
	 */
    void pop_back() {
		_check();
        _pop(at::back);
    }

	/**
	 * @brief	check whether Buffer is empty or not
	 * @return	true if Dvector is empty, false otherwise
	 */
    bool empty() const {
        return size() == 0;
    }

	/**
	 * @brief	capacity getter
	 * @return 	capacity of Buffer
	 */
    std::size_t capacity() const {
        return _capacity;
    }

	/**
	 * @brief	size getter
	 * @return 	number of elements stored in Buffer
	 */
    std::size_t size() const {
        return _size;
    }

	/**
	 * @brief	clears Buffer
	 * 			size is changed to 0
	 * 			invalidates all iterators
	 */
    void clear() {
		if (_begin > _end) {
			std::destroy(_begin, _storage_end);
			std::destroy(_storage_begin, _end);
		} else {
			std::destroy(_begin, _end);
		}
	}

	/**
	 * @brief	changes size to n
	 * 			destructs possible excessive elements
	 * 			can create new elements
	 * 			can invalidate all iterator if n > capacity()
	 * @param 	n			new size
	 */
    void resize(std::size_t n) {
		if (n > _capacity)
			_reallocate(n);
		uint64_t diff = n - _size;

		if (diff > 0) { //  n is greater than size - creating elements
			std::uninitialized_value_construct(_end, _end + diff);
			_end += diff;
		} else {        //  n is lesser than size - removing elements from back
			for (; diff != 0; ++diff)
				pop_back();
		}
    }

	/**
	 * @brief	makes capacity at least n
	 * 			does not truncate the Dvector
	 * 			does not construct any new elements
	 * @param 	n			new capacity
	 */
    void reserve(std::size_t n) {
		if (n > _capacity)
			_reallocate(n);
    }

	/**
	 * @brief	swaps with the other Dvector
	 * 			uses move-assigment operator
	 * @param 	o 			the other Dvector
	 */
    void swap(DVector& o) {
		using std::swap;
		swap(_storage_begin, o._storage_begin);
		swap(_storage_end, o._storage_end);
		swap(_begin, o._begin);
		swap(_end, o._end);
		swap(_capacity, o._capacity);
		swap(_size, o._size);
	}

	/**
	 * @return 	iterator to beginning
	 */
    iterator begin() {
        return iterator(_begin, _storage_begin, _storage_end);
    }

	/**
	 * @return 	const_iterator to beginning
	 */
    const_iterator begin() const {
        return const_iterator(_begin, _storage_begin, _storage_end);
    }

	/**
	 * @return 	const_iterator to beginning
	 */
    const_iterator cbegin() const {
        return begin();
    }

	/**
	 * @return 	iterator to end
	 */
    iterator end() {
        return iterator(_end, _storage_begin, _storage_end);
    }

	/**
	 * @return 	const_iterator to end
	 */
    const_iterator end() const {
        return const_iterator(_end, _storage_begin, _storage_end);
    }

	/**
	 * @return 	const_iterator to end
	 */
    const_iterator cend() const {
        return end();
    }

	/**
	 * @brief	access element in ix'th position
	 * 			does no checks whatsoever
	 * @param 	ix 		position
	 * @return 	reference to ix'th element
	 */
    reference operator[](size_t ix) {
        return _reference(ix);
    }

	/**
	 * @brief	access element in ix'th position
	 * 			does no checks whatsoever
	 * @param 	ix 		position
	 * @return 	const_reference to ix'th element
	 */
    const_reference operator[](size_t ix) const {
        return _reference(ix);
    }

private:
	pointer _storage_begin = nullptr;
	pointer _storage_end   = nullptr;

	pointer _begin = nullptr;
	pointer _end   = nullptr;

	std::size_t _capacity = 0;
	std::size_t _size     = 0;


	/**
	 * @brief	reallocates storage
	 * 			prefers moving elements instead of copying them
	 * @param 	n
	 */
	void _reallocate(std::size_t n) {
		pointer tmp_s_begin = Allocator().allocate(n + 1);
		if (_storage_begin) {
			if (_begin > _end) {
				std::size_t offset = _storage_end - _begin;
				std::uninitialized_move(_storage_end - offset, _storage_end, tmp_s_begin);
				std::uninitialized_move(_storage_begin, _end, tmp_s_begin + offset);
			} else {
				std::uninitialized_move(_begin, _end, tmp_s_begin);
			}
			_deallocate();
		}
		_storage_begin = tmp_s_begin;
		_storage_end = _storage_begin + n + 1;
		_begin = _storage_begin;
		_end = _begin + _size;
		_capacity = n;
	}

	/**
	 * @brief	unified push function
	 * 			can push elements to the front or to the back
	 * 			reallocates storage if Buffer is full before insertion
	 * 			can invalidate all iterators
	 * 			begin or end iterator are always invalidated depending
	 * 			on direction of push
	 * @param 	value
	 * @param 	where			front/back
	 */
	template <typename _T>
	void _push(_T&& value, at where) {
		static_assert(std::is_same<_T, const T&>()
					  || std::is_same<typename std::add_rvalue_reference<_T>::type, T&&>());

		if (_size == _capacity)
			_reallocate(!_capacity ? 10 : _capacity * 2);
		if (where == at::front) {
			if (_begin == _storage_begin)
				_begin = _storage_end;
			_create(--_begin, std::forward<_T>(value), std::is_lvalue_reference<_T>());
		} else {
			if (_end == _storage_end)
				_end = _storage_begin;
			_create(_end++, std::forward<_T>(value), std::is_lvalue_reference<_T>());
		}
		++_size;
	}

	/**
	 * @brief	creates element at provided adress via copy constructor
	 * @param 	ptr
	 * @param 	value
	 */
	template <typename _T>
	void _create(pointer ptr, const _T& value, std::true_type /*is lvalue ref*/) {
		new (ptr) T(value);
	}

	/**
	 * @brief	creates element at provided adress via move constructor
	 * @param	 ptr
	 * @param	 value
	 */
	template <typename _T>
	void _create(pointer ptr, _T&& value, std::false_type/*is rvalue ref*/) {
		new (ptr) T(std::move(value));
	}

	/**
	 * @brief	unified pop function
	 * 			can remove element from front or back
	 * 			invalidates begin or iterators depending on
	 * 			direction of removal
	 * @param 	where			front/back
	 */
	void _pop(at where) {
		if (where == at::front) {
			std::destroy_at(_begin);
			if (++_begin == _storage_end)
				_begin = _storage_begin;
		} else {
			if (_end == _storage_begin)
				_end = _storage_end;
			std::destroy_at(--_end);
		}
		--_size;
	}

	/**
	 * @brief	function finding position of x'th element within the Buffer
	 * 			function does not check if x is out of bounds thus
	 * 			SIGSEGV might happen if used incorrectly
	 * @param 	x
	 * @return 	reference to element
	 */
	reference _reference(std::size_t x) const {
		if (_begin > _end && _begin + x >= _storage_end) {
			return *(_storage_begin + (x - (_storage_end - _begin)));
		}
		return *(_begin + x);
	}

	/**
	 * @brief	clears Buffer and deallocates storage owned by Buffer
	 */
	void _deallocate() {
		if (_storage_begin) {
			clear();
			Allocator().deallocate(_storage_begin, _capacity + 1);
		}
	}


	/**
	 * @brief	function used by many methods
	 * 			checks if Dvector is empty, whi is invalid state
	 * 			to call some of the Dvector's methods
	 * @throw	std::runtime_error if Dvector is empty
	 */
	void _check() const {
		if (empty())
			throw std::runtime_error("vector is empty");
	}

	/**
	 * 	bidirectional iterator used by Buffer:
	 * 		Iterator needs to "move" on cyclical interpretation
	 * 		of Buffer - pointer to Buffer is providied to Iterator via constructors
	 */
	template <typename _T>
	class Iterator : public std::iterator<std::bidirectional_iterator_tag, typename std::iterator_traits<_T>::value_type > {
		using traits = std::iterator_traits<_T>;
	public:
		using value_type = typename traits::value_type;
		using reference  = typename traits::reference;
		using pointer    = typename traits::pointer;

		/**
		 * @brief	constructs default Iterator
		 */
		Iterator() = default;

		/**
		 * @brief	constructs Iterator with provided parameters
		 * @param 	base
		 * @param 	buf				pointer to Buffer
		 */
		Iterator(_T base, _T const bgn, _T const end)
				: _current(base),
				  _storage_begin(bgn),
				  _storage_end(end) {}

		/**
		 * @brief	constructs Iterator as copy the other Iterator
		 * @param 	it				the other Iterator
		 */
		Iterator(const Iterator& it)
				: _current(it._current),
				  _storage_begin(it._storage_begin),
				  _storage_end(it._storage_end) {}

		/**
		 * @brief	dereferencing operator
		 * @return 	reference to the object Iterator is pointing to
		 */
		reference operator*() const {
			return *_current;
		}

		/**
		 * @brief	arrow operator
		 * @return 	pointer to the object Iterator is pointing to
		 */
		pointer operator->() const {
			return _current;
		}

		/**
		 * @brief	incrementation prefix operator
		 * @return 	reference to this instance after incrementation
		 */
		Iterator& operator++() {
			++_current;
			if (_current >= _storage_end)
				_current = _storage_begin;
			return *this;
		}

		/**
		 * @brief	incrementation sufix operator
		 * @return	copy of this instance before incrementation took place
		 */
		Iterator operator++(int) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		/**
		 * @brief	decrementation prefix operator
		 * @return 	reference to this instance after decrementation
		 */
		Iterator& operator--() {
			--_current;
			if (_current < _storage_begin)
				_current = _storage_end - 1;
			return *this;
		}

		/**
		 * @brief	decrementation suffix operator
		 * @return 	copy of this instance before decremeantation took place
		 */
		Iterator operator--(int) {
			Iterator tmp = *this;
			--(*this);
			return tmp;
		}

		/**
		 * @brief	comparing functions
		 * @param 	it				the other Iterator
		 * @return	result of comparison
		 */
		bool operator==(const Iterator& it) const {
			return _current == it._current;
		}
		bool operator!=(const Iterator& it) const {
			return !(*this == it);
		}

	private:
		_T _current = nullptr;
		_T _storage_begin = nullptr;
		_T _storage_end   = nullptr;
	};
}; // Dvector

