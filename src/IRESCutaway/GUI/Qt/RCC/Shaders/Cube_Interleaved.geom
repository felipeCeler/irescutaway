#version 430

layout ( points ) in;
layout ( triangle_strip, max_vertices=24 ) out;

uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];
uniform int cut_volume_size;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

out VertexData
{
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
flat	bool culled[8];

} cube[1];

struct Edge
{
	int source;
	int target;
};

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
Edge edges[12];

vec4 proxyFace[6];

/// Culling Procedure
Face cutVolume[6];

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

// Rendering lines
vec4 vp[4];

void renderProxy ( in vec4 color )
{

	vec4 ve[8];
	CutPlane cutPlaneIn;

	// Plane Line Intersection - geomalgorithms.com/a05-_intersect-1.html
	float s;
	vec4 w;
	vec4 u;
	
	vec4 source;
	vec4 target;

	int proxy_index = 0;
	// For each cut volume
	for (int j = 0 ; j < 1 ; j++)
	{
		ve[0] = vec4(center_points[j].xyz + ext_x + ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		ve[1] = vec4(center_points[j].xyz + ext_x + ext_y - ext_z,1.0);
		ve[2] = vec4(center_points[j].xyz - ext_x + ext_y - ext_z,1.0);
		ve[3] = vec4(center_points[j].xyz - ext_x + ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);

		ve[4] = vec4(center_points[j].xyz + ext_x - ext_y + ext_z + 100*ext_z+ 5*ext_x,1.0);
		ve[5] = vec4(center_points[j].xyz - ext_x - ext_y + ext_z + 100*ext_z- 5*ext_x,1.0);
		ve[6] = vec4(center_points[j].xyz - ext_x - ext_y - ext_z,1.0);
		ve[7] = vec4(center_points[j].xyz + ext_x - ext_y - ext_z,1.0);

		// For each Edge in the cube
		for ( int edge_index = 0; edge_index < 12; edge_index++)
		{

			for ( int i = 0; i < 6; i++)
			{
				vec3 normal = normalize (cross ( (ve[cutVolume[i].vertices[3]].xyz - ve[cutVolume[i].vertices[0]].xyz),
							 					 (ve[cutVolume[i].vertices[1]].xyz - ve[cutVolume[i].vertices[0]].xyz) ) );

				cutPlaneIn.point  = ve[cutVolume[i].vertices[3]];
				cutPlaneIn.normal = vec4(-normal,0.0);
			
			
				source = cube[0].v[edges[edge_index].source];
				target = cube[0].v[edges[edge_index].target];
				
				w = source - cutPlaneIn.point;
				u = target - source;
				
				s =  ( dot (-cutPlaneIn.normal, w) ) / ( dot (cutPlaneIn.normal , u) );
				
				// Project target into the plane in the direction of the edge
				if (  ( s >= 0.0 ) && ( s <= 1.0 )  )
					 
				{
					proxyFace[proxy_index] = vec4( source + s*(target - source));
					if ( proxy_index < 6 )
						proxy_index++;
				}
								
			}


		}

	}
	
	if ( proxy_index >= 3 )
	{
	
		VertexOut.proxy = true;
		dist = vec4(0,0,-1,0);
		VertexOut.normal = vec4(1.0,0.0,0.0,0.0);
		VertexOut.color = color;
		
		VertexOut.vertice  = ViewMatrix * proxyFace[0];
		gl_Position = ProjectionMatrix * ViewMatrix * proxyFace[0];
		EmitVertex();
		
		VertexOut.vertice  = ViewMatrix * proxyFace[1];;
		gl_Position =  ProjectionMatrix * ViewMatrix * proxyFace[1];
		EmitVertex();
		
		VertexOut.vertice  = ViewMatrix * proxyFace[2];
		gl_Position = ProjectionMatrix * ViewMatrix * proxyFace[2];
		EmitVertex();
	
		EndPrimitive();
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


		VertexOut.normal = cube[0].n[i];
		VertexOut.color = color;
		VertexOut.proxy = false;

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

	// Cube Orientation as IRESv2
	// top
	edges[0].source  = 0; edges[0].target  = 1;
	edges[1].source  = 1; edges[1].target  = 2;
	edges[2].source  = 2; edges[2].target  = 3;
	edges[3].source  = 3; edges[3].target  = 0;

	edges[4].source  = 4; edges[4].target  = 5;
	edges[5].source  = 5; edges[5].target  = 6;
	edges[6].source  = 6; edges[6].target  = 7;
	edges[7].source  = 7; edges[7].target  = 0;

	edges[8].source  = 4; edges[8].target  = 0;
	edges[9].source  = 5; edges[9].target  = 1;
	edges[10].source = 6; edges[10].target = 2;
	edges[11].source = 7; edges[11].target = 3;
	
	/// Culling Procedure

	ext_x = new_x*0.2;
	ext_y = new_y*0.3;
	ext_z = new_z*0.1;

	// top
	cutVolume[0].vertices[0] = 0;
	cutVolume[0].vertices[1] = 1;
	cutVolume[0].vertices[2] = 2;
	cutVolume[0].vertices[3] = 3;
	// bottom
	cutVolume[1].vertices[0] = 4;
	cutVolume[1].vertices[1] = 5;
	cutVolume[1].vertices[2] = 6;
	cutVolume[1].vertices[3] = 7;
	// front
	cutVolume[2].vertices[0] = 0;
	cutVolume[2].vertices[1] = 3;
	cutVolume[2].vertices[2] = 5;
	cutVolume[2].vertices[3] = 4;
	// back
	cutVolume[3].vertices[0] = 1;
	cutVolume[3].vertices[1] = 7;
	cutVolume[3].vertices[2] = 6;
	cutVolume[3].vertices[3] = 2;
	// right
	cutVolume[4].vertices[0] = 0;
	cutVolume[4].vertices[1] = 4;
	cutVolume[4].vertices[2] = 7;
	cutVolume[4].vertices[3] = 1;
	// left
	cutVolume[5].vertices[0] = 2;
	cutVolume[5].vertices[1] = 6;
	cutVolume[5].vertices[2] = 5;
	cutVolume[5].vertices[3] = 3;

	// Not culled
	if ( (cube[0].culled[0]) &&
		 (cube[0].culled[1]) &&
		 (cube[0].culled[2]) &&
		 (cube[0].culled[3]) &&
		 (cube[0].culled[4]) &&
		 (cube[0].culled[5]) &&
		 (cube[0].culled[6]) &&
		 (cube[0].culled[7]) )
	{

		renderCube(cube[0].color);

	// To be partial culled
	}else if ((!cube[0].culled[0])||
			  (!cube[0].culled[1]) ||
			  (!cube[0].culled[2]) ||
			  (!cube[0].culled[3]) ||
			  (!cube[0].culled[4]) ||
			  (!cube[0].culled[5]) ||
			  (!cube[0].culled[6]) ||
			  (!cube[0].culled[7]) )
	{

		renderProxy( vec4(1.0,0.0,0.0,1.0) );
		renderCube(vec4(1.0,0.0,0.0,1.0));
		
	// Full culled
	}else
	{
	
	}



}
