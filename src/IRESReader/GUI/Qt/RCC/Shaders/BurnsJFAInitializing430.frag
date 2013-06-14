#version 420 compatibility

in vec3 normal;
in vec4 color;

out vec4 fragmentColor;
//
//
void main ( void )
{
	ragmentColor = vec4 ( gl_FragCoord.xyz,1.0 );

	//fragmentColor = vec4 ( 1.0,0.0,0.0,1.0 );
}
