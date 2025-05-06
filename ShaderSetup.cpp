//
//  ShaderSetup.cpp
//
//  Sets up a GLSL shader based on supplied source files.
//
//  Originally based on code from www.lighthouse3d.com
//
//  This module provides a simple way to create GLSL shader programs.
//  It has these entry points:
//
//     readTextFile(name)
//         Takes a C-style string containing the name of a file.
//         Returns a dynamically-allocated buffer containing the
//         contents of the specified file as a NUL-terminated string,
//         or NULL if the file couldn't be opened or read.
//
//     shaderSetup(vsfile,fsfile,err)
//     shaderSetup(vsfile,fsfile,fsfile,err)
//     shaderSetupStr(vsstr,fsstr,gsstr,err)
//         These functions take C-style strings as the first two
//         parameters.  For shaderSetup(), these are pathnames of files
//         containing the GLSL code for the vertex and fragment shaders;
//         for shaderSetupStr(), they are strings containing the GLSL
//         code.  Each will create shader objects, attach the source
//         code to them, and compile them; next, they create a program
//         object, attached the shader objects, and link the program.
//
//     shaderCreate(src,type,err)
//     shaderLink(ids,num,err)
//         These functions provide more flexibility.  The first parameter
//         to shaderCreate() is an argv-style array of string pointers;
//         a shader object of the specified type is created, all the
//         source strings are attached to it, and it is compiled.  This
//         allows creation of any type of shader (e.g., geometry), but
//         doesn't do any linking of the result into a shader program. The
//         shaderLink() function takes a collection of shader object ids,
//         creates a shader program object, attaches all the shader
//         objects to it, and links the result.
//

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "ShaderSetup.h"
#include "Utils.h"

using namespace std;

///
/// readTextFile(name)
///
/// Read the text file given as 'name'.
///
/// Returns the contents of the file in a dynamically-allocated
/// string buffer, or NULL if an error occurs.
///
GLchar *readTextFile( const char *name ) {
	GLchar *content = nullptr;

	if( name != nullptr ) {

		// Attempt to open the file
		ifstream shaderFile( name );
		if( !shaderFile ) {
			return nullptr;
		}

		// read its contents and close it
		stringstream str;
		str << shaderFile.rdbuf();
		shaderFile.close();

		// convert the contents to a C-style string
		string code = str.str();
		content = new GLchar[ code.length() + 1 ];
		if( !content ) {
			return nullptr;
		}
		strcpy( content, code.c_str() );

	} else {
		cerr << "readTextFile:  no file name specified" << endl;
	}

	return content;
}

///
/// printShaderInfoLog(shader)
///
/// Print the information log from a shader compilation attempt
///
void printShaderInfoLog( GLuint shader ) {
	GLint length = 0;
	GLsizei nchars  = 0;
	char *log;

	// Determine the length of the information log
	glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );

	if( length > 0 ) {

		// Allocate a buffer for the log
		log = new char[ length ];

		if( log != nullptr ) {

			// Retrieve the log
			glGetShaderInfoLog( shader, length, &nchars, log );

			// Report it
			if( log[0] != '\0' ) {
				cout << "Shader log:" << endl << log << endl;
			}

			delete [] log;

		} else {

			cerr << "Shader log non-empty, but allocate failed" << endl;

		}
	}
}

///
/// printProgramInfoLog(shader)
///
/// Print a program information log
///
/// This is identical to printShaderInfoLog(), except that it uses
/// glGetProgramiv() and glGetProgramInfoLog() instead of the *Shader*()
/// versions.
///
void printProgramInfoLog( GLuint shader ) {
	GLint length = 0;
	GLsizei nchars  = 0;
	char *log;

	// Determine the length of the information log
	glGetProgramiv( shader, GL_INFO_LOG_LENGTH, &length );

	if( length > 0 ) {

		// Allocate a buffer for the log
		log = new char[ length ];

		if( log != nullptr ) {

			// Retrieve the log
			glGetProgramInfoLog( shader, length, &nchars, log );

			// Report it
			if( log[0] != '\0' ) {
				cout << "Program log:" << endl << log << endl;
			}

			delete [] log;

		} else {

			cerr << "Program log non-empty, but allocate failed" << endl;

		}
	}
}

