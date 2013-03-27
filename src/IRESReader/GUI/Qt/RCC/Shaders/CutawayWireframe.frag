#version 420
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;


noperspective in vec4 dist;


in VertexData
{
    vec4 propertyColor;
    vec4 normal;

} geometryShader_VertexData;


uniform vec3 lightDirection;

out vec4 fragmentColor;

uniform int cutaway;

void main ( void )
{


	if ( cutaway == 1 )
	{
		vec4 color = texture2DRect ( primary , gl_FragCoord.st ).rgba;

		float cutaway = color.w;

		if ( gl_FragCoord.z < ( cutaway ) )
		{
			discard;
		}
	}

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);

	fragmentColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * (geometryShader_VertexData.propertyColor);

}


