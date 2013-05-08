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
   vec3 normal;
} VertexOut;


void main(void)
{

	float near = 0.1;
	float far  = 500.0;

	vec4 v[8];

	v[0] = vec4( max_point.x , max_point.y , max_point.z, 1.0 );
	v[1] = vec4( max_point.x , max_point.y , min_point.z, 1.0 );
	v[2] = vec4( min_point.x , max_point.y , min_point.z, 1.0 );
	v[3] = vec4( min_point.x , max_point.y , max_point.z, 1.0 );

	v[4] = vec4( max_point.x , min_point.y , max_point.z, 1.0 );
	v[5] = vec4( min_point.x , min_point.y , max_point.z, 1.0 );
	v[6] = vec4( min_point.x , min_point.y , min_point.z, 1.0 );
	v[7] = vec4( max_point.x , min_point.y , min_point.z, 1.0 );

	v[0] =  ViewMatrix * v[0];
	v[1] =  ViewMatrix * v[1];
	v[2] =  ViewMatrix * v[2];
	v[3] =  ViewMatrix * v[3];

	v[4] =  ViewMatrix * v[4];
	v[5] =  ViewMatrix * v[5];
	v[6] =  ViewMatrix * v[6];
	v[7] =  ViewMatrix * v[7];


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


	v[0] = vec4( pmin.x , pmax.y , pmax.z, 1.0 );
	v[1] = vec4( pmax.x , pmax.y , pmax.z, 1.0 );
	v[2] = vec4( pmin.x , pmin.y , pmax.z, 1.0 );
	v[3] = vec4( pmax.x , pmin.y , pmax.z, 1.0 );

	v[4] = vec4( pmin.x , pmax.y , pmin.z/300, 1.0 );
	v[5] = vec4( pmax.x , pmax.y , pmin.z/300, 1.0 );
	v[6] = vec4( pmin.x , pmin.y , pmin.z/300, 1.0 );
	v[7] = vec4( pmax.x , pmin.y , pmin.z/300, 1.0 );

	v[0] =  ProjectionMatrix * v[0];
	v[1] =  ProjectionMatrix * v[1];
	v[2] =  ProjectionMatrix * v[2];
	v[3] =  ProjectionMatrix * v[3];

	v[4] =  ProjectionMatrix * v[4];
	v[5] =  ProjectionMatrix * v[5];
	v[6] =  ProjectionMatrix * v[6];
	v[7] =  ProjectionMatrix * v[7];


	//back face
	vec3 back_normal = normalize (cross ( (v[1] - v[0]).xyz, (v[1] - v[3]).xyz ) );
	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
	VertexOut.normal = back_normal;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
	VertexOut.normal = back_normal;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
	VertexOut.normal = back_normal;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
	VertexOut.normal = back_normal;
	gl_Position = v[3]/v[3].w;
	EmitVertex();

	EndPrimitive();

//	// front face
//	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
//	gl_Position = v[4]/v[4].w;
//	EmitVertex();
//	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
//	gl_Position = v[5]/v[5].w;
//	EmitVertex();
//	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
//	gl_Position = v[6]/v[6].w;
//	EmitVertex();
//	VertexOut.color = vec4( 1.0,0.0,0.0,0.25);
//	gl_Position = v[7]/v[7].w;
//	EmitVertex();
//
//	EndPrimitive();

	// top face
	vec3 top_normal = normalize (cross ( (v[0] - v[1]).xyz, (v[4] - v[1]).xyz ) );
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = top_normal;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = top_normal;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = top_normal;
	gl_Position = v[4]/v[4].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = top_normal;
	gl_Position = v[5]/v[5].w;
	EmitVertex();

	EndPrimitive();

	// bottom face
	vec3 bottom_normal = -normalize (cross ( (v[6] - v[2]).xyz, (v[3] - v[2]).xyz ) );
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = bottom_normal;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = bottom_normal;
	gl_Position = v[6]/v[6].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = bottom_normal;
	gl_Position = v[3]/v[3].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = bottom_normal;
	gl_Position = v[7]/v[7].w;
	EmitVertex();

	EndPrimitive();

	// right face
	vec3 right_normal = -normalize (cross ( (v[4] - v[0]).xyz, (v[2] - v[0]).xyz ) );
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = right_normal;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = right_normal;
	gl_Position = v[4]/v[4].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = right_normal;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = right_normal;
	gl_Position = v[6]/v[6].w;
	EmitVertex();

	EndPrimitive();

	// left face
	vec3 left_normal = -normalize (cross ( (v[3] - v[1]).xyz, (v[5] - v[1]).xyz ) );
	VertexOut.color = VertexOut.color = vec4( 1.0,1.0,1.0,0.75);
	VertexOut.normal = left_normal;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = left_normal;
	gl_Position = v[3]/v[3].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = left_normal;
	gl_Position = v[5]/v[5].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,1.0,1.0,1.0);
	VertexOut.normal = left_normal;
	gl_Position = v[7]/v[7].w;
	EmitVertex();

	EndPrimitive();

}

