/**
 * \file test.h
 * \brief libgnode 를 테스트하기 위해 사용되는 헤더파일
 */
#ifndef _GNODE_TEST_H_
#define _GNODE_TEST_H_
#include <iostream>

/* Test */
#define GNODE_EQUAL(expected,real)						\
	do {                                                \
		if (expected != (real)) {                       \
            _TL ("They are not equal. expected : ");    \
            _TL (expected);                             \
            _TL (", real : ");                          \
            _TLn (real);                                \
            abort ();                                   \
		}                                               \
	} while (0)

/* Test Run */
#define GNODE_TEST(expected,expr)								\
	do {                                                \
		if (expected == (expr)) {                       \
            _TL ("test pass : ");                       \
            _TLn (#expr);                               \
		}                                               \
	} while (0)

#endif //_TP3_TEST_H_
