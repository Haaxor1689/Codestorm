#include "dvector.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <deque>
#include <unordered_map>
#include <type_traits>

template class DVector< int >;

template< typename It >
It slownext( It it, size_t offset = 1 ) {
    for ( ; offset; --offset ) {
        ++it;
    }
    return it;
}

template< typename It >
It slowprev( It it, size_t offset = 1 ) {
    for ( ; offset; --offset ) {
        --it;
    }
    return it;
}

struct NotConstructible {
    NotConstructible() = delete;
};

struct NotCopyable {
    NotCopyable() = default;
    NotCopyable( const NotCopyable & ) = delete;
    NotCopyable( NotCopyable && ) = default; // NOLINT
};

template< typename T >
struct TestAllocator {

    using value_type = T;

    struct State {
        std::unordered_map< T *, size_t > allocs;
        int alloc = 0, failedAlloc = 0, free = 0;
        bool failNext = false;

        void start( bool firstFails = false ) {
            allocs.clear();
            alloc = free = failedAlloc = 0;
            failNext = firstFails;
        }

        void end() {
            REQUIRE( allocs.size() == 0 ); // memory leak // NOLINT
            REQUIRE( alloc == free );
        }
    };

    TestAllocator() = default;

    T* allocate( size_t n0 ) {
        int n = n0 * sizeof( T );
        if ( !state.failNext ) {
            ++state.alloc;
            T *ptr = static_cast< T * >( ::operator new( n ) );
            bool r = state.allocs.emplace( ptr, n0 ).second;
            REQUIRE( r ); // pointer not freed by TestAllocator
            return ptr;
        }
        ++state.failedAlloc;
        state.failNext = false;
        throw std::bad_alloc();
    }
    void deallocate( T* p, size_t n ) {
        ++state.free;
        auto it = state.allocs.find( p );
        REQUIRE( it != state.allocs.end() ); // pointer not allocated by TestAllocator
        REQUIRE( it->second == n ); // wrong size passed to allocator
        state.allocs.erase( it );
        ::operator delete( p );
    }

    void failNext( bool v ) { state->failNext = v; }

    static State state;
};

template< typename T >
typename TestAllocator< T >::State TestAllocator< T >::state;

static_assert( std::is_empty< TestAllocator< int > >::value, "Allocator is not staleless" );

template< typename T >
struct AllocatorRAII {

    using State = typename TestAllocator< T >::State;

    explicit AllocatorRAII( bool firstFails = false ) {
        TestAllocator< T >::state.start( firstFails );
    }
    ~AllocatorRAII() {
        TestAllocator< T >::state.end();
    }

    State &state() { return TestAllocator< T >::state; }
    void failNext() { state().failNext = true; }
};

template< typename T >
using TestVec = DVector< T, TestAllocator< T > >;

template< typename T >
bool operator==( const TestAllocator< T > &a, const TestAllocator< T > &b ) {
    return a.state == b.state;
}
template< typename T >
bool operator!=( const TestAllocator< T > &a, const TestAllocator< T > &b ) {
    return a.state != b.state;
}

static_assert( std::is_same< std::iterator_traits< TestVec< int >::iterator >::reference, int & >::value,
               "invalid reference type in DVector::iterator" );
static_assert( std::is_same< std::iterator_traits< TestVec< int >::const_iterator >::reference, const int & >::value,
               "invalid reference type in DVector::const_iterator" );

TEST_CASE( "empty" ) {
    AllocatorRAII< int > _( true );
    REQUIRE_NOTHROW( TestVec< int >() );
    REQUIRE( noexcept( TestVec< int >() ) );
    _.failNext();
    TestVec< int > vi;
    AllocatorRAII< NotConstructible > _nctor( true );
    TestVec< NotConstructible > dnct;
    AllocatorRAII< NotCopyable > _ncopy( true );
    TestVec< NotCopyable > dnco;
    REQUIRE( vi.empty() );
    REQUIRE( vi.size() == 0 );
    REQUIRE( vi.capacity() == 0 );
    REQUIRE( dnct.empty() );
    REQUIRE( dnct.size() == 0 );
    REQUIRE( dnct.capacity() == 0 );
    REQUIRE( dnco.empty() );
    REQUIRE( dnco.size() == 0 );
    REQUIRE( dnco.capacity() == 0 );
}

