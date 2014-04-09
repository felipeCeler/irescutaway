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

// freeze camera matrix
uniform int freeze;

//uniform vec3 camera_center;

uniform float x;
uniform float y;

vec3 ext_x;
vec3 ext_y;
vec3 ext_z;

vec4 v[8];


uniform float min_property;
uniform float max_property;

uniform int property_index;
uniform float min_range;
uniform float max_range;

vec4 axis_x;
vec4 axis_y;
vec4 axis_z;
vec4 extends;

uniform vec3 box_min;
uniform vec3 box_max;
uniform float paper;

vec4 propertyColor ( in float min_range, in float max_range, in int index )
{

	float normalized_color = ( static_properties[index] - min_property ) / ( max_property - min_property );

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

bool intersect ( in vec4 p )
{
        return ( ( p.x >= box_min.x ) && ( p.x  < box_max.x ) &&
                 ( p.y >= box_min.y ) && ( p.y  < box_max.y ) &&
                 ( p.z  >= box_min.z ) && ( p.z  < box_max.z ) );
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

        if (paper == 1.0 )
        {
               return isInside();

        }else if ( static_properties[property_index] > min_range && static_properties[property_index] < max_range )
        {
                return true;
        }
        return false;

}

void main(void)
{

	if ( (4000 < gl_VertexID &&  gl_VertexID < 4010) || (5000 < gl_VertexID &&  gl_VertexID < 5010) )//isPrimary() )
	{
		vec3 center_of_mass = ((v0+v1+v2+v3+v4+v5+v6+v7)*0.125).xyz;

		axis_x = vec4 (1.0,0.0,0.0,0.0);
		axis_y = vec4 (0.0,1.0,0.0,0.0);
		axis_z = vec4 (0.0,0.0,1.0,0.0);;
		extends= vec4 ( abs(v0.x-v1.x), abs(v0.y-v4.y), abs(v0.z-v3.z), 0.0 );

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
	}else
	{
		cube.v[0] = vec4(0.0);
		cube.v[1] = vec4(0.0);
		cube.v[2] = vec4(0.0);
		cube.v[3] = vec4(0.0);

		cube.v[4] = vec4(0.0);
		cube.v[5] = vec4(0.0);
		cube.v[6] = vec4(0.0);
		cube.v[7] = vec4(0.0);

		cube.n[0] = vec3(0.0);
		cube.n[1] = vec3(0.0);
		cube.n[2] = vec3(0.0);
		cube.n[3] = vec3(0.0);
		cube.n[4] = vec3(0.0);
		cube.n[5] = vec3(0.0);

		gl_Position = vec4(0.0);
	}

}
