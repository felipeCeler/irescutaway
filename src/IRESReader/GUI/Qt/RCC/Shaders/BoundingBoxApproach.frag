#version 430 core


in VertexData
{
   vec4 color;
   vec3 vert;
   vec3 normal;
} VertexOut;

out vec4 outputColor;

in vec2 texcoord;

void main(void)
{



	outputColor =  vec4 ( VertexOut.vert , gl_FragCoord.z );//VertexOut.color;
	//outputColor =  VertexOut.color;
}