TEST_CASE( "range ctors" ) {
    AllocatorRAII< int > _;
    REQUIRE_FALSE( noexcept( TestVec< int >( { } ) ) );
    TestVec< int > vi = { 1, 2, 3, 4, 5 };
    REQUIRE( _.state().alloc == 1 ); // no reallocation when filling from initializer_list
    REQUIRE( vi.begin() != vi.end() );
    REQUIRE( vi.size() == 5 );
    REQUIRE( vi.capacity() >= 5 );

    for ( int i = 0; i < 5; ++i ) {
        REQUIRE( vi[ i ] == i + 1 );
        REQUIRE( *std::next( vi.begin(), i ) == i + 1 );
        REQUIRE( *slownext( vi.begin(), i ) == i + 1 );
        REQUIRE( *std::prev( vi.end(), vi.size() - i ) == i + 1 );
        REQUIRE( *slowprev( vi.end(), vi.size() - i ) == i + 1 );
    }

    std::vector< int > vec( vi.begin(), vi.end() );
    int *ptr = nullptr;
    REQUIRE_FALSE( noexcept( TestVec< int >( ptr, ptr ) ) );
    TestVec< int > vi2( vec.begin(), vec.end() );

    for ( int i = 0; i < 5; ++i ) {
        REQUIRE( vi2[ i ] == i + 1 );
        REQUIRE( *std::next( vi2.begin(), i ) == i + 1 );
        REQUIRE( *slownext( vi2.begin(), i ) == i + 1 );
        REQUIRE( *std::prev( vi2.end(), vi2.size() - i ) == i + 1 );
        REQUIRE( *slowprev( vi2.end(), vi2.size() - i ) == i + 1 );
    }
}

TEST_CASE( "copy ctor" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi = { 1, 2, 3, 4, 5 };
    REQUIRE( _.state().alloc == 1 ); // no reallocation when filling from initializer_list
    TestVec< int > vi2 = vi;
    REQUIRE( _.state().alloc == 2 ); // no reallocation when copying
    REQUIRE( vi.size() == 5 );
    REQUIRE( vi2.size() == 5 );

    for ( int i = 0; i < 5; ++i ) {
        REQUIRE( vi[ i ] == vi2[ i ] );
        REQUIRE( &vi[ i ] != &vi2[ i ] );
    }
}

TEST_CASE( "move ctor" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi = { 1, 2, 3, 4, 5 };
    REQUIRE( _.state().alloc == 1 ); // no reallocation when filling from initializer_list
    _.failNext();
    auto it = std::next( vi.begin(), 2 );
    auto &ref = *it;

    TestVec< int > vi2 = std::move( vi );
    REQUIRE( _.state().alloc == 1 ); // no allocation on move
    REQUIRE( vi.empty() );
    REQUIRE( vi2.size() == 5 );
    REQUIRE( *it == 3 );
    REQUIRE( ref == 3 );

    for ( int i = 0; i < 5; ++i ) {
        REQUIRE( vi2[ i ] == i + 1 );
    }
}

