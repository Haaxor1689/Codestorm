#include <iostream>
#include "matrix.hpp"

int main() {
	Matrix<int, 3, 6> m;
	const decltype(m)& cm = m;
	cm[0][5] = 100;
	std::cout << "Hello, World!" << std::endl;
	return 0;
}
