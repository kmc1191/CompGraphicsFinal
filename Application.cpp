//
//  Application.cpp
//
//  Assignment-specific code.
//
//  Created by Warren R. Carithers on 2019/09/09.
//  Updates: 2021/11/11 by wrc
//  Based on earlier versions created by Joe Geigel and Warren R. Carithers
//  Copyright 2021 Rochester Institute of Technology. All rights reserved.
//
//  This file should not be modified by students.
//

#include <cstring>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

//
// GLEW and GLFW header files also pull in the OpenGL definitions
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Application.h"

#include "Buffers.h"
#include "Canvas.h"
#include "Lighting.h"
#include "Materials.h"
#include "Models.h"
#include "ShaderSetup.h"
#include "Testing.h"
#include "Types.h"
#include "Utils.h"
#include "Viewing.h"

using namespace std;

//
// PRIVATE GLOBALS
//

// names of our GLSL shader files
static const char *vs_f = "f150.vert";
static const char *fs_f = "f150.frag";
static const char *vs_t = "texture.vert";
static const char *fs_t = "texture.frag";

// our Canvas
static Canvas *canvas;

// buffers for our shapes
static BufferSet buffers[N_OBJECTS];

// shader program handles
static GLuint flat, texture;

// our VAO
static GLuint vao;

// do we need to do a display() call?
static bool updateDisplay = true;

// animation control
static bool animating[N_OBJECTS];  // individual animation flags
static float angles[N_OBJECTS];    // individual rotation angles

// which object(s) to texture map
static bool map_obj[N_OBJECTS];

// object transformations
// static glm::vec3 quad_s( 1.75f,  1.75f,  1.75f );
// static glm::vec3 quad_x( -1.25f, 0.5f, -1.5f );
// static glm::vec3 cyl_s( 1.5f,  1.5f,  1.5f );
// static glm::vec3 cyl_x( 1.0f,  0.5f, -1.5f );

// New transformations
glm::vec3 road_s(20.0f, 1.0f, 20.0f);
glm::vec3 road_x(0.0f, -0.01f, 0.0f); 
glm::vec3 road_r(-90.0f, 0.0f, 0.0f);

glm::vec3 siloBody_s(0.5f, 3.0f, 0.5f);
glm::vec3 siloBody_x(-2.0f, 0.0f, -2.0f);

glm::vec3 siloRoof_s(0.5f, 3.0f, 0.5f);
glm::vec3 siloRoof_x(-2.0f, 0.0f, -2.0f);

glm::vec3 mainBarnBody_s(2.0f, 1.5f, 2.0f);
glm::vec3 mainBarnBody_x(0.0f, 0.0f, -2.0f);

glm::vec3 mainBarnRoof_s(2.2f, 1.0f, 2.2f);
glm::vec3 mainBarnRoof_x(0.0f, 2.0f, -2.0f);

glm::vec3 altBarnBody_s(2.0f, 1.5f, 2.0f);
glm::vec3 altBarnBody_x(0.0f, 0.0f, -2.0f);

glm::vec3 altBarnRoof_s(2.2f, 1.0f, 2.2f);
glm::vec3 altBarnRoof_x(0.0f, 2.0f, -2.0f);

glm::vec3 miniBarnBody_s(2.0f, 1.5f, 2.0f);
glm::vec3 miniBarnBody_x(0.0f, 0.0f, -2.0f);

glm::vec3 miniBarnRoof_s(2.2f, 1.0f, 2.2f);
glm::vec3 miniBarnRoof_x(0.0f, 2.0f, -2.0f);

// ALL ABOVE IS TODO!!!


// light animation
static bool animateLight = false;
static float delta = 1.0f;
static const float lightMax = 50.0f, lightMin = -50.0f;

//
// PUBLIC GLOBALS
//

//
// Drawing-related variables
//

// dimensions of the drawing window
int w_width  = 600;
int w_height = 600;

// drawing window title
const char *w_title = "Texture Mapping";

// GL context we're using (we assume 3.2, for GLSL 1.50)
int gl_maj = 3;
int gl_min = 2;

