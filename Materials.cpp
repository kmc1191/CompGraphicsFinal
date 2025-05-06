//
//  Materials.cpp
//
//  Simple class for setting up material property parameters.
//
//  Created by Warren R. Carithers 2021/11/11
//  Based on code created by Joe Geigel on 1/23/13.
//  Copyright 2021 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Kaitlynn Clement
//

#include <iostream>

#include "Materials.h"

#include "Models.h"
#include "Lighting.h"
#include "Utils.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

// this is here in case you are using SOIL;
// if you're not, it can be deleted.
#include <SOIL/SOIL.h>

using namespace std;

// reflective coefficients (order depends upon the Object type in Models.h)
static glm::vec3 k( 0.5, 0.7f, 1.0f );

// specular highlights (order depends upon the Object type in Models.h)
static GLfloat specExp[N_OBJECTS] = {
	50.0f,  // Quad
	20.0f,  // Cylinder
	20.0f,   // Discs
        20.0f   // Discs
};

// These variables are used in the framework only when doing Flat shading
static glm::vec4 cyl_ambient(     0.50f, 0.10f, 0.90f, 1.00f );
static glm::vec4 cyl_diffuse(     0.89f, 0.00f, 0.00f, 1.00f );
static glm::vec4 quad_ambdiffuse( 0.10f, 0.85f, 0.20f, 1.00f );

// Add any global definitions and/or variables you need here.
static GLuint quadFrontTexture = 0;
static GLuint quadBackTexture = 0;
static GLuint cylinderTexture = 0;
static GLuint discTexture = 0;

//New textures
static GLuint brickTexture = 0;
static GLuint dirtyTileTexture = 0;
static GLuint metalRoofTexture = 0;
static GLuint redWoodTexture = 0;
static GLuint roadTexture = 0;
static GLuint roofTexture = 0;


///
/// Load an image into the current texture unit
///
/// @param file the image file to be loaded
/// @return the texture ID, or 0 on error
///
static GLuint load_texture( const char *file ) {
	GLuint tid;
	unsigned char *image;
	int width, height, channels;

	glGenTextures( 1, &tid );
	glBindTexture( GL_TEXTURE_2D, tid );

	checkErrors( "load_texture gen/bind" );

	image = SOIL_load_image( file, &width, &height, &channels, SOIL_LOAD_AUTO );

	checkErrors( "load_texture SOIL" );

	if( image == nullptr ) {
		cerr << "SOIL loading error for '" << file << "': '"
			 << SOIL_last_result() << "'" << endl;
		return 0;
	}

	// We need to invert the image, because the pixels in it are
	// organized with (0,0) at the upper-left corner, and our
	// virtual universe has (0,0) at the lower-left corner.

	for( int j = 0; j*2 < height; ++j ) {
		int ix1 = j * width * channels;
		int ix2 = (height - 1 - j) * width * channels;
		for( int i = width * channels; i > 0; --i ) {
			unsigned char temp = image[ix1];
			image[ix1] = image[ix2];
			image[ix2] = temp;
			++ix1;
			++ix2;
		}
	}

        
	//
	// You will need to add code here to load the image into the
	// texture object, free the dynamic storage returned from SOIL,
	// and set any other texture parameters (e.g., texture wrapping
	// and filtering).
	//

        // Upload the texture to the GPU
        glTexImage2D(
                GL_TEXTURE_2D,      // target
                0,                  // level (0 = base level)
                GL_RGBA,            // internal format you want in GPU
                width,              // width of the texture
                height,             // height of the texture
                0,                  // border (must be 0)
                (channels == 4 ? GL_RGBA : GL_RGB), // format of the loaded image
                GL_UNSIGNED_BYTE,   // data type
                image               // pointer to image data
        );

        // Set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // horizontal wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  // vertical wrapping

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // when shrinking
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // when enlarging

        // Free the image memory
        SOIL_free_image_data(image);

	checkErrors( "load_texture finish" );

	return tid;
}

///
/// This function initializes all texture-related data structures for
/// the program.  This is where texture buffers should be created, where
/// texture images should be loaded, etc.
///
/// You will need to write this function, and maintain all of the values
/// needed to be sent to the various shaders.
///
void initTextures( void )
{
        //Load all texture images
        quadFrontTexture = load_texture( "obverse.png" );
        quadBackTexture = load_texture( "reverse.png" );
        cylinderTexture = load_texture( "wall.png" );
        discTexture = load_texture( "disc.png" );

        //New texture images
        brickTexture = load_texture( "brickWallTexture.png" );
        dirtyTileTexture = load_texture( "dirtyTilesTexture.png" );
        metalRoofTexture = load_texture( "metalRoofTexture.png" );
        redWoodTexture = load_texture( "redWoodPlanks.png" );
        roadTexture = load_texture( "roadTexture.png" );
        roofTexture = load_texture( "roofTexture.png" );


}

