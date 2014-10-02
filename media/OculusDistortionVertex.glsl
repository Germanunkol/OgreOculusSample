#version 130

// Values automatically defined by Ogre/OpenGL:
attribute vec4 vertex;
attribute vec4 uv0;			// Red channel
attribute vec4 uv1;			// Green channel
attribute vec4 uv2;			// Blue channel

// Load in values defined in the material:
uniform mat4 worldViewProj;
// uniform float layer;

void main(void)
{
	gl_TexCoord[0] = uv0;
	gl_TexCoord[1] = uv1;
	gl_TexCoord[2] = uv2;

	gl_Position = worldViewProj * vertex;

}
