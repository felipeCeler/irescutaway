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

void main(void)
{


//	// Top  and Bottom Face
//	v[0] = vec4( max_point.x , max_point.y , max_point.z, 1.0 );
//	v[1] = vec4( max_point.x , max_point.y , min_point.z, 1.0 );
//	v[2] = vec4( min_point.x , max_point.y , min_point.z, 1.0 );
//	v[3] = vec4( min_point.x , max_point.y , max_point.z, 1.0 );
//
//	v[4] = vec4( max_point.x , min_point.y , max_point.z, 1.0 );
//	v[5] = vec4( min_point.x , min_point.y , max_point.z, 1.0 );
//	v[6] = vec4( min_point.x , min_point.y , min_point.z, 1.0 );
//	v[7] = vec4( max_point.x , min_point.y , min_point.z, 1.0 );
//
//	mat4 base = ViewMatrix;
//
//	v[0] = base * v[0];
//	v[1] = base * v[1];
//	v[2] = base * v[2];
//	v[3] = base * v[3];
//
//	v[4] = base * v[4];
//	v[5] = base * v[5];
//	v[6] = base * v[6];
//	v[7] = base * v[7];
//
//	mat3 normalMatrix = mat3(1.0);
//
//	vertex[0] = v[0];
//	vertex[1] = v[1];
//	vertex[2] = v[2];
//	vertex[3] = v[3];
//
//	vertex[4] = v[4];
//	vertex[5] = v[5];
//	vertex[6] = v[6];
//	vertex[7] = v[7];
//
//
//	vec4 pmin = v[0];
//	vec4 pmax = pmin;
//
//	for ( int i = 0; i < 8; i++ )
//	{
//		if ( pmin.x > v[i].x )
//		{
//			pmin.x = v[i].x;
//
//		}else if ( pmax.x < v[i].x )
//		{
//			pmax.x = v[i].x;
//		}
//
//		if ( pmin.y > v[i].y )
//		{
//			pmin.y = v[i].y;
//
//		}else if ( pmax.y < v[i].y )
//		{
//			pmax.y = v[i].y;
//		}
//
//		if ( pmin.z > v[i].z )
//		{
//			pmin.z = v[i].z;
//
//		}else if ( pmax.z < v[i].z )
//		{
//			pmax.z = v[i].z;
//
//		}
//	}
//
//
//	float scaledy = 20;
//	float scaledx = 20;
//	float max_z   = 10;
//
//	/// Back and Front Face
//	/// Como ajustar o Z min/max , parametricamente.
//	v[0] = vec4( pmax.x*scaledx , pmax.y*scaledy , max_z, 1.0 );
//	v[1] = vec4( pmax.x , pmax.y , pmin.z, 1.0 );
//	v[2] = vec4( pmin.x , pmax.y , pmin.z, 1.0 );
//	v[3] = vec4( pmin.x*scaledx , pmax.y*scaledy , max_z, 1.0 );
//
//	v[4] = vec4( pmax.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
//	v[5] = vec4( pmin.x*scaledx , pmin.y*scaledy , max_z, 1.0 );
//	v[6] = vec4( pmin.x , pmin.y , pmin.z, 1.0  );
//	v[7] = vec4( pmax.x , pmin.y , pmin.z, 1.0 );

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
	VertexOut.normal = normalize(cross( (v[3].xyz-v[0].xyz) , (v[1].xyz-v[0].xyz) ));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
	VertexOut.vertice =  ViewMatrix * v[0];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
	VertexOut.vertice =  ViewMatrix * v[3];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
	VertexOut.vertice =  ViewMatrix * v[1];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
	VertexOut.vertice =  ViewMatrix * v[2];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();

	EndPrimitive();

	// Bottom
	VertexOut.normal = normalize(cross( (v[7]-v[4]).xyz , (v[5]-v[4]).xyz));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
	VertexOut.vertice = ViewMatrix * v[4];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
	VertexOut.vertice = ViewMatrix * v[5];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
	VertexOut.vertice = ViewMatrix * v[7];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
	VertexOut.vertice = ViewMatrix * v[6];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();

	EndPrimitive();

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
