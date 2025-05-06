#version 150

// Gouraud shading vertex shader
//
// Does all vertex transformation, and set up relevant
// shading vectors to be passed to the fragment shader.

// INCOMING DATA

// Vertex position (in model space)
in vec4 vPosition;

// Normal vector at vertex (in model space)
in vec3 vNormal;

//
// Uniform data
//

// Transformations
uniform mat4 viewMat;  // view (camera)
uniform mat4 projMat;  // projection

// Model transformation matrices
uniform mat4 modelMat; // composite

// Light position is given in world space
uniform vec4 lightPosition;

// Light color
uniform vec4 lightColor;
uniform vec4 ambientLight;

// Material properties
uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec3 kCoeff;

// OUTGOING DATA

// Color
flat out vec4 color;

void main()
{
    // transform our positions
    mat4 mvMat = viewMat * modelMat;
    vec3 vPos = vec3(mvMat * vPosition);
    vec3 lPos = vec3(viewMat * lightPosition);

    // transform the normal vector
    vec3 vNorm = vec3( inverse(transpose(mat3(mvMat))) * vNormal);

    // color calculations
    vec3 L = normalize( lPos - vPos );
    vec3 N = normalize( vNorm );
    vec4 ambient = ambientLight * ambientColor;
    vec4 diffuse = lightColor * diffuseColor * max(dot(N,L),0.0);

    // calculate the final color
    color = (kCoeff.x * ambient) + (kCoeff.y * diffuse);

    // send the vertex position into clip space
    gl_Position =  projMat * mvMat * vPosition;
}
