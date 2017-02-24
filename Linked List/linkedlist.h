/*

LinkeList

Here your task is to implement doubly linked list in C++14.
- You should use unique_ptr for memory management and avoid any explicit deletes.
  - Keep in mind that not all pointers can be unique_ptrs.
- You should try to avoid code duplication.
- There are tests provided for you.
- Solution will be provided after both seminars end.

 */

#include <memory>

template< typename T >
struct LinkedList {

    struct Node {

        Node() = default;
        Node( T val );

        T &value();
        const T &value() const;

        Node *next();
        const Node *next() const;

        Node *prev();
        const Node *prev() const;
    };

    LinkedList() = default;
    LinkedList( const LinkedList &o );

    bool empty() const;

    Node *first();
    const Node *first() const;

    Node *last();
    const Node *last() const;

    Node *find( const T &val );
    const Node *find( const T &val ) const;

    void erase( T val );
    void erase( Node *n );

    void insert_before( Node *n, T val );
    void insert_after( Node *n, T val );

    void push_back( T val );
    void push_front( T val );
};
