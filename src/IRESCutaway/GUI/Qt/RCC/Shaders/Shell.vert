#version 430 core

layout(location = 0) in vec3 v0;
layout(location = 1) in vec3 v1;
layout(location = 2) in vec3 v2;
layout(location = 3) in vec3 v3;

layout(location = 4) in int isShell;

layout(location = 5) in vec3 color;





out VertexData
{
    vec4 v[4];
    vec4 color;

} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


void main(void)
{

//	VertexOut.color  = vec4(color,1.0 );
//	VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * vec4(v0,1.0);
//	VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * vec4(v1,1.0);
//	VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * vec4(v2,1.0);
//	VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * vec4(v3,1.0);

	if ( isShell == 0)
	{
		VertexOut.color  = vec4(color,1.0 );
		VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * vec4(v0,1.0);
		VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * vec4(v1,1.0);
		VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * vec4(v2,1.0);
		VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * vec4(v3,1.0);
	}
	else
	{
		VertexOut.color  = vec4(color,1.0 );
		VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * vec4(v0,0.0);
		VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * vec4(v1,0.0);
		VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * vec4(v2,0.0);
		VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * vec4(v3,0.0);
	}

	gl_Position = vec4(v0,1.0);
}
