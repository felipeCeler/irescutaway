#version 400
	 
layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Normal;

out vec4 ex_Color;
	 
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 in_Color;
	 
void main(void)
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * in_Position;
	ex_Color = vec4(in_Normal.xyz,1.0);
}
