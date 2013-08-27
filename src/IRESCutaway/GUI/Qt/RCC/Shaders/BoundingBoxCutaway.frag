#version 430 core

layout(location = 0) uniform sampler2D normals;
//layout(location = 1) uniform sampler2D vertices;

in VertexData
{
		vec4 vertice;
		vec4 normal;
		vec4 color;
} VertexIn;

noperspective in vec4 dist;

vec2 dist_neighbor [4] = {vec2(-1,0), vec2(1,0), vec2(0,-1), vec2(0,1)};


out vec4 outputColor;



void main(void)
{

        vec4 cutaway = texture( normals , gl_FragCoord.xy / vec2(textureSize(normals,0)).xy ).rgba;

        if ( gl_FragCoord.z < ( cutaway.w ) )
        {
                discard;
        }


        //vec4 vertice = texture( vertices , gl_FragCoord.xy / vec2(textureSize(vertices,0)).xy ).rgba;

	float d = min(dist[0], min(dist[1], min(dist[2], dist[3])));
	float I = exp2(-2.0 * d * d);


	vec3 newNormal = VertexIn.normal.xyz;
	vec3 newVert = VertexIn.vertice.xyz;
	vec4 color_t = VertexIn.color;

        vec2 vector = gl_FragCoord.xy - cutaway.xy;

        //I = 0;

        gl_FragDepth = gl_FragCoord.z;

        newNormal = normalize ( newNormal );
        if (dot ( newNormal.xyz, vec3(0.0,0.0,-1.0)) < 0.0) {
            discard;
        }

        I = 0;

        //if ( (abs(gl_FragCoord.z - (cutaway.w)) < 0.0000015) )
        {
            // check the neighbors, we are only interested in border pixels (neighbors to discarded pixels)
            const int size = 4;
            float zneighbor[size];
            for (int i = 0; i < size; ++i) {
                zneighbor[i] = texture( normals , (gl_FragCoord.xy + dist_neighbor[i]) / vec2(textureSize(normals,0)).xy).w;
            }

//            zneighbor[0] = texture( normals , (gl_FragCoord.xy + vec2( -1, 0)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[1] = texture( normals , (gl_FragCoord.xy + vec2( 1, 0)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[2] = texture( normals , (gl_FragCoord.xy + vec2( 0, -1)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[3] = texture( normals , (gl_FragCoord.xy + vec2( 0,  1)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[4] = texture( normals , (gl_FragCoord.xy + vec2( -1, -1)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[5] = texture( normals , (gl_FragCoord.xy + vec2( -1,  1)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[6] = texture( normals , (gl_FragCoord.xy + vec2( 1, -1)) / vec2(textureSize(normals,0)).xy).w;
//            zneighbor[7] = texture( normals , (gl_FragCoord.xy + vec2( 1,  1)) / vec2(textureSize(normals,0)).xy).w;


            for (int i = 0; i < size; ++i) {
                if (gl_FragCoord.z < zneighbor[i]) {
                   I = 1;
                }
            }

        }

	vec3 toLight = normalize ( -newVert.xyz );

	vec3 light_dir = vec3 ( 0.0 , 0.0 , 1.0 );
	vec3 eye_dir = normalize ( -newVert.xyz );

	//vec4 ( 0.75 , 0.75 , 1.0 , 1.0 );//geometryShader_VertexData.propertyColor;

	vec3 ref = normalize ( -reflect ( light_dir , newNormal ) );
	vec4 la = vec4 ( 0.2 );
	vec4 ld = color_t * 0.9 * max ( 0.0 , dot ( newNormal , light_dir ) );
	vec4 ls = color_t * 0.4 * pow ( max ( 0.0 , dot ( eye_dir , ref ) ) , 5.0 );

	outputColor = I * vec4(0.0, 0.0, 0.0, 1.0) + (1.0 - I) * ( VertexIn.color );

        //outputColor = vec4(cutaway.www, 1.0);


}
