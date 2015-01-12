#version 430

layout(location = 0) in vec4 v0;
layout(location = 1) in vec4 v1;
layout(location = 2) in vec4 v2;
layout(location = 3) in vec4 v3;
layout(location = 4) in vec4 v4;
layout(location = 5) in vec4 v5;
layout(location = 6) in vec4 v6;
layout(location = 7) in vec4 v7;

// Four property x = Bubble Point Pressure
//               y = Pressure
//               z = Porosity
//               w = Modified Block Volume
layout(location = 8) in vec4  static_properties;
layout(location = 9) in float dynamic_properties;

/// FIXME - Research for the best away to alignment data on Shader.
out CubeData
{
                vec4 v[8];
                vec4 n[12];
                vec4 color;
} cube;



// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform float min_property_dynamic;
uniform float max_property_dynamic;
uniform float min_property_static;
uniform float max_property_static;

uniform int property_index;
uniform int property_type;

uniform float min_range_static;
uniform float max_range_static;

uniform float min_range_dynamic;
uniform float max_range_dynamic;


uniform vec3 box_min;
uniform vec3 box_max;

uniform float move_x;
uniform float move_y;
uniform float move_z;

uniform float paper;

vec4 propertyColor ( )
{

        vec4 color              = vec4 ( 1.0f , 1.0f , 1.0f , 1.0f );
        float normalized_color  = 0.0f;

        if ( property_type == 0 )
        {
                normalized_color = ( dynamic_properties - min_property_dynamic ) / ( max_property_dynamic - min_property_dynamic );
        }
        else
        {
                normalized_color = ( static_properties[property_index] - min_property_static ) / ( max_property_static - min_property_static );
        }

        float fourValue = 4 * normalized_color;
        float red   = min(fourValue - 1.5, -fourValue + 4.5);
        float green = min(fourValue - 0.5, -fourValue + 3.5);
        float blue  = min(fourValue + 0.5, -fourValue + 2.5);

        red     = max(0.0f, min(red, 1.0f));
        green   = max(0.0f, min(green, 1.0f));
        blue    = max(0.0f, min(blue, 1.0f));

        color = vec4 ( red , green , blue , 1.0f );

        return color;
}

bool intersect (  vec4 p )
{
        float s =  0.8;
                                                                                                                                        ;
        vec3 b_min = (box_min*s + vec3(move_x,move_y,move_z));
        vec3 b_max = (box_max*s + vec3(move_x,move_y,move_z));


        return ( ( p.x >= b_min.x ) && ( p.x  < b_max.x ) &&
                 ( p.y >= b_min.y ) && ( p.y  < b_max.y ) &&
                 ( p.z >= b_min.z ) && ( p.z  < b_max.z ) );
}

bool isInside ( )
{
        if ( intersect(v0) &&
             intersect(v1) &&
             intersect(v2) &&
             intersect(v3) &&
             intersect(v4) &&
             intersect(v5) &&
             intersect(v6) &&
             intersect(v7)
            )
        {
                return true;
        }

        return false;
}


bool isPrimary (  )
{

//        if (paper == 0.0 )
//        {
//               return isInside();
//
//
//        }

        return ( ( dynamic_properties > min_range_dynamic) && ( dynamic_properties < max_range_dynamic ));
//        if ( property_type == 0 )
//        {
//                return ( ( dynamic_properties > min_range_dynamic) && ( dynamic_properties < max_range_dynamic ));
//
//        }else
//        {
//                return ( ( static_properties[property_index] > min_range_static) && ( static_properties[property_index] < max_range_static ));
//        }
//
//        return false;
}