static const char *msgs[E_NUM_ERRORS] = {
	// generic
	"No error", /* E_NO_ERROR */
	"No shader source string(s) provided",      /* E_NO_STRING */
	"Need at least two shader objects",         /* E_NO_OBJECTS */
	"Error allocating (unknown) shader object", /* E_US_ALLOC */
	"Error loading (unknown) shader code",      /* E_US_LOAD */
	"Error compiling (unknown) shader code",    /* E_US_COMPILE */
	// vertex shader
	"Error allocating vertex shader object",    /* E_VS_ALLOC */
	"Error loading vertex shader code",         /* E_VS_LOAD */
	"Error compiling vertex shader code",       /* E_VS_COMPILE */
	"No vertex shader source string provided",  /* E_NO_VSHADER */
	// fragment shader
	"Error allocating fragment shader object",  /* E_FS_ALLOC */
	"Error loading fragment shader code",       /* E_FS_LOAD */
	"Error compiling fragment shader code",     /* E_FS_COMPILE */
	// geometry shader
	"Error allocating geometry shader object",  /* E_GS_ALLOC */
	"Error loading geometry shader code",       /* E_GS_LOAD */
	"Error compiling geometry shader code",     /* E_GS_COMPILE */
	// program
	"Error allocating program object",          /* E_PROG_ALLOC */
	"Error linking shader program"              /* E_PROG_LINK */
};

///
/// errorString( code )
///
/// Returns a const char* with a text version of the supplied error code.
///
const char *errorString( ShaderError code ) {
	static char buffer[256];

	if( code < 0 || code >= E_NUM_ERRORS ) {
		std::string str = std::to_string( code );
		strcpy( buffer, str.c_str() );
		return buffer;
	}

	return msgs[code];
}

///
/// shaderCreate( const GLchar **src, GLenum type, ShaderError &err )
///
/// Create a GLSL shader object from a set of one or more source strings.
///
/// If more than one source string is provided, care should be taken
/// that only the first string contains a #version directive.
///
/// @param  src  array of GLchar * containing shader source code
/// @param  type type of shader object to create
/// @param  err  reference to status variable
/// @return      id of the new shader object, or 0 on error, with
///              an error code in err
///
GLuint shaderCreate( const GLchar **src, GLenum type, ShaderError &err ) {
	GLuint id;
	GLint flag;
	int num;

	// Check for bad parameter
	if( src == 0 ) {
		return( 0 );
	}

	// Count the source pointers; make sure we have at least one
	num = 0;
	while( src[num] != 0 ) {
		++num;
	}

	if( num < 1 ) {
		err = E_NO_STRING;
		return( 0 );
	}

	// Create the shader object
	id = glCreateShader( type );

	// Verify that we were able to get an ID
	if( id == 0 ) {
		switch( type ) {
		case GL_VERTEX_SHADER:    err = E_VS_ALLOC; break;
		case GL_FRAGMENT_SHADER:  err = E_FS_ALLOC; break;
		case GL_GEOMETRY_SHADER:  err = E_GS_ALLOC; break;
		default:                  err = E_US_ALLOC; break;
		}
		return( 0 );
	}

	// Attach the source to the shaders
	glShaderSource( id, num, src, nullptr );

	// Compile the source, and print any relevant message logs
	glCompileShader( id );
	glGetShaderiv( id, GL_COMPILE_STATUS, &flag );
	printShaderInfoLog( id );

	// If the creation failed, dump the shader object
	if( flag == GL_FALSE ) {
		glDeleteShader( id );
		switch( type ) {
		case GL_VERTEX_SHADER:    err = E_VS_COMPILE; break;
		case GL_FRAGMENT_SHADER:  err = E_FS_COMPILE; break;
		case GL_GEOMETRY_SHADER:  err = E_GS_COMPILE; break;
		default:                  err = E_US_COMPILE; break;
		}
		return( 0 );
	}

	return( id );
}

