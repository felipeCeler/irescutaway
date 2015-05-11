#version 430 core

layout(location = 0) in vec4 va;
layout(location = 1) in vec4 vb;
layout(location = 2) in vec4 vc;
layout(location = 3) in vec4 vd;

layout(location = 4) in vec4 faceType;

layout(location = 5) in vec4 static_properties;
layout(location = 6) in float dynamic_properties;



out VertexData
{
    vec4 v[4];
    vec4 n[2];
    vec4 eye[4];
flat    vec4 color;

} VertexOut;

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

uniform int time_step;

uniform int faults;


vec4 propertyColor ( int type )
{

        vec4 color              = vec4 ( 1.0f , 1.0f , 1.0f , 1.0f );
        float normalized_color  = 0.0f;

        if ( type == 0 )
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

        VertexOut.n[0] = vec4(normalize(cross(vb.xyz-va.xyz, vd.xyz-va.xyz)),0.0);
        VertexOut.n[1] = vec4(normalize(cross(vc.xyz-vb.xyz, vd.xyz-vb.xyz)),0.0);

        mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

        VertexOut.n[0] = vec4(normalMatrix * VertexOut.n[0].xyz,0.0);
        VertexOut.n[1] = vec4(normalMatrix * VertexOut.n[1].xyz,0.0);

        VertexOut.eye[0] =  ModelMatrix * ViewMatrix * vec4(va);
        VertexOut.eye[1] =  ModelMatrix * ViewMatrix * vec4(vb);
        VertexOut.eye[2] =  ModelMatrix * ViewMatrix * vec4(vc);
        VertexOut.eye[3] =  ModelMatrix * ViewMatrix * vec4(vd);

        VertexOut.color  =  propertyColor ( property_type );

        if ( (isPrimary() ) )
        {
                VertexOut.color.a = 0.0;
        }
        else
        {
                VertexOut.color.a = 1.0;

                if ( (faceType.x == 1.0) )
                {
                        VertexOut.color.a += 1.0;
                }
        }

        if ( faceType.x == 3.0 )
        {
                VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
                VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
                VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
                VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
        }else
        {
                VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(va);
                VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vb);
                VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vc);
                VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vd);
        }

        gl_Position = vec4(va);
}
