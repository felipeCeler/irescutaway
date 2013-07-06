#version 430

layout ( triangles_adjacency ) in;
layout ( triangle_strip, max_vertices=24 ) out;

in VertexData
{
    vec4 vertice;
    vec3 normal;
    vec4 color;
    vec4 focus;
flat    ivec4 IJK;

} cell_properties[6];

out VertexData
{
    vec4 vertice;
    vec3 normal;
    vec4 color;
    vec4 focus;
flat    ivec4 IJK;

} VertexOut;

struct Face
{
	int 	vertices[4];
	vec3 	normal;
};

struct CutPlane
{
   vec4 point;
   vec4 normal;
};

uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];

uniform int cut_volume_size;

uniform vec2 WIN_SCALE;

noperspective out vec4 dist;

uniform vec3 center_point;
uniform vec4 max_point;
uniform vec4 min_point;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


vec4 v[8];

Face faces[6];

void main(void)
{


	v[4] = vec4 ( gl_in[0].gl_Position.xyz , 1.0 );
	v[5] = vec4 ( gl_in[1].gl_Position.xyz , 1.0 );
	v[7] = vec4 ( gl_in[2].gl_Position.xyz , 1.0 );
	v[6] = vec4 ( gl_in[3].gl_Position.xyz , 1.0 );

	v[0] = vec4 ( gl_in[4].gl_Position.xyz , 1.0 );
	v[3] = vec4 ( gl_in[5].gl_Position.xyz , 1.0 );

	v[1] = vec4 ( gl_in[0].gl_Position.w , gl_in[1].gl_Position.w , gl_in[2].gl_Position.w , 1.0 );
	v[2] = vec4 ( gl_in[3].gl_Position.w , gl_in[4].gl_Position.w , gl_in[5].gl_Position.w , 1.0 );


	CutPlane cutPlaneIn;
	Face faces[6];

	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);

	vec4 ve[8];

	vec3 ext_x = new_x*0.1;
	vec3 ext_y = new_y*0.3;
	vec3 ext_z = new_z*0.1;

	vec3 centerp = center_point;

