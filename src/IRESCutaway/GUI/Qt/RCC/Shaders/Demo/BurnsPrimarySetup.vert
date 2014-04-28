#version 430

// For while, all transformations come from the Celer::Camera.

uniform vec3 v0;
uniform vec3 v1;
uniform vec3 v2;
uniform vec3 v3;

uniform vec3 v4;
uniform vec3 v5;
uniform vec3 v6;
uniform vec3 v7;

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

// freeze camera matrix
uniform int freeze;

//uniform vec3 camera_center;

uniform float x;
uniform float y;

uniform float move;

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

vec4 v[8];


vec4 axis_x;
vec4 axis_y;
vec4 axis_z;
vec4 extends;

void main()
{

        float scale = 0.7;

        vec3 center_of_mass = (v0+v1+v2+v3+v4+v5+v6+v7) * 0.125;

        center_of_mass += move*0.1;

        axis_x = vec4 (1.0,0.0,0.0,0.0);
        axis_y = vec4 (0.0,1.0,0.0,0.0);
        axis_z = vec4 (0.0,0.0,1.0,0.0);;
        extends= vec4 ( abs(v3.x-v6.x), abs(v3.y-v6.y), abs(v3.z-v6.z), 0.0 );
        //extends= vec4 ( 0.1 , 0.1, 0.1, 0.0 );

        extends *= 0.2;


        mat4 invView;
        if ( freeze == 0 )
                invView = inverse ( ViewMatrix );
        else
                invView = inverse ( FreezeViewMatrix );

        vec3 camera_center = ( invView * vec4 ( 0.0 , 0.0 , 0.0 , 1.0 ) ).xyz;

        // coordinate system for the cutaway frustum
        vec3 camera_up = normalize ( ( invView * vec4 ( 0.0 , 1.0 , 0.0 , 0.0 ) ).xyz );
        vec3 camera_dir = normalize ( camera_center - center_of_mass );
        vec3 camera_right = normalize ( cross ( camera_dir.xyz , camera_up.xyz ) );


        float ex = max ( max ( abs ( dot ( camera_right , axis_x.xyz * extends.x ) ) , abs ( dot ( camera_right , axis_y.xyz * extends.y ) ) ) , abs ( dot ( camera_right , axis_z.xyz * extends.z ) ) );
        float ey = max ( max ( abs ( dot ( camera_up , axis_x.xyz * extends.x ) ) , abs ( dot ( camera_up , axis_y.xyz * extends.y ) ) ) , abs ( dot ( camera_up , axis_z.xyz * extends.z ) ) );
        float ez = max ( max ( abs ( dot ( camera_dir , axis_x.xyz * extends.x ) ) , abs ( dot ( camera_dir , axis_y.xyz * extends.y ) ) ) , abs ( dot ( camera_dir , axis_z.xyz * extends.z ) ) );

        ext_x = ( camera_right ) * ex;
        ext_y = ( camera_up ) * ey;
        ext_z = ( camera_dir ) * ez;

        float zfactor = 50.0;
        v[0] = vec4 ( center_of_mass + ext_x + ext_y + zfactor * ext_z + x * ext_x + y * ext_y , 1.0 );
        v[1] = vec4 ( center_of_mass + ext_x + ext_y - ext_z , 1.0 );
        v[2] = vec4 ( center_of_mass - ext_x + ext_y - ext_z , 1.0 );
        v[3] = vec4 ( center_of_mass - ext_x + ext_y + zfactor * ext_z - x * ext_x + y * ext_y , 1.0 );
        v[4] = vec4 ( center_of_mass + ext_x - ext_y + zfactor * ext_z + x * ext_x - y * ext_y , 1.0 );
        v[5] = vec4 ( center_of_mass - ext_x - ext_y + zfactor * ext_z - x * ext_x - y * ext_y , 1.0 );
        v[6] = vec4 ( center_of_mass - ext_x - ext_y - ext_z , 1.0 );
        v[7] = vec4 ( center_of_mass + ext_x - ext_y - ext_z , 1.0 );

        mat3 normalMatrix = ( inverse ( transpose ( mat3 ( ViewMatrix ) ) ) );

        // Top Face
        vec3 normal_top = normalize ( cross ( ( v[3] - v[0] ).xyz , ( v[1] - v[0] ).xyz ) );
        normal_top = normalize ( normalMatrix * normal_top );

        // Bottom
        vec3 normal_bottom = normalize ( cross ( ( v[7] - v[4] ).xyz , ( v[5] - v[4] ).xyz ) );
        normal_bottom = normalize ( normalMatrix * normal_bottom );

        // Front
        vec3 normal_front = normalize ( cross ( ( v[4] - v[0] ).xyz , ( v[3] - v[0] ).xyz ) );
        normal_front = normalize ( normalMatrix * normal_front );

        // Back
        vec3 normal_back = normalize ( cross ( ( v[2] - v[1] ).xyz , ( v[7] - v[1] ).xyz ) );
        normal_back = normalize ( normalMatrix * normal_back );

        // Right
        vec3 normal_right = normalize ( cross ( ( v[1] - v[0] ).xyz , ( v[4] - v[0] ).xyz ) );
        normal_right = normalize ( normalMatrix * normal_right );

        // Left
        vec3 normal_left = normalize ( cross ( ( v[3] - v[2] ).xyz , ( v[6] - v[2] ).xyz ) );
        normal_left = normalize ( normalMatrix * normal_left );

//        cube.v[0] = vec4(v0,1.0);
//        cube.v[1] = vec4(v1,1.0);
//        cube.v[2] = vec4(v2,1.0);
//        cube.v[3] = vec4(v3,1.0);
//
//        cube.v[4] = vec4(v4,1.0);
//        cube.v[5] = vec4(v5,1.0);
//        cube.v[6] = vec4(v6,1.0);
//        cube.v[7] = vec4(v7,1.0);

        cube.v[0] = v[0];
        cube.v[1] = v[1];
        cube.v[2] = v[2];
        cube.v[3] = v[3];

        cube.v[4] = v[4];
        cube.v[5] = v[5];
        cube.v[6] = v[6];
        cube.v[7] = v[7];

        cube.n[0] = normal_top;
        cube.n[1] = normal_bottom;
        cube.n[2] = -normal_front;
        cube.n[3] = normal_back;
        cube.n[4] = normal_right;
        cube.n[5] = normal_left;

        gl_Position = vec4 ( v[0] );

}

