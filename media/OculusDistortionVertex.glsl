#version 130

// Values automatically defined by Ogre/OpenGL:
attribute vec4 vertex;
attribute vec4 uv0;			// Red channel
attribute vec4 uv1;			// Green channel
attribute vec4 uv2;			// Blue channel

// Load in values defined in the material:
uniform mat4 worldViewProj;
uniform vec2 EyeToSourceUVScale;
uniform vec2 EyeToSourceUVOffset;
uniform mat4 EyeRotationStart;
uniform mat4 EyeRotationEnd;

float2 TimewarpTexCoord( float2 texCoord, mat4 rotMat )
{
	float3 transformed = vec3( (rotMat * vec4( texCoord.xy, 1, 1) ).xyz );
	
	float2 flattened = (transformed.xy / transformed.z );
	
	return( EyeToSourceUVScale * flattened + EyeToSourceUVOffset );
}

void main(void)
{
	/*mat4 lerpedEyeRot = mix( EyeRotationStart, EyeRotationEnd, timewarpLerpFactor );

	gl_TexCoord[0] = TimewarpTexCoord( uv0, lerpedEyeRot );
	gl_TexCoord[1] = TimewarpTexCoord( uv1, lerpedEyeRot );
	gl_TexCoord[2] = TimewarpTexCoord( uv2, lerpedEyeRot );*/
	
	gl_TexCoord[0] = return( EyeToSourceUVScale * uv0 + EyeToSourceUVOffset );
	gl_TexCoord[1] = return( EyeToSourceUVScale * uv1 + EyeToSourceUVOffset );
	gl_TexCoord[2] = return( EyeToSourceUVScale * uv2 + EyeToSourceUVOffset );

	gl_Position = worldViewProj * vertex;

}

