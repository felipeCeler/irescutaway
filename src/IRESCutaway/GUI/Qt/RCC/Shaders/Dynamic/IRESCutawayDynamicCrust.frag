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

out vec4 outputColor;

uniform vec2 WIN_SCALE;
uniform int num_lights;
uniform vec3 lights[4];

uniform int Wall;
uniform int Line;
uniform int Cool;
uniform bool isPerspective_;


uniform mat4 ProjectionMatrix;

uniform float nearPlane_;
uniform float farPlane_;

void main(void)
{

        vec3 newNormal = VertexIn.normal.xyz;
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float Bias = exp2(-2.0 * d * d);

        bool backface = false;
        int linesize = 1;
        int linesizediag = 1;

        bool wire = false;

        // push frontface back, so for double faces we see the interior of the cube (backface)
        if (newNormal.z > 0.0)
        {
            discard;
            wire = true;
        }

        float I = 0;


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

        //if ( cutaway.w == 0.0)
        //        discard;

        int size = 8;

        // check the neighbors, we are only interested in border pixels (neighbors to discarded pixels)
        float zsurface = 0;
        float zneighbor = 0;

       float aspect_ratio = (WIN_SCALE.x/WIN_SCALE.y);

       float frustum_h = nearPlane_ / ProjectionMatrix[0][0];
       float frustum_v = nearPlane_ / ProjectionMatrix[1][1];

        for (int i = 0; i < size; ++i) {
            if (I != 1)
            {
                // neighbor coordinate in range [0,1]
                vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy ;

                // depth of cutaway surface at neighbor pixel
                zsurface = texelFetch( normals, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;


                if ( isPerspective_ )
                {
                       // invert the projection
                       vec3 pixel = vec3( vec2(neighbor*2.0 - vec2(1.0)), -nearPlane_);
                       pixel.x *= frustum_h;
                       pixel.y *= frustum_v;

                       vec3 ray =  normalize(pixel);
                       float dotln = dot (ray, newNormal);


                       // intersection ray from point in image plane with plane containing current 3D point
                       zneighbor = dot ( (newVert.xyz - pixel.xyz), newNormal.xyz) / dotln;

                       // compute the distance in z axis from pixel to 3D plane, and add near plane distance
                       // (z distance from origin to pixel)
                       // -nearPlane to be compatible with newVert.z
                       zneighbor = (ray * zneighbor).z - nearPlane_;

                }else
                {
                        // invert the orthographic projection (considering ortho planes are in range [-1,1]
                        vec2 pixel = (neighbor*2.0 - vec2(1.0));
                        pixel.x *= frustum_h;
                        pixel.y *= frustum_v;

                        vec3 ray =  vec3(0.0,0.0,1.0);

                        float dotln = dot (ray, newNormal);

                        // intersection ray from point in image plane with plane containing current 3D point
                        // note that the denominator is dot(l,n), but the ray in ortho is just (0,0,1)
                        zneighbor = (dot (newVert.xyz - vec3(pixel.xy, 0.0), newNormal.xyz)) / dotln;
                        zneighbor = (ray * zneighbor).z;
                }

                // if neighbor is in front of surface (was discarded), curent pixel is an edge pixel
                if (zneighbor > zsurface) {
                    I = 1;
                }
            }
         }

//
//       for (int i = 0; i < size; ++i)
//       {
//           if (I != 1)
//           {
//               // neighbor coordinate in range [0,1]
//               vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy ;
//               //vec2 neighbor = (pixel_pos) / vec2(textureSize(normals,0)).xy ;
//
//               // depth of cutaway surface at neighbor pixel
//               zsurface = texelFetch( normals, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;
//               //zsurface = texelFetch( normals, ivec2(pixel_pos), 0 ).w;
//
//               // invert the projection
//               vec3 pixel = vec3( vec2(neighbor*2.0 - vec2(1.0)), -nearPlane);
//               pixel.x *= frustum_h;
//               pixel.y *= frustum_v;
//
//               vec3 ray =  normalize(pixel);
//               float dotln = dot (ray, newNormal);
//
//
//               // intersection ray from point in image plane with plane containing current 3D point
//               zneighbor = dot ( (newVert.xyz - pixel.xyz), newNormal.xyz) / dotln;
//
//               // compute the distance in z axis from pixel to 3D plane, and add near plane distance
//               // (z distance from origin to pixel)
//               // -nearPlane to be compatible with newVert.z
//               zneighbor = (ray * zneighbor).z - nearPlane;
//
//               // if neighbor is in front of surface (was discarded), curent pixel is an edge pixel
//               if (zneighbor > zsurface)
//               {
//                   I = 1;
//                   break;
//               }
//           }
//       }

       if ( (abs(newVert.z - (cutaway.w)) < 0.003) ) {
          Bias = 1;
       }

        // for back faces use the normal of the cutaway surface (simulate a cut inside the cells)
       if ( Wall == 0 )
               newNormal = -cutaway.xyz;
       else
               newNormal = cutaway.xyz;
        vec3 eye_dir = normalize ( -newVert.xyz );

        vec4 la = vec4(0.0);
        vec4 ld = vec4(0.0);
        vec4 ls = vec4(0.0);

        // compute illumination for each light

        vec3 lightz[4];

        //color_t = vec4(0.0,0.0,0.5,1.0);

        lightz[0] = vec3(0.0,1.0,0.0);
        lightz[1] = vec3(0.0,0.0,1.0);
        lightz[2] = vec3(0.0,0.0,0.0);
        lightz[3] = vec3(0.0,1.0,0.0);

        for (int i = 0; i < num_lights; ++i) {
            vec3 light_dir = normalize(lights[i]);
            vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );

            la += vec4 ( 0.3 / float(num_lights) );
            ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
            ls += color_t * (0.4 / float(num_lights)) * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
        }

        vec4 color = la + ld + ls;
        color.a = 1.0;

        // uncomment to turn off illumination
        //color = color_t;


        if ( Line == 0 )
                I = 0;
        // interior cutaway lines (back face intersection with cutaway)
        if ( I == 1 )
            outputColor = I * vec4(vec3(0.1), 1.0) + (1.0 - I) * ( color );
           //     outputColor = ( color );
        // cutaway border lines (front face intersection with cutaway)
        // lines outside cutaway (remaining front faces)
        else
        {
//                float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
//                float I = exp2(-2.0 * d * d);
//           outputColor = I * vec4(vec3(0.0), 1.0) + (1.0 - I) * ( color );
           outputColor =  ( color );
        }


        if ( Wall == 0 )
        {

//                        outputColor = I * vec4(vec3(0.1), 1.0) + (1.0 - I) * ( color );
                        outputColor = Bias * vec4(vec3(0.1), 1.0) + (1.0 - Bias) * ( color );
        }

        //if ( newVert.z > cutaway.x )
        //        outputColor = vec4(vec3(abs(cutaway.z)*1.0), 1.0);
        //else
        //        outputColor = vec4(0.0, abs(newVert.z)*0.15, 1.0, 1.0);

        //outputColor = vec4(vec3(abs(cutaway.w)*0.15), 1.0);

        //outputColor = I * vec4(vec3(0.0), 1.0) + (1.0 - I) * ( color );

//                vec4 la = vec4(0.0);
//                vec4 ld = vec4(0.0);
//                vec4 ls = vec4(0.0);
//
//
//                for (int i = 0; i < num_lights; ++i) {
//                    vec3 light_dir = normalize(lights[i]);
//                    vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
//                    la += vec4 ( 0.3 / float(num_lights) );
//                    ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
//                    //ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
//                }
//
//                        vec4 color = la + ld + ls;
//                        color.a = 1.0;
//        outputColor = I * vec4(vec3(0.7), 1.0) + (1.0 - I) * ( color );
}