//	vec3 ext_x = vec3(1,0,0);
//	vec3 ext_y = vec3(0,1,0);
//	vec3 ext_z = vec3(0,0,1);
//
//	vec3 centerp = vec3(0,0,0);

	// top
	faces[0].vertices[0] = 0;
	faces[0].vertices[1] = 1;
	faces[0].vertices[2] = 2;
	faces[0].vertices[3] = 3;
	// bottom
	faces[1].vertices[0] = 4;
	faces[1].vertices[1] = 5;
	faces[1].vertices[2] = 6;
	faces[1].vertices[3] = 7;
	// front
	faces[2].vertices[0] = 0;
	faces[2].vertices[1] = 3;
	faces[2].vertices[2] = 5;
	faces[2].vertices[3] = 4;
	//back
	faces[3].vertices[0] = 1;
	faces[3].vertices[1] = 7;
	faces[3].vertices[2] = 6;
	faces[3].vertices[3] = 2;
	// right
	faces[4].vertices[0] = 0;
	faces[4].vertices[1] = 4;
	faces[4].vertices[2] = 7;
	faces[4].vertices[3] = 1;
	// left
	faces[5].vertices[0] = 2;
	faces[5].vertices[1] = 6;
	faces[5].vertices[2] = 5;
	faces[5].vertices[3] = 3;

	vec4 center_v = (v[0] + v[1] + v[2] + v[3] + v[4] + v[5] + v[6] + v[7])/8;

	bool isclipped_globally = false;

	for (int j = 0 ; j < cut_volume_size ; j++)
	{
		ve[0] = vec4(center_points[j].xyz + ext_x + ext_y + ext_z + 100*ext_z+ 50*ext_x,1.0);
		ve[1] = vec4(center_points[j].xyz + ext_x + ext_y - ext_z,1.0);
		ve[2] = vec4(center_points[j].xyz - ext_x + ext_y - ext_z,1.0);
		ve[3] = vec4(center_points[j].xyz - ext_x + ext_y + ext_z + 100*ext_z- 50*ext_x,1.0);

		ve[4] = vec4(center_points[j].xyz + ext_x - ext_y + ext_z + 100*ext_z+ 50*ext_x,1.0);
		ve[5] = vec4(center_points[j].xyz - ext_x - ext_y + ext_z + 100*ext_z- 50*ext_x,1.0);
		ve[6] = vec4(center_points[j].xyz - ext_x - ext_y - ext_z,1.0);
		ve[7] = vec4(center_points[j].xyz + ext_x - ext_y - ext_z,1.0);

		bool isclipped_locally = true;

		for ( int i = 0; i < 6; i++)
		{
			vec3 normal = normalize (cross ( (ve[faces[i].vertices[3]].xyz - ve[faces[i].vertices[0]].xyz),
							 (ve[faces[i].vertices[1]].xyz - ve[faces[i].vertices[0]].xyz) ) );

			cutPlaneIn.point  = ve[faces[i].vertices[3]];
			cutPlaneIn.normal = vec4(-normal,1.0);

			 if (dot(cutPlaneIn.normal,(cutPlaneIn.point - center_v) )  > 0.01)
			 {}
			 else
			 {
				 isclipped_locally = false;
			 }


		}


		isclipped_globally = isclipped_globally || isclipped_locally;
	}

	if ( isclipped_globally )
	{

	}else
	{



		// top
		faces[0].vertices[0] = 4;
		faces[0].vertices[1] = 5;
		faces[0].vertices[2] = 6;
		faces[0].vertices[3] = 7;
		// bottom
		faces[1].vertices[0] = 0;
		faces[1].vertices[1] = 3;
		faces[1].vertices[2] = 2;
		faces[1].vertices[3] = 1;
		// front
		faces[2].vertices[0] = 0;
		faces[2].vertices[1] = 1;
		faces[2].vertices[2] = 5;
		faces[2].vertices[3] = 4;
		//back
		faces[3].vertices[0] = 2;
		faces[3].vertices[1] = 3;
		faces[3].vertices[2] = 7;
		faces[3].vertices[3] = 6;
		// right
		faces[4].vertices[0] = 1;
		faces[4].vertices[1] = 2;
		faces[4].vertices[2] = 6;
		faces[4].vertices[3] = 5;
		// left
		faces[5].vertices[0] = 0;
		faces[5].vertices[1] = 4;
		faces[5].vertices[2] = 7;
		faces[5].vertices[3] = 3;


		vec4 vp[4];

		for ( int i = 0; i < 6; i++)
		{
			vec3 normal = normalize (cross ( (v[faces[i].vertices[1]].xyz - v[faces[i].vertices[0]].xyz),
							 (v[faces[i].vertices[3]].xyz - v[faces[i].vertices[0]].xyz) ) );

			vp[0] = ProjectionMatrix * ViewMatrix * v[faces[i].vertices[0]];
			vp[1] = ProjectionMatrix * ViewMatrix * v[faces[i].vertices[1]];
			vp[2] = ProjectionMatrix * ViewMatrix * v[faces[i].vertices[2]];
			vp[3] = ProjectionMatrix * ViewMatrix * v[faces[i].vertices[3]];

			vec2 p0 = WIN_SCALE * (vp[0].xy / vp[0].w);
			vec2 p1 = WIN_SCALE * (vp[1].xy / vp[1].w);
			vec2 p2 = WIN_SCALE * (vp[3].xy / vp[3].w);
			vec2 p3 = WIN_SCALE * (vp[2].xy / vp[2].w);

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

			normal =  normalize(normalMatrix * normal);

			VertexOut.normal = normal;
			VertexOut.color = cell_properties[0].color;

			//Top face
			dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
			VertexOut.vertice  = v[faces[i].vertices[0]];
			gl_Position = vp[0];
			EmitVertex();
			dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
			VertexOut.vertice  = v[faces[i].vertices[1]];
			gl_Position = vp[1];
			EmitVertex();
			dist = vec4(0, area2/length(v3), area1/length(v0), 0);
			VertexOut.vertice  = v[faces[i].vertices[3]];
			gl_Position = vp[3];
			EmitVertex();
			dist = vec4(0, 0, area3/length(v0), area4/length(v2));
			VertexOut.vertice  = v[faces[i].vertices[2]];
			gl_Position = vp[2];
			EmitVertex();

			EndPrimitive();
		}
	}
}