///
/// shaderLink( GLuint ids[], size_t num, ShaderError &err )
///
/// Link a collection of shaders into a shader program.
///
/// @param ids   array of shader object ids
/// @param num   number of elements in the array
/// @param err   reference to status variable
/// @return      id of the new shader program, or 0 on error, with
///              an error code in err
///
GLuint shaderLink( GLuint ids[], size_t num, ShaderError &err ) {
	GLuint prog;
	GLint flag;

	if( num < 2 ) {
		err = E_NO_OBJECTS;
		return( 0 );
	}

	// Create the program and attach the shaders
	prog = glCreateProgram();

	if( prog == 0 ) {
		err = E_PROG_ALLOC;
		return( 0 );
	}

	for( int i = 0; i < num; ++i ) {
		glAttachShader( prog, ids[i] );
	}

	// Report any message log information
	printProgramInfoLog( prog );

	// Link the program, and print any message log information
	glLinkProgram( prog );
	glGetProgramiv( prog, GL_LINK_STATUS, &flag );
	printProgramInfoLog( prog );
	if( flag == GL_FALSE ) {
		err = E_PROG_LINK;
		glDeleteProgram( prog );
		return( 0 );
	}

	return( prog );
}

///
/// shaderSetupStr(vertex,fragment,geometry,err)
///
/// Set up a GLSL shader program.
///
/// Requires three strings containing the source code for GLSL vertex,
/// fragment, and geometry shaders.  Returns a program ID for the linked
/// program.
///
/// @param  vsrc   vertex shader program source code
/// @param  fsrc   fragment shader program source code, or NULL
/// @param  gsrc   geometry shader program source code, or NULL
/// @param  err    reference to status variable
/// @return shader program handle, or 0
///
/// On success:
///      Returns the GLSL shader program handle, and sets the 'err'
///      parameter to E_NO_ERROR.
///
/// On failure:
///      Returns 0, and assigns an error code to 'err'.
///
GLuint shaderSetupStr( const GLchar *vsrc, const GLchar *fsrc,
					   const GLchar *gsrc, ShaderError &err ) {
	GLuint vs, fs, gs, prog;
	const GLchar *src[2];
	int nshaders = 1;

	// must have a vertex shader!
	if( vsrc == nullptr ) {
		err = E_NO_VSHADER;
		return( 0 );
	}

	// from here on, other functions will set 'err' if something fails
	err = E_NO_ERROR;

	// Create the shader objects
	src[0] = vsrc;
	src[1] = 0;

	vs = shaderCreate( src, GL_VERTEX_SHADER, err );

	if( vs == 0 ) {
		return( 0 );
	}

	// if we have a fragment shader, create it
	if( fsrc != nullptr ) {
		src[0] = fsrc;
		fs = shaderCreate( src, GL_FRAGMENT_SHADER, err );

		if( fs == 0 ) {
			glDeleteShader( vs );
			return( 0 );
		}

		nshaders += 1;
	}

	// if we have a geometry shader, create it
	if( gsrc != nullptr ) {
		src[0] = gsrc;
		gs = shaderCreate( src, GL_GEOMETRY_SHADER, err );

		if( gs == 0 ) {
			glDeleteShader( vs );
			if( fsrc != nullptr ) glDeleteShader( fs );
			return( 0 );
		}

		nshaders += 1;
	}

	// Create the program and attach the shaders

	// This declaration causes Visual Studio to complain bitterly,
	// so we will dynamically allocate the array instead.
	// GLuint ids[nshaders];
	GLuint *ids = new GLuint[nshaders];

	// always have a vertex shader
	ids[0] = vs;
	int n = 1;

	// do we have a fragment shader?
	if( fsrc != nullptr ) {
		ids[n++] = fs;
	}

	// do we have a geometry shader?
	if( gsrc != nullptr ) {
		ids[n++] = gs;
	}

	// sanity check!
	if( n != nshaders ) {
		cerr << "Internal error: nshaders " << nshaders
			<< " but only found " << n << " to link?" << endl;
		glDeleteShader( vs );
		if( fsrc != nullptr ) glDeleteShader( fs );
		if( gsrc != nullptr ) glDeleteShader( gs );
		delete [] ids;
		return( 0 );
	}

	// OK, we have all the shaders; create the program
	prog = shaderLink( ids, n, err );

	if( prog == 0 ) {
		glDeleteShader( vs );
		if( fsrc != nullptr ) glDeleteShader( fs );
		if( gsrc != nullptr ) glDeleteShader( gs );
	}

	delete [] ids;
	return( prog );
}

