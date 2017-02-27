#include <cstddef>
#include <memory>
#include <array>
#include <cassert>
#include <vector>

#ifndef MATRIX_MATRIX_HPP
#define MATRIX_MATRIX_HPP

template <typename Type, std::size_t _width, std::size_t _height>
class Matrix {
	using reference = Type&;
	using const_reference = const Type&;
	using pointer = Type*;
	using const_pointer = const Type*;
	using size_type = std::size_t;
protected:

public:

	Matrix() = default;
	Matrix(const Matrix&);
	Matrix(Matrix&&);

	reference at(size_type x, size_type y) {
		if (x >= _width || y >= _height)
			throw std::out_of_range("Given index is outside the matrix boundaries.");
		return _elements.at(y).at(x);
	}

	const_reference at(size_type x, size_type y) const {
		if (x >= _width || y >= _height)
			throw std::out_of_range("Given index is outside the matrix boundaries.");
		return _elements.at(y).at(x);
	}

private:
	std::vector<std::vector<Type> > _elements = std::vector<std::vector<Type> >(_height, std::vector<Type>(_width)) ;
};

#endif //MATRIX_MATRIX_HPP
