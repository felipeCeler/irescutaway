#version 430


noperspective in vec4 dist;

uniform vec3 lightDirection;

in VertexData
{
	vec4 vertice;
    vec4 color;
    vec4 normal;

} VertexIn;

flat out vec4 fragmentColor;


void main(void)
{

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2 * d * d);

	fragmentColor = vec4 ( gl_FragCoord.xyz,gl_FragCoord.z );
}

