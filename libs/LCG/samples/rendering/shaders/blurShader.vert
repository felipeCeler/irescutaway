#version 400

layout(location=0) in vec4 in_Position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {

	mat4 modelViewMatrix = viewMatrix*modelMatrix;

    gl_Position = (projectionMatrix * modelViewMatrix) * in_Position;

}