///
/// shaderSetup(vertex,fragment,err)
///
/// Set up a GLSL shader program.
///
/// Requires the names of vertex and fragment program source
/// files.  Returns a handle to the created GLSL program.
///
/// @param  vert   vertex shader program source file
/// @param  frag   fragment shader program source file, or NULL
/// @param  err    reference to status variable
/// @return shader program handle, or 0
///
/// On success:
///      Returns the GLSL shader program handle, and sets the 'err'
///      parameter to E_NO_ERROR.
///
/// On failure:
///      Returns 0, and assigns an error code to 'err'.
///
GLuint shaderSetup( const char *vert, const char *frag, ShaderError &err ) {
	// cheat - call the three-shader version with a NULL pointer
	return( shaderSetup(vert,frag,nullptr,err) );
}

///
/// shaderSetup(vertex,fragment,geometry,err)
///
/// Set up a GLSL shader program.
///
/// Requires the names of vertex and fragment program source
/// files.  Returns a handle to the created GLSL program.
///
/// @param  vert   vertex shader program source file
/// @param  frag   fragment shader program source file, or NULL
/// @param  geom   geometry shader program source file, or NULL
/// @param  err    reference to status variable
/// @return shader program handle, or 0
///
/// On success:
///      Returns the GLSL shader program handle, and sets the 'err'
///      parameter to E_NO_ERROR.
///
/// On failure:
///      Returns 0, and assigns an error code to 'err'.
///
GLuint shaderSetup( const char *vert, const char *frag,
					const char *geom, ShaderError &err ) {
	GLchar *vsrc = nullptr, *fsrc = nullptr, *gsrc = nullptr;

	// Must have a vertex shader
	vsrc = readTextFile( vert );
	if( vsrc == nullptr ) {
		cerr << "Error reading vertex shader file " << vert << endl;
		err = E_VS_LOAD;
		return( 0 );
	}

	if( frag != nullptr ) {
		fsrc = readTextFile( frag );
		if( fsrc == nullptr ) {
			cerr << "Error reading fragment shader file " << frag << endl;
			err = E_FS_LOAD;
			delete [] vsrc;
			return( 0 );
		}
	}

	if( geom != nullptr ) {
		gsrc = readTextFile( geom );
		if( gsrc == nullptr ) {
			cerr << "Error reading geometry shader file " << geom << endl;
			err = E_GS_LOAD;
			delete [] vsrc;
			if( fsrc != nullptr ) delete [] fsrc;
			return( 0 );
		}
	}

	// Do the actual setup
	GLuint ret = shaderSetupStr( vsrc, fsrc, gsrc, err );

	// Whatever happened, we're done with the source code now
	delete [] vsrc;
	if( fsrc != nullptr ) delete [] fsrc;
	if( gsrc != nullptr ) delete [] gsrc;

	return( ret );
}
