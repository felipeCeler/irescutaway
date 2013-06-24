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

uniform vec4 max_point;
uniform vec4 min_point;

uniform vec3 center_point;
uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


vec4 v[8];

vec4 cutVolume[8];


bool intersect_point_volume ( in vec4 p )
{

	// Top Face
	vec3 normal_top = normalize (cross ( (cutVolume[3].xyz - cutVolume[0].xyz).xyz, (cutVolume[1].xyz - cutVolume[0].xyz).xyz ) );

	// Bottom
	vec3 normal_bottom = normalize (cross ( (cutVolume[7].xyz - cutVolume[4].xyz).xyz, (cutVolume[5].xyz - cutVolume[4].xyz).xyz ) );

	// Back
	vec3 normal_back = normalize (cross ( (cutVolume[2].xyz - cutVolume[1].xyz).xyz, (cutVolume[7].xyz - cutVolume[1].xyz).xyz ) );

	// Right
	vec3 normal_right = normalize (cross ( (cutVolume[1].xyz - cutVolume[0].xyz).xyz, (cutVolume[4].xyz - cutVolume[0].xyz).xyz ) );

	// Left
	vec3 normal_left = normalize (cross ( (cutVolume[3].xyz - cutVolume[2].xyz).xyz, (cutVolume[6].xyz - cutVolume[2].xyz).xyz ) );

	if (  (dot (cutVolume[3].xyz-p.xyz,normal_top)    < 0 ) &&
	      (dot (cutVolume[7].xyz-p.xyz,normal_bottom) < 0 ) &&
	      (dot (cutVolume[2].xyz-p.xyz,normal_back)   < 0 ) &&
	      (dot (cutVolume[1].xyz-p.xyz,normal_right)  < 0 ) &&
	      (dot (cutVolume[3].xyz-p.xyz,normal_left)   < 0 )
	    )
	{
		return false;
	}



// 	if( ( p.x >= pmin.x ) && ( p.x < pmax.x ) &&
//	    ( p.y >= pmin.y ) && ( p.y < pmax.y ) &&
//	    ( p.z >= pmin.z ) && ( p.z < pmax.z ) )
//	{
//		return false;
//	}

	return true;

}

