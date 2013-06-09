#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices=24) out;


uniform vec4 max_point;
uniform vec4 min_point;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out VertexData
{
   vec4 color;
   vec3 vert;
   vec3 normal;
} VertexOut;


void works ( )
{

	float near = 0.1;
	float far  = 500.0;

	vec4 v[8];
	vec4 vertex[8];

	// Top  and Bottom Face
	v[0] = vec4( max_point.x , max_point.y , max_point.z, 1.0 );
	v[1] = vec4( max_point.x , max_point.y , min_point.z, 1.0 );
	v[2] = vec4( min_point.x , max_point.y , min_point.z, 1.0 );
	v[3] = vec4( min_point.x , max_point.y , max_point.z, 1.0 );

	v[4] = vec4( max_point.x , min_point.y , max_point.z, 1.0 );
	v[5] = vec4( min_point.x , min_point.y , max_point.z, 1.0 );
	v[6] = vec4( min_point.x , min_point.y , min_point.z, 1.0 );
	v[7] = vec4( max_point.x , min_point.y , min_point.z, 1.0 );


	mat4 base = ViewMatrix;

	v[0] = base * v[0];
	v[1] = base * v[1];
	v[2] = base * v[2];
	v[3] = base * v[3];

	v[4] = base * v[4];
	v[5] = base * v[5];
	v[6] = base * v[6];
	v[7] = base * v[7];


	mat3 normalMatrix = mat3(1.0);

	//mat3 normalMatrix = mat3(ViewMatrix);
	//normalMatrix = inverse(normalMatrix);
	//normalMatrix = transpose(normalMatrix);

	vertex[0] = v[0];
	vertex[1] = v[1];
	vertex[2] = v[2];
	vertex[3] = v[3];

	vertex[4] = v[4];
	vertex[5] = v[5];
	vertex[6] = v[6];
	vertex[7] = v[7];


	vec4 pmin = v[0];
	vec4 pmax = pmin;

	for ( int i = 0; i < 8; i++ )
	{
		if ( pmin.x > v[i].x )
		{
			pmin.x = v[i].x;

		}else if ( pmax.x < v[i].x )
		{
			pmax.x = v[i].x;
		}

		if ( pmin.y > v[i].y )
		{
			pmin.y = v[i].y;

		}else if ( pmax.y < v[i].y )
		{
			pmax.y = v[i].y;
		}

		if ( pmin.z > v[i].z )
		{
			pmin.z = v[i].z;

		}else if ( pmax.z < v[i].z )
		{
			pmax.z = v[i].z;

		}
	}


	float scaledy = 0.5;
	float scaledx = 0.5;
	float max_z   = -1;
	/// Back and Front Face
	/// Como ajustar o Z min/max , parametricamente.
	v[0] = vec4( pmax.x*scaledx , pmax.y*scaledy , max_z, 1.0 );
	v[1] = vec4( pmax.x , pmax.y , pmin.z, 1.0 );
	v[2] = vec4( pmin.x , pmax.y , pmin.z, 1.0 );
	v[3] = vec4( pmin.x*scaledx , pmax.y*scaledy , max_z, 1.0 );

	v[4] = vec4( pmax.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
	v[5] = vec4( pmin.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
	v[6] = vec4( pmin.x , pmin.y , pmin.z, 1.0  );
	v[7] = vec4( pmax.x , pmin.y , pmin.z, 1.0 );

	// Top Face
	vec3 normal_top = normalize (cross ( (v[3] - v[0]).xyz, (v[1] - v[0]).xyz ) );
	normal_top =  normalize(normalMatrix * normal_top);

	// Bottom
	vec3 normal_bottom = normalize (cross ( (v[7] - v[4]).xyz, (v[5] - v[4]).xyz ) );
	normal_bottom = normalize(normalMatrix * normal_bottom);

	// Back
	vec3 normal_back = normalize (cross ( (v[2] - v[1]).xyz, (v[7] - v[1]).xyz ) );
	normal_back =  normalize(normalMatrix * normal_back);

	// Right
	vec3 normal_right = normalize (cross ( (v[1] - v[0]).xyz, (v[4] - v[0]).xyz ) );
	normal_right = normalize(normalMatrix * normal_right);

	// Left
	vec3 normal_left = normalize (cross ( (v[3] - v[2]).xyz, (v[6] - v[2]).xyz ) );
	normal_left = normalize(normalMatrix * normal_left);

	mat4 projection = ProjectionMatrix ;


//	v[0] = inverse(base) * v[0];
//	v[1] = inverse(base) * v[1];
//	v[2] = inverse(base) * v[2];
//	v[3] = inverse(base) * v[3];
//
//	v[4] = inverse(base) * v[4];
//	v[5] = inverse(base) * v[5];
//	v[6] = inverse(base) * v[6];
//	v[7] = inverse(base) * v[7];



	vertex[0] = v[0];
	vertex[1] = v[1];
	vertex[2] = v[2];
	vertex[3] = v[3];

	vertex[4] = v[4];
	vertex[5] = v[5];
	vertex[6] = v[6];
	vertex[7] = v[7];

	v[0] =  projection * v[0];
	v[1] =  projection * v[1];
	v[2] =  projection * v[2];
	v[3] =  projection * v[3];

	v[4] =  projection * v[4];
	v[5] =  projection * v[5];
	v[6] =  projection * v[6];
	v[7] =  projection * v[7];

	//Top face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[0].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[0];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[1];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[3].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[3];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[2];
	EmitVertex();

	EndPrimitive();

	// Bottom face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[4].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[4];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[5].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[5];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[7];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[6];
	EmitVertex();

	EndPrimitive();

	// Back face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[1];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[7];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[2];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[6];
	EmitVertex();

	EndPrimitive();

	//Right face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[0].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[0];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[4].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[4];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[1];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[7];
	EmitVertex();

	EndPrimitive();

	//Left face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[2];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[6];
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[3].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[3];
	EmitVertex();
	VertexOut.color = vec4 ( 1.0 , 0.0 , 0.0 , 1.0 );
	VertexOut.vert = vertex[5].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[5];
	EmitVertex();

	EndPrimitive();
}

void justBox()
{

	vec4 v[8];
	vec4 vertex[8];

	// Top  and Bottom Face
	v[0] = vec4( max_point.x , max_point.y , max_point.z, 1.0 );
	v[1] = vec4( max_point.x , max_point.y , min_point.z, 1.0 );
	v[2] = vec4( min_point.x , max_point.y , min_point.z, 1.0 );
	v[3] = vec4( min_point.x , max_point.y , max_point.z, 1.0 );

	v[4] = vec4( max_point.x , min_point.y , max_point.z, 1.0 );
	v[5] = vec4( min_point.x , min_point.y , max_point.z, 1.0 );
	v[6] = vec4( min_point.x , min_point.y , min_point.z, 1.0 );
	v[7] = vec4( max_point.x , min_point.y , min_point.z, 1.0 );


	mat4 base = ViewMatrix;

	v[0] = base * v[0];
	v[1] = base * v[1];
	v[2] = base * v[2];
	v[3] = base * v[3];

	v[4] = base * v[4];
	v[5] = base * v[5];
	v[6] = base * v[6];
	v[7] = base * v[7];


	mat3 normalMatrix = mat3(1.0);
//
//	//mat3 normalMatrix = mat3(ViewMatrix);
//	//normalMatrix = inverse(normalMatrix);
//	//normalMatrix = transpose(normalMatrix);
//
//
	vertex[0] = v[0];
	vertex[1] = v[1];
	vertex[2] = v[2];
	vertex[3] = v[3];

	vertex[4] = v[4];
	vertex[5] = v[5];
	vertex[6] = v[6];
	vertex[7] = v[7];


	vec4 pmin = v[0];
	vec4 pmax = pmin;

	for ( int i = 0; i < 8; i++ )
	{
		if ( pmin.x > v[i].x )
		{
			pmin.x = v[i].x;

		}else if ( pmax.x < v[i].x )
		{
			pmax.x = v[i].x;
		}

		if ( pmin.y > v[i].y )
		{
			pmin.y = v[i].y;

		}else if ( pmax.y < v[i].y )
		{
			pmax.y = v[i].y;
		}

		if ( pmin.z > v[i].z )
		{
			pmin.z = v[i].z;

		}else if ( pmax.z < v[i].z )
		{
			pmax.z = v[i].z;

		}
	}


	float scaledy = 6;
	float scaledx = 6;
	float max_z   = pmax.z;
	float min_z   = pmin.z;
	/// Back and Front Face
	/// Como ajustar o Z min/max , parametricamente.
	v[0] = vec4( pmax.x*scaledx , pmax.y*scaledy , max_z, 1.0 );
	v[1] = vec4( pmax.x , pmax.y , min_z, 1.0 );
	v[2] = vec4( pmin.x , pmax.y , min_z, 1.0 );
	v[3] = vec4( pmin.x*scaledx , pmax.y*scaledy , max_z, 1.0 );

	v[4] = vec4( pmax.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
	v[5] = vec4( pmin.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
	v[6] = vec4( pmin.x , pmin.y , min_z, 1.0  );
	v[7] = vec4( pmax.x , pmin.y , min_z, 1.0 );

	// Top Face
	vec3 normal_top = normalize (cross ( (v[3] - v[0]).xyz, (v[1] - v[0]).xyz ) );
	normal_top =  normalize(normalMatrix * normal_top);

	// Bottom
	vec3 normal_bottom = normalize (cross ( (v[7] - v[4]).xyz, (v[5] - v[4]).xyz ) );
	normal_bottom = normalize(normalMatrix * normal_bottom);

	// Back
	vec3 normal_back = normalize (cross ( (v[2] - v[1]).xyz, (v[7] - v[1]).xyz ) );
	normal_back =  normalize(normalMatrix * normal_back);

	// Right
	vec3 normal_right = normalize (cross ( (v[1] - v[0]).xyz, (v[4] - v[0]).xyz ) );
	normal_right = normalize(normalMatrix * normal_right);

	// Left
	vec3 normal_left = normalize (cross ( (v[3] - v[2]).xyz, (v[6] - v[2]).xyz ) );
	normal_left = normalize(normalMatrix * normal_left);

	mat4 projection = ProjectionMatrix * ViewMatrix ;


	v[0] = inverse(base) * v[0];
	v[1] = inverse(base) * v[1];
	v[2] = inverse(base) * v[2];
	v[3] = inverse(base) * v[3];

	v[4] = inverse(base) * v[4];
	v[5] = inverse(base) * v[5];
	v[6] = inverse(base) * v[6];
	v[7] = inverse(base) * v[7];

	vertex[0] = v[0];
	vertex[1] = v[1];
	vertex[2] = v[2];
	vertex[3] = v[3];

	vertex[4] = v[4];
	vertex[5] = v[5];
	vertex[6] = v[6];
	vertex[7] = v[7];

	v[0] =  projection * v[0];
	v[1] =  projection * v[1];
	v[2] =  projection * v[2];
	v[3] =  projection * v[3];

	v[4] =  projection * v[4];
	v[5] =  projection * v[5];
	v[6] =  projection * v[6];
	v[7] =  projection * v[7];

	//Top face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[0].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[3].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[3]/v[3].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[2]/v[2].w;
	EmitVertex();

	EndPrimitive();

	// Bottom face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[4].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[4]/v[4].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[5].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[5]/v[5].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[7]/v[7].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[6]/v[6].w;
	EmitVertex();

	EndPrimitive();

	// Back face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[7]/v[7].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[6]/v[6].w;
	EmitVertex();

	EndPrimitive();

	//Right face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[0].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[4].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[4]/v[4].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[7]/v[7].w;
	EmitVertex();

	EndPrimitive();

	//Left face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[6]/v[6].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[3].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[3]/v[3].w;
	EmitVertex();
	VertexOut.color = vec4 ( 1.0 , 0.0 , 0.0 , 1.0 );
	VertexOut.vert = vertex[5].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[5] / v[5].w;
	EmitVertex();

	EndPrimitive();
}

void main(void)
{
	//works();
	justBox();
}

// FIXME O Quad Funfou.

//layout(points) in;
//layout(triangle_strip, max_vertices = 4) out;
//
//out vec2 texcoord;
//
//void main()
//{
//    gl_Position = vec4( 1.0, 1.0, 0.0, 1.0 );
//    texcoord = vec2( 1.0, 1.0 );
//    EmitVertex();
//
//    gl_Position = vec4(-1.0, 1.0, 0.0, 1.0 );
//    texcoord = vec2( 0.0, 1.0 );
//    EmitVertex();
//
//    gl_Position = vec4( 1.0,-1.0, 0.0, 1.0 );
//    texcoord = vec2( 1.0, 0.0 );
//    EmitVertex();
//
//    gl_Position = vec4(-1.0,-1.0, 0.0, 1.0 );
//    texcoord = vec2( 0.0, 0.0 );
//    EmitVertex();
//
//    EndPrimitive();
//}

