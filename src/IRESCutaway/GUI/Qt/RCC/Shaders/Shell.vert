#version 430 core

layout(location = 0) in vec4 va;
layout(location = 1) in vec4 vb;
layout(location = 2) in vec4 vc;
layout(location = 3) in vec4 vd;

layout(location = 4) in vec4 isShell;
layout(location = 5) in vec4 color;
layout(location = 6) in vec4 IJK;





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


	VertexOut.n[0] = vec4(normalize(cross(vb.xyz-va.xyz, vd.xyz-va.xyz)),0.0);
	VertexOut.n[1] = vec4(normalize(cross(vc.xyz-vb.xyz, vd.xyz-vb.xyz)),0.0);

	mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

	VertexOut.n[0] = vec4(normalMatrix * VertexOut.n[0].xyz,0.0);
	VertexOut.n[1] = vec4(normalMatrix * VertexOut.n[1].xyz,0.0);

	VertexOut.eye[0] =  ModelMatrix * ViewMatrix * vec4(va);
	VertexOut.eye[1] =  ModelMatrix * ViewMatrix * vec4(vb);
	VertexOut.eye[2] =  ModelMatrix * ViewMatrix * vec4(vc);
	VertexOut.eye[3] =  ModelMatrix * ViewMatrix * vec4(vd);

        if ( isShell.x == 1.0 ) // Shell faces
	{
		VertexOut.color  = vec4( color );
		VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(va);
		VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vb);
		VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vc);
		VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vd);
	}
	else  // Interior faces
	{
		VertexOut.color  = vec4( 1.0,0.0,0.0,1.0);
		VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
		VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
		VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
		VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
	}

	gl_Position = vec4(va);
}
