#version 430

layout (location = 0 ) in vec3 in_Position;
layout (location = 1 ) in vec3 in_Normal;
layout (location = 2 ) in vec4 in_Color;

out vec4 vert;
out vec3 normal;
out vec4 color;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out float translation;

// if attribute in_Color exists or not
uniform bool has_color;
const vec4 default_color = vec4(0.75, 0.25, 0.25, 1.0);

void main(void)
{
    mat4 modelViewMatrix = viewMatrix*modelMatrix;
    mat4 normalMatrix = transpose(inverse(modelViewMatrix));
    normal = normalize(vec3(normalMatrix * vec4(in_Normal, 0.0)).xyz);
    vert = modelViewMatrix * vec4(in_Position,1.0);

    if (has_color)
        color = in_Color;
    else
        color = default_color;

    gl_Position = (projectionMatrix * modelViewMatrix) * vec4(in_Position,1.0);
}
