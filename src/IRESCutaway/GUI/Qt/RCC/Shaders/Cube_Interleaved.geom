#version 430

layout ( points ) in;
layout ( triangle_strip, max_vertices=24 ) out;

out VertexData
{
	vec4 vertice;
	vec4 normal;
	vec4 color;
} VertexOut;

in CubeData
{
	vec4 v[8];
	vec4 n[6];
	vec4 color;
	vec4 IJK;
	vec4 focus;
	vec4 center;

} cube[1];
// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{

	vec4 v[8];


//	    vertex[0] = Center - extAxis0 - extAxis1 - extAxis2;
//	    vertex[1] = Center + extAxis0 - extAxis1 - extAxis2;
//	    vertex[2] = Center + extAxis0 + extAxis1 - extAxis2;
//	    vertex[3] = Center - extAxis0 + extAxis1 - extAxis2;
//	    vertex[4] = Center - extAxis0 - extAxis1 + extAxis2;
//	    vertex[5] = Center + extAxis0 - extAxis1 + extAxis2;
//	    vertex[6] = Center + extAxis0 + extAxis1 + extAxis2;
//	    vertex[7] = Center - extAxis0 + extAxis1 + extAxis2;


	// Top
	VertexOut.normal = cube[0].n[0];
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[4];
	VertexOut.vertice =  ViewMatrix * cube[0].v[4];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[5];
	VertexOut.vertice =  ViewMatrix * cube[0].v[5];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[7];
	VertexOut.vertice =  ViewMatrix * cube[0].v[7];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[6];
	VertexOut.vertice =  ViewMatrix * cube[0].v[6];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();

	EndPrimitive();


	// Bottom
	VertexOut.normal = cube[0].n[1];
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[0];
	VertexOut.vertice = ViewMatrix * cube[0].v[0];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[3];
	VertexOut.vertice = ViewMatrix * cube[0].v[3];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[1];
	VertexOut.vertice = ViewMatrix * cube[0].v[1];
	VertexOut.normal =  VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();
	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[2];
	VertexOut.vertice = ViewMatrix * cube[0].v[2];
	VertexOut.normal = VertexOut.normal;
	VertexOut.color  = cube[0].color;
	EmitVertex();

	EndPrimitive();
//
//	// Back
//	VertexOut.normal = normalize(cross( (cube[0].v[7]-cube[0].v[1]).xyz , (cube[0].v[2]-cube[0].v[1]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[1];
//	VertexOut.vertice = ViewMatrix * cube[0].v[1];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[7];
//	VertexOut.vertice = ViewMatrix * cube[0].v[7];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[2];
//	VertexOut.vertice = ViewMatrix * cube[0].v[2];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[6];
//	VertexOut.vertice = ViewMatrix * cube[0].v[6];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
//	EndPrimitive();
//
//	// Front
//	VertexOut.normal = normalize(cross( (cube[0].v[3]-cube[0].v[0]).xyz , (cube[0].v[4]-cube[0].v[0]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[0];
//	VertexOut.vertice = ViewMatrix * cube[0].v[0];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[3];
//	VertexOut.vertice = ViewMatrix * cube[0].v[3];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[4];
//	VertexOut.vertice = ViewMatrix * cube[0].v[4];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[5];
//	VertexOut.vertice = ViewMatrix * cube[0].v[5];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
//	EndPrimitive();
//
//	// Left
//	VertexOut.normal = normalize(cross( (cube[0].v[4]-cube[0].v[0]).xyz , (cube[0].v[1]-cube[0].v[0]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[0];
//	VertexOut.vertice = ViewMatrix * cube[0].v[0];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[4];
//	VertexOut.vertice = ViewMatrix * cube[0].v[4];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[1];
//	VertexOut.vertice = ViewMatrix * cube[0].v[1];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[7];
//	VertexOut.vertice = ViewMatrix * cube[0].v[7];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
////	EndPrimitive();
//
//	// Right
//	VertexOut.normal = normalize(cross( (cube[0].v[6]-cube[0].v[2]).xyz , (cube[0].v[3]-cube[0].v[2]).xyz));
//
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[2];
//	VertexOut.vertice = ViewMatrix * cube[0].v[2];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[6];
//	VertexOut.vertice = ViewMatrix * cube[0].v[6];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[3];
//	VertexOut.vertice = ViewMatrix * cube[0].v[3];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//	gl_Position =  ProjectionMatrix * ViewMatrix  * cube[0].v[5];
//	VertexOut.vertice = ViewMatrix * cube[0].v[5];
//	VertexOut.normal = normalMatrix * VertexOut.normal;
//	VertexOut.color  = vec4 (VertexOut.normal,1.0);
//	EmitVertex();
//
//	EndPrimitive();

}
