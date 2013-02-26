#version 420
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;

noperspective in vec3 dist;

uniform vec3 lightDirection;

in VertexData
{
    vec4 propertyColor;
    vec4 normal;

} geometryShaderOut_VertexData;

out vec4 fragmentColor;

uniform int cutaway;

void main ( void )
{

	float d = min ( dist[0] , min ( dist[1] , dist[2] ) );
	float I = exp2 ( -2 * d * d );


	if ( cutaway == 1 )
	{
		vec4 color = texture2DRect ( primary , gl_FragCoord.st ).rgba;

		float cutaway = color.w;

		if ( gl_FragCoord.z < ( cutaway ) )
		{
			discard;
		}
	}

	fragmentColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * geometryShaderOut_VertexData.propertyColor;

}
