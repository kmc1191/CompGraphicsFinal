#version 150

// Flat shading fragment shader
//
// Just applies color computed by the vertex shader.

// Color "attached" to fragment
flat in vec4 color;

// Color being sent back to the pipeline
out vec4 fragColor;


void main()
{
    fragColor = color;
}