void main(void)
{

	float diagonal = distance(max_point,min_point);

	float side_of_cube = (diagonal * sqrt(3.0) ) / 3.0;

	vec3 ext_x = new_x * side_of_cube;
	vec3 ext_y = new_y * side_of_cube;
	vec3 ext_z = new_z * side_of_cube;

//	vec3 ext_x = vec3(1,0,0);
//	vec3 ext_y = vec3(0,1,0);
//	vec3 ext_z = vec3(0,0,1);

	vec3 center_of_mass =  center_point;
	float t = 0.09;

	cutVolume[0] = vec4(center_of_mass + ext_x + ext_y + ext_z + ext_x*t, 1.0);
	cutVolume[1] = vec4(center_of_mass + ext_x + ext_y - ext_z, 1.0);
	cutVolume[2] = vec4(center_of_mass - ext_x + ext_y - ext_z, 1.0);
	cutVolume[3] = vec4(center_of_mass - ext_x + ext_y + ext_z - ext_x*t, 1.0);

	cutVolume[4] = vec4(center_of_mass + ext_x - ext_y + ext_z + ext_x*t , 1.0);
	cutVolume[5] = vec4(center_of_mass - ext_x - ext_y + ext_z - ext_x*t, 1.0);
	cutVolume[6] = vec4(center_of_mass - ext_x - ext_y - ext_z, 1.0);
	cutVolume[7] = vec4(center_of_mass + ext_x - ext_y - ext_z, 1.0);

//	mat4 base = ModelMatrix;
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
	// --- When Emit

//	    Celer::Vector4<float> ( vecs[4], 1.0f),
//	    Celer::Vector4<float> ( vecs[5], 1.0f),
//	    Celer::Vector4<float> ( vecs[7], 1.0f),
//	    Celer::Vector4<float> ( vecs[6], 1.0f),
//	    // Bottom Face
//	    Celer::Vector4<float> ( vecs[0], 1.0f),
//	    Celer::Vector4<float> ( vecs[3], 1.0f),
//	    Celer::Vector4<float> ( vecs[1], 1.0f),
//	    Celer::Vector4<float> ( vecs[2], 1.0f),


	/// This order come from Charles IRES v2 -- @see Inkscape scheme.
	v[4] = vec4 ( gl_in[0].gl_Position.xyz , 1.0 );
	v[5] = vec4 ( gl_in[1].gl_Position.xyz , 1.0 );
	v[7] = vec4 ( gl_in[2].gl_Position.xyz , 1.0 );
	v[6] = vec4 ( gl_in[3].gl_Position.xyz , 1.0 );

	v[0] = vec4 ( gl_in[4].gl_Position.xyz , 1.0 );
	v[3] = vec4 ( gl_in[5].gl_Position.xyz , 1.0 );

	v[1] = vec4 ( gl_in[0].gl_Position.w , gl_in[1].gl_Position.w , gl_in[2].gl_Position.w , 1.0 );
	v[2] = vec4 ( gl_in[3].gl_Position.w , gl_in[4].gl_Position.w , gl_in[5].gl_Position.w , 1.0 );


	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));

	// Top

	if ( intersect_point_volume (v[0]) &&
	     intersect_point_volume (v[1]) &&
	     intersect_point_volume (v[2]) &&
	     intersect_point_volume (v[3]) &&
	     intersect_point_volume (v[4]) &&
	     intersect_point_volume (v[5]) &&
	     intersect_point_volume (v[6]) &&
	     intersect_point_volume (v[7]))
	{
		VertexOut.normal = normalize ( cross ( ( v[3].xyz - v[0].xyz ) , ( v[1].xyz - v[0].xyz ) ) );

		gl_Position = ProjectionMatrix * ViewMatrix * v[0];
		VertexOut.vertice = ViewMatrix * v[0];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[3];
		VertexOut.vertice = ViewMatrix * v[3];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[1];
		VertexOut.vertice = ViewMatrix * v[1];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[2];
		VertexOut.vertice = ViewMatrix * v[2];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );

		EndPrimitive ( );

	// Bottom

		VertexOut.normal = normalize ( cross ( ( v[5] - v[4] ).xyz , ( v[7] - v[4] ).xyz ) );

		gl_Position = ProjectionMatrix * ViewMatrix * v[4];
		VertexOut.vertice = ViewMatrix * v[4];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[5];
		VertexOut.vertice = ViewMatrix * v[5];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[7];
		VertexOut.vertice = ViewMatrix * v[7];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );
		gl_Position = ProjectionMatrix * ViewMatrix * v[6];
		VertexOut.vertice = ViewMatrix * v[6];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color = cell_properties[0].color;
		EmitVertex ( );

		EndPrimitive ( );

		// Front

		VertexOut.normal = normalize(cross( (v[1]-v[0]).xyz , (v[4]-v[0]).xyz));

		gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
		VertexOut.vertice = ViewMatrix * v[4];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
		VertexOut.vertice = ViewMatrix * v[0];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
		VertexOut.vertice = ViewMatrix * v[5];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
		VertexOut.vertice = ViewMatrix * v[1];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();

		EndPrimitive();

	//	// Back

		VertexOut.normal = normalize(cross( (v[3]-v[2]).xyz , (v[6]-v[2]).xyz));

		gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
		VertexOut.vertice = ViewMatrix * v[2];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
		VertexOut.vertice = ViewMatrix * v[3];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
		VertexOut.vertice = ViewMatrix * v[6];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
		VertexOut.vertice = ViewMatrix * v[7];
		VertexOut.normal = VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();

		EndPrimitive();

	//	// Right

		VertexOut.normal = normalize(cross( (v[2]-v[1]).xyz , (v[5]-v[1]).xyz));

		gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
		VertexOut.vertice = ViewMatrix * v[1];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
		VertexOut.vertice = ViewMatrix * v[2];
		VertexOut.normal =  VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
		VertexOut.vertice = ViewMatrix * v[5];
		VertexOut.normal =  VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
		VertexOut.vertice = ViewMatrix * v[6];
		VertexOut.normal =  VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();

		EndPrimitive();

		// Left

		VertexOut.normal = normalize(cross( (v[4]-v[0]).xyz , (v[3]-v[0]).xyz));

		gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
		VertexOut.vertice = ViewMatrix * v[0];
		VertexOut.normal = normalMatrix * VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
		VertexOut.vertice = ViewMatrix * v[3];
		VertexOut.normal =  VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
		VertexOut.vertice = ViewMatrix * v[4];
		VertexOut.normal =  VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();
		gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
		VertexOut.vertice = ViewMatrix * v[7];
		VertexOut.normal =  VertexOut.normal;
		VertexOut.color  = cell_properties[0].color;
		EmitVertex();

		EndPrimitive();
	}
}
