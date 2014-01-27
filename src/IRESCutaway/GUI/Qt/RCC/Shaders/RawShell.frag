#version 430 core

uniform sampler2D xtoon_texture;
uniform ivec2 viewportSize;

in VertexData
{
	vec4 vertice;
   flat vec4 normal;
	vec4 color;
} VertexIn;

noperspective in vec4 dist;

uniform int num_lights;
uniform vec3 lights[4];

out vec4 outputColor;




void main(void)
{

	vec2 texCoord = vec2(gl_FragCoord.x/viewportSize[0], gl_FragCoord.y/viewportSize[1]);

	vec3 newNormal = VertexIn.normal.xyz;
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        // xToon aerial perspective
        vec2 xtoon_texCoord = vec2(0.0);

	float zmin = -5.0;
	float zmax = -4.0;
	xtoon_texCoord.t = log(newVert.z / zmin) / log(zmax/zmin);

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2(-0.75 * d * d);

        vec4 la = vec4(0.0);
        vec4 ld = vec4(0.0);
        vec4 ls = vec4(0.0);

        // compute illumination for each light
        for (int i = 0; i < num_lights; ++i) {
            vec3 light_dir = normalize(lights[i]);
            vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
            la += vec4 ( 0.3 / float(num_lights) );
            ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
            //ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );

            // xToon aerial perspective
            xtoon_texCoord.s = abs(dot ( newNormal , light_dir ) );

        }

        vec4 color = la + ld + ls;

        vec4 xtoon = texture(xtoon_texture, xtoon_texCoord ).rgba;

        color = xtoon;

        outputColor = I * vec4(vec3(1.0), 1.0) + (1.0 - I) * ( color );


}
