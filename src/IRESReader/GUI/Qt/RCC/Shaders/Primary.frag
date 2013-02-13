#version 420

in vec3 normal;
in vec4 color;

out vec4 fragmentColor;
//
//
void main ( void )
{
	fragmentColor = vec4 ( color );
}
