#version 420 compatibility

uniform  vec3 lightDirection;
in vec3 vert;
in vec3 normal;

uniform int num_lights;
uniform vec3 lights[4];

out vec4 fragmentColor;
//
//
void main ( void )
{

	   vec3 newNormal = normal;
	   vec3 newVert   = vert;


	   newNormal = normalize(newNormal);

	   vec3 toLight = normalize(-newVert.xyz);

	   vec3 light_dir = vec3(0.0, 0.0, 1.0 );
	   vec3 eye_dir = normalize( -newVert.xyz );

//	   vec4 color_t = vec4(0.75,0.0,0.0,1.0);
//
//	   vec3 ref = normalize( -reflect( light_dir, newNormal ) );
//	   vec4 la = vec4(0.2);
//	   vec4 ld =  color_t*0.9 * max(0.0, dot(newNormal, light_dir));
//	   vec4 ls =  color_t*0.4 * pow(max( 0.0,dot(eye_dir, ref)), 5.0);

	   vec4 color_t = vec4(1.0,0.0,0.0,1.0);

           vec4 la = vec4(0.0);
           vec4 ld = vec4(0.0);
           vec4 ls = vec4(0.0);

        // compute illumination for each light
           for (int i = 0; i < num_lights; ++i) {
                   vec3 light_dir = normalize(lights[i]);
                   vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
                   la += vec4 ( 0.3 / float(num_lights) );
                   ld += color_t * (1.0 / float(num_lights)) * max ( 0.0 , abs(dot ( newNormal , light_dir ) ));
                   ls += color_t * 0.0 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );
           }

           vec4 color = la + ld + ls;
           color.a = 1.0;

	   //fragmentColor = vec4(la.rgb+ld.xyz+ls.rgb, 1.0);
	   fragmentColor = vec4(color);

}
