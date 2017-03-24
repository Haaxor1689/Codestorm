#include "dvector.h"

struct A {
	A() = default;
	A(const A& a) : _name(a._name) {}
	A(A&& a) : _name(std::move(a._name)) {}

private:
	std::string _name;
};

struct NotCopyable {
	NotCopyable() = default;
	NotCopyable( const NotCopyable & ) = delete;
	NotCopyable( NotCopyable && ) = default; // NOLINT
};

int main() {
	static_assert(std::is_move_constructible<NotCopyable>());
//
//	using Buffer = Buffer<NotCopyable, std::allocator<NotCopyable> >;
//	Buffer buf;
//
//	buf._push(NotCopyable(), Buffer::at::front);

	DVector<NotCopyable> v;
	v.push_back(NotCopyable());

//	DVector<int> v;
//	for (unsigned i = 0; i < 2; ++i)
//		v.push_front(i);
//	v.push_back(2);
//	v.push_back(3);
////	v.clear();
//	for (const auto& e : v)
//		std::cout << e << " ";
////	for (unsigned i = 0; i < 10; ++i)
////		std::cout << v[i] << " ";
////	for (auto p = v.begin(); p != v.end(); ++p)
////		std::cout << *p << " ";
//	DVector<A> v2;
//	for (unsigned i = 0; i < 10; ++i) {
//		A a;
//		v2.push_back(std::move(a));
//	}
//	return 0;
}
