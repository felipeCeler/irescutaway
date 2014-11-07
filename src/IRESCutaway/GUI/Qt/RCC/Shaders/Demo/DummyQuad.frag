#version 430 core

uniform sampler2D texture;


in VertexData
{
        vec4 vertice;
        vec4 normal;
        vec4 color;
} VertexIn;

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

        vec3 newNormal = normalize(VertexIn.normal.xyz);
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;


        vec4 la = vec4(0.0);
        vec4 ld = vec4(0.0);
        vec4 ls = vec4(0.0);

        // compute illumination for each light
        for (int i = 0; i < num_lights; ++i) {
            vec3 light_dir = normalize(lights[i]);
            //vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
            la += vec4 ( 0.3 / float(num_lights) );
            ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
            //ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
        }


        vec4 color = la + ld + ls;
        color.a = 0.7;

        //vec4 color = texture2D ( texture , texcoord ).rgba;
        //outputColor =  vec4 ( color );
        //outputColor =  vec4 ( 1.0, 0.0, 0.0, 1.0 );

        discard;

        //outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * vec4 ( 0.7, 0.7, 0.7, 1.0 );;
        outputColor = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * color;
}