TEST_CASE( "push_back" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;
    REQUIRE( vi.size() == 0 );

    vi.push_back( 1 );
    REQUIRE( vi.size() == 1 );
#define DO_CHECK \
    for ( int i = 0; i < int( vi.size() ); ++i ) { \
        REQUIRE( vi[i] == i + 1 ); \
        REQUIRE( *std::next( vi.begin(), i ) == i + 1 ); \
        REQUIRE( *slownext( vi.begin(), i ) == i + 1 ); \
    } \
    REQUIRE( std::next( vi.begin(), vi.size() ) == vi.end() ); \
    REQUIRE( slownext( vi.begin(), vi.size() ) == vi.end() ); \
    REQUIRE( std::prev( vi.end(), vi.size() ) == vi.begin() ); \
    REQUIRE( slowprev( vi.end(), vi.size() ) == vi.begin() ); \
    REQUIRE( vi.size() <= vi.capacity() )
    DO_CHECK;

    vi.push_back( 2 );
    REQUIRE( vi.size() == 2 );
    DO_CHECK;

    vi.push_back( 3 );
    REQUIRE( vi.size() == 3 );
    DO_CHECK;

    vi.push_back( 4 );
    REQUIRE( vi.size() == 4 );
    DO_CHECK;

    vi.push_back( 5 );
    REQUIRE( vi.size() == 5 );
    DO_CHECK;

    vi.push_back( 6 );
    REQUIRE( vi.size() == 6 );
    DO_CHECK;
#undef DO_CHECK
}

TEST_CASE( "push_front" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;
    REQUIRE( vi.size() == 0 );

    vi.push_front( 1 );
    REQUIRE( vi.size() == 1 );
#define DO_CHECK \
    for ( int i = 0; i < int( vi.size() ); ++i ) { \
        REQUIRE( vi[i] == int( vi.size() - i ) ); \
        REQUIRE( *std::next( vi.begin(), i ) == int( vi.size() - i ) ); \
        REQUIRE( *slownext( vi.begin(), i ) == int( vi.size() - i ) ); \
    } \
    REQUIRE( std::next( vi.begin(), vi.size() ) == vi.end() ); \
    REQUIRE( slownext( vi.begin(), vi.size() ) == vi.end() ); \
    REQUIRE( std::prev( vi.end(), vi.size() ) == vi.begin() ); \
    REQUIRE( slowprev( vi.end(), vi.size() ) == vi.begin() ); \
    REQUIRE( vi.size() <= vi.capacity() )
    DO_CHECK;

    vi.push_front( 2 );
    REQUIRE( vi.size() == 2 );
    DO_CHECK;

    vi.push_front( 3 );
    REQUIRE( vi.size() == 3 );
    DO_CHECK;

    vi.push_front( 4 );
    REQUIRE( vi.size() == 4 );
    DO_CHECK;

    vi.push_front( 5 );
    REQUIRE( vi.size() == 5 );
    DO_CHECK;

    vi.push_front( 6 );
    REQUIRE( vi.size() == 6 );
    DO_CHECK;
#undef DO_CHECK
}

TEST_CASE( "uses allocator - exception" ) {
    AllocatorRAII< int > _( true );
    TestVec< int > vi;
    SECTION( "push_back nomem" ) {
        REQUIRE_THROWS_AS( vi.push_back( 42 ), const std::bad_alloc & );
        REQUIRE( _.state().failedAlloc == 1 );
    }
    SECTION( "push_front nomem" ) {
        REQUIRE_THROWS_AS( vi.push_front( 42 ), const std::bad_alloc & );
        REQUIRE( _.state().failedAlloc == 1 );
    }
}

TEST_CASE( "uses allocator - counting" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;
    SECTION( "push_back" ) {
        REQUIRE_NOTHROW( vi.push_back( 42 ) );
        REQUIRE( _.state().alloc == 1 );
        REQUIRE( _.state().allocs.size() == 1 );
    }
    SECTION( "push_front" ) {
        REQUIRE_NOTHROW( vi.push_front( 42 ) );
        REQUIRE( _.state().alloc == 1 );
        REQUIRE( _.state().allocs.size() == 1 );
    }
}

