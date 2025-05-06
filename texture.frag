#version 150

// Texture shader skeleton

//
// Texture fragment shader
//
// @author  RIT CS Department
// @author  Kaitlynn
//

//
// INCOMING DATA
//

//
// Data coming from the vertex shader
//

// Light position
in vec3 lPos;

// Vertex position (in clip space)
in vec3 vPos;

// Vertex normal
in vec3 vNorm;

// Texture coordinates
in vec2 texCoord;

//
// Data coming from the application
//

// Light color
uniform vec4 lightColor;
uniform vec4 ambientLight;

// Material properties
uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform float specExp;
uniform vec3 kCoeff;

//
// ADD ANY OTHER INCOMING VARIABLES YOU NEED (FROM THE
// APPLICATION OR FROM THE VERTEX SHADER) HERE
//

// Texture samplers
uniform sampler2D mainTex;    // Front texture
uniform sampler2D backTex;     // Back texture

// OUTGOING DATA

// The final fragment color
out vec4 fragColor;

void main()
{
	// calculate lighting vectors
	vec3 L = normalize( lPos - vPos );
	vec3 N = normalize( vNorm );
	vec3 R = normalize( reflect(-L, N) );
	vec3 V = normalize( -(vPos) );

	vec4 ambient  = vec4(0.0);  // ambient color component
	vec4 diffuse  = vec4(0.0);  // diffuse color component
	vec4 specular = vec4(0.0);  // specular color component

        // Texture sampling
        vec4 textureColor;

        //Used for differentiating between front and back of disk
        if(gl_FrontFacing){
                textureColor= texture(mainTex, texCoord);
        }
        else{
                textureColor = texture(backTex, texCoord);
        }

        //Apply texture color to ambient and diffuse 
	ambient  = ambientLight * textureColor;
	diffuse  = lightColor * textureColor * max(dot(N,L),0.0);
	specular = lightColor * vec4(1.0) * pow( max(dot(R,V),0.0), specExp );

	// calculate the final color
	fragColor = (kCoeff.x * ambient) +
				 (kCoeff.y * diffuse) +
				 (kCoeff.z * specular);

        // Set alpha 
	fragColor.a = 1.0;
}
