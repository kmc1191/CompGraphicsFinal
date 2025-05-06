//
//  Models.cpp
//
//  Assignment-specific code for objects.  This module will typically be
//  heavily modified to fit the needs of each assignment.
//
//  Created by Warren R. Carithers on 2021/09/02.
//  Based on earlier versions created by Joe Geigel and Warren R. Carithers
//  Copyright 2021 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Kaitlynn Clement
//

#include <iostream>
#include <cmath>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Models.h"

// data for the three objects
#include "CylinderData.h"
#include "QuadData.h"

using namespace std;

//
// PRIVATE GLOBALS
//

//
// PUBLIC GLOBALS
//

// object names (must match the sequence in Models.h)
const char *objects[ N_OBJECTS ] = {
	"Quad"
	, "Cylinder"
        , "Sphere"      //TODO
	, "Discs"
};

//
// PRIVATE FUNCTIONS
//

///
/// makeQuad() - create a quad object
///
static void makeQuad( Canvas &C )
{
	for( int i = 0; i < quadElementsLength - 2; i += 3 ) {

		// Calculate the base indices of the three vertices
		int point1 = quadElements[i];
		int point2 = quadElements[i + 1];
		int point3 = quadElements[i + 2];

		Vertex p1 = quadVertices[point1];
		Vertex p2 = quadVertices[point2];
		Vertex p3 = quadVertices[point3];

		// Add this triangle to the collection
		C.addTriangleWithNorms( p1, quadNorm, p2, quadNorm, p3, quadNorm );

		// Add the texture coordinates
		C.addTextureCoords( quadUV[point1], quadUV[point2],
							quadUV[point3] );
	}
}

///
/// makeCylinder() - create the cylinder body
///
void makeCylinder( Canvas &C )
{
	// Only use the vertices for the body itself
	for( int i = body.first; i <= body.last - 2; i += 3 ) {

		// Calculate the base indices of the three vertices
		int point1 = cylinderElements[i];
		int point2 = cylinderElements[i + 1];
		int point3 = cylinderElements[i + 2];

		Vertex p1 = cylinderVertices[point1];
		Vertex p2 = cylinderVertices[point2];
		Vertex p3 = cylinderVertices[point3];

		// Calculate the normal vectors for each vertex
		Normal n1, n2, n3;

		// Normals on the body run from the axis to the vertex, and
		// are in the XZ plane; thus, for a vertex at (Px,Py,Pz), the
		// corresponding point on the axis is (0,Py,0), and the normal is
		// P - Axis, or just (Px,0,Pz).

		n1 = (Normal) { p1.x, 0.0f, p1.z };
		n2 = (Normal) { p2.x, 0.0f, p2.z };
		n3 = (Normal) { p3.x, 0.0f, p3.z };

		// Add this triangle to the collection
		C.addTriangleWithNorms( p1, n1, p2, n2, p3, n3 );

                // --------------

		// You will need to add code here to determine texture
		// coordinates for each vertex, and will need to call the
		// addTextureCoords() function to add those coordinates
		// to the Canvas.

                // Add the texture coordinates
                TexCoord uv1, uv2, uv3;

                float theta = atan2(p1.z, p1.x); // angle in radians
                float u = (theta + M_PI) / (2.0f * M_PI); // map from [-π, π] to [0,1]
                float v = (p1.y + 0.5f); // assuming y ranges from -0.5 to 0.5
                
                uv1 = (TexCoord) {u, v};

                theta = atan2(p2.z, p2.x); // angle in radians
                u = (theta + M_PI) / (2.0f * M_PI); // map from [-π, π] to [0,1]
                v = (p2.y + 0.5f); // assuming y ranges from -0.5 to 0.5
                
                uv2 = (TexCoord) {u, v};

                theta = atan2(p3.z, p3.x); // angle in radians
                u = (theta + M_PI) / (2.0f * M_PI); // map from [-π, π] to [0,1]
                v = (p3.y + 0.5f); // assuming y ranges from -0.5 to 0.5
                
                uv3 = (TexCoord) {u, v};

                C.addTextureCoords(uv1, uv2, uv3);
        }
}

///
/// makeSphere() - create the spehere body
///
void makeSphere( Canvas &C )
{
	
}

///
/// makeDiscs() - create the cylinder discs
///
static void makeDiscs( Canvas &C )
{
	// Only use the vertices for the top and bottom discs

	for( int disc = 0; disc < 2; ++disc ) {

		// Select the starting and ending indices, and create the surface
		// normal for this disc.  For the top and bottom, the normals are
		// parallel to the Y axis.  Points on the disk all have Y == 0.5,
		// and those on the bottom have Y == -0.5.
		int first, last;
		Normal nn;

		if( disc == 0 ) { // bottom disc
			first = bdisc.first;
			last  = bdisc.last;
			nn = (Normal) { 0.0f, -1.0f, 0.0f };
		} else {
			first = tdisc.first;
			last  = tdisc.last;
			nn = (Normal) { 0.0f, 1.0f, 0.0f };
		}

		// Create the triangles
		for( int i = first; i <= last - 2; i += 3 ) {

			// Calculate the base indices of the three vertices
			int point1 = cylinderElements[i];
			int point2 = cylinderElements[i + 1];
			int point3 = cylinderElements[i + 2];

			Vertex p1 = cylinderVertices[point1];
			Vertex p2 = cylinderVertices[point2];
			Vertex p3 = cylinderVertices[point3];

			// Add this triangle to the collection
			C.addTriangleWithNorms( p1, nn, p2, nn, p3, nn );

                        // --------------

			// You will need to add code here to determine texture
			// coordinates for each vertex, and will need to call the
			// addTextureCoords() function to add those coordinates
			// to the Canvas.  Use planar mapping; remember that Y
			// is constant for all the disc vertices, and remember that
			// the disc coordinates range from -0.5 to 0.5 in X and Z,
			// but texture coordinates need to range from 0.0 to 1.0.

                        float u1 = p1.x + 0.5f;
                        float v1 = p1.z + 0.5f;

                        float u2 = p2.x + 0.5f;
                        float v2 = p2.z + 0.5f;

                        float u3 = p3.x + 0.5f;
                        float v3 = p3.z + 0.5f;

                        C.addTextureCoords( TexCoord {u1, v1}, TexCoord {u2, v2}, TexCoord {u3, v3} );
		}
	}
}

//
// PUBLIC FUNCTIONS
//

///
/// Create an object
///
/// @param C      the Canvas we'll be using
/// @param obj    which object to draw
/// @param buf    BufferSet to use for the object
///
void createObject( Canvas &C, Object obj, BufferSet &buf )
{
	// start with a fresh Canvas
	C.clear();

	// create the specified object
	switch( obj ) {
	case Quad:      makeQuad( C );      break;
	case Cylinder:  makeCylinder( C );  break;
	case Discs:     makeDiscs( C );     break;
        case Sphere:    makeSphere( C );    break;
	default:
		cerr << "createObject(" << obj << ") - unknown object type" << endl;
		return;
	}

	// create the buffers for the object
	buf.createBuffers( C );
}
