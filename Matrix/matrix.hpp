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
		Row(size_type num, const Matrix& m) {
			assert(num < _height);
			for (size_type i = 0; i < _width; ++i) {
				_elems[i] = const_cast<pointer>(&(m._elements[i].operator[](num)));
			}
		}

		reference operator[](size_type pos) {
			return *_elems[pos];
		}
		const_reference operator[](size_type pos) const {
			return *_elems[pos];
		}
	private:
		std::array<pointer, _width> _elems;
	};

public:

	Matrix() = default;
	Matrix(const Matrix&);
	Matrix(Matrix&&);
	~Matrix() = default;

	Row operator[](size_type row_num) {
		return Row(row_num, const_cast<const Matrix&>(*this));
	}
	const Row operator[](size_type row_num) const {
		return Row(row_num, *this);
	}

private:
	std::vector<std::vector<Type> > _elements = std::vector<std::vector<Type> >(_width, std::vector<Type>(_height));
};

#endif //MATRIX_MATRIX_HPP
