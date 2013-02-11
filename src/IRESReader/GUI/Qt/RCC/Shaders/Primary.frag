#version 420

in  vec3 normal;

out vec4 fragmentColor;
//
//
void main ( void )
{
	fragmentColor = vec4 ( (normal), 1.0 );
}
