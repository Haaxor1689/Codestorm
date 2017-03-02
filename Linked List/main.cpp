#include <iostream>
#include <cassert>
#include <fstream>
#include <vector>
#include "linkedlist.h"

//class A {
//public:
//	A() = delete;
//	A(int) {}
//};
//
//class SuperStorm {
//public:
//	SuperStorm() {
//		std::cout << "default\n";
//	}
//	SuperStorm(const SuperStorm&) {
//		std::cout << "cpy\n";
//	}
//	SuperStorm(SuperStorm&&) {
//		std::cout << "mov\n";
//	}
//};



class A {
public:
	A() : attr(0) {
		std::cout << "def ctor\n";
	}

	A(int v) : attr(v) {}
	A(const A& a) : attr(a.attr) {
		std::cout << "cpy ctor\n";
	}

	A(A&& a) : attr(a.attr) {
		a.attr = 0;
		std::cout << "mov ctor\n";
	}

	~A() {
		attr = -1;
	}

private:
	int attr;
};

class B {
public:
	B(const A& ar = A()) : a(std::move(ar)) {}
private:
	A a;
};



template <typename T>
class TD;

int main() {
	/*
	LinkedList<unsigned> ll;
	{
		ll.clear();
		/// push_back
		std::cout << "push_back" << "\n";
		for (auto i = 0U; i < 10; ++i) {
			ll.push_back(i);
		}
		unsigned i = 0;
		const auto* p = ll.first();
		while (p) {
			if (p->value() != i)
				std::cerr << "FAIL: " << p->value() << " != " << i << "\n";
			p = p->next();
			++i;
		}
		assert(i != 9);
		std::cout << ll;
	}

	{
		ll.clear();
		/// push_front
		std::cout << "push_front" << "\n";
		for (auto i = 0U; i < 10; ++i) {
			ll.push_front(i);
		}
		unsigned i = 9;
		const auto* p = ll.first();
		while (p) {
			if (p->value() != i)
				std::cerr << "FAIL: " << p->value() << " != " << i << "\n";
			p = p->next();
			--i;
		}
		assert(i != 0);
		std::cout << ll;
	}

	{
		/// find
		std::cout << "find" << "\n";
		const auto* p = ll.last();
		bool ok = true;
		for (unsigned i = 0; i < 10; ++i, p = p->prev()) {
			if (p != ll.find(i)) {
				std::cerr << "FAIL" << "\n";
				ok = false;
			}
		}
		if (ok) {
			std::cout << "OK" << "\n";
		}
	}

	{
		/// erase
		std::cout << "erase" << "\n";
		ll.erase(9);
		std::cout << ll;
		ll.erase(5);
		std::cout << ll;
		ll.erase(0u);
		std::cout << ll;
	}

	{
		/// insert_before
		std::cout << "insert_before" << "\n";
		ll.clear();
		ll.push_back(10);
		for (unsigned i = 9; i > 0; --i) {
			ll.insert_before(ll.find(10), i);
		}

		std::cout << ll;
	}

	{
		/// insert_after
		std::cout << "insert_after" << "\n";
		ll.clear();
		ll.push_front(0);
		for (unsigned i = 0; i < 10; ++i) {
			ll.insert_after(ll.find(0), i);
		}
		std::cout << ll;
	}

//	auto moo = std::move(ll);
	decltype(ll) moo;
	moo = {1, 0, 1, 69};
	if (moo.find(69))
		std::cout << "cool\n";
//	TD<decltype(ll)> td;
//	moo = std::move(ll);
	std::cout << "moo:\n" << moo;
	std::swap(ll, moo);
	std::cout << "swap:\n" << moo << ll;

	std::cout << ll.pop_front() << "\n" << ll
			  << ll.pop_front() << "\n" << ll
			  << ll.pop_front() << "\n" << ll
			  << ll.pop_front() << "\n" << ll;

	{
		LinkedList<SuperStorm> lt;
		lt.push_back(SuperStorm());
		lt.pop_back();
	}
	std::cout << "iterators\n";
//	for (auto& i : moo) {
//		i = 100;
//		std::cout << i << " ";
//	}
	std::allocator a;
	std::vector<unsigned> v(50, 69);

	decltype(ll) boo(v.begin(), v.end());
	std::cout << "boo\n" << boo;
	auto it = moo.begin();
//	TD<decltype(it)> td;
	(*it) = 100;
	*/

	A a(100);
	B b;

	LinkedList<int> ll;

	return 0;
}