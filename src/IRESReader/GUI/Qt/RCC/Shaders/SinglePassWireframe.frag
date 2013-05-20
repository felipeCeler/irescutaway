#version 430 core

in VertexData
{
	vec4 color;
} VertexIn;

noperspective in vec4 dist;

out vec4 outputColor;

void main(void)
{
	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2.0 * d * d);

	outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * VertexIn.color;
}
