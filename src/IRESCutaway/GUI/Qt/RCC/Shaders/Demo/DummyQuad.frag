#version 430 core

uniform sampler2D texture;

out vec4 outputColor;

in vec2 texcoord;

noperspective in vec4 dist;
uniform vec2 WIN_SCALE;

uniform int num_lights;
uniform vec3 lights[4];

void main(void)
{

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2( -0.1 * d * d);

        vec4 color = texture2D ( texture , texcoord ).rgba;
        //outputColor =  vec4 ( color );
        //outputColor =  vec4 ( 1.0, 0.0, 0.0, 1.0 );

        outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( 0.75, 0.75, 0.75, 0.7 );;
}
