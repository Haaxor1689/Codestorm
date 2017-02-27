#include <iostream>
#include "matrix.hpp"

int main() {
	Matrix<int, 3, 6> m;
	m.at(2, 3) = 100;
	std::cout << "Hello, World!" << std::endl;
	return 0;
}
