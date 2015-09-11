#version 430

layout(location = 0) in vec4 v0;
layout(location = 1) in vec4 v1;
layout(location = 2) in vec4 v2;
layout(location = 3) in vec4 v3;
layout(location = 4) in vec4 v4;
layout(location = 5) in vec4 v5;
layout(location = 6) in vec4 v6;
layout(location = 7) in vec4 v7;

layout(location = 8) in vec4 static_properties;


out CubeData
{
                vec4 v[8];
                vec3 n[6];
                vec4 color;
} cube;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 FreezeViewMatrix;
uniform mat4 ProjectionMatrix;



uniform float min_property_static;
uniform float max_property_static;

uniform int property_index;


vec4 propertyColor ( in float min_range, in float max_range, in int index )
{

        float normalized_color = ( static_properties[index] - min_property_static ) / ( max_property_static - min_property_static );

        float fourValue = 4 * normalized_color;
        float red   = min(fourValue - 1.5, -fourValue + 4.5);
        float green = min(fourValue - 0.5, -fourValue + 3.5);
        float blue  = min(fourValue + 0.5, -fourValue + 2.5);

        red     = max(0.0f, min(red, 1.0f));
        green   = max(0.0f, min(green, 1.0f));
        blue    = max(0.0f, min(blue, 1.0f));

        vec4 color = vec4 ( red , green , blue , 1.0f );

        return color;
}

void main(void)
{

	gl_position = v0;

}
