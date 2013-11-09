#version 430 core

layout( points ) in;
//layout( line_strip, max_vertices=8 ) out;
layout( triangle_strip, max_vertices=4 ) out;

uniform vec2 WIN_SCALE;

noperspective out vec4 dist;

in VertexData
{
	    vec4 v[4];
	    vec4 n[2];
	    vec4 eye[4];
	    vec4 color;
	    vec4 faceType;
} VertexIn[1];


out VertexData
{
	vec4 vertice;
   flat vec4 normal;
	vec4 color;
} VertexOut;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{

	vec2 p0 = WIN_SCALE * (VertexIn[0].v[0].xy / VertexIn[0].v[0].w);
	vec2 p1 = WIN_SCALE * (VertexIn[0].v[1].xy / VertexIn[0].v[1].w);
	vec2 p2 = WIN_SCALE * (VertexIn[0].v[3].xy / VertexIn[0].v[3].w);
	vec2 p3 = WIN_SCALE * (VertexIn[0].v[2].xy / VertexIn[0].v[2].w);

	vec2 v0 = p1 - p0;
	vec2 v1 = p1 - p2;
	vec2 v2 = p2 - p0;
	vec2 v3 = p1 - p3;
	vec2 v4 = p3 - p2;
	vec2 v5 = p3 - p0;
	vec2 v6 = p2 - p0;
	float area1 = abs(v1.x * v6.y - v1.y * v6.x);
	float area2 = abs(v1.x * v4.y - v1.y * v4.x);
	float area3 = abs(v0.x * v5.y - v0.y * v5.x);
	float area4 = abs(v2.x * v5.y - v2.y * v5.x);

	VertexOut.normal = VertexIn[0].n[0];

	dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
	VertexOut.color   = VertexIn[0].color;
	VertexOut.vertice = VertexIn[0].eye[0];
	gl_Position = VertexIn[0].v[0];
	EmitVertex();

	dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
	VertexOut.color   = VertexIn[0].color;
	VertexOut.vertice = VertexIn[0].eye[1];
	gl_Position = VertexIn[0].v[1];
	EmitVertex();

	dist = vec4(0, area2/length(v3), area1/length(v0), 0);
	VertexOut.color   = VertexIn[0].color;
	VertexOut.vertice = VertexIn[0].eye[3];
	gl_Position = VertexIn[0].v[3];
	EmitVertex();

	VertexOut.normal = VertexIn[0].n[1];

	dist = vec4(0, 0, area3/length(v0), area4/length(v2));
	VertexOut.color   = VertexIn[0].color;
	VertexOut.vertice = VertexIn[0].eye[2];
	gl_Position = VertexIn[0].v[2];
	EmitVertex();

	EndPrimitive();

}
