//
//  Utils
//
//  Various utility functions.
//
//  The functions in this module are intended to be used as debugging
//  routines; they provide feedback to the user on potential error
//  situations within OpenGL programs.
//
//  Created by Warren R. Carithers on 2019/11/18.
//  Copyright 2019 Rochester Institute of Technology. All rights reserved.
//
//  This file should not be modified by students.
//

#include <cstdlib>
#include <iostream>
#include <iomanip>


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Utils.h"

using namespace std;

struct emap {
	unsigned int code;
	const char *msg;
};

static struct emap errormap[] = {
	{ GL_INVALID_ENUM,	"Invalid enum" },
	{ GL_INVALID_VALUE,	"Invalid value" },
	{ GL_INVALID_OPERATION,	"Invalid operation" },
	{ GL_STACK_OVERFLOW,	"Stack overflow" },
	{ GL_STACK_UNDERFLOW,	"Stack underflow" },
	{ GL_OUT_OF_MEMORY,	"Out of memory" },
	{ GL_INVALID_FRAMEBUFFER_OPERATION,	"Invalid FB operation" },
	{ GL_CONTEXT_LOST,	"Context lost" }
};

const int nerrors = sizeof(errormap) / sizeof(errormap[0]);

///
/// OpenGL error checking
///
/// Can be called at any time to query the internal OpenGL error state,
/// and reports all outstanding OpenGL error conditions at the time of
/// the call.  If used periodically within an OpenGL application, this
/// can help pinpoint the OpenGL API call(s) that encountered an error.
///
/// @param msg  message prefix to print with each error message
///
void checkErrors( const char *msg ) {
	GLenum code;
	const char *str;

	while( (code = glGetError()) != GL_NO_ERROR ) {
		cerr << "*** " << msg << ", GL error code 0x" << hex << code << ":";
		str = nullptr;
		for( int i = 0; i < nerrors; ++i ) {
			if( code == errormap[i].code ) {
				str = errormap[i].msg;
				break;
			}
		}
		if( str == nullptr ) {
			cerr << "*** UNKNOWN ERROR";
		} else {
			cerr << str;
		}
		cerr << endl;
	}
}

///
/// Convert a type number to a string.
///
/// Used to decode GLSL data type numbers when printing error messages.
/// Decodes the most commonly-used types; others are printed as integer
/// strings.  (In the latter case, the return value points to a static
/// character array that may be overwritten in subsequent calls.)
///
/// @param num  the type
/// @return a string containing a text description
///
static const char *type2str( GLenum num ) {
	const char *ret;

	// This would be so much easier if these were nice, small
	// integer constants, but they're not, so we can't just use
	// them to index into an array of char* pointers.

	switch( num ) {
	case GL_FLOAT:             ret = "float";      break;
	case GL_FLOAT_VEC2:        ret = "vec2";       break;
	case GL_FLOAT_VEC3:        ret = "vec3";       break;
	case GL_FLOAT_VEC4:        ret = "vec4";       break;
	case GL_FLOAT_MAT2:        ret = "mat2";       break;
	case GL_FLOAT_MAT3:        ret = "mat3";       break;
	case GL_FLOAT_MAT4:        ret = "mat4";       break;
	case GL_FLOAT_MAT2x3:      ret = "mat2x3";     break;
	case GL_FLOAT_MAT2x4:      ret = "mat2x4";     break;
	case GL_FLOAT_MAT3x2:      ret = "mat3x2";     break;
	case GL_FLOAT_MAT3x4:      ret = "mat3x4";     break;
	case GL_FLOAT_MAT4x2:      ret = "mat4x2";     break;
	case GL_FLOAT_MAT4x3:      ret = "mat4x3";     break;
	case GL_BOOL:              ret = "bool";       break;
	case GL_BOOL_VEC2:         ret = "bvec2";      break;
	case GL_BOOL_VEC3:         ret = "bvec3";      break;
	case GL_BOOL_VEC4:         ret = "bvec4";      break;
	case GL_INT:               ret = "int";        break;
	case GL_INT_VEC2:          ret = "ivec2";      break;
	case GL_INT_VEC3:          ret = "ivec3";      break;
	case GL_INT_VEC4:          ret = "ivec4";      break;
	case GL_UNSIGNED_INT:      ret = "uint";       break;
	case GL_UNSIGNED_INT_VEC2: ret = "uvec2";      break;
	case GL_UNSIGNED_INT_VEC3: ret = "uvec3";      break;
	case GL_UNSIGNED_INT_VEC4: ret = "uvec4";      break;
	case GL_SAMPLER_1D:        ret = "sampler1D";  break;
	case GL_SAMPLER_2D:        ret = "sampler2D";  break;
	case GL_SAMPLER_3D:        ret = "sampler3D";  break;
	default:
		ret = nullptr;
	}

	return( ret );
}

///
/// Dump the list of active attributes and uniforms from a shader program
///
/// @param desc  description of the shader program
/// @param prog  which shader program to query
///
void dumpActives( const char *desc, GLuint prog ) {
	GLint count, size;
	GLsizei length;
	GLenum type;
	const GLsizei bufSize = 32;
	GLchar name[bufSize];

	cerr << "Shader actives";
	if( desc != nullptr ) {
		cerr << ", " << desc;
	} else {
		cerr << ", prog #" << prog;
	}
	cerr << ":" << endl;

	glGetProgramiv( prog, GL_ACTIVE_ATTRIBUTES, &count );
	cerr << "Active attributes:";
	if( count < 1 ) {
		cerr << " none" << endl;
	} else {
		cerr << endl;
		for( int i = 0; i < count; ++i ) {
			glGetActiveAttrib( prog, i, bufSize, &length, &size, &type, name );
			const char *tstr = type2str( type );
			cerr << "  #" << i << " Type: ";
			if( tstr == nullptr ) {
				cerr << hex << type;
			} else {
				cerr << tstr;
			}
			cerr << " Name: '" << name << "'" << endl;
		}
	}

	glGetProgramiv( prog, GL_ACTIVE_UNIFORMS, &count );
	cerr << "Active uniforms:";
	if( count < 1 ) {
		cerr << " none" << endl;
	} else {
		cerr << endl;
		for( int i = 0; i < count; ++i ) {
			glGetActiveUniform( prog, i, bufSize, &length, &size, &type, name );
			const char *tstr = type2str( type );
			cerr << "  #" << i << " Type: ";
			if( tstr == nullptr ) {
				cerr << hex << type;
			} else {
				cerr << tstr;
			}
			cerr << " Name: '" << name << "'" << endl;
		}
	}
}

///
/// Retrieve a Uniform variable's location and verify the result
///
/// @param prog  the shader program
/// @param name  the name of the desired variable
///
GLint getUniformLoc( GLuint prog, const GLchar *name ) {
	GLint loc;

	loc = glGetUniformLocation( prog, name );
	if( loc < 0 ) {
		cerr << "Bad uniform, program " << prog
			<< " variable '" << name << "'" << endl;
	}

	return( loc );
}

///
/// Retrieve an Attribute variable's location and verify the result
///
/// @param prog  the shader program
/// @param name  the name of the desired variable
///
GLint getAttribLoc( GLuint prog, const GLchar *name ) {
	GLint loc;

	loc = glGetAttribLocation( prog, name );
	if( loc < 0 ) {
		cerr << "Bad attribute, program " << prog
			<< " variable '" << name << "'" << endl;
	}

	return( loc );
}
