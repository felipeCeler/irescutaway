#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

uniform vec4 center_points[256];
uniform vec4 max_points[256];
uniform vec4 min_points[256];
uniform int cut_volume_size;

uniform vec3 new_x;
uniform vec3 new_y;
uniform vec3 new_z;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out VertexData
{
   vec4 color;
   vec3 vert;
   vec3 normal;
} VertexOut;

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

void byCentroid ( )
{
	vec4 v[8];
	vec4 vertex[8];


//	vec3 center_of_mass =  vec3(0,0,0);
	vec3 center_of_mass =  center_points[0].xyz;

	v[0] = vec4(center_of_mass + ext_x + ext_y + 100*ext_z + 5*ext_x,1.0);
	v[1] = vec4(center_of_mass + ext_x + ext_y - ext_z,1.0);
	v[2] = vec4(center_of_mass - ext_x + ext_y - ext_z,1.0);
	v[3] = vec4(center_of_mass - ext_x + ext_y + 100*ext_z - 5*ext_x,1.0);

	v[4] = vec4(center_of_mass + ext_x - ext_y + 100*ext_z + 5*ext_x,1.0);
	v[5] = vec4(center_of_mass - ext_x - ext_y + 100*ext_z - 5*ext_x,1.0);
	v[6] = vec4(center_of_mass - ext_x - ext_y - ext_z,1.0);
	v[7] = vec4(center_of_mass + ext_x - ext_y - ext_z,1.0);


	vertex[0] = ViewMatrix * v[0];
	vertex[1] = ViewMatrix * v[1];
	vertex[2] = ViewMatrix * v[2];
	vertex[3] = ViewMatrix * v[3];

	vertex[4] = ViewMatrix * v[4];
	vertex[5] = ViewMatrix * v[5];
	vertex[6] = ViewMatrix * v[6];
	vertex[7] = ViewMatrix * v[7];

//	mat3 normalMatrix = mat3(1.0);

	mat3 normalMatrix = mat3(ViewMatrix);
	normalMatrix = inverse(normalMatrix);
	normalMatrix = transpose(normalMatrix);

	// Top Face
	vec3 normal_top = normalize (cross ( (v[3] - v[0]).xyz, (v[1] - v[0]).xyz ) );
	normal_top =  normalize(normalMatrix * normal_top);

	// Bottom
	vec3 normal_bottom = normalize (cross ( (v[7] - v[4]).xyz, (v[5] - v[4]).xyz ) );
	normal_bottom = normalize(normalMatrix * normal_bottom);

	// Back
	vec3 normal_back = normalize (cross ( (v[2] - v[1]).xyz, (v[7] - v[1]).xyz ) );
	normal_back =  normalize(normalMatrix * normal_back);

	// Right
	vec3 normal_right = normalize (cross ( (v[1] - v[0]).xyz, (v[4] - v[0]).xyz ) );
	normal_right = normalize(normalMatrix * normal_right);

	// Left
	vec3 normal_left = normalize (cross ( (v[3] - v[2]).xyz, (v[6] - v[2]).xyz ) );
	normal_left = normalize(normalMatrix * normal_left);

	mat4 projection = ProjectionMatrix * ViewMatrix ;

	v[0] =  projection * v[0];
	v[1] =  projection * v[1];
	v[2] =  projection * v[2];
	v[3] =  projection * v[3];

	v[4] =  projection * v[4];
	v[5] =  projection * v[5];
	v[6] =  projection * v[6];
	v[7] =  projection * v[7];

	//Top face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[0].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[3].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[3]/v[3].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_top;
	gl_Position = v[2]/v[2].w;
	EmitVertex();

	EndPrimitive();

	// Bottom face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[4].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[4]/v[4].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[5].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[5]/v[5].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[7]/v[7].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_bottom;
	gl_Position = v[6]/v[6].w;
	EmitVertex();

	EndPrimitive();

	// Back face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[7]/v[7].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_back;
	gl_Position = v[6]/v[6].w;
	EmitVertex();

	EndPrimitive();

	//Right face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[0].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[0]/v[0].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[4].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[4]/v[4].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[1].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[1]/v[1].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[7].xyz;
	VertexOut.normal = normal_right;
	gl_Position = v[7]/v[7].w;
	EmitVertex();

	EndPrimitive();

	//Left face
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[2].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[2]/v[2].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[6].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[6]/v[6].w;
	EmitVertex();
	VertexOut.color = vec4( 1.0,0.0,0.0,1.0);
	VertexOut.vert  = vertex[3].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[3]/v[3].w;
	EmitVertex();
	VertexOut.color = vec4 ( 1.0 , 0.0 , 0.0 , 1.0 );
	VertexOut.vert = vertex[5].xyz;
	VertexOut.normal = normal_left;
	gl_Position = v[5] / v[5].w;
	EmitVertex();

	EndPrimitive();

}


void main(void)
{

	ext_x = new_x*0.3;
	ext_y = new_y*0.3;
	ext_z = new_z*0.1;

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

