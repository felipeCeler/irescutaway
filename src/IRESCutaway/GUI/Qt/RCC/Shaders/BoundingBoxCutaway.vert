#version 430

layout(location = 0) in vec4 v0;
layout(location = 1) in vec4 v1;
layout(location = 2) in vec4 v2;
layout(location = 3) in vec4 v3;
layout(location = 4) in vec4 v4;
layout(location = 5) in vec4 v5;
layout(location = 6) in vec4 v6;
layout(location = 7) in vec4 v7;

layout(location = 8)  in vec4 color;
layout(location = 9)  in vec4 IJK;
layout(location = 10) in vec4 focus;
layout(location = 11) in vec4 center;

/// FIXME - Do research and understand the best away to align data on Shader.
out CubeData
{
		vec4 v[8];
                vec4 n[12];
		vec4 color;
} cube;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


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

                mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));
                vec4 norm_alt;

                // for each quad, keeprs the normal (from each triangle pair) that has higher z (avoids bad
                // backface classification in frag shader)

                // top
                cube.n[0] = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[4]).xyz , (cube.v[7]-cube.v[4]).xyz )),0.0);
                cube.n[6]  = vec4 ( normalize( normalMatrix * cross( (cube.v[7]-cube.v[6]).xyz , (cube.v[5]-cube.v[6]).xyz )),0.0);

                // bottom
                cube.n[1]  = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[0]).xyz , (cube.v[1]-cube.v[0]).xyz )),0.0);
                cube.n[7] = vec4 ( normalize( normalMatrix * cross( (cube.v[1]-cube.v[2]).xyz , (cube.v[3]-cube.v[2]).xyz )),0.0);

                // front
                cube.n[2] = vec4 ( normalize( normalMatrix * cross( (cube.v[0]-cube.v[4]).xyz , (cube.v[5]-cube.v[4]).xyz )),0.0);
                cube.n[8]  = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[1]).xyz , (cube.v[0]-cube.v[1]).xyz )),0.0);

                // back
                cube.n[3] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[2]).xyz , (cube.v[6]-cube.v[2]).xyz )),0.0);
                cube.n[9]  = vec4 ( normalize( normalMatrix * cross( (cube.v[6]-cube.v[7]).xyz , (cube.v[3]-cube.v[7]).xyz )),0.0);

                // left
                cube.n[4] = vec4 ( normalize( normalMatrix * cross( (cube.v[2]-cube.v[1]).xyz , (cube.v[5]-cube.v[1]).xyz )),0.0);
                cube.n[10]  = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[6]).xyz , (cube.v[2]-cube.v[6]).xyz )),0.0);

                // right
                cube.n[5] = vec4 ( normalize( normalMatrix * cross( (cube.v[4]-cube.v[0]).xyz , (cube.v[3]-cube.v[0]).xyz )),0.0);
                cube.n[11]  = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[7]).xyz , (cube.v[4]-cube.v[7]).xyz )),0.0);

		gl_Position =  center;
}