// our GLFWwindow
GLFWwindow *w_window;

//
// PRIVATE FUNCTIONS
//

///
/// Create our shapes
///
/// @param C  - the Canvas to use when drawing the objects
///
static void createImage( Canvas &C )
{
    // Dont draw these
	// createObject( C, Quad,     buffers[Quad] );
	// createObject( C, Cylinder, buffers[Cylinder] );
	// createObject( C, Discs,    buffers[Discs] );

    createObject( C, SiloBody, buffers[SiloBody] );
    createObject( C, SiloRoof,   buffers[SiloRoof] );
    createObject( C, MainBarnBody, buffers[MainBarnBody] );
    createObject( C, MainBarnRoof, buffers[MainBarnRoof] );
    createObject( C, AltBarnBody, buffers[AltBarnBody] );
    ceateObject( C, AltBarnRoof, buffers[AltBarnRoof] );
    ceateObject( C, MiniBarnBody, buffers[MiniBarnBody] );
    ceateObject( C, MiniBarnRoof, buffers[MiniBarnRoof] );
    createObject( C, Floor,     buffers[Floor]);
}

///
/// Increment or reset a rotation angle.
///
/// @param obj  the object being rotated
///
// static void rotate( int obj, float amt ) {
// 	angles[obj] += amt;
// 	if( angles[obj] >= 360.0f ) {
// 		angles[obj] = 0.0f;
// 	}
// }

///
/// Animation routine
///
// SHOULD NOT BE USED
static void animate( void ) {

	// if( animating[Quad] ) {
	// 	rotate( Quad, 2.0f );
	// 	updateDisplay = true;
	// }

	// if( animating[Cylinder] ) {
	// 	rotate( Cylinder, 1.0f );
	// 	rotate( Discs, 1.0f );
	// 	updateDisplay = true;
	// }

	// if( animateLight ) {
	// 	if( (delta > 0.0f && lightpos.x >= lightMax) ||
	// 		(delta < 0.0f && lightpos.x <= lightMin) ) {
	// 		delta *= -1.0f;
	// 	}
	// 	lightpos.x += delta;
	// 	updateDisplay = true;
	// }
}

//
// Event callback routines for this assignment
//

