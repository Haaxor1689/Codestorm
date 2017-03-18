#pragma once

#include <iterator>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <string>
#include <iostream>
#include <initializer_list>

// A double-ended vector implemented as contiguous memory round buffer which
// reallocates when full. While providing similar interface to std::deque
// (amortized constant-time insert and remove to both ends and constant-time
// indexing) it has more compact memory footprint, but requires elements to be
// moveable and does not have stable iterators or references.
//
// Similarly to std::vector it keeps a reserve memory and grows only when this
// is exhausted. It should grow to twice the previous capacity to ensure
// amortized constant time insertion.
//
// The memory that does not contain actual data should not contain constructed
// objects, it should be uninitialised.  Uninitialized storage can be obtained
// from allocator (using allocate and deallocate).
//
// Several nice utilities for using uninitialised storage are available in C++17
// but not yet in most clang/gcc implementations (they are in LLVM 4.0 on FI
// computers), so they are provided also by this header.
// See http://en.cppreference.com/w/cpp/header/memory#Uninitialized_storage for
// documentation.

#include "cxx17memory"

template <typename T, typename Allocator>
class Buffer;

/**
 *  implementation of bidirectional vector:
 * 		provides fast insertion both ways
 * 		works similarily as std::dequeue
 */
template< typename T, typename Allocator = std::allocator< T > >
class DVector {
public:
    using buffer_type = Buffer<T, Allocator>;
    using value_type = typename buffer_type::value_type;
    using reference = typename buffer_type::reference;
    using const_reference = typename buffer_type::const_reference;
    using pointer = typename buffer_type::pointer;
    using iterator = typename buffer_type::iterator;
    using const_iterator = typename buffer_type::const_iterator;


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
    ~DVector() = default;

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
            : _buffer(std::move(o._buffer)) {}

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
		clear();
		_buffer = std::move(o._buffer);
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
        _buffer.push_back(val);
    }

	/**
	 * @brief	move overload of push_back(const_reference)
	 */
    void push_back(T&& val){
        _buffer.push_back(std::move(val));
    }

	/**
	 * @brief	pushes value to thefront of Dvector
	 * 			can invladite all iterators if reallocation takes place
	 * @param 	val
	 */
    void push_front(const_reference val) {
        _buffer.push_front(val);
    }
	/**
	 * @brief	move overload of push_front(const_reference)
	 */
    void push_front(T&& val) {
        _buffer.push_front(std::move(val));
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
        _buffer.pop_front();
    }

	/**
	 * @brief	removes element from the back
	 * 			invalidates past-the-end and (former) last element iterator
	 * 			other iterators remain valid
	 * @throw	std::runtime_error if Dvector is empty
	 */
    void pop_back() {
		_check();
        _buffer.pop_back();
    }

	/**
	 * @brief	check whether Buffer is empty or not
	 * @return	true if Dvector is empty, false otherwise
	 */
    bool empty() const {
        return _buffer.empty();
    }

	/**
	 * @brief	capacity getter
	 * @return 	capacity of Buffer
	 */
    std::size_t capacity() const {
        return _buffer.capacity();
    }

	/**
	 * @brief	size getter
	 * @return 	number of elements stored in Buffer
	 */
    std::size_t size() const {
        return _buffer.size();
    }

	/**
	 * @brief	clears Buffer
	 * 			size is changed to 0
	 * 			invalidates all iterators
	 */
    void clear() {
		_buffer.clear();
	}

	/**
	 * @brief	changes size to n
	 * 			destructs possible excessive elements
	 * 			can create new elements
	 * 			can invalidate all iterator if n > capacity()
	 * @param 	n			new size
	 */
    void resize(std::size_t n) {
        _buffer.resize(n);
    }

	/**
	 * @brief	makes capacity at least n
	 * 			does not truncate the Dvector
	 * 			does not construct any new elements
	 * @param 	n			new capacity
	 */
    void reserve(std::size_t n) {
        _buffer.reserve(n);
    }

	/**
	 * @brief	swaps with the other Dvector
	 * 			uses move-assigment operator
	 * @param 	o 			the other Dvector
	 */
    void swap(DVector& o) {
		DVector tmp = std::move(*this);
		*this = std::move(o);
		o = std::move(tmp);
	}

	/**
	 * @return 	iterator to beginning
	 */
    iterator begin() {
        return _buffer.begin();
    }

	/**
	 * @return 	const_iterator to beginning
	 */
    const_iterator begin() const {
        return _buffer.begin();
    }

	/**
	 * @return 	const_iterator to beginning
	 */
    const_iterator cbegin() const {
        return _buffer.begin();
    }

	/**
	 * @return 	iterator to end
	 */
    iterator end() {
        return _buffer.end();
    }

	/**
	 * @return 	const_iterator to end
	 */
    const_iterator end() const {
        return _buffer.end();
    }

	/**
	 * @return 	const_iterator to end
	 */
    const_iterator cend() const {
        return _buffer.end();
    }

	/**
	 * @brief	access element in ix'th position
	 * 			does no checks whatsoever
	 * @param 	ix 		position
	 * @return 	reference to ix'th element
	 */
    reference operator[](size_t ix) {
        return _buffer[ix];
    }

	/**
	 * @brief	access element in ix'th position
	 * 			does no checks whatsoever
	 * @param 	ix 		position
	 * @return 	const_reference to ix'th element
	 */
    const_reference operator[](size_t ix) const {
        return _buffer[ix];
    }

private:
    buffer_type _buffer;

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
}; // Dvector


