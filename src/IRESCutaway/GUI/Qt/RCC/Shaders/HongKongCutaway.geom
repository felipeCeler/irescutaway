#version 430

layout ( points ) in;
layout ( triangle_strip, max_vertices=64 ) out;

uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];
uniform int cut_volume_size;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

out VertexData
{
		vec4 vertices[2];
		vec4 vertice;
		vec4 normal;
		vec4 color;
flat	bool proxy;
} VertexOut;

noperspective out vec4 dist;

in CubeData
{
	vec4 v[8];
	vec4 n[6];
	vec4 color;
} cube[1];

struct Face
{
	int 	vertices[4];
};

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform vec2 WIN_SCALE;

Face faces[6];

/// Culling Procedure
Face cutSurface[6];

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

// Rendering lines
vec4 vp[4];

mat3 normalMatrix;

void renderProxy ( in vec4 color )
{

	vec4 v[8];
	
	// For each cut volume
	for (int cut_surface_index = 0 ; cut_surface_index < 1 ; cut_surface_index++)
	{
		v[0] = vec4(center_points[cut_surface_index].xyz + ext_x + ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		v[1] = vec4(center_points[cut_surface_index].xyz + ext_x + ext_y - ext_z,1.0);
		v[2] = vec4(center_points[cut_surface_index].xyz - ext_x + ext_y - ext_z,1.0);
		v[3] = vec4(center_points[cut_surface_index].xyz - ext_x + ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);

		v[4] = vec4(center_points[cut_surface_index].xyz + ext_x - ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		v[5] = vec4(center_points[cut_surface_index].xyz - ext_x - ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);
		v[6] = vec4(center_points[cut_surface_index].xyz - ext_x - ext_y - ext_z,1.0);
		v[7] = vec4(center_points[cut_surface_index].xyz + ext_x - ext_y - ext_z,1.0);
	
		
		for ( int i = 0; i < 6; i++ )
		{
			//VertexOut.normals[i].xyz = normalize ( cross ( ( v[cutSurface[i].vertices[3]].xyz - v[cutSurface[i].vertices[0]].xyz ) , 
			//											  ( v[cutSurface[i].vertices[1]].xyz - v[cutSurface[i].vertices[0]].xyz ) ) );
														  
		    //VertexOut.normals[i].xyz = (normalMatrix * VertexOut.normals[i].xyz);
			//VertexOut.normals[i].w = 0.0;
		
			vec3 normal = normalize ( cross ( ( v[cutSurface[i].vertices[3]].xyz - v[cutSurface[i].vertices[0]].xyz ) , 
											  ( v[cutSurface[i].vertices[1]].xyz - v[cutSurface[i].vertices[0]].xyz ) ) );
			
			normal = normalize( normalMatrix * normal);
				
			VertexOut.proxy = true;
			VertexOut.vertices[0] = ViewMatrix * v[0];
			VertexOut.vertices[1] = ViewMatrix * v[6];
			
			VertexOut.color 	  = cube[0].color;
			VertexOut.normal   	  = vec4(normal,0.0);//VertexOut.normals[i];
			//Top face
			dist = vec4(0, 0, 0, 0);
			
			VertexOut.vertice  = ViewMatrix * v[cutSurface[i].vertices[0]];
			gl_Position = ProjectionMatrix * ViewMatrix * v[cutSurface[i].vertices[0]];
			EmitVertex();
			
			VertexOut.vertice  = ViewMatrix * v[cutSurface[i].vertices[1]];
			gl_Position = ProjectionMatrix * ViewMatrix * v[cutSurface[i].vertices[1]];
			EmitVertex();
			
			VertexOut.vertice  = ViewMatrix * v[cutSurface[i].vertices[3]];
			gl_Position = ProjectionMatrix * ViewMatrix * v[cutSurface[i].vertices[3]];
			EmitVertex();
			
			VertexOut.vertice  = ViewMatrix * v[cutSurface[i].vertices[2]];
			gl_Position = ProjectionMatrix * ViewMatrix * v[cutSurface[i].vertices[2]];
			EmitVertex();
	
			EndPrimitive();
		}
	}
}

void renderCube( in vec4 color )
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


		VertexOut.proxy = false;
		VertexOut.vertices[0] = ViewMatrix * cube[0].v[faces[0].vertices[1]];
		VertexOut.vertices[1] = ViewMatrix * cube[0].v[faces[1].vertices[1]];
		
		//VertexOut.normals[0] = cube[0].n[0];
		//VertexOut.normals[1] = cube[0].n[1];
		//VertexOut.normals[2] = cube[0].n[2];
		//VertexOut.normals[3] = cube[0].n[3];
		//VertexOut.normals[4] = cube[0].n[4];
		//VertexOut.normals[5] = cube[0].n[5];
		VertexOut.normal   	 = cube[0].n[i];
		VertexOut.color 	 = cube[0].color;

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

void main(void)
{

	 normalMatrix = inverse(transpose(mat3(ViewMatrix)));

	// Cube Orientation as IRESv2
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

	/// Culling Procedure

	ext_x = new_x*0.2;
	ext_y = new_y*0.3;
	ext_z = new_z*0.1;

	// top
	cutSurface[0].vertices[0] = 0;
	cutSurface[0].vertices[1] = 1;
	cutSurface[0].vertices[2] = 2;
	cutSurface[0].vertices[3] = 3;
	// bottom
	cutSurface[1].vertices[0] = 4;
	cutSurface[1].vertices[1] = 5;
	cutSurface[1].vertices[2] = 6;
	cutSurface[1].vertices[3] = 7;
	// front
	cutSurface[2].vertices[0] = 0;
	cutSurface[2].vertices[1] = 3;
	cutSurface[2].vertices[2] = 5;
	cutSurface[2].vertices[3] = 4;
	// back
	cutSurface[3].vertices[0] = 1;
	cutSurface[3].vertices[1] = 7;
	cutSurface[3].vertices[2] = 6;
	cutSurface[3].vertices[3] = 2;
	// right
	cutSurface[4].vertices[0] = 0;
	cutSurface[4].vertices[1] = 4;
	cutSurface[4].vertices[2] = 7;
	cutSurface[4].vertices[3] = 1;
	// left
	cutSurface[5].vertices[0] = 2;
	cutSurface[5].vertices[1] = 6;
	cutSurface[5].vertices[2] = 5;
	cutSurface[5].vertices[3] = 3;

	
	renderCube(cube[0].color);

	renderProxy(vec4(1.0,0.0,0.0,1.0));
	



}
