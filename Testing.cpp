//
//  Testing.cpp
//
//  Support for selecting individual tests.
//
//  Created by Warren R. Carithers on 2025/01/31.
//  Copyright 2025 Rochester Institute of Technology. All rights reserved.
//

#include <cstdlib>
#include <iostream>

#include "Testing.h"

using namespace std;

//
// PUBLIC GLOBALS
//

//
// Support for individual tests
//

unsigned int which = TEST_ALL;

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
void selectTests( const int argc, char *argv[] ) {
	char *endptr;
	bool saw_x = false;

	// if the first argument is "-h", print a help message and exit
	if( argv[1][0] == '-' && argv[1][1] == 'h' ) {
		cout << "usage: main [ -h ] [ c [ c ... ] ]" << endl;
		cout << "Each 'c' is one of the following characters:" << endl;
		cout << "'q' (quad)" << ", 'c' (cylinder)" << ", 'd' (discs)"
			<< ", 'x' (none)" << endl;
		exit( 0 );
	}

	// if there are fewer than two tests possible, nothing to do
	if( N_TESTS < 2 ) {
		return;
	}

	// assume nothing
	which = 0;

	// set the appropriate test bits
	for( int i = 1; i < argc; ++i ) {
		switch( argv[i][0] ) {
		case 'q': which |= (1 << Quad); break;
		case 'c': which |= (1 << Cylinder); break;
		case 'd': which |= (1 << Discs); break;
		// 'x' means don't do any of them
		case 'x': saw_x = true; break;
		default:
			cerr  << "bad object character '" << argv[i][0]
				<< "' ignored" << endl;
		}
	}

	if( saw_x ) {
		which = 0;
	} else if( which == 0 ) {
		which = TEST_ALL;
	}
}
