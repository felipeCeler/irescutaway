#version 430 core

in VertexData
{
                vec4 vertice;
                flat vec4 normal;
		vec4 color;
} VertexIn;

noperspective in vec4 dist;

out vec4 outputColor;

uniform int num_lights;
uniform vec3 lights[4];

void main(void)
{

        vec3 newNormal = VertexIn.normal.xyz;
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2(-1.0 * d * d);

        vec4 la = vec4(0.0);
        vec4 ld = vec4(0.0);
        vec4 ls = vec4(0.0);

        vec3 eye_dir = normalize ( -newVert.xyz );

        // compute illumination for each light
        for (int i = 0; i < num_lights; ++i) {
            vec3 light_dir = normalize(lights[i]);
            vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
            la += vec4 ( 0.3 / float(num_lights) );
            ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
            //ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
        }

        vec4 color = la + ld + ls;
        outputColor = I * vec4(vec3(1.0), 1.0) + (1.0 - I) * ( color );


}
