#include <cstddef>
#include <memory>
#include <array>

#ifndef MATRIX_MATRIX_HPP
#define MATRIX_MATRIX_HPP

template <typename Type, std::size_t x, std::size_t y, typename Alloc = std::allocator<Type> >
class Matrix {
public:
	using reference = Type&;
	using const_reference = const Type&;
	using pointer = Type*;
	using const_pointer = const Type*;
	using size_type = std::size_t;

	Matrix();
	Matrix(const Matrix&);
	Matrix(Matrix&&);

	reference operator[](size_type);
	const_reference operator[](size_type) const;
protected:
	class Row {
	public:
		Row(pointer const Obj);

		reference operator[](size_type);
		const_reference operator[](size_type) const;
	private:
		std::array<reference, x> _elements;
	};

private:
	using columns = std::array<Type, y>;

	std::array<columns, x>* _elems;
};

#endif //MATRIX_MATRIX_HPP
