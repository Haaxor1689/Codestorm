#include <cstddef>
#include <memory>
#include <array>
#include <cassert>
#include <vector>

#ifndef MATRIX_MATRIX_HPP
#define MATRIX_MATRIX_HPP

template <typename Type, std::size_t _height, std::size_t _width>
class Matrix {
	using reference = Type&;
	using const_reference = const Type&;
	using pointer = Type*;
	using const_pointer = const Type*;
	using size_type = std::size_t;
protected:
	class Row {
	public:
		Row(size_type col, const Matrix& m) {
			assert(col < _height);
			for (size_type i = 0; i < _width; ++i) {
				_elems[i] = const_cast<pointer>(&(m._elements[i].operator[](col)));
			}
		}

		reference operator[](size_type col) noexcept {
			return *_elems[col];
		}
		const_reference operator[](size_type col) const noexcept {
			return *_elems[col];
		}
	private:
		std::array<pointer, _width> _elems;
	};

public:

	Matrix() = default;
	Matrix(const Matrix&);
	Matrix(Matrix&&);
	~Matrix() = default;

	Row operator[](size_type row) noexcept {
		return Row(row, const_cast<const Matrix&>(*this));
	}
	const Row operator[](size_type row) const noexcept {
		return Row(row, *this);
	}

	reference at(size_type x, size_type y) {
		if (x >= _width || y >= _height)
			throw std::out_of_range("Given index is outside the matrix boundaries.");
		return *this[x][y];
	}

	const_reference at(size_type x, size_type y) const {
		if (x >= _width || y >= _height)
			throw std::out_of_range("Given index is outside the matrix boundaries.");
		return *this[x][y];
	}

private:
	std::vector<std::vector<Type> > _elements = std::vector<std::vector<Type> >(_width, std::vector<Type>(_height));
};

#endif //MATRIX_MATRIX_HPP