TEST_CASE( "pop_front" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;

    for ( int i = 0; i < 10; ++i ) {
        vi.push_back( i );
    }
    REQUIRE( vi.size() == 10 );
    for ( int i = 0; i < 10; ++i ) {
        int s;
        REQUIRE( (s = vi.size()) > 0 );
        REQUIRE( vi.front() == i );
        vi.pop_front();
        REQUIRE( vi.size() == (s - 1) );
    }
    REQUIRE( vi.empty() );
}

TEST_CASE( "pop_back" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;

    for ( int i = 0; i < 10; ++i ) {
        vi.push_front( i );
    }
    REQUIRE( vi.size() == 10 );
    for ( int i = 0; i < 10; ++i ) {
        int s;
        REQUIRE( (s = vi.size()) > 0 );
        REQUIRE( vi.back() == i );
        vi.pop_back();
        REQUIRE( vi.size() == (s - 1) );
    }
    REQUIRE( vi.empty() );
}

//TEST_CASE( "resize does not copy" ) {
//    AllocatorRAII< NotCopyable > _;
//    TestVec< NotCopyable > vnc;
//    int capaChanged = 0;
//    size_t lastCapa = 0;
//    do {
//        lastCapa = vnc.capacity();
//        vnc.push_back( NotCopyable() );
//        if ( vnc.capacity() != lastCapa )
//            ++capaChanged;
//
//    } while ( capaChanged < 2 );
//
//    vnc.resize( 4 * vnc.capacity() );
//}

TEST_CASE( "base combination" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;

    vi.push_front( 1 );
    REQUIRE( vi.size() == 1 );
    REQUIRE( vi[ 0 ] == 1 );

    SECTION( "push_back" ) {
        vi.push_back( 2 );
        REQUIRE( vi.size() == 2 );
        REQUIRE( vi[ 0 ] == 1 );
        REQUIRE( vi[ 1 ] == 2 );
    }
    SECTION( "push_front" ) {
        vi.push_front( 2 );
        REQUIRE( vi.size() == 2 );
        REQUIRE( vi[ 0 ] == 2 );
        REQUIRE( vi[ 1 ] == 1 );
    }
}

TEST_CASE( "combination - push/pop" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;

    vi.push_back( 2 );
    vi.push_front( 1 );
    vi.push_back( 3 );
    vi.push_front( 0 );

    SECTION( "pop_front first" ) {
        vi.pop_front();
        REQUIRE( vi.size() == 3 );
        REQUIRE( vi[ 0 ] == 1 );
        REQUIRE( *vi.begin() == 1 );
        REQUIRE( vi[ 2 ] == 3 );

        vi.pop_back();
        REQUIRE( vi.size() == 2 );
        REQUIRE( vi[ 0 ] == 1 );
        REQUIRE( *vi.begin() == 1 );
        REQUIRE( vi[ 1 ] == 2 );
    }

    SECTION( "pop_back first" ) {
        vi.pop_back();
        REQUIRE( vi.size() == 3 );
        REQUIRE( vi[ 0 ] == 0 );
        REQUIRE( *vi.begin() == 0 );
        REQUIRE( vi[ 2 ] == 2 );

        vi.pop_front();
        REQUIRE( vi.size() == 2 );
        REQUIRE( vi[ 0 ] == 1 );
        REQUIRE( *vi.begin() == 1 );
        REQUIRE( vi[ 1 ] == 2 );
    }
}

TEST_CASE( "grow uses allocator and throws" ) {
    AllocatorRAII< int > _;
    TestVec< int > vi;
    auto &alloc = _.state();
    vi.push_back( 1 );

    while ( vi.size() < vi.capacity() ) {
        vi.push_back( 1 );
    }
    REQUIRE( alloc.failedAlloc == 0 );
    alloc.failNext = true;
    REQUIRE_THROWS_AS( (vi.push_back( 42 ), vi.push_back( 42 )),
                       const std::bad_alloc & );
    REQUIRE( alloc.failedAlloc == 1 );
}
