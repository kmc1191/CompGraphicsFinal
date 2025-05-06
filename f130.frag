#version 130

// Flat shading fragment shader
//
// Just applies color computed by the vertex shader.

// Color "attached" to fragment
flat in vec4 color;

void main()
{
    gl_FragColor = color;
}