///
/// Handle keyboard input
///
/// @param window   GLFW window being used
/// @param key      code for the key that was pressed
/// @param scan     scan code of the key
/// @param action   type of event (press, release, etc.)
/// @param mods     modifiers in use (shift, etc.)
///
static void keyboard( GLFWwindow *window, int key, int scan,
					  int action, int mods )
{
	bool any_anim = false;

	// only react to "press" events
	if( action != GLFW_PRESS ) {
		return;
	}

	switch( key ) {

	// termination
	case GLFW_KEY_ESCAPE:  // FALL THROUGH
	case GLFW_KEY_X:
		glfwSetWindowShouldClose( w_window, 1 );
		// return without updating the display
		return;
		// NOTREACHED

	// animation control

	// case GLFW_KEY_A:    // start/stop all object animation
	// 	// are we currently animating anything?
	// 	any_anim = false;
	// 	for( int i = 0; i < N_OBJECTS; ++i ) {
	// 		any_anim = any_anim || animating[i];
	// 	}
	// 	// invert the condition
	// 	any_anim = !any_anim;
	// 	// propogate it
	// 	for( int i = 0; i < N_OBJECTS; ++i ) {
	// 		animating[i] = any_anim;
	// 	}
	// 	break;

	// case GLFW_KEY_Q: // start/stop animating the quad
	// 	animating[Quad] = !animating[Quad];
	// 	break;

	// case GLFW_KEY_C: // start/stop animating the cylinder and discs
	// 	animating[Cylinder] = !animating[Cylinder];
	// 	animating[Discs] = !animating[Discs];
	// 	break;

	case GLFW_KEY_L:    // start/stop animating the light
		animateLight = !animateLight;
		break;

	// light position

	case GLFW_KEY_I: // move the light into the scene
		lightpos[2] -= 2.0f;
		break;

	case GLFW_KEY_O: // move the light out from the scene
		lightpos[2] += 2.0f;
		break;

	// print out potentially useful information

	// case GLFW_KEY_R: // rotation angles
	// 	cerr << "Rotation: quad " << angles[Quad]
	// 		 << ", cyl/disc " << angles[Cylinder]
	// 		 << endl;
	// 	break;

	case GLFW_KEY_P: // light position
		cerr << "Light is at (" << lightpos[0] << "," << lightpos[1]
			 << "," << lightpos[2] << ")" << endl;
		break;

	// Reset parameters

	// case GLFW_KEY_1: // reset all object rotations
	// 	angles[Quad] = angles[Cylinder]
	// 				 = angles[Discs]
	// 				 = 0.0f;
	// 	animating[Quad] = animating[Cylinder]
	// 					= animating[Discs]
	// 					= false;
	// 	break;

	case GLFW_KEY_2: // reset light position
		lightpos[0] = lpDefault[0];
		lightpos[1] = lpDefault[1];
		lightpos[2] = lpDefault[2];
		lightpos[3] = lpDefault[3];
		animateLight = false;
		break;

	// help message
	case GLFW_KEY_H:
		cout << "  Key(s)             Action" << endl;
		cout << "=========   =======================" << endl;
		cout << "ESC, x, X   Terminate the program" << endl;
		cout << "  h, H      Print this message" << endl;
		cout << "  l, L      Toggle light animation" << endl;
		cout << "  i, I      Move light 'into' the scene"
			<< " (toward the objects)" << endl;
		cout << "  o, O      Move light 'out' from the scene"
			<< " (away from the objects)" << endl;
		cout << "  p, P      Print light position" << endl;
		cout << "   2        Reset light position, stop animation" << endl;
		// return without updating the display
		return;
		// NOTREACHED

	default:
		cerr << "Unknown keycode " << key << " ignored" << endl;
		// return without updating the display
		return;
		// NOTREACHED
	}

	updateDisplay = true;
}

///
/// Set the titlebar in the display window
///
void setTitle( void ) {
	char buf[128];

	strcpy( buf, "Texture mapping:" );
	// if( map_obj[Quad] )     strcat( buf, " Quad" );
	// if( map_obj[Cylinder] ) strcat( buf, " Cylinder" );
	// if( map_obj[Discs] )    strcat( buf, " Discs" );

	glfwSetWindowTitle( w_window, buf );
}

///
/// Display the current image
///
static void display( void )
{
	// clear the frame buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// set our titlebar
	setTitle();

	// check for any errors to this point
	checkErrors( "display init" );

	// draw the individual objects
	for( int obj = SiloBody; obj < N_OBJECTS; ++obj ) {

		// select the proper shader program
		GLuint program = map_obj[obj] ? texture : flat;
		glUseProgram( program );

		// set up the common transformations
		setCamera( program );
		setProjection( program );
		checkErrors( "display camera" );

		// and our lighting
		setLighting( program );
		checkErrors( "display lighting" );

		// set texture parameters OR material properties
		setMaterials( program, (Object) obj, map_obj[obj] );
		checkErrors( "display materials" );

		// select the correct rotation angles
		glm::vec3 ang( angles[obj], angles[obj], angles[obj] );

		// send all the transformation data
		switch( obj ) {

        case SiloBody:
            setTransforms( program, siloBody_s, ang, siloBody_x );
			break;
        case SiloRoof:
            setTransforms( program, siloRoof_s, ang, siloRoof_x );
			break;


        case MainBarnBody:
            setTransforms( program, mainBarnBody_s, ang, mainBarnBody_x );
			break;
        case MainBarnRoof:
            setTransforms( program, mainBarnRoof_s, ang, mainBarnRoof_x );
			break;


        case AltBarnBody:
            setTransforms( program, altBarnBody_s, ang, altBarnBody_x );
			break;
        case AltBarnRoof:
            setTransforms( program, altBarnRoof_s, ang, altBarnRoof_x );
			break;


        case MiniBarnBody:
            setTransforms( program, miniBarnBody_s, ang, miniBarnBody_x );
			break;
        case MiniBarnRoof:
            setTransforms( program, miniBarnRoof_s, ang, miniBarnRoof_x );
			break;


        case Floor:
            setTransforms( program, road_s, road_r, road_x );
			break;

		}

		checkErrors( "display xforms" );

		// draw it
		buffers[obj].selectBuffers( program,
			"vPosition", NULL, "vNormal", 
			map_obj[obj] ? "vTexCoord" : NULL );
		checkErrors( "display select" );

		glDrawElements( GL_TRIANGLES, buffers[obj].numElements,
						GL_UNSIGNED_INT, (void *) 0 );
		checkErrors( "display draw" );
	}
}

