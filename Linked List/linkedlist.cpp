#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "linkedlist.h"

#include <array>


///         TESTING SECTION         ///
struct NotConstructible {
    NotConstructible( int /*unused*/ ) { }
    NotConstructible() = delete;
};

TEST_CASE( "ctor" ) {
    LinkedList< int > ll;
    LinkedList< int > ll2( ll ); // NOLINT
    LinkedList< NotConstructible > lla;
}

TEST_CASE( "singleton" ) {
    LinkedList< int > ll;
    REQUIRE( ll.empty() );
    SECTION( "push_front" ) {
        ll.push_front( 1 );
    }
    SECTION( "push_back" ) {
        ll.push_back( 1 );
    }
    REQUIRE_FALSE( ll.empty() );
    REQUIRE( ll.first() );
    REQUIRE( ll.last() );
    REQUIRE( ll.first() == ll.last() );
    REQUIRE( ll.first()->value() == 1 );
    REQUIRE( ll.first()->prev() == nullptr );
    REQUIRE( ll.first()->next() == nullptr );

    LinkedList< int > ll2( ll );
    REQUIRE_FALSE( ll2.empty() );
    REQUIRE( ll2.first() );
    REQUIRE( ll2.last() );
    REQUIRE( ll2.first() == ll2.last() );
    REQUIRE( ll2.first()->value() == 1 );
    REQUIRE( ll2.first()->prev() == nullptr );
    REQUIRE( ll2.first()->next() == nullptr );
    REQUIRE( ll2.first() != ll.first() );
}

TEST_CASE( "size2" ) {
    LinkedList< int > ll;
    std::array< int, 2 > check;
    ll.push_back( 1 );
    SECTION( "push_front" ) {
        ll.push_front( 2 );
        check = { { 2, 1 } };
    }
    SECTION( "push_back" ) {
        ll.push_back( 2 );
        check = { { 1, 2 } };
    }
    REQUIRE_FALSE( ll.empty() );
    REQUIRE( ll.first() );
    REQUIRE( ll.last() );
    REQUIRE( ll.first() != ll.last() );
    REQUIRE( ll.first()->value() == check[0] );
    REQUIRE( ll.last()->value() == check[1] );
    REQUIRE( ll.first()->prev() == nullptr );
    REQUIRE( ll.last()->next() == nullptr );
    REQUIRE( ll.first()->next() == ll.last() );
    REQUIRE( ll.last()->prev() == ll.first() );

    LinkedList< int > ll2( ll );
    REQUIRE_FALSE( ll2.empty() );
    REQUIRE( ll2.first() );
    REQUIRE( ll2.last() );
    REQUIRE( ll2.first() != ll2.last() );
    REQUIRE( ll2.first()->value() == check[0] );
    REQUIRE( ll2.last()->value() == check[1] );
    REQUIRE( ll2.first() != ll.first() );
    REQUIRE( ll2.last() != ll.last() );
    REQUIRE( ll2.first()->prev() == nullptr );
    REQUIRE( ll2.last()->next() == nullptr );
    REQUIRE( ll2.first()->next() == ll2.last() );
    REQUIRE( ll2.last()->prev() == ll2.first() );
}

TEST_CASE( "size3" ) {
    LinkedList< int > ll;
    std::array< int, 3 > check;
    ll.push_back( 1 );
    SECTION( "push_front" ) {
        ll.push_front( 2 );
        SECTION( "push_front:2" ) {
            ll.push_front( 3 );
            check = { { 3, 2, 1 } };
        }
        SECTION( "push_back:2" ) {
            ll.push_back( 3 );
            check = { { 2, 1, 3 } };
        }
    }
    SECTION( "push_back" ) {
        ll.push_back( 2 );
        SECTION( "push_front:2" ) {
            ll.push_front( 3 );
            check = { { 3, 1, 2 } };
        }
        SECTION( "push_back:2" ) {
            ll.push_back( 3 );
            check = { { 1, 2, 3 } };
        }
    }
    REQUIRE_FALSE( ll.empty() );
    REQUIRE( ll.first() );
    REQUIRE( ll.last() );
    REQUIRE( ll.first() != ll.last() );
    REQUIRE( ll.first()->value() == check[0] );
    REQUIRE( ll.last()->value() == check[2] );
    REQUIRE( ll.first()->next()->value() == check[1] );
    REQUIRE( ll.first()->next() == ll.last()->prev() );
    REQUIRE( ll.first()->prev() == nullptr );
    REQUIRE( ll.last()->next() == nullptr );
    REQUIRE( ll.first()->next()->next() == ll.last() );
    REQUIRE( ll.last()->prev()->prev() == ll.first() );

    LinkedList< int > ll2( ll );
    REQUIRE( ll2.first() );
    REQUIRE( ll2.last() );
    REQUIRE( ll2.first() != ll2.last() );
    REQUIRE( ll2.first()->value() == check[0] );
    REQUIRE( ll2.last()->value() == check[2] );
    REQUIRE( ll2.first()->next()->value() == check[1] );
    REQUIRE( ll2.first()->next() == ll2.last()->prev() );
    REQUIRE( ll2.first()->prev() == nullptr );
    REQUIRE( ll2.last()->next() == nullptr );
    REQUIRE( ll2.first()->next()->next() == ll2.last() );
    REQUIRE( ll2.last()->prev()->prev() == ll2.first() );
    REQUIRE( ll2.first() != ll.first() );
    REQUIRE( ll2.last() != ll.last() );
    REQUIRE( ll2.first()->next() != ll.first()->next() );
}

