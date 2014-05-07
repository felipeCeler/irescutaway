#version 430 core

in VertexData
{
	vec4 vertice;
   flat vec4 normal;
	vec4 color;
} VertexIn;

out vec4 outputColor;


void main(void)
{

	outputColor = VertexIn.color;

}