///
/// OpenGL initialization
///
/// @return boolean indicating status of initialization
///
static bool init( void )
{
	// Check the OpenGL major version
	if( gl_maj < 3 ) {
		cerr << "Caution: OpenGL " << gl_maj << "." << gl_min
			<< " does not support GLSL 1.30 or newer" << endl;
	} else if( gl_maj == 3 && gl_min < 2 ) {
		// select the other Flat shader
		vs_f = "f130.vert";
		fs_f = "f130.frag";
		// warn about GLSL versions
		cerr << "Caution: OpenGL " << gl_maj << "." << gl_min
			<< " does not support GLSL 1.50 or newer" << endl;
	}

	checkErrors( "init start" );

	// Load shaders and use the resulting shader program
	ShaderError error;
	flat = shaderSetup( vs_f, fs_f, error );
	if( !flat ) {
		cerr << "Error setting up flat shader - "
			 << errorString(error) << endl;
		return( false );
	}
	checkErrors( "init shaders 1" );

	texture = shaderSetup( vs_t, fs_t, error );
	if( !texture ) {
		cerr << "Error setting up texture shader - "
			 << errorString(error) << endl;
		return( false );
	}
	checkErrors( "init shaders 2" );

#ifdef DEBUG
	// Define the CPP symbol 'DEBUG' and recompile to enable the compilation
	// of this code into your program for debugging purposes

	// Dump the list of active global variables in the shader programs
	dumpActives( "Flat shader", flat );
	dumpActives( "Texture shader", texture );
	checkErrors( "init actives" );
#endif

	// create our Canvas
	canvas = new Canvas( w_width, w_height );
	if( canvas == NULL ) {
		cerr << "Error - cannot create Canvas" << endl;
		return( false );
	}

	// need a VAO if we're using a core context;
	// doesn't hurt even if we're not using one
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	checkErrors( "init vao" );

	// OpenGL state initialization
	glEnable( GL_DEPTH_TEST );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDepthFunc( GL_LEQUAL );
	glClearDepth( 1.0f );
	checkErrors( "init setup" );

	// create the geometry for our shapes.
	createImage( *canvas );
	checkErrors( "init image" );

	// initialize all texture-related things
	initTextures();
	checkErrors( "init textures" );

	// register our callbacks
	glfwSetKeyCallback( w_window, keyboard );
	checkErrors( "init callback" );

	return( true );
}

//
// PUBLIC FUNCTIONS
//

///
/// application-specific processing
///
void application( void )
{
	// set up the map_obj array
	for( int i = 0; i < N_OBJECTS; ++i ) {
		if( (which & (1<<i)) != 0 ) {
			map_obj[i] = true;
			cout << "Texture mapping " << objects[i] << endl;
		} else {
			cout << "Flat shading " << objects[i] << endl;
		}
	}

	// set up the objects and the scene
	if( !init() ) {
		return;
	}

	checkErrors( "after init" );

	// loop until it's time to quit
	while( !glfwWindowShouldClose(w_window) ) {
		animate();
		if( updateDisplay ) {
			updateDisplay = false;
			display();
			glfwSwapBuffers( w_window );
			checkErrors( "event loop" );
		}
		glfwPollEvents();
	}
}