TEST_CASE( "find" ) {
    LinkedList< int > ll;
    ll.push_back( 1 );
    ll.push_back( 2 );
    ll.push_back( 3 );

    REQUIRE( ll.find( 1 ) == ll.first() );
    REQUIRE( ll.find( 2 ) == ll.first()->next() );
    REQUIRE( ll.find( 3 ) == ll.first()->next()->next() );
}

TEST_CASE( "insert_before" ) {
    LinkedList< int > ll;
    ll.push_back( 1 );
    ll.push_back( 2 );
    std::array< int, 3 > check;

    SECTION( "first" ) {
        ll.insert_before( ll.first(), 3 );
        check = { { 3, 1, 2 } };
    }
    SECTION( "last" ) {
        ll.insert_before( ll.last(), 3 );
        check = { { 1, 3, 2 } };
    }

    REQUIRE( ll.first() );
    REQUIRE( ll.last() );
    REQUIRE( ll.first()->next()->next() == ll.last() );
    REQUIRE( ll.last()->prev()->prev() == ll.first() );
    REQUIRE( ll.first()->prev() == nullptr );
    REQUIRE( ll.last()->next() == nullptr );
    REQUIRE( ll.first()->value() == check[0] );
    REQUIRE( ll.first()->next()->value() == check[1] );
    REQUIRE( ll.first()->next()->next()->value() == check[2] );
}

TEST_CASE( "insert_after" ) {
    LinkedList< int > ll;
    ll.push_back( 1 );
    ll.push_back( 2 );
    std::array< int, 3 > check;

    SECTION( "first" ) {
        ll.insert_after( ll.first(), 3 );
        check = { { 1, 3, 2 } };
    }
    SECTION( "last" ) {
        ll.insert_after( ll.last(), 3 );
        check = { { 1, 2, 3 } };
    }

    REQUIRE( ll.first() );
    REQUIRE( ll.last() );
    REQUIRE( ll.first()->next()->next() == ll.last() );
    REQUIRE( ll.last()->prev()->prev() == ll.first() );
    REQUIRE( ll.first()->prev() == nullptr );
    REQUIRE( ll.last()->next() == nullptr );
    REQUIRE( ll.first()->value() == check[0] );
    REQUIRE( ll.first()->next()->value() == check[1] );
    REQUIRE( ll.first()->next()->next()->value() == check[2] );
}

TEST_CASE( "erase" ) {
    LinkedList< int > ll;
    ll.push_back( 1 );
    ll.push_back( 2 );
    ll.push_back( 3 );

    SECTION( "erase first" ) {
        SECTION( "by ptr" ) {
            ll.erase( ll.first() );
        }
        SECTION( "by val" ) {
            ll.erase( 1 );
        }
        REQUIRE_FALSE( ll.empty() );
        REQUIRE( ll.first() );
        REQUIRE( ll.first()->value() == 2 );
        REQUIRE( ll.first()->next()->value() == 3 );
        REQUIRE( ll.first()->prev() == nullptr );
    }
    SECTION( "erase last" ) {
        SECTION( "by ptr" ) {
            ll.erase( ll.last() );
        }
        SECTION( "by val" ) {
            ll.erase( 3 );
        }
        REQUIRE_FALSE( ll.empty() );
        REQUIRE( ll.last() );
        REQUIRE( ll.last()->value() == 2 );
        REQUIRE( ll.last()->prev()->value() == 1 );
        REQUIRE( ll.last()->next() == nullptr );
    }
    SECTION( "erase middle" ) {
        SECTION( "by ptr" ) {
            ll.erase( ll.first()->next() );
        }
        SECTION( "by val" ) {
            ll.erase( 2 );
        }
        REQUIRE_FALSE( ll.empty() );
        REQUIRE( ll.last() );
        REQUIRE( ll.first() );
        REQUIRE( ll.first()->value() == 1 );
        REQUIRE( ll.last()->value() == 3 );
        REQUIRE( ll.first()->next() == ll.last() );
        REQUIRE( ll.last()->prev() == ll.first() );
    }
}
