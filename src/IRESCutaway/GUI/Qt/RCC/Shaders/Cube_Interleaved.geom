#version 430

layout ( points ) in;
layout ( triangle_strip, max_vertices=24 ) out;

out VertexData
{
	vec4 vertice;
	vec4 normal;
	vec4 color;
} VertexOut;

noperspective out vec4 dist;

in CubeData
{

	vec4 v[8];
	vec4 n[6];
	vec4 color;
flat	vec4 IJK;
flat	vec4 focus;
	vec4 center;
flat	bool culled[8];

} cube[1];

struct Face
{
	int 	vertices[4];
	vec3 	normal;
};

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform vec2 WIN_SCALE;

void main(void)
{
	Face faces[6];

	// top
	faces[0].vertices[0] = 4;
	faces[0].vertices[1] = 5;
	faces[0].vertices[2] = 7;
	faces[0].vertices[3] = 6;
	// bottom
	faces[1].vertices[0] = 0;
	faces[1].vertices[1] = 3;
	faces[1].vertices[2] = 1;
	faces[1].vertices[3] = 2;
	// front
	faces[2].vertices[0] = 4;
	faces[2].vertices[1] = 0;
	faces[2].vertices[2] = 5;
	faces[2].vertices[3] = 1;
	// back
	faces[3].vertices[0] = 2;
	faces[3].vertices[1] = 3;
	faces[3].vertices[2] = 6;
	faces[3].vertices[3] = 7;
	// right
	faces[4].vertices[0] = 1;
	faces[4].vertices[1] = 2;
	faces[4].vertices[2] = 5;
	faces[4].vertices[3] = 6;
	// left
	faces[5].vertices[0] = 0;
	faces[5].vertices[1] = 3;
	faces[5].vertices[2] = 4;
	faces[5].vertices[3] = 7;

	vec4 vp[4];

	if ( (!cube[0].culled[0])
//             (!cube[0].culled[1]) &&
//             (!cube[0].culled[2]) &&
//             (!cube[0].culled[3]) &&
//             (!cube[0].culled[4]) &&
//             (!cube[0].culled[5]) &&
//             (!cube[0].culled[6]) &&
//             (!cube[0].culled[7])
             )
	{

		for ( int i = 0; i < 6; i++)
		{

			vp[0] = ProjectionMatrix * ViewMatrix * cube[0].v[faces[i].vertices[0]];
			vp[1] = ProjectionMatrix * ViewMatrix * cube[0].v[faces[i].vertices[1]];
			vp[2] = ProjectionMatrix * ViewMatrix * cube[0].v[faces[i].vertices[2]];
			vp[3] = ProjectionMatrix * ViewMatrix * cube[0].v[faces[i].vertices[3]];

			vec2 p0 = WIN_SCALE * (vp[0].xy / vp[0].w);
			vec2 p1 = WIN_SCALE * (vp[1].xy / vp[1].w);
			vec2 p2 = WIN_SCALE * (vp[2].xy / vp[2].w);
			vec2 p3 = WIN_SCALE * (vp[3].xy / vp[3].w);

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


			VertexOut.normal = cube[0].n[i];
			VertexOut.color = cube[0].color;

			//Top face
			dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
			VertexOut.vertice  = ViewMatrix * cube[0].v[faces[i].vertices[0]];
			gl_Position = vp[0];
			EmitVertex();
			dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
			VertexOut.vertice  = ViewMatrix * cube[0].v[faces[i].vertices[1]];
			gl_Position = vp[1];
			EmitVertex();
			dist = vec4(0, area2/length(v3), area1/length(v0), 0);
			VertexOut.vertice  = ViewMatrix * cube[0].v[faces[i].vertices[2]];
			gl_Position = vp[2];
			EmitVertex();
			dist = vec4(0, 0, area3/length(v0), area4/length(v2));
			VertexOut.vertice  = ViewMatrix * cube[0].v[faces[i].vertices[3]];
			gl_Position = vp[3];
			EmitVertex();

			EndPrimitive();
		}

	}

//	if ( !cube[0].culled[0] )
//	{
//		// Top
//		VertexOut.normal = cube[0].n[0];
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[4];
//		VertexOut.vertice =  ViewMatrix * cube[0].v[4];
//		VertexOut.normal = VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[5];
//		VertexOut.vertice =  ViewMatrix * cube[0].v[5];
//		VertexOut.normal = VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[7];
//		VertexOut.vertice =  ViewMatrix * cube[0].v[7];
//		VertexOut.normal = VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[6];
//		VertexOut.vertice =  ViewMatrix * cube[0].v[6];
//		VertexOut.normal = VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//
//		EndPrimitive();
//
//
//		// Bottom
//		VertexOut.normal = cube[0].n[1];
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[0];
//		VertexOut.vertice = ViewMatrix * cube[0].v[0];
//		VertexOut.normal = VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[3];
//		VertexOut.vertice = ViewMatrix * cube[0].v[3];
//		VertexOut.normal =  VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[1];
//		VertexOut.vertice = ViewMatrix * cube[0].v[1];
//		VertexOut.normal =  VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//		gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[2];
//		VertexOut.vertice = ViewMatrix * cube[0].v[2];
//		VertexOut.normal = VertexOut.normal;
//		VertexOut.color  = cube[0].color;
//		EmitVertex();
//
//		EndPrimitive();
//	}

}
