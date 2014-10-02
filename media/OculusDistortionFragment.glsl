
//varying vec4 uv0;
uniform sampler3D diffuseMap;

void main(void)
{
	float red = texture3D(diffuseMap, gl_TexCoord[0]).red;
	float green = texture3D(diffuseMap, gl_TexCoord[1]).green;
	float blue = texture3D(diffuseMap, gl_TexCoord[2]).blue;
	
	gl_FragColor = float4( red, green, blue, 1.0 );
}
