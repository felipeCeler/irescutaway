#version 430

layout ( points ) in;
layout ( triangle_strip, max_vertices=24 ) out;

out VertexData
{
     vec4 vertice;
flat vec4 normal;
     vec4 color;
} VertexOut;

noperspective out vec4 dist;

in CubeData
{
        vec4 v[8];
        vec4 n[12];
        vec4 color;

} cube[];

struct Face
{
        int     vertices[4];
};


// For while, all transformations come from the Celer::Camera.
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform vec2 WIN_SCALE;

Face faces[6];

// Rendering lines
vec4 vp[4];

void renderCube( in vec4 color )
{

        for ( int i = 0; i < 6; i++)
        {

                vp[0] = ProjectionMatrix * ViewMatrix * ModelMatrix * cube[0].v[faces[i].vertices[0]];
                vp[1] = ProjectionMatrix * ViewMatrix * ModelMatrix * cube[0].v[faces[i].vertices[1]];
                vp[2] = ProjectionMatrix * ViewMatrix * ModelMatrix * cube[0].v[faces[i].vertices[2]];
                vp[3] = ProjectionMatrix * ViewMatrix * ModelMatrix * cube[0].v[faces[i].vertices[3]];

                vec2 p0 = WIN_SCALE * (vp[0].xy / vp[0].w);
                vec2 p1 = WIN_SCALE * (vp[1].xy / vp[1].w);
                vec2 p2 = WIN_SCALE * (vp[2].xy / vp[2].w);
                vec2 p3 = WIN_SCALE * (vp[3].xy / vp[3].w);

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


                VertexOut.normal = cube[0].n[i];
                VertexOut.color = color;

                //face
                dist = vec4(area4/length(v4), area3/length(v3), 0, 0);
                VertexOut.vertice  = ViewMatrix * (cube[0].v[faces[i].vertices[0]]);
                gl_Position = vp[0];
                EmitVertex();
                dist = vec4(area2/length(v4), 0, 0, area1/length(v2));
                VertexOut.vertice  = ViewMatrix * (cube[0].v[faces[i].vertices[1]]);
                gl_Position = vp[1];
                EmitVertex();
                dist = vec4(0, area2/length(v3), area1/length(v0), 0);
                VertexOut.vertice  = ViewMatrix * (cube[0].v[faces[i].vertices[2]]);
                gl_Position = vp[2];
                EmitVertex();

                VertexOut.normal = cube[0].n[i+6];
                dist = vec4(0, 0, area3/length(v0), area4/length(v2));
                VertexOut.vertice  = ViewMatrix * (cube[0].v[faces[i].vertices[3]]);
                gl_Position = vp[3];
                EmitVertex();

                EndPrimitive();

        }
}

void main(void)
{

        // Cube Orientation as IRESv2
        // top
        faces[0].vertices[0] = 4;
        faces[0].vertices[1] = 5;
        faces[0].vertices[2] = 7;
        faces[0].vertices[3] = 6;
        // bottom
        faces[1].vertices[0] = 0;
        faces[1].vertices[1] = 3;
        faces[1].vertices[2] = 1;
        faces[1].vertices[3] = 2;
        // front
        faces[2].vertices[0] = 4;
        faces[2].vertices[1] = 0;
        faces[2].vertices[2] = 5;
        faces[2].vertices[3] = 1;
        // back
        faces[3].vertices[0] = 2;
        faces[3].vertices[1] = 3;
        faces[3].vertices[2] = 6;
        faces[3].vertices[3] = 7;
        // right
        faces[4].vertices[0] = 1;
        faces[4].vertices[1] = 2;
        faces[4].vertices[2] = 5;
        faces[4].vertices[3] = 6;
        // left
        faces[5].vertices[0] = 0;
        faces[5].vertices[1] = 3;
        faces[5].vertices[2] = 4;
        faces[5].vertices[3] = 7;


        renderCube(cube[0].color);

}
