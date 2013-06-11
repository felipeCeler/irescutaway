#version 430

layout ( triangles_adjacency ) in;
layout ( triangle_strip, max_vertices=24 ) out;

out VertexData
{
	vec4 vertice;
	vec3 normal;
	vec4 color;
} VertexOut;

uniform vec4 max_point;
uniform vec4 min_point;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


vec4 cutVolume[8];
vec4 p_min;
vec4 p_max;

bool intersect_point_volume ( in vec4 p )
{

	if( ( p.x >= p_min.x ) && ( p.x < p_max.x ) &&
	    ( p.y >= p_min.y ) && ( p.y < p_max.y ) &&
	    ( p.z >= p_min.z ) && ( p.z < p_max.z ) )
	{
		return false;
	}
	else
	{
		return true ;
	}

}

void main(void)
{
	// Top  and Bottom Face
//	cutVolume[0] = vec4( max_point.x , max_point.y , max_point.z, 1.0 );
//	cutVolume[1] = vec4( max_point.x , max_point.y , min_point.z, 1.0 );
//	cutVolume[2] = vec4( min_point.x , max_point.y , min_point.z, 1.0 );
//	cutVolume[3] = vec4( min_point.x , max_point.y , max_point.z, 1.0 );
//
//	cutVolume[4] = vec4( max_point.x , min_point.y , max_point.z, 1.0 );
//	cutVolume[5] = vec4( min_point.x , min_point.y , max_point.z, 1.0 );
//	cutVolume[6] = vec4( min_point.x , min_point.y , min_point.z, 1.0 );
//	cutVolume[7] = vec4( max_point.x , min_point.y , min_point.z, 1.0 );
//
//	mat4 base = ViewMatrix;
////
//	cutVolume[0] = base * cutVolume[0];
//	cutVolume[1] = base * cutVolume[1];
//	cutVolume[2] = base * cutVolume[2];
//	cutVolume[3] = base * cutVolume[3];
//
//	cutVolume[4] = base * cutVolume[4];
//	cutVolume[5] = base * cutVolume[5];
//	cutVolume[6] = base * cutVolume[6];
//	cutVolume[7] = base * cutVolume[7];
////
//	vec4 pmin = cutVolume[0];
//	vec4 pmax = pmin;
//
//	for ( int i = 0; i < 8; i++ )
//	{
//		if ( pmin.x > cutVolume[i].x )
//		{
//			pmin.x = cutVolume[i].x;
//
//		}else if ( pmax.x < cutVolume[i].x )
//		{
//			pmax.x = cutVolume[i].x;
//		}
//
//		if ( pmin.y > cutVolume[i].y )
//		{
//			pmin.y = cutVolume[i].y;
//
//		}else if ( pmax.y < cutVolume[i].y )
//		{
//			pmax.y = cutVolume[i].y;
//		}
//
//		if ( pmin.z > cutVolume[i].z )
//		{
//			pmin.z = cutVolume[i].z;
//
//		}else if ( pmax.z < cutVolume[i].z )
//		{
//			pmax.z = cutVolume[i].z;
//
//		}
//	}
//
	vec4 pmin = min_point;
	vec4 pmax = max_point;

	float scaledy = 10;
	float scaledx = 10;
	float max_z   = 10*pmax.z;
	float min_z   = 10*pmin.z;

	/// Top and Bottom Faces
	/// Como ajustar o Z min/max , parametricamente.
	cutVolume[0] = vec4( pmax.x*scaledx , pmax.y*scaledy , max_z, 1.0 );
	cutVolume[1] = vec4( pmax.x*scaledx , pmax.y*scaledy , min_z, 1.0 );
	cutVolume[2] = vec4( pmin.x*scaledx , pmax.y*scaledy , min_z, 1.0 );
	cutVolume[3] = vec4( pmin.x*scaledx , pmax.y*scaledy , max_z, 1.0 );

	cutVolume[4] = vec4( pmax.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
	cutVolume[5] = vec4( pmin.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
	cutVolume[6] = vec4( pmin.x*scaledx , pmin.y*scaledy , min_z, 1.0 );
	cutVolume[7] = vec4( pmax.x*scaledx , pmin.y*scaledy , min_z, 1.0 );
//
//
//	base = inverse(ViewMatrix);
//
//	cutVolume[0] = base * cutVolume[0];
//	cutVolume[1] = base * cutVolume[1];
//	cutVolume[2] = base * cutVolume[2];
//	cutVolume[3] = base * cutVolume[3];
//
//	cutVolume[4] = base * cutVolume[4];
//	cutVolume[5] = base * cutVolume[5];
//	cutVolume[6] = base * cutVolume[6];
//	cutVolume[7] = base * cutVolume[7];
//
	p_min = cutVolume[6];
	p_max = cutVolume[0];


	// --- When Emit

	vec4 v[8];

	v[0] = vec4 ( gl_in[0].gl_Position.xyz , 1.0 );
	v[3] = vec4 ( gl_in[1].gl_Position.xyz , 1.0 );
	v[1] = vec4 ( gl_in[2].gl_Position.xyz , 1.0 );
	v[2] = vec4 ( gl_in[3].gl_Position.xyz , 1.0 );

	v[4] = vec4 ( gl_in[4].gl_Position.xyz , 1.0 );
	v[7] = vec4 ( gl_in[5].gl_Position.xyz , 1.0 );

	v[5] = vec4 ( gl_in[0].gl_Position.w , gl_in[1].gl_Position.w , gl_in[2].gl_Position.w , 1.0 );
	v[6] = vec4 ( gl_in[3].gl_Position.w , gl_in[4].gl_Position.w , gl_in[5].gl_Position.w , 1.0 );


	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));

	// Top

	if ( intersect_point_volume ( v[0] ) &&
	     intersect_point_volume ( v[1] ) &&
	     intersect_point_volume ( v[2] ) &&
	     intersect_point_volume ( v[3] ) &&
	     intersect_point_volume ( v[4] ) &&
	     intersect_point_volume ( v[5] ) &&
	     intersect_point_volume ( v[6] ) &&
	     intersect_point_volume ( v[7] ))
	{
		VertexOut.normal = normalize ( cross ( ( v[3].xyz - v[0].xyz ) , ( v[1].xyz - v[0].xyz ) ) );

		gl_Position = ProjectionMatrix * ViewMatrix * v[0];
		VertexOut.vertice = ViewMatrix * v[0];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[3];
		VertexOut.vertice = ViewMatrix * v[3];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[1];
		VertexOut.vertice = ViewMatrix * v[1];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[2];
		VertexOut.vertice = ViewMatrix * v[2];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );

		EndPrimitive ( );

	// Bottom

		VertexOut.normal = normalize ( cross ( ( v[7] - v[4] ).xyz , ( v[5] - v[4] ).xyz ) );

		gl_Position = ProjectionMatrix * ViewMatrix * v[4];
		VertexOut.vertice = ViewMatrix * v[4];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[5];
		VertexOut.vertice = ViewMatrix * v[5];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[7];
		VertexOut.vertice = ViewMatrix * v[7];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[6];
		VertexOut.vertice = ViewMatrix * v[6];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = vec4 ( VertexOut.normal , 1.0 );
		EmitVertex ( );

		EndPrimitive ( );

	// Front

	VertexOut.normal = normalize(cross( (v[7]-v[0]).xyz , (v[3]-v[0]).xyz));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
	VertexOut.vertice = ViewMatrix * v[0];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
	VertexOut.vertice = ViewMatrix * v[7];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
	VertexOut.vertice = ViewMatrix * v[3];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
	VertexOut.vertice = ViewMatrix * v[4];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();

	EndPrimitive();

//	// Back

	VertexOut.normal = normalize(cross( (v[2]-v[1]).xyz , (v[6]-v[1]).xyz));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
	VertexOut.vertice = ViewMatrix * v[1];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
	VertexOut.vertice = ViewMatrix * v[2];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
	VertexOut.vertice = ViewMatrix * v[6];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
	VertexOut.vertice = ViewMatrix * v[5];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();

	EndPrimitive();

//	// Right

	VertexOut.normal = normalize(cross( (v[3]-v[2]).xyz , (v[5]-v[2]).xyz));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
	VertexOut.vertice = ViewMatrix * v[2];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
	VertexOut.vertice = ViewMatrix * v[3];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
	VertexOut.vertice = ViewMatrix * v[5];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
	VertexOut.vertice = ViewMatrix * v[4];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();

	EndPrimitive();

	// Left

	VertexOut.normal = normalize(cross( (v[1]-v[0]).xyz , (v[7]-v[0]).xyz));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
	VertexOut.vertice = ViewMatrix * v[0];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
	VertexOut.vertice = ViewMatrix * v[1];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
	VertexOut.vertice = ViewMatrix * v[7];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
	VertexOut.vertice = ViewMatrix * v[6];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();

	EndPrimitive();
	}
}
