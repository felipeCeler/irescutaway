#version 430 core


layout(location = 0) uniform sampler2D normals;

uniform vec3 lightDirection;

in VertexData
{
   vec4 color;
   vec4 verticeEye;
   vec4 verticeProj;
   vec3 normal;
} VertexOut;

out vec4 outputColor;

in vec2 texcoord;

void main(void)
{


	 vec4 cutaway = texture( normals , gl_FragCoord.xy / vec2(textureSize(normals).xy)).rgba;

	vec3 newNormal = VertexOut.normal.xyz;
	vec3 newVert = VertexOut.verticeEye.xyz;

	newNormal = normalize ( newNormal );

	vec3 toLight = normalize ( -lightDirection.xyz );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	vec4 color_t = vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );


	// @link http://stackoverflow.com/questions/15151076/gl-fragcoord-x-y-z-being-1-for-all-pixels-and-w-being-the-depth
	float zbuffer = cutaway.w/35;

	outputColor = vec4(zbuffer, zbuffer, zbuffer, 1.0);
	//outputColor =    vec4 ( la.rgb + ld.xyz + ls.rgb , 1.0 ) ;


}
