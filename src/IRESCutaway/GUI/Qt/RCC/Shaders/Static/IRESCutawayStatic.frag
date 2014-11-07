#version 430 core

layout(location = 0) uniform sampler2D normals;

in VertexData
{
                vec4 vertice;
flat            vec4 normal;
		vec4 color;
} VertexIn;


out vec4 outputColor;

uniform vec2 WIN_SCALE;
uniform int num_lights;
uniform vec3 lights[4];

void main(void)
{

        vec3 newNormal = normalize(VertexIn.normal.xyz);
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        bool backface = false;
        int linesize = 1;
        int linesizediag = 1;

        float I = 0;

//        //outputColor =  vec4( 1.0,0.0,0.0,1.0 );
//
//        //push frontface back, so for double faces we see the interior of the cube (backface)
        if (newNormal.z > 0.0)
        {
            discard;
        }


        vec2 dist_neighbor[8] = {vec2(linesize,0), vec2(-linesize,0), vec2(0,linesize), vec2(0,-linesize),
                                vec2(-linesizediag,-linesizediag), vec2(-linesizediag,linesizediag), vec2(linesize,-linesize), vec2(linesizediag,-linesizediag)};


        // make sure we are at the center of the pixel to make the right texture access
        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
        // cutaway normal = rgb, and cutaway depth in camera space = w
        vec4 cutaway = texelFetch( normals, ivec2(pixel_pos), 0 ).rgba;

        // discard point in front of the cutaway surface
        if ( newVert.z > cutaway.w ) {
            discard;
        }

        int size = 8;

        // check the neighbors, we are only interested in border pixels (neighbors to discarded pixels)
        float zsurface = 0;
        float zneighbor = 0;

       float aspect_ratio = (WIN_SCALE.x/WIN_SCALE.y);


        I = 0;

        for (int i = 0; i < size; ++i) {
            if (I != 1)
            {
                // neighbor coordinate in range [0,1]
                vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy ;

                // depth of cutaway surface at neighbor pixel
                zsurface = texelFetch( normals, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;

                // invert the orthographic projection (considering ortho planes are in range [-1,1]
                vec2 pixel = (neighbor*2.0 - vec2(1.0));
                pixel.x *= aspect_ratio;

                // intersection ray from point in image plane with plane containing current 3D point
                // note that the denominator is dot(l,n), but the ray in ortho is just (0,0,1)
                zneighbor = (dot (newVert.xyz - vec3(pixel.xy, 0.0), newNormal.xyz)) / newNormal.z;

                // if neighbor is in front of surface (was discarded), curent pixel is an edge pixel
                if (zneighbor > zsurface)
                {
                    I = 1;
                    break;
                }
            }
        }

//        if ( (abs(newVert.z - (cutaway.w)) < 0.009) ) {
//          I = 1;
//        }

        // for back faces use the normal of the cutaway surface (simulate a cut inside the cells)


        vec3 eye_dir = vec3(0.0);
        vec4 la = vec4(0.0);
        vec4 ld = vec4(0.0);
        vec4 ls = vec4(0.0);

        vec4 color = vec4(0.0);
        // uncomment to turn off illumination
        //color = color_t;

        newNormal = -cutaway.xyz;
         eye_dir = normalize ( -newVert.xyz );

          la = vec4(0.0);
          ld = vec4(0.0);
          ls = vec4(0.0);

         // compute illumination for each light
         for (int i = 0; i < num_lights; ++i) {
             vec3 light_dir = normalize(lights[i]);
             vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
             la += vec4 ( 0.3 / float(num_lights) );
             ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
             ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
         }

         color = la + ld + ls;
         color.a = 1.0;

        // interior cutaway lines (back face intersection with cutaway)

          outputColor = I * vec4(vec3(0.1), 1.0) + (1.0 - I) * (( color ));


}
