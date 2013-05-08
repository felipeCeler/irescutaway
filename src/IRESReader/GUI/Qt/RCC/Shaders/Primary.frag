#version 420

noperspective in vec4 dist;

uniform vec3 lightDirection;

in VertexData
{
    vec4 propertyColor;
    vec4 normal;

} geometryShader_VertexData;

flat out vec4 fragmentColor;



void main(void)
{

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);

	//fragmentColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * (geometryShader_VertexData.propertyColor);
	fragmentColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * (vec4(1.0,1.0,1.0,1.0));

}
