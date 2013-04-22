#version 430 core


layout (location = 6 ) in vec2 Position;
layout (location = 7 ) in vec2 texCoords;

uniform mat4 ProjectionMatrix;

out vec2 texCoord;

void main()
{

    texCoord = texCoords;

    gl_Position =  vec4(Position,0.0,  1.0);

}