/**
 *  implementation of circular buffer:
 * 		treats linear storage as circular
 * 		built on RAII principles:
 * 			allocation and deallocation take place internally
 * 			before deallocation destructors are called
 * 	NOTE: actual storage capacity is always one block more
 * 	than reported capacity to ensure easier implementation
 * 	of iterators
 */
template <typename T, typename Allocator>
class Buffer {
private:
	/**
	 * @brief	enum class used internally with private methods
	 */
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
	 * @brief	constructs default Buffer
	 */
    Buffer() = default;

	/**
	 * @brief	constructs buffer by moving resources from the other Buffer
	 * @param 	buf			the other Buffer
	 */
    Buffer(Buffer&& buf) noexcept
            : _storage_begin(buf._storage_begin),
              _storage_end(buf._storage_end),
              _begin(buf._begin),
              _end(buf._end),
              _capacity(buf._capacity),
              _size(buf._size) {
        buf._storage_begin = nullptr;
        buf._storage_end = nullptr;
        buf._begin = nullptr;
        buf._end = nullptr;
        buf._capacity = 0;
        buf._size = 0;
    }

	/**
	 * @brief	move-assigment operator
	 * 			moves resources to Buffer
	 * 			before moving deallocates previously allocated storage
	 * @param 	buf				the other Buffer
	 * @return 	reference
	 */
	Buffer& operator=(Buffer&& buf) noexcept {
		_deallocate();
		_storage_begin = buf._storage_begin;
		_storage_end = buf._storage_end;
		_begin = buf._begin;
		_end = buf._end;
		_capacity = buf._capacity;
		_size = buf._size;
		buf._storage_begin = nullptr;
		buf._storage_end = nullptr;
		buf._begin = nullptr;
		buf._end = nullptr;
		buf._capacity = 0;
		buf._size = 0;
		return *this;
	}

	/**
	 * @brief	destructs stored elements and deallocates allocated storage
	 */
	~Buffer() {
       _deallocate();
    }

	/**
	 * @brief	checks whether Buffer is empty or not
	 * @return 	true is Buffer is empty, false otherwise
	 */
	bool empty() const {
		return _size == 0;
	}

	/**
	 * @brief	size getter
	 * @return 	number of stored elements within buffer
	 */
    std::size_t size() const {
        return _size;
    }

	/**
	 * @brief	capacity getter
	 * @return  number of elements that can be stored before reallocation is needed
	 */
    std::size_t capacity() const {
        return _capacity;
    }

	/**
	 * @brief	pushes value to the back
	 * @param 	value
	 */
    template <typename _T>
    void push_back(_T&& value) {
        _push(std::forward<_T>(value), at::back);
    }

	/**
	 * @brief	pushes value to the front
	 * @param 	value
	 */
    template <typename _T>
    void push_front(_T&& value) {
        _push(std::forward<_T>(value), at::front);
    }

	/**
	 * @brief	destroys value at the end
	 * 			function does not check if Buffer is empty or not
	 */
    void pop_back() {
        _pop(at::back);
    }

	/**
	 * @brief	destroys value at the beginning
	 * 			function does not check if Buffer is empty or not
	 */
    void pop_front() {
        _pop(at::front);
    }

	/**
	 * @brief	random access bracket operator
	 * 			function does not check if x is out of bounds
	 * @param 	x 			position
	 * @return 	reference to x'th element of vector
	 */
    reference operator[](std::size_t x) {
        return _reference(x);
    }

	/**
	 * @brief	const random access bracket operator
	 * 			function does not check if x is out of bounds
	 * @param 	x 			position
	 * @return 	const_reference to x'th element of vector
	 */
    const_reference operator[](std::size_t x) const {
        return _reference(x);
    }

	/**
	 * @brief	resizes Buffer
	 * 			does not reallocate storage
	 * 			constructs new elements if n > size()
	 * 			destorys elements from back if n < size()
	 * @param 	n			new size of Buffer
	 */
    void resize(std::size_t n) {
        if (n > _capacity)
            _reallocate(n);
        long diff = n - _size; // NOLINT
        if (diff > 0) { //  n is greater than size - creating elements
            std::uninitialized_value_construct(_end, _end + diff);
			_end += diff;
        } else {        //  n is lesser than size - removing elements from back
            for (; diff != 0; ++diff)
                pop_back();
        }
    }

	/**
	 * @brief	makes Buffer capacity to be at least n
	 * @param 	n
	 */
    void reserve(std::size_t n) {
        if (n > _capacity)
            _reallocate(n);
    }

	/**
	 * @brief	destroys all objects within Buffer
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
	 * @return	iterator to beginning
	 */
    iterator begin() {
        return iterator(_begin, this);
    }

	/**
	 * @return	const_iterator to beginning
	 */
    const_iterator begin() const {
        return const_iterator(_begin, this);
    }

	/**
	 * @return 	iterator to end
	 */
    iterator end() {
        return iterator(_end, this);
    }

	/**
	 * @return 	const_iterator to end
	 */
    const_iterator end() const {
        return const_iterator(_end, this);
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
        Iterator(_T base, const Buffer* buf)
                : _current(base), _buf(buf) {}

		/**
		 * @brief	constructs Iterator as copy the other Iterator
		 * @param 	it				the other Iterator
		 */
        Iterator(const Iterator& it)
                : _current(it._current), _buf(it._buf) {}

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
            if (_buf->_begin > _buf->_end && _current >= _buf->_storage_end)
                _current = _buf->_storage_begin;
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
            if (_buf->_begin > _buf->_end && _current < _buf->_storage_begin)
                _current = _buf->_storage_end - 1;
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
        const Buffer* _buf = nullptr;
    };
}; // Buffer
