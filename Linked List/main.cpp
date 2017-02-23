#include <iostream>
#include "linkedlist.h"

class A {
public:
	A() = delete;
	A(int) {}
};

int main() {
	LinkedList<A> l;
	l.push_front(-1);
	l.push_back(-1);
	for (auto i = 0U; i < 400; ++i) {
		if (i < 100)
			l.push_back(i);
		if (i >= 100 && i < 200)
			l.push_front(i);
//		if (i >= 200 && i < 300)
//			l.insert_before(l.find(i % 50), i);
//		if (i >= 300 && i < 400)
//			l.insert_after(l.find(i % 50), i);
	}
//	l.erase(5);
//	l.insert_after(l.find(6), 5);
//	std::cout << l;
	return 0;
}