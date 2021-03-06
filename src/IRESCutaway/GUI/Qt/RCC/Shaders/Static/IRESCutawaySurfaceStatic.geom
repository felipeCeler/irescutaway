#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in CubeData
{
		vec4 v[8];
		vec3 n[6];
		vec4 color;
} cube[];


out VertexData
{
   vec4 color;
   vec4 verticeEye;
   vec4 verticeProj;
   vec3 normal;
} VertexOut;


struct Face
{
	int 	vertices[6];
	vec3 	normal;
};

Face cutVolume[6];

void byCentroid ( )
{

                for ( int i = 0; i < 6; i++ )
                {

                        VertexOut.normal = cube[0].n[i];
                        VertexOut.color = cube[0].color;

                        VertexOut.verticeEye = ViewMatrix * cube[0].v[cutVolume[i].vertices[0]];
                        gl_Position = ProjectionMatrix * VertexOut.verticeEye;
                        VertexOut.verticeProj = gl_Position;
                        EmitVertex ( );

                        VertexOut.verticeEye = ViewMatrix * cube[0].v[cutVolume[i].vertices[1]];
                        gl_Position = ProjectionMatrix * ModelMatrix * VertexOut.verticeEye;
                        VertexOut.verticeProj = gl_Position;
                        EmitVertex ( );

                        VertexOut.verticeEye = ViewMatrix * cube[0].v[cutVolume[i].vertices[3]];
                        gl_Position = ProjectionMatrix * ModelMatrix * VertexOut.verticeEye;
                        VertexOut.verticeProj = gl_Position;
                        EmitVertex ( );

                        VertexOut.verticeEye = ViewMatrix * cube[0].v[cutVolume[i].vertices[2]];
                        gl_Position = ProjectionMatrix * ModelMatrix * VertexOut.verticeEye;
                        VertexOut.verticeProj = gl_Position;
                        EmitVertex ( );

                        EndPrimitive ( );
                }
}


void main(void)
{


	// top
	cutVolume[0].vertices[0] = 0;
	cutVolume[0].vertices[1] = 1;
	cutVolume[0].vertices[2] = 2;
	cutVolume[0].vertices[3] = 3;
	// bottom
	cutVolume[1].vertices[0] = 4;
	cutVolume[1].vertices[1] = 5;
	cutVolume[1].vertices[2] = 6;
	cutVolume[1].vertices[3] = 7;
	// front
	cutVolume[2].vertices[0] = 0;
	cutVolume[2].vertices[1] = 3;
	cutVolume[2].vertices[2] = 5;
	cutVolume[2].vertices[3] = 4;
	// back
	cutVolume[3].vertices[0] = 1;
	cutVolume[3].vertices[1] = 7;
	cutVolume[3].vertices[2] = 6;
	cutVolume[3].vertices[3] = 2;
	// right
	cutVolume[4].vertices[0] = 0;
	cutVolume[4].vertices[1] = 4;
	cutVolume[4].vertices[2] = 7;
	cutVolume[4].vertices[3] = 1;
	// left
	cutVolume[5].vertices[0] = 2;
	cutVolume[5].vertices[1] = 6;
	cutVolume[5].vertices[2] = 5;
	cutVolume[5].vertices[3] = 3;

	byCentroid();
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

