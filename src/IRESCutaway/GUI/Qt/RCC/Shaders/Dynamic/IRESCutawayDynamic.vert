#version 430

layout(location = 0) in vec4 v;
layout(location = 1) in vec4 n;

layout(location = 3) in float dynamic_properties;


out VertexData
{
                vec4 vertice;
                vec4 normal;
                vec4 color;
} VertexOut;


/// FIXME - Do research and understand the best away to align data on Shader.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;


uniform float min_property;
uniform float max_property;


vec4 propertyColor ( )
{

	float normalized_color = ( dynamic_properties - min_property ) / ( max_property - min_property );

	float fourValue = 4 * normalized_color;
	float red   = min(fourValue - 1.5, -fourValue + 4.5);
	float green = min(fourValue - 0.5, -fourValue + 3.5);
	float blue  = min(fourValue + 0.5, -fourValue + 2.5);

	red 	= max(0.0f, min(red, 1.0f));
	green 	= max(0.0f, min(green, 1.0f));
	blue 	= max(0.0f, min(blue, 1.0f));

	vec4 color = vec4 ( red , green , blue , 1.0f );

	return color;
}

void main(void)
{

		// We revert the vertices order to fit in the triangle strip pipeline
		// Triangle strips request vertices in zig-zag order.


                VertexOut.color  = propertyColor ( );

                mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

                // for each quad, keeprs the normal (from each triangle pair) that has higher z (avoids bad
                // backface classification in frag shader)

                // top
                VertexOut.normal =  vec4 ( normalize( normalMatrix * n.xyz ),0.0);

                VertexOut.vertice = ViewMatrix *  v;



                if ( n.w > 1.0)
                        gl_Position =  vec4(0.0,0.0,0.0,0.0);
                else
                        gl_Position =  ProjectionMatrix * ViewMatrix *  v;


}
