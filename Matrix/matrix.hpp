#include <array>
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

public:
	Matrix() = default;
	Matrix(std::initializer_list<Type&&> list) {
		if (list.size() != size())
			throw std::logic_error("Wrong number of arguments given.");
		for (size_type i = 0; i < list.size(); ++i)
			_elements[i] = std::move(list[i]);
	}

	Matrix(const Matrix&);
	Matrix(Matrix&&);

	size_type size() const noexcept { return _elements.size(); }
	size_type width() const noexcept { return _width; }
	size_type height() const noexcept { return _height; }

	reference operator()(size_type x, size_type y) noexcept { return _elements[x + y * _width]; }
	const_reference operator()(size_type x, size_type y) const noexcept { return _elements[x + y * _width]; }

	reference at(size_type x, size_type y) {
		if (x >= _width || y >= _height)
			throw std::out_of_range("Given index is outside the matrix boundaries.");
		return _elements.at(x + y * _width);
	}

	const_reference at(size_type x, size_type y) const {
		if (x >= _width || y >= _height)
			throw std::out_of_range("Given index is outside the matrix boundaries.");
		return _elements.at(x + y * _width);
	}

private:
	std::array<Type, _width * _height> _elements = std::array<Type, _width * _height>();
	//std::vector<std::vector<Type> > _elements = std::vector<std::vector<Type> >(_height, std::vector<Type>(_width)) ;
};

#endif //MATRIX_MATRIX_HPP
