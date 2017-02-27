#include <iostream>
#include "matrix.hpp"

struct P {
public:
	P() = default;
	P(const P&) = default;
	P(P&&) = delete;

private:
	unsigned x = 0;
};

template <typename Type, size_t size>
class Vec {
public:
	Vec(std::array<Type, size>& data) : data(data) {}
	Vec(const Vec& other) = default;
	Vec(Vec&& other) = default;
	virtual ~Vec() = default;

	virtual Type& operator[](size_t pos) noexcept { return data[pos]; }
	virtual const Type& operator[](size_t pos) const noexcept { return data[pos]; }

protected:
	std::array<Type, size> data;
};

template <typename Type>
class Position : public Vec<Type, 4> {
public:
	Position(Type x, Type y, Type z, Type w = 1)
			: Vec<Type, 4>({x, y, z, w}),
			  x(this->data[0]),
			  y(this->data[1]),
			  z(this->data[2]),
			  w(this->data[3]) {}

	Type& x, y, z, w;
};

template <typename Type, size_t size>
class Color : public Vec<Type, 4> {
	Color(Type r, Type g, Type b, Type a = 1)
			: Vec<Type, 4>({ r, g, b, a }),
			  r(this->data[0]),
			  g(this->data[1]),
			  b(this->data[2]),
			  a(this->data[3]) {}

	Type& r, g, b, a;
};

int main() {
	Matrix<int, 3, 6> m;
	Matrix<int, 1, 1> n = { 69 };
//	Matrix<P, 2, 2> o = { P(), P(), P(), P()};
	Matrix<unsigned, 3, 3> p;
	p = { 1, 0, 0,
		  0, 1, 0,
		  0, 0, 1 };
	p.at(2, 2) = 100;
	const decltype(p)& r = p;
	r.at(2,2);
	r.swap(p);
	std::cout << "Hello, World!" << std::endl;
	return 0;
}
