#version 430 core

layout(location = 0) uniform sampler2D normals;
layout(location = 1) uniform sampler2D primaryBuffer;

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

        vec3 newNormal = VertexIn.normal.xyz;
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2(-2.0 * d * d);

        bool backface = false;
        int linesize = 1;
        int linesizediag = 1;

        // make sure we are at the center of the pixel to make the right texture access
        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
        // cutaway normal = rgb, and cutaway depth in camera space = w
        vec4 cutaway = texelFetch( normals, ivec2(pixel_pos), 0 ).rgba;

        vec4 primary = texelFetch( primaryBuffer, ivec2(pixel_pos), 0 ).rgba;


//        if ( newVert.z > primary.w )
//        	discard;

        if ( newNormal.z <= 0.0)
                backface = true;
//        else
//        	discard;

        // discard point in front of the cutaway surface
        if ( newVert.z > cutaway.w ) {
            if (newNormal.z >= 0.0 )
            {
        	  //if (dot(newNormal.xyz,cutaway.xyz)> 0)
        		  discard;
            }
        }
       else
        {
        	if ( newNormal.z < 0.0)
        	{
        		 discard;//	backface = true;
        	 }
//
        }



        vec2 dist_neighbor[8] = {vec2(linesize,0), vec2(-linesize,0), vec2(0,linesize), vec2(0,-linesize),
                                vec2(-linesizediag,-linesizediag), vec2(-linesizediag,linesizediag), vec2(linesize,-linesize), vec2(linesizediag,-linesizediag)};

//        // discard point in front of the cutaway surface
//        if ( newVert.z > cutaway.w ) {
//            discard;
//        }

        int size = 4;

        // check the neighbors, we are only interested in border pixels (neighbors to discarded pixels)
        float zsurface = 0;
        float zneighbor = 0;

        for (int i = 0; i < size; ++i) {
            if (I != 1)
            {
                // neighbor coordinate in range [0,1]
                vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy ;

                // depth of cutaway surface at neighbor pixel
                zsurface = texelFetch( normals, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;

                // invert the orthographic projection (considering ortho planes are in range [-1,1]
                vec2 pixel = neighbor*2.0 - vec2(1.0);

                // intersection ray from point in image plane with plane containing current 3D point
                // note that the denominator is dot(l,n), but the ray in ortho is just (0,0,1)
                zneighbor = (dot (newVert.xyz - vec3(pixel.xy, 0.0), newNormal.xyz)) / newNormal.z;

                // if neighbor is in front of surface (was discarded), curent pixel is an edge pixel
                if ( backface)
                {
                	if (zneighbor < zsurface) {
                	     I = 1;
                	}
                }else
                {
                	if (zneighbor > zsurface) {
                	     I = 1;
                	}
                }
            }
        }

        // for back faces use the normal of the cutaway surface (simulate a cut inside the cells)
        //if (backface) newNormal = -cutaway.xyz;
        //vec3 eye_dir = normalize ( -newVert.xyz );

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
        }

        vec4 color = la + ld + ls;
        //color.a = 1.0;

        // uncomment to turn off illumination
        //color = color_t;

        // interior cutaway lines (back face intersection with cutaway)
//        if (backface && I == 1)
//        {
//        	I = exp2(-2.0 * d * d);
//            outputColor = I * vec4(vec3(0.1), 1.0) + (1.0 - I) * ( color );
//        }
        // cutaway border lines (front face intersection with cutaway)
         if (I == 1)
        {   //outputColor = I * vec4(vec3(0.1), 1.0) + (1.0 - I) * ( color );
            outputColor = I * vec4(vec3(1.0,0.0,0.0), 1.0) + (1.0 - I) * ( color );
        }
        // lines outside cutaway (remaining front faces)
        else
        {

        	if (backface)
        		color.rgb += vec3(0.5);
            outputColor = I * vec4(vec3(0.0), 1.0) + (1.0 - I) * ( color_t );
        }
}
