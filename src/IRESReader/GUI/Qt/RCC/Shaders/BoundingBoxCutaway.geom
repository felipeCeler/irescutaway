//#version 430 core
//
//layout( lines_adjance ) in;
//layout( triangle_strip, max_vertices = 3 ) out;
//
//uniform vec2 WIN_SCALE;
//
//noperspective out vec3 dist;
//
//in VertexData
//{
//	vec4 color;
//} VertexIn[4];
//
//out VertexData
//{
//   vec4 color;
//} VertexOut;
//
//
//
//void main(void)
//{
//	vec2 p0 = WIN_SCALE * (gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w);
//	vec2 p1 = WIN_SCALE * (gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w);
//	vec2 p2 = WIN_SCALE * (gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w);
//
//
//	float a = length ( p1 - p2 );
//	float b = length ( p2 - p0 );
//	float c = length ( p1 - p0 );
//
//	float alpha = acos ( ( b * b + c * c - a * a ) / ( 2.0 * b * c ) );
//	float beta = acos ( ( a * a + c * c - b * b ) / ( 2.0 * a * c ) );
//
//	float ha = abs ( c * sin ( beta ) );
//	float hb = abs ( c * sin ( alpha ) );
//	float hc = abs ( b * sin ( alpha ) );
//
//
//	dist = vec3(ha, 0.0, 0.0);
//	VertexOut.color = VertexIn[0].color;
//	gl_Position = gl_in[0].gl_Position;
//	EmitVertex();
//
//	dist = vec3(0.0, hb, 0.0);
//	VertexOut.color = VertexIn[1].color;
//	gl_Position = gl_in[1].gl_Position;
//	EmitVertex();
//
//	dist = vec3(0.0, 0.0, hc);
//	VertexOut.color = VertexIn[2].color;
//	gl_Position = gl_in[2].gl_Position;
//	EmitVertex();
//
//
//	EndPrimitive();
//}
#version 430 core

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices=4) out;

uniform vec2 WIN_SCALE;

noperspective out vec4 dist;
in VertexData
{
	vec4 color;
	vec4 normal;
} VertexIn[4];

out VertexData
{
   vec4 color;
   vec4 normal;
} VertexOut;

void main(void)
{
	vec2 p0 = WIN_SCALE * (gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w);
	vec2 p1 = WIN_SCALE * (gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w);
	vec2 p2 = WIN_SCALE * (gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w);
	vec2 p3 = WIN_SCALE * (gl_in[3].gl_Position.xy / gl_in[3].gl_Position.w);

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

	dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
	VertexOut.color = VertexIn[0].color;
	VertexOut.normal = VertexIn[0].normal;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
	VertexOut.color = VertexIn[1].color;
	VertexOut.normal = VertexIn[1].normal;
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	dist = vec4(0, area2/length(v3), area1/length(v0), 0);
	VertexOut.color = VertexIn[2].color;
	VertexOut.normal = VertexIn[2].normal;
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	dist = vec4(0, 0, area3/length(v0), area4/length(v2));
	VertexOut.color = VertexIn[3].color;
	VertexOut.normal = VertexIn[3].normal;
	gl_Position = gl_in[3].gl_Position;
	EmitVertex();

	EndPrimitive();
}
