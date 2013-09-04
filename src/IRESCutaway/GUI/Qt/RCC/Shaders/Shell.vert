#version 430 core

layout(location = 0) in vec3 va;
layout(location = 1) in vec3 vb;
layout(location = 2) in vec3 vc;
layout(location = 3) in vec3 vd;

layout(location = 4) in int isShell;

layout(location = 5) in vec3 color;





out VertexData
{
    vec4 v[4];
    vec4 n[2];
    vec4 eye[4];
    vec4 color;

} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


void main(void)
{

//	VertexOut.color  = vec4(color,1.0 );
//	VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * vec4(va,1.0);
//	VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * vec4(vb,1.0);
//	VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * vec4(vc,1.0);
//	VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * vec4(vd,1.0);


	VertexOut.n[0] = vec4(normalize(cross(vb-va, vd-va)),0.0);
	VertexOut.n[1] = vec4(normalize(cross(vc-vb, vd-vb)),0.0);

	mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

	VertexOut.n[0] = vec4(normalMatrix * VertexOut.n[0].xyz,0.0);
	VertexOut.n[1] = vec4(normalMatrix * VertexOut.n[1].xyz,0.0);

	VertexOut.eye[0] =  ModelMatrix * ViewMatrix * vec4(va,1.0);
	VertexOut.eye[1] =  ModelMatrix * ViewMatrix * vec4(vb,1.0);
	VertexOut.eye[2] =  ModelMatrix * ViewMatrix * vec4(vc,1.0);
	VertexOut.eye[3] =  ModelMatrix * ViewMatrix * vec4(vd,1.0);

        if ( isShell == 1)
	{
		VertexOut.color  = vec4(color,1.0 );
		VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(va,1.0);
		VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vb,1.0);
		VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vc,1.0);
		VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vd,1.0);
	}
	else
	{
		VertexOut.color  = vec4(color,1.0 );
		VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(va,0.0);
		VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vb,0.0);
		VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vc,0.0);
		VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vd,0.0);
	}

	gl_Position = vec4(va,1.0);
}
