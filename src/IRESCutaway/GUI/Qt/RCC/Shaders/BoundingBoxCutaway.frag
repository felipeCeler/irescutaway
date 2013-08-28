#version 430 core

layout(location = 0) uniform sampler2D normals;
//layout(pixel_center_integer) in vec4 gl_FragCoord;

in VertexData
{
                vec4 vertice;
                flat vec4 normal;
		vec4 color;
} VertexIn;

noperspective in vec4 dist;

const vec2 dist_neighbor[8] = {vec2(1,0), vec2(-1,0), vec2(0,1), vec2(0,-1),
                                vec2(-1,-1), vec2(-1,1), vec2(1,-1), vec2(1,-1)};

out vec4 outputColor;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;

void main(void)
{

        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
        vec4 cutaway = texelFetch( normals , ivec2(pixel_pos), 0 ).rgba;

        if ( VertexIn.vertice.z > cutaway.w ) {
            discard;
        }

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2.0 * d * d);

	vec3 newNormal = VertexIn.normal.xyz;
	vec3 newVert = VertexIn.vertice.xyz;
	vec4 color_t = VertexIn.color;

        //gl_FragDepth = cutaway.w;

        //newNormal = normalize ( newNormal );
        if (dot ( newNormal.xyz, vec3(0.0,0.0,-1.0)) < 0.0) {
            discard;
        }




        I = 0;

        int size = 4;
        {
            // check the neighbors, we are only interested in border pixels (neighbors to discarded pixels)

            float zsurface;
            float zneighbor;

            for (int i = 0; i < size; ++i) {

                // neighbor coordinate in range [0,1]
                vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy ;

                // depth of cutaway surface at neighbor pixel
                zsurface = texelFetch( normals, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;

                // invert the orthographic projection (considering ortho planes are in range [-1,1]
                vec2 pixel = neighbor*2.0 - vec2(1.0);
                pixel /= ModelMatrix[0][0];

                zneighbor = dot (newVert.xyz - vec3(pixel.xy, 0.0), newNormal.xyz) / newNormal.z;

                // find the displacement vector (parallel to image plane) in camera space
                //vec3 dvec = vec3( pixel.xy - newVert.xy, 0.0 );

                // compute the depth value in camera space for the neighbor point in face plane
                //zneighbor = newVert.z + dot(newNormal, dvec);

                if (zneighbor > zsurface)
                {
                   I=1;
                }
            }

        }


	vec3 toLight = normalize ( -newVert.xyz );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	//vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );//geometryShader_VertexData.propertyColor;

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );

	outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * ( VertexIn.color );
        //outputColor = vec4(cutaway.www, 1.0);

}
