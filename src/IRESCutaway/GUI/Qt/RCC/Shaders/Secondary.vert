#version 430

layout(location = 0) in vec4 v0;
layout(location = 1) in vec4 v1;
layout(location = 2) in vec4 v2;
layout(location = 3) in vec4 v3;
layout(location = 4) in vec4 v4;
layout(location = 5) in vec4 v5;
layout(location = 6) in vec4 v6;
layout(location = 7) in vec4 v7;

// Four property x = Bubble Point Pressure
//               y = Pressure
//	         z = Porosity
//               w = Modified Block Volume
layout(location = 8) in vec4 static_properties;

/// FIXME - Do research and understand the best away to alignment data on Shader.
out CubeData
{
		vec4  v[8];
		vec4  n[6];
		vec4  color;
flat	float internal;
} cube;

// For while, all transformations come from the Celer::Camera.

layout(std140) uniform GlobalMatrices
{
    mat4 ModelMatrix;
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
} globalMatrices;

uniform vec3 min_IJK;
uniform vec3 max_IJK;


void main(void)
{


        cube.v[4] = v4;
        cube.v[5] = v5;
        cube.v[7] = v7;
        cube.v[6] = v6;

        cube.v[0] = v0;
        cube.v[3] = v3;
        cube.v[1] = v1;
        cube.v[2] = v2;

        cube.color    = color;

        mat3 normalMatrix = inverse(transpose(mat3(globalMatrices.ViewMatrix)));

        cube.n[0] = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[4]).xyz , (cube.v[7]-cube.v[4]).xyz )),0.0);
        cube.n[1] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[0]).xyz , (cube.v[1]-cube.v[0]).xyz )),0.0);

        cube.n[2] = vec4 ( normalize( normalMatrix * cross( (cube.v[1]-cube.v[0]).xyz , (cube.v[4]-cube.v[0]).xyz )),0.0);
        cube.n[3] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[2]).xyz , (cube.v[6]-cube.v[2]).xyz )),0.0);

        cube.n[4] = vec4 ( normalize( normalMatrix * cross( (cube.v[2]-cube.v[1]).xyz , (cube.v[5]-cube.v[1]).xyz )),0.0);
        cube.n[5] = vec4 ( normalize( normalMatrix * cross( (cube.v[4]-cube.v[0]).xyz , (cube.v[3]-cube.v[0]).xyz )),0.0);

        gl_Position =  v0;



}
