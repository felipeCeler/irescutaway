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

const int linesize = 1;
const int linesizediag = 1;
const vec2 dist_neighbor[8] = {vec2(linesize,0), vec2(-linesize,0), vec2(0,linesize), vec2(0,-linesize),
                                vec2(-linesizediag,-linesizediag), vec2(-linesizediag,linesizediag), vec2(linesize,-linesize), vec2(linesizediag,-linesizediag)};

out vec4 outputColor;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;

void main(void)
{

        vec3 newNormal = normalize(VertexIn.normal.xyz);
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2(-2.0 * d * d);
        //float I = 0;

        bool backface = false;
        int linesize = 1;
        int linesizediag = 1;

        // push frontface back, so for double faces we see the interior of the cube (backface)
        if (dot ( newNormal.xyz, vec3(0.0,0.0,-1.0)) < 0.0) {            
            gl_FragDepth = gl_FragCoord.z + 0.0000001;
            linesize++;
            linesizediag++;
            //discard;
        }
        else {
            I = 0;
            backface = true;
            gl_FragDepth = gl_FragCoord.z;
        }

        vec2 dist_neighbor[8] = {vec2(linesize,0), vec2(-linesize,0), vec2(0,linesize), vec2(0,-linesize),
                                vec2(-linesizediag,-linesizediag), vec2(-linesizediag,linesizediag), vec2(linesize,-linesize), vec2(linesizediag,-linesizediag)};


        // make sure we are at the center of the pixel to make the right texture access
        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
        vec4 cutaway = texelFetch( normals, ivec2(pixel_pos), 0 ).rgba;

        // discard point in front of the cutaway surface
        if ( newVert.z > cutaway.w ) {
            discard;
        }

        int size = 8;
        {
            // check the neighbors, we are only interested in border pixels (neighbors to discarded pixels)
            float zsurface = 0;
            float zneighbor = 0;

            for (int i = 0; i < size; ++i) {

                // neighbor coordinate in range [0,1]
                vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy ;

                // depth of cutaway surface at neighbor pixel
                zsurface = texelFetch( normals, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;

                // invert the orthographic projection (considering ortho planes are in range [-1,1]
                vec2 pixel = neighbor*2.0 - vec2(1.0);
                pixel /= ModelMatrix[0][0];

                // intersection ray from point in image plane with plane containing current 3D point
                // note that the denominator is dot(l,n), but the ray in ortho is just (0,0,1)
                zneighbor = dot (newVert.xyz - vec3(pixel.xy, 0.0), newNormal.xyz) / newNormal.z;

                // if neighbor is in front of surface (was discarded), curent pixel is an edge pixel
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

        // interior cutaway lines (back face intersection with cutaway)
        if (backface && I == 1)
            outputColor = I * vec4(vec3(0.0), 1.0) + (1.0 - I) * ( VertexIn.color );
        // cutaway border lines (front face intersection with cutaway)
        else if (I == 1)
            outputColor = I * vec4(vec3(1.0,0.2,0.2), 1.0) + (1.0 - I) * ( VertexIn.color );
        // lines outside cutaway (remaining front faces)
        else
            outputColor = I * vec4(vec3(0.7), 1.0) + (1.0 - I) * ( VertexIn.color );
}
