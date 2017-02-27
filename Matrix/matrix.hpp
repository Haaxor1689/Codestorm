#include <cstddef>
#include <memory>
#include <array>

#ifndef MATRIX_MATRIX_HPP
#define MATRIX_MATRIX_HPP

template <typename Type, std::size_t x, std::size_t y, typename Alloc = std::allocator<Type> >
class Matrix {
	using reference = Type&;
	using const_reference = const Type&;
	using pointer = Type*;
	using const_pointer = const Type*;
	using size_type = std::size_t;
protected:
	class Row {
	public:
		Row(pointer const Obj);

		reference operator[](size_type);
		const_reference operator[](size_type) const;
	private:
		std::array<reference, x> _elements;
	};
public:
	using row = Row;
	using const_row = const Row;

	Matrix();
	Matrix(const Matrix&);
	Matrix(Matrix&&);

	row operator[](size_type);
	const_row operator[](size_type) const;

private:
	using columns = std::array<Type, y>;

	std::array<columns, x>* _elems;
};

#endif //MATRIX_MATRIX_HPP