///
/// This function sets up the appearance parameters for the object.
///
/// @param program        The ID of an OpenGL (GLSL) shader program to which
///                       parameter values are to be sent
/// @param obj            The object type of the object being drawn
/// @param usingTextures  Are we texture-mapping this object?
///
void setMaterials( GLuint program, Object obj, bool usingTextures )
{
	///////////////////////////////////////////////////
	// CODE COMMON TO FLAT SHADING AND TEXTURE MAPPING
	//
	// DO NOT REMOVE THIS SECTION OF CODE
	///////////////////////////////////////////////////

	// Send down the reflective coefficients
	GLint loc = getUniformLoc( program, "kCoeff" );
	if( loc >= 0 ) {
		glUniform3fv( loc, 1, glm::value_ptr(k) );
	}

	///////////////////////////////////////////////////////////
	// CODE DIFFERING BETWEEN FLAT SHADING AND TEXTURE MAPPING
	///////////////////////////////////////////////////////////

	//
	// Check to see whether or not we are using texture mapping
	// on the object being drawn at the moment.
	//

	if( !usingTextures ) {

		///////////////////////////////////////////////////////////
		// THIS CODE IS USED ONLY IF TEXTURE MAPPING IS ***NOT***
		// BEING DONE ON THIS OBJECT
		///////////////////////////////////////////////////////////

		// ambient and diffuse vary from one object to another
		GLint dloc = getUniformLoc( program, "diffuseColor" );
		GLint aloc = getUniformLoc( program, "ambientColor" );

		switch( obj ) {
		case Quad:
			if( aloc >= 0 ) {
				glUniform4fv( aloc, 1, glm::value_ptr(quad_ambdiffuse) );
			}
			if( dloc >= 0 ) {
				glUniform4fv( dloc, 1, glm::value_ptr(quad_ambdiffuse) );
			}
			break;
		case Cylinder: // FALL THROUGH
		case Discs:
			if( aloc >= 0 ) {
				glUniform4fv( aloc, 1, glm::value_ptr(cyl_ambient) );
			}
			if( dloc >= 0 ) {
				glUniform4fv( dloc, 1, glm::value_ptr(cyl_diffuse) );
			}
			break;
		}

		return;
	}

	///////////////////////////////////////////////////////////
	// CODE FROM THIS POINT ON WILL BE USED ONLY IF TEXTURE
	// MAPPING IS BEING DONE ON THIS OBJECT
	///////////////////////////////////////////////////////////

	// Set the specular exponent for the object
	loc  = getUniformLoc( program, "specExp" );
	switch( obj ) {
	case Quad:
		glUniform1f( loc, specExp[Quad] );
		break;
	case Cylinder:
		glUniform1f( loc, specExp[Cylinder] );
		break;
	case Discs:
		glUniform1f( loc, specExp[Discs] );
		break;
        case Sphere:
		glUniform1f( loc, specExp[Sphere] );
		break;
	}

	//
	// You will need to add code here to send the necessary information
	// to your shader program to allow the program to access the texture
	// data you've put into your texture units.
	//

        ///////////////////////////////////////////////////////////
        // Bind textures for each object (Quad, Cylinder, Discs)
        ///////////////////////////////////////////////////////////
        GLint texLoc;

        switch( obj ) {
        case Quad:
                //Front texture for disk
                glActiveTexture(GL_TEXTURE0);  // Texture unit 0
                glBindTexture(GL_TEXTURE_2D, quadFrontTexture);  // Front texture
                texLoc = getUniformLoc(program, "mainTex");
                if (texLoc >= 0) {
                        glUniform1i(texLoc, 0);  // Set the sampler to texture unit 0
                }

                //Back Texture for disk
                glActiveTexture(GL_TEXTURE1);  // Texture unit 1
                glBindTexture(GL_TEXTURE_2D, quadBackTexture);  // Back texture
                texLoc = getUniformLoc(program, "backTex");
                if (texLoc >= 0) {
                glUniform1i(texLoc, 1);  // Set the sampler to texture unit 1
                }
                break;

        case Cylinder:
                glActiveTexture(GL_TEXTURE0);  // Texture unit 0
                glBindTexture(GL_TEXTURE_2D, cylinderTexture);  // Front texture
                texLoc = getUniformLoc(program, "mainTex");
                if (texLoc >= 0) {
                        glUniform1i(texLoc, 0);  // Set the sampler to texture unit 0
                }

                //Send same texture for back
                glActiveTexture(GL_TEXTURE1);  // Texture unit 1
                glBindTexture(GL_TEXTURE_2D, cylinderTexture);  // Back texture
                texLoc = getUniformLoc(program, "backTex");
                if (texLoc >= 0) {
                glUniform1i(texLoc, 1);  // Set the sampler to texture unit 1
                }
                break;

        case Discs:
                glActiveTexture(GL_TEXTURE0);  // Texture unit 0
                glBindTexture(GL_TEXTURE_2D, discTexture);  // Front texture
                texLoc = getUniformLoc(program, "mainTex");
                if (texLoc >= 0) {
                        glUniform1i(texLoc, 0);  // Set the sampler to texture unit 0
                }

                //Send same texture for back
                glActiveTexture(GL_TEXTURE1);  // Texture unit 1
                glBindTexture(GL_TEXTURE_2D, discTexture);  // Back texture
                texLoc = getUniformLoc(program, "backTex");
                if (texLoc >= 0) {
                glUniform1i(texLoc, 1);  // Set the sampler to texture unit 1
                }
                break;
        case Sphere:
                glActiveTexture(GL_TEXTURE0);  // Texture unit 0
                glBindTexture(GL_TEXTURE_2D, metalRoofTexture);  // Front texture
                texLoc = getUniformLoc(program, "mainTex");
                if (texLoc >= 0) {
                        glUniform1i(texLoc, 0);  // Set the sampler to texture unit 0
                }

                //Send same texture for back
                glActiveTexture(GL_TEXTURE1);  // Texture unit 1
                glBindTexture(GL_TEXTURE_2D, metalRoofTexture);  // Back texture
                texLoc = getUniformLoc(program, "backTex");
                if (texLoc >= 0) {
                glUniform1i(texLoc, 1);  // Set the sampler to texture unit 1
                }
                break;
        }

}
