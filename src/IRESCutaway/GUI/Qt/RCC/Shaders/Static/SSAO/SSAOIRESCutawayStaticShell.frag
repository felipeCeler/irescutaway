#version 450 core

layout(location = 0) uniform sampler2D normal;
layout(location = 1) uniform sampler2D vertex;

in VertexData
{
	vec4 vertice;
    vec4 normal;
	vec4 color;
} VertexIn;

noperspective in vec4 dist;

uniform vec2 WIN_SCALE;
uniform int num_lights;
uniform vec3 lights[4];

/// Saturation Intensity
uniform float saturation_;
/// Luminance Intensity
uniform float luminance_;


/// If True, change the projection matrix to perspective otherwise orthographic
uniform bool isPerspective_;
uniform mat4 ProjectionMatrix;
/// From Perspective projection
uniform float nearPlane_;
uniform float farPlane_;

/// SSAO output Buffers
out vec4 out_Coords;
out vec4 out_Normal;
out vec4 out_Color;
out vec4 out_Silhouette_;

vec3 RGB2HSL(vec3 color)
{
        vec3 hsl; // init to 0 to avoid warnings ? (and reverse if + remove first part)

        float fmin = min(min(color.r, color.g), color.b); //Min. value of RGB
        float fmax = max(max(color.r, color.g), color.b); //Max. value of RGB
        float delta = fmax - fmin; //Delta RGB value

        hsl.z = (fmax + fmin) / 2.0; // Luminance

        if (delta == 0.0)        //This is a gray, no chroma...
        {
                hsl.x = 0.0;    // Hue
                hsl.y = 0.0;    // Saturation
        }
        else //Chromatic data...
        {
                if (hsl.z < 0.5)
                        hsl.y = delta / (fmax + fmin); // Saturation
                else
                        hsl.y = delta / (2.0 - fmax - fmin); // Saturation

                float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
                float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
                float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;

                if (color.r == fmax )
                        hsl.x = deltaB - deltaG; // Hue
                else if (color.g == fmax)
                        hsl.x = (1.0 / 3.0) + deltaR - deltaB; // Hue
                else if (color.b == fmax)
                        hsl.x = (2.0 / 3.0) + deltaG - deltaR; // Hue

                if (hsl.x < 0.0)
                        hsl.x += 1.0; // Hue
                else if (hsl.x > 1.0)
                        hsl.x -= 1.0; // Hue
        }

        return hsl;
}

float HueToRGB(float f1, float f2, float hue)
{
        if (hue < 0.0)
                hue += 1.0;
        else if (hue > 1.0)
                hue -= 1.0;
        float res;
        if ((6.0 * hue) < 1.0)
                res = f1 + (f2 - f1) * 6.0 * hue;
        else if ((2.0 * hue) < 1.0)
                res = f2;
        else if ((3.0 * hue) < 2.0)
                res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
        else
                res = f1;
        return res;
}

vec3 HSLToRGB(vec3 hsl)
{
        vec3 rgb;

        if (hsl.y == 0.0)
                rgb = vec3(hsl.z); // Luminance
        else
        {
                float f2;

                if (hsl.z < 0.5)
                        f2 = hsl.z * (1.0 + hsl.y);
                else
                        f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);

                float f1 = 2.0 * hsl.z - f2;

                rgb.r = HueToRGB(f1, f2, hsl.x + (1.0/3.0));
                rgb.g = HueToRGB(f1, f2, hsl.x);
                rgb.b= HueToRGB(f1, f2, hsl.x - (1.0/3.0));
        }

        return rgb;
}


void main(void)
{

        vec3 newNormal = normalize(VertexIn.normal.xyz);
        vec3 newVert = VertexIn.vertice.xyz;
        vec4 color_t = VertexIn.color;

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2(-2.0 * d * d);

        bool backface = false;
        int linesize = 2;
        int linesizediag = 2;
        // make sure we are at the center of the pixel to make the right texture access
        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
        // cutaway normal = rgb, and cutaway depth in camera space = w
        vec4 cutaway = texelFetch( normal, ivec2(pixel_pos), 0 ).rgba;


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
                vec2 neighbor = (pixel_pos + dist_neighbor[i]) / vec2(textureSize(normal,0)).xy ;

                // depth of cutaway surface at neighbor pixel
                zsurface = texelFetch( normal, ivec2(pixel_pos + dist_neighbor[i]), 0 ).w;


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
                        vec2 pixel = neighbor*2.0 - vec2(1.0);
                        pixel.x *= aspect_ratio;

                        // intersection ray from point in image plane with plane containing current 3D point
                        // note that the denominator is dot(l,n), but the ray in ortho is just (0,0,1)
                        zneighbor = (dot (newVert.xyz - vec3(pixel.xy, 0.0), newNormal.xyz)) / newNormal.z;

                }

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
            //vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
            la += vec4 ( 0.3 / float(num_lights) );
            ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
            //ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
        }


        vec4 color = la + ld + ls;
        color.a = 1.0;

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
            //outputColor = I * vec4(vec3(1.0,1.0,1.0), 1.0) + (1.0 - I) * ( color );

            out_Coords = vec4 (newVert.xyz, gl_FragCoord.z);
            out_Normal = vec4 (newNormal.xyz, 1.0);
            out_Color = I * vec4(vec3(1.0,1.0,1.0), 1.0) + (1.0 - I) * ( color );
        }
        // lines outside cutaway (remaining front faces)
        else
        {
        	if (backface)
        	{
        	            vec3 hsl = RGB2HSL(color.rgb);

        	            hsl.g *= 1.0 + (saturation_-50) / 50.0;
        	            hsl.b *= 1.0 + (luminance_-50)  / 50.0;

        	            color.rgb = HSLToRGB(hsl);
        	}

                out_Coords = vec4 (newVert.xyz, gl_FragCoord.z);
                out_Normal = vec4 (newNormal.xyz, 1.0);
                out_Color = I * vec4(vec3(0.0,0.0,0.0), 1.0) + (1.0 - I) * ( color );

        }
}
