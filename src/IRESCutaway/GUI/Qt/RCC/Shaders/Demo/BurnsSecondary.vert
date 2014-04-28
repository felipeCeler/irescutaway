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
    vec4 color;

} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform float min_StaticProperty;
uniform float max_StaticProperty;

uniform float min_DynamicProperty;
uniform float max_DynamicProperty;

uniform float property_type;

uniform int property_index;

uniform int faults;

uniform vec3 displacement;


vec4 propertyColor ( float property, float min_property, float max_property  )
{

        float normalized_color = ( property - min_property ) / ( max_property - min_property );

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

        VertexOut.n[0] = vec4(normalize(cross(vb.xyz-va.xyz, vd.xyz-va.xyz)),0.0);
        VertexOut.n[1] = vec4(normalize(cross(vc.xyz-vb.xyz, vd.xyz-vb.xyz)),0.0);

        mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

        VertexOut.n[0] = vec4(normalMatrix * VertexOut.n[0].xyz,0.0);
        VertexOut.n[1] = vec4(normalMatrix * VertexOut.n[1].xyz,0.0);

        VertexOut.eye[0] =  ModelMatrix * ViewMatrix * vec4(va);
        VertexOut.eye[1] =  ModelMatrix * ViewMatrix * vec4(vb);
        VertexOut.eye[2] =  ModelMatrix * ViewMatrix * vec4(vc);
        VertexOut.eye[3] =  ModelMatrix * ViewMatrix * vec4(vd);


        if ( property_type == 1.0 )
                VertexOut.color    =  propertyColor ( static_properties[property_index], min_StaticProperty, max_StaticProperty);
        else
                VertexOut.color    =  propertyColor ( dynamic_properties, min_DynamicProperty, max_DynamicProperty);

        float z = 0.0;

        if ( ((dot(vec3(0.0,0.0,1.0),va.xyz-vec3(0.0,0.0,displacement.x)) > 0.0) &&
             ( dot(vec3(0.0,0.0,1.0),vb.xyz-vec3(0.0,0.0,displacement.x)) > 0.0) &&
             ( dot(vec3(0.0,0.0,1.0),vc.xyz-vec3(0.0,0.0,displacement.x)) > 0.0) &&
             ( dot(vec3(0.0,0.0,1.0),vd.xyz-vec3(0.0,0.0,displacement.x)) > 0.0))
             &&
             ((dot(vec3(0.0,1.0,0.0),va.xyz-vec3(0.0,displacement.y,0.0)) > 0.0) &&
             ( dot(vec3(0.0,1.0,0.0),vb.xyz-vec3(0.0,displacement.y,0.0)) > 0.0) &&
             ( dot(vec3(0.0,1.0,0.0),vc.xyz-vec3(0.0,displacement.y,0.0)) > 0.0) &&
             ( dot(vec3(0.0,1.0,0.0),vd.xyz-vec3(0.0,displacement.y,0.0)) > 0.0))
             &&
             ((dot(vec3(1.0,0.0,0.0),va.xyz-vec3(displacement.z,0.0,0.0)) > 0.0) &&
             ( dot(vec3(1.0,0.0,0.0),vb.xyz-vec3(displacement.z,0.0,0.0)) > 0.0) &&
             ( dot(vec3(1.0,0.0,0.0),vc.xyz-vec3(displacement.z,0.0,0.0)) > 0.0) &&
             ( dot(vec3(1.0,0.0,0.0),vd.xyz-vec3(displacement.z,0.0,0.0)) > 0.0))
             )
        {
                VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(va);
                VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vb);
                VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vc);
                VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vd);
        }
        else
        {
                VertexOut.v[0] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
                VertexOut.v[1] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
                VertexOut.v[2] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
                VertexOut.v[3] =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(0.0);
        }

        gl_Position = vec4(va);
}
