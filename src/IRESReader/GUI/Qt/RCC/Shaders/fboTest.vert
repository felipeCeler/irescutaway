#version 420


layout (location = 3 ) in vec2 Position;
layout (location = 4 ) in vec2 texCoords;

uniform mat4 ProjectionMatrix;

out vec2 texCoord;

void main()
{

    texCoord = texCoords;

    gl_Position = ProjectionMatrix * vec4(Position,0.0,  1.0);

}

