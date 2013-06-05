#version 430

layout ( triangles_adjacency ) in;
layout ( triangle_strip, max_vertices=24 ) out;

out VertexData
{
	vec4 vertice;
	vec3 normal;
	vec4 color;
} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{

	vec4 v[8];

	v[0] = vec4 ( gl_in[0].gl_Position.xyz , 1.0 );
	v[1] = vec4 ( gl_in[1].gl_Position.xyz , 1.0 );
	v[3] = vec4 ( gl_in[3].gl_Position.xyz , 1.0 );
	v[2] = vec4 ( gl_in[2].gl_Position.xyz , 1.0 );

	v[4] = vec4 ( gl_in[4].gl_Position.xyz , 1.0 );
	v[7] = vec4 ( gl_in[5].gl_Position.xyz , 1.0 );

	v[5] = vec4 ( gl_in[0].gl_Position.w , gl_in[1].gl_Position.w , gl_in[2].gl_Position.w , 1.0 );
	v[6] = vec4 ( gl_in[3].gl_Position.w , gl_in[4].gl_Position.w , gl_in[5].gl_Position.w , 1.0 );


	mat3 normalMatrix = inverse(transpose(mat3(ViewMatrix)));


	// Top
	VertexOut.normal = normalize(cross( (v[1].xyz-v[0].xyz) , (v[2].xyz-v[0].xyz) ));

	gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
	VertexOut.vertice =  ViewMatrix * v[0];
	VertexOut.normal = normalMatrix * VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
	VertexOut.vertice =  ViewMatrix * v[1];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = vec4 (VertexOut.normal,1.0);
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
	VertexOut.vertice =  ViewMatrix * v[3];
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
	VertexOut.normal = normalize(cross( (v[5]-v[4]).xyz , (v[7]-v[4]).xyz));

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
//
//	// Back
//	VertexOut.normal = normalize(cross( (v[7]-v[1]).xyz , (v[2]-v[1]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
//	VertexOut.vertice = ViewMatrix * v[1];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
//	VertexOut.vertice = ViewMatrix * v[7];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
//	VertexOut.vertice = ViewMatrix * v[2];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
//	VertexOut.vertice = ViewMatrix * v[6];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
//	EndPrimitive();
//
//	// Front
//	VertexOut.normal = normalize(cross( (v[3]-v[0]).xyz , (v[4]-v[0]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
//	VertexOut.vertice = ViewMatrix * v[0];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
//	VertexOut.vertice = ViewMatrix * v[3];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
//	VertexOut.vertice = ViewMatrix * v[4];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
//	VertexOut.vertice = ViewMatrix * v[5];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
//	EndPrimitive();
//
//	// Left
//	VertexOut.normal = normalize(cross( (v[4]-v[0]).xyz , (v[1]-v[0]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[0];
//	VertexOut.vertice = ViewMatrix * v[0];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[4];
//	VertexOut.vertice = ViewMatrix * v[4];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[1];
//	VertexOut.vertice = ViewMatrix * v[1];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[7];
//	VertexOut.vertice = ViewMatrix * v[7];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
////	EndPrimitive();
//
//	// Right
//	VertexOut.normal = normalize(cross( (v[6]-v[2]).xyz , (v[3]-v[2]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[2];
//	VertexOut.vertice = ViewMatrix * v[2];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[6];
//	VertexOut.vertice = ViewMatrix * v[6];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[3];
//	VertexOut.vertice = ViewMatrix * v[3];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * v[5];
//	VertexOut.vertice = ViewMatrix * v[5];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
//	EndPrimitive();
}
