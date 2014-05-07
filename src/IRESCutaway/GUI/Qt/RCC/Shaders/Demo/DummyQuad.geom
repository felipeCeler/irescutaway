#version 430 core

// FIXME .
layout(points) in;
layout(triangle_strip, max_vertices = 12) out;

out vec2 texcoord;

uniform vec3 displacement;
uniform vec3 max_displacement;
uniform vec2 WIN_SCALE;
noperspective out vec4 dist;

out VertexData
{
        vec4 vertice;
        vec4 normal;
        vec4 color;
} VertexOut;

// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    float x = -3;
    float y = -3;
    float z = -3;


    vec4 vertex0 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( -x, -y, displacement.x, 1.0 );
    vec4 vertex1 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4(  x, -y, displacement.x, 1.0 );
    vec4 vertex2 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4(  x,  y, displacement.x, 1.0 );
    vec4 vertex3 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( -x,  y, displacement.x, 1.0 );


    vec2 p0 = WIN_SCALE * (vertex0.xy / vertex0.w);
    vec2 p1 = WIN_SCALE * (vertex1.xy / vertex1.w);
    vec2 p2 = WIN_SCALE * (vertex3.xy / vertex3.w);
    vec2 p3 = WIN_SCALE * (vertex2.xy / vertex2.w);

    vec2 v0 = p1 - p0;
    vec2 v1 = p1 - p2;
    vec2 v2 = p2 - p0;
    vec2 v3 = p1 - p3;
    vec2 v4 = p3 - p2;
    vec2 v5 = p3 - p0;
    vec2 v6 = p2 - p0;

    float area1 = abs(v1.x * v6.y - v1.y * v6.x);
    float area2 = abs(v1.x * v4.y - v1.y * v4.x);
    float area3 = abs(v0.x * v5.y - v0.y * v5.x);
    float area4 = abs(v2.x * v5.y - v2.y * v5.x);

    mat3 normalMatrix = mat3(inverse(transpose((ModelMatrix*ViewMatrix))));

    VertexOut.color = vec4(0.7,0.0,0.0,0.75);
    VertexOut.normal = vec4(normalMatrix * vec3(0.0,0.0,1.0),0.0);

    gl_Position = vertex0;
    VertexOut.vertice = ViewMatrix * vec4( -x, -y, displacement.x, 1.0 );
    dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vertex1;
    VertexOut.vertice = ViewMatrix * vec4( x, -y, displacement.x, 1.0 );
    dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
    texcoord = vec2( 0.0, 1.0 );
    EmitVertex();

    gl_Position = vertex3;
    VertexOut.vertice = ViewMatrix * vec4( x, y, displacement.x, 1.0 );
    dist = vec4(0, area2/length(v3), area1/length(v0), 0);
    texcoord = vec2( 1.0, 0.0 );
    EmitVertex();

    gl_Position = vertex2;
    VertexOut.vertice = ViewMatrix * vec4( -x, y, displacement.x, 1.0 );
    dist = vec4(0, 0, area3/length(v0), area4/length(v2));
    texcoord =  vec2( 0.0, 0.0 );
    EmitVertex();

    EndPrimitive();

    // Y

    vertex0 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( -x, displacement.y, -z, 1.0 );
    vertex1 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4(  x, displacement.y, -z, 1.0 );
    vertex2 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4(  x, displacement.y,  z, 1.0 );
    vertex3 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( -x, displacement.y,  z, 1.0 );


    p0 = WIN_SCALE * (vertex0.xy / vertex0.w);
    p1 = WIN_SCALE * (vertex1.xy / vertex1.w);
    p2 = WIN_SCALE * (vertex3.xy / vertex3.w);
    p3 = WIN_SCALE * (vertex2.xy / vertex2.w);

    v0 = p1 - p0;
    v1 = p1 - p2;
    v2 = p2 - p0;
    v3 = p1 - p3;
    v4 = p3 - p2;
    v5 = p3 - p0;
    v6 = p2 - p0;

    area1 = abs(v1.x * v6.y - v1.y * v6.x);
    area2 = abs(v1.x * v4.y - v1.y * v4.x);
    area3 = abs(v0.x * v5.y - v0.y * v5.x);
    area4 = abs(v2.x * v5.y - v2.y * v5.x);

    VertexOut.color = vec4(0.7,0.0,0.0,0.75);
    VertexOut.normal = vec4(normalMatrix * vec3(0.0,1.0,0.0),0.0);

    gl_Position = vertex0;
    VertexOut.vertice = ViewMatrix * vec4( -x, displacement.y, -z, 1.0 );
    dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vertex1;
    VertexOut.vertice = ViewMatrix * vec4( x, displacement.y, -z, 1.0 );
    dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
    texcoord = vec2( 0.0, 1.0 );
    EmitVertex();

    gl_Position = vertex3;
    VertexOut.vertice = ViewMatrix * vec4( x, displacement.y, z, 1.0 );
    dist = vec4(0, area2/length(v3), area1/length(v0), 0);
    texcoord = vec2( 1.0, 0.0 );
    EmitVertex();

    gl_Position = vertex2;
    VertexOut.vertice = ViewMatrix * vec4( -x, displacement.y, z, 1.0 );
    dist = vec4(0, 0, area3/length(v0), area4/length(v2));
    texcoord =  vec2( 0.0, 0.0 );
    EmitVertex();

    EndPrimitive();

    // Z

    vertex0 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( displacement.z, -y, -z, 1.0 );
    vertex1 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( displacement.z,  y, -z, 1.0 );
    vertex2 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( displacement.z,  y,  z, 1.0 );
    vertex3 = ProjectionMatrix * ModelMatrix * ViewMatrix * vec4( displacement.z, -y,  z, 1.0 );


    p0 = WIN_SCALE * (vertex0.xy / vertex0.w);
    p1 = WIN_SCALE * (vertex1.xy / vertex1.w);
    p2 = WIN_SCALE * (vertex3.xy / vertex3.w);
    p3 = WIN_SCALE * (vertex2.xy / vertex2.w);

    v0 = p1 - p0;
    v1 = p1 - p2;
    v2 = p2 - p0;
    v3 = p1 - p3;
    v4 = p3 - p2;
    v5 = p3 - p0;
    v6 = p2 - p0;

    area1 = abs(v1.x * v6.y - v1.y * v6.x);
    area2 = abs(v1.x * v4.y - v1.y * v4.x);
    area3 = abs(v0.x * v5.y - v0.y * v5.x);
    area4 = abs(v2.x * v5.y - v2.y * v5.x);


    VertexOut.color = vec4(0.7,0.0,0.0,0.75);
    VertexOut.normal = vec4(normalMatrix * vec3(1.0,0.0,0.0),0.0);

    gl_Position = vertex0;
    VertexOut.vertice = ViewMatrix * vec4( displacement.z, -y, -z, 1.0 );
    dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vertex1;
    VertexOut.vertice = ViewMatrix * vec4( displacement.z,  y, -z, 1.0 );
    dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
    texcoord = vec2( 0.0, 1.0 );
    EmitVertex();

    gl_Position = vertex3;
    VertexOut.vertice = ViewMatrix * vec4( displacement.z, y, z, 1.0 );
    dist = vec4(0, area2/length(v3), area1/length(v0), 0);
    texcoord = vec2( 1.0, 0.0 );
    EmitVertex();

    gl_Position = vertex2;
    VertexOut.vertice = ViewMatrix * vec4( displacement.z, -y, z, 1.0 );
    dist = vec4(0, 0, area3/length(v0), area4/length(v2));
    texcoord =  vec2( 0.0, 0.0 );
    EmitVertex();

    EndPrimitive();

}
