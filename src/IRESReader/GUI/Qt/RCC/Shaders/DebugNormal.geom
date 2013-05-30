#version 430 core

layout(lines_adjacency) in;
layout(line_strip, max_vertices=4) out;

in VertexData
{
	vec4 color;
	vec4 normal;
} VertexIn[4];

out VertexData
{
   vec4 color;
} VertexOut;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	float dist = 0.25 * distance( gl_in[1].gl_Position , gl_in[2].gl_Position );


	 vec3 face_normal = VertexIn[0].normal.xyz;


	vec4 centerFristTriangle  = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
	vec4 centerSecondTriangle = (gl_in[1].gl_Position + gl_in[2].gl_Position + gl_in[3].gl_Position) / 3.0;

	vec4 v1 = centerFristTriangle;
	v1 = ProjectionMatrix * ViewMatrix * v1;
	VertexOut.color = VertexIn[0].color;
	gl_Position = v1;
	EmitVertex();

	v1 = (vec4(face_normal * dist,0.0)  + centerFristTriangle);
	v1 = ProjectionMatrix * ViewMatrix * v1;
	VertexOut.color = VertexIn[1].color;
	gl_Position = v1;
	EmitVertex();

	EndPrimitive();
	v1 = centerSecondTriangle;
	v1 = ProjectionMatrix * ViewMatrix * v1;
	VertexOut.color = VertexIn[2].color;
	gl_Position = v1;
	EmitVertex();

	v1 = (vec4(face_normal * dist,0.0) + centerSecondTriangle);
	v1 = ProjectionMatrix * ViewMatrix * v1;
	VertexOut.color = VertexIn[3].color;
	gl_Position = v1;
	EmitVertex();

	EndPrimitive();
}
