//
//  Testing.h
//
//  Support for selecting individual tests.
//
//  Created by Warren R. Carithers on 2025/01/31.
//  Copyright 2025 Rochester Institute of Technology. All rights reserved.
//

#ifndef TESTING_H_
#define TESTING_H_

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

// This defines N_POLYS, which we use to define N_TESTS.
#include "Models.h"

using namespace std;

//
// PUBLIC GLOBALS
//

//
// Support for individual tests
//

extern unsigned int which;

///
/// Declarations and testing macros
///

// number of possible tests for this assignment
#ifndef N_TESTS
#define N_TESTS		N_POLYS
#endif

// by default, all tests will be performed
#define TEST_ALL	0xffffffff

//
// Testing macros:
//      IFTEST(n) stmt
//      DOIF(n,stuff)
//
#define IFTEST(n)	if( which & (1 << (n)) )
#define DOIF(n,...)	IFTEST(n) { __VA_ARGS__ }

///
/// iterate through argv converting numeric parameters
/// and setting test bits accordingly
///
/// invoke as:   selectTests( argc, argv );
///
/// check as:    IFTEST(n,)
///              DOIF(n, ...)
///
/// @param argc   number of command-line arguments
/// @param argv   command-line argument strings
///
void selectTests( const int argc, char *argv[] );

#endif