void main(void)
{

        if ( isPrimary( ) )
        {
                // We revert the vertices order to fit in the triangle strip pipeline
                // Triangle strips request vertices in zig-zag order.

                // Top
                cube.v[4] = v4;
                cube.v[5] = v5;
                cube.v[7] = v7;
                cube.v[6] = v6;
                // Bottom
                cube.v[0] = v0;
                cube.v[3] = v3;
                cube.v[1] = v1;
                cube.v[2] = v2;

                cube.color    =  propertyColor ( );

                mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

                // for each quad, keeprs the normal (from each triangle pair) that has higher z (avoids bad
                // backface classification in frag shader)

                // top
                cube.n[0] = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[4]).xyz , (cube.v[7]-cube.v[4]).xyz )),0.0);
                cube.n[6]  = vec4 ( normalize( normalMatrix * cross( (cube.v[7]-cube.v[6]).xyz , (cube.v[5]-cube.v[6]).xyz )),0.0);

                // bottom
                cube.n[1]  = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[0]).xyz , (cube.v[1]-cube.v[0]).xyz )),0.0);
                cube.n[7] = vec4 ( normalize( normalMatrix * cross( (cube.v[1]-cube.v[2]).xyz , (cube.v[3]-cube.v[2]).xyz )),0.0);

                // front
                cube.n[2] = vec4 ( normalize( normalMatrix * cross( (cube.v[0]-cube.v[4]).xyz , (cube.v[5]-cube.v[4]).xyz )),0.0);
                cube.n[8]  = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[1]).xyz , (cube.v[0]-cube.v[1]).xyz )),0.0);

                // back
                cube.n[3] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[2]).xyz , (cube.v[6]-cube.v[2]).xyz )),0.0);
                cube.n[9]  = vec4 ( normalize( normalMatrix * cross( (cube.v[6]-cube.v[7]).xyz , (cube.v[3]-cube.v[7]).xyz )),0.0);

                // left
                cube.n[4] = vec4 ( normalize( normalMatrix * cross( (cube.v[2]-cube.v[1]).xyz , (cube.v[5]-cube.v[1]).xyz )),0.0);
                cube.n[10]  = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[6]).xyz , (cube.v[2]-cube.v[6]).xyz )),0.0);

                // right
                cube.n[5] = vec4 ( normalize( normalMatrix * cross( (cube.v[4]-cube.v[0]).xyz , (cube.v[3]-cube.v[0]).xyz )),0.0);
                cube.n[11]  = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[7]).xyz , (cube.v[4]-cube.v[7]).xyz )),0.0);

        }
        else
        {
                // We revert the vertices order to fit in the triangle strip pipeline
                // Triangle strips request vertices in zig-zag order.

                cube.v[4] = vec4(0.0,0.0,0.0,0.0);
                cube.v[5] = vec4(0.0,0.0,0.0,0.0);
                cube.v[7] = vec4(0.0,0.0,0.0,0.0);
                cube.v[6] = vec4(0.0,0.0,0.0,0.0);

                cube.v[0] = vec4(0.0,0.0,0.0,0.0);
                cube.v[3] = vec4(0.0,0.0,0.0,0.0);
                cube.v[1] = vec4(0.0,0.0,0.0,0.0);
                cube.v[2] = vec4(0.0,0.0,0.0,0.0);

                cube.color    =  propertyColor ( );

                mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

                // for each quad, keeprs the normal (from each triangle pair) that has higher z (avoids bad
                // backface classification in frag shader)

                // top
                cube.n[0] = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[4]).xyz , (cube.v[7]-cube.v[4]).xyz )),0.0);
                cube.n[6]  = vec4 ( normalize( normalMatrix * cross( (cube.v[7]-cube.v[6]).xyz , (cube.v[5]-cube.v[6]).xyz )),0.0);

                // bottom
                cube.n[1]  = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[0]).xyz , (cube.v[1]-cube.v[0]).xyz )),0.0);
                cube.n[7] = vec4 ( normalize( normalMatrix * cross( (cube.v[1]-cube.v[2]).xyz , (cube.v[3]-cube.v[2]).xyz )),0.0);

                // front
                cube.n[2] = vec4 ( normalize( normalMatrix * cross( (cube.v[0]-cube.v[4]).xyz , (cube.v[5]-cube.v[4]).xyz )),0.0);
                cube.n[8]  = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[1]).xyz , (cube.v[0]-cube.v[1]).xyz )),0.0);

                // back
                cube.n[3] = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[2]).xyz , (cube.v[6]-cube.v[2]).xyz )),0.0);
                cube.n[9]  = vec4 ( normalize( normalMatrix * cross( (cube.v[6]-cube.v[7]).xyz , (cube.v[3]-cube.v[7]).xyz )),0.0);

                // left
                cube.n[4] = vec4 ( normalize( normalMatrix * cross( (cube.v[2]-cube.v[1]).xyz , (cube.v[5]-cube.v[1]).xyz )),0.0);
                cube.n[10]  = vec4 ( normalize( normalMatrix * cross( (cube.v[5]-cube.v[6]).xyz , (cube.v[2]-cube.v[6]).xyz )),0.0);

                // right
                cube.n[5] = vec4 ( normalize( normalMatrix * cross( (cube.v[4]-cube.v[0]).xyz , (cube.v[3]-cube.v[0]).xyz )),0.0);
                cube.n[11]  = vec4 ( normalize( normalMatrix * cross( (cube.v[3]-cube.v[7]).xyz , (cube.v[4]-cube.v[7]).xyz )),0.0);

        }

        gl_Position = v0;

}
