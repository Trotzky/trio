#pragma once

#define CONCAT2(First, Second) (First ## Second)
#define CONCAT(First, Second) CONCAT2(First, Second)

#ifdef __cplusplus
//#include "loki/static_check.h"

//copied from Boost library
namespace boost
{
	template <bool x> struct STATIC_ASSERTION_FAILURE;
	template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
	template<int x> struct static_assert_test{};
}

#define BOOST_STATIC_ASSERT_BOOL_CAST(x) (bool)(x)


#ifdef __GNUC__
#define BOOST_STATIC_ASSERT( B ) \
   typedef ::boost::static_assert_test<\
      sizeof(::boost::STATIC_ASSERTION_FAILURE< BOOST_STATIC_ASSERT_BOOL_CAST( B ) >)>\
         (CONCAT(boost_static_assert_typedef_, __LINE__)) __attribute__((unused))
#else
#define BOOST_STATIC_ASSERT( B ) \
   typedef ::boost::static_assert_test<\
      sizeof(::boost::STATIC_ASSERTION_FAILURE< BOOST_STATIC_ASSERT_BOOL_CAST( B ) >)>\
         (CONCAT(boost_static_assert_typedef_, __LINE__))
#endif
 
#endif

#define C_STATIC_ASSERT(expr) typedef char CONCAT(static_assert_failed_at_line_, __LINE__) [(expr) ? 1 : -1]
