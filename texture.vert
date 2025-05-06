#version 150

// Texture shader skeleton

//
// Texture vertex shader
//
// @author  RIT CS Department
// @author  Kaitlynn Clement
//

//
// Vertex attributes
//

// Vertex position (in model space)
in vec4 vPosition;

// Normal vector at vertex (in model space)
in vec3 vNormal;

// Texture coordinate for this vertex
in vec2 vTexCoord;

//
// Uniform data
//

// Camera and projection matrices
uniform mat4 viewMat;   // view (camera)
uniform mat4 projMat;   // projection

uniform mat4 modelMat;  // composite

// Light position is given in world space
uniform vec4 lightPosition;

// OUTGOING DATA

// Vectors to "attach" to vertex and get sent to fragment shader
// Vectors and points will be passed in "eye" space
out vec3 lPos;
out vec3 vPos;
out vec3 vNorm;

// ADD ANY OUTGOING VARIABLES YOU NEED HERE
out vec2 texCoord;


void main()
{
	// transform our positions
	mat4 mvMat = viewMat * modelMat;

	vPos = vec3(mvMat * vPosition);
	lPos = vec3(viewMat * lightPosition);

	// transform the normal vector
	vNorm = vec3( inverse(transpose(mat3(mvMat))) * vNormal);

        // Pass the texture coordinate to the fragment shader
	texCoord = vTexCoord;

	gl_Position =  projMat * mvMat * vPosition;
}
