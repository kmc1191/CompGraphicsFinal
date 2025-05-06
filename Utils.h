//
//  Utils.h
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

#ifndef UTILS_H_
#define UTILS_H_

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
void checkErrors( const char *msg );

///
/// Dump the list of active attributes and uniforms from a shader program
///
/// @param desc  description of the program
/// @param prog  which shader program to query
///
void dumpActives( const char *desc, GLuint prog );

///
/// Retrieve a Uniform variable's location and verify the result
///
/// @param prog  the shader program
/// @param name  the name of the desired variable
///
GLint getUniformLoc( GLuint prog, const GLchar *name );

///
/// Retrieve an Attribute variable's location and verify the result
///
/// @param prog  the shader program
/// @param name  the name of the desired variable
///
GLint getAttribLoc( GLuint prog, const GLchar *name );

#endif
