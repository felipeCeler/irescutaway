#version 430 core

layout(location = 4) uniform sampler2D silhouette;

in VertexData
{
	vec4 vertice;
   flat vec4 normal;
	vec4 color;
} VertexIn;

noperspective in vec4 dist;

uniform vec2 WIN_SCALE;
uniform int num_lights;
uniform vec3 lights[4];

/// SSAO output Buffers
out vec4 out_Coords;
out vec4 out_Normal;
out vec4 out_Color;

//out vec4 fragmentColor;

/// Saturation Intensity
uniform float saturation_;
/// Luminance Intensity
uniform float luminance_;

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

        vec3 eye_dir = normalize ( -newVert.xyz );


//        // discard point in front of the cutaway surface
//        if ( newVert.z > cutaway.w ) {
//            discard;
//        }

        float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
        float I = exp2(-0.7 * d * d);

        vec4 la = vec4(0.0);
        vec4 ld = vec4(0.0);
        vec4 ls = vec4(0.0);

        vec4 color_t = VertexIn.color;//vec4(0.5, 1.0, 0.0, 1.0);

        // compute illumination for each light
        for (int i = 0; i < num_lights; ++i) {
            vec3 light_dir = normalize(lights[i]);
            vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
            la += vec4 ( 0.2 / float(num_lights) );
            ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
            ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
        }

        vec4 color = la + ld + ls;
        color.a = 1.0;

        //fragmentColor = I * vec4 ( 1.0 , 1.0 , 1.0 , 1.0 ) + ( 1.0 - I ) * color;

        vec3 hsl = RGB2HSL(color.rgb);

        hsl.g *= 1.0 + (saturation_-50) / 50.0;
        hsl.b *= 1.0 + (luminance_-50)  / 50.0;

        color.rgb = HSLToRGB(hsl);


 	   // Add the pixels which make up our window to the pixel array.

//
//        // make sure we are at the center of the pixel to make the right texture access
//        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
//        // cutaway normal = rgb, and cutaway depth in camera space = w
//        vec4 cutaway = texelFetch( normal, ivec2(pixel_pos), 0 ).rgba;

        int size = 4;
        vec4 back = vec4(0.0,0.0,0.0,0.0);


        // make sure we are at the center of the pixel to make the right texture access
        vec2 pixel_pos = vec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y)) + vec2(0.5);
        // cutaway normal = rgb, and cutaway depth in camera space = w


		for (int dX = -size; dX <= size; ++dX) {
			for (int dY = -size; dY <= size; ++dY) {
				vec2 offset = vec2(float(dX), float(dY));

				// If a pixel in the window is located at (x+dX, y+dY), put it at index (dX + R)(2R + 1) + (dY + R) of the
				// pixel array. This will fill the pixel array, with the top left pixel of the window at pixel[0] and the
				// bottom right pixel of the window at pixel[N-1].
				back = texelFetch(silhouette, ivec2(gl_FragCoord.st+offset), 0).rgba;

				if ((back.a < 51.0)) {
					color = vec4(0.0,0.0,0.0,1.0);
				}
			}
		}

        out_Coords = vec4 (newVert.xyz, 1.0);
        out_Normal = vec4 (newNormal.xyz, 1.0);
        out_Color = I * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) + ( 1.0 - I ) * color;

}
