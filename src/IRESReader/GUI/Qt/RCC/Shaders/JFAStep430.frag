#version 430 core
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect primary;

uniform vec2 viewport;


uniform int stepSize;
uniform float pm_sz;
uniform float theta;

in vec2 texCoord;

out vec4 fragmentColor;
/*
 * Jumping Order
 *   ----------------------
        7->    0-> 	  1->
 *    |[1, 1]|[0,1]|[-1, 1]|
 *  6 |[1, 0]|[0,1]|[-1, 0]|  2
 *    |[1,-1]|[0,1]|[-1,-1]|
        <-5   <-4     <-3
 *   ----------------------
 */


float cutawaySurface ( in vec2 current_position, in vec3 neighborInformation )
{
	vec2 vector = current_position - ( neighborInformation.xy );

	float distance = length(vector)/length(viewport);

	return (neighborInformation.z - ((-theta)*(pm_sz+neighborInformation.z)*(distance)) );
	//return (neighborInformation.z - theta*(distance) );
}

float distanceTransforme ( in vec2 current_position, in vec2 neighborInformation )
{
	vec2 vector = current_position - ( neighborInformation.xy );

	float distance = dot(vector, vector);

	return distance;
}



void main()
{
	vec3  neighborInformation = vec3(0.0);
	vec3  NearestCoord 		 = vec3(0.0);
	float distance			 = 0.0;
	float min_distance	 	 = 0.0;
	vec2  vector			 = vec2(0.0);


	vec2 current_position = gl_FragCoord.xy;

	// Compute a criterion value using the information of neighbor0
	if (gl_FragCoord.t+stepSize < viewport.y)
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s    	   ,gl_FragCoord.t+stepSize  )).rgb;


		if (neighborInformation.z > 0.0)
		{
			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}
	// Compute a criterion value using the information of neighbor1
	if ( (gl_FragCoord.s-stepSize >= 0) && (gl_FragCoord.t+stepSize < viewport.y))
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s-stepSize,gl_FragCoord.t+stepSize ) ).rgb;
		if (neighborInformation.z > 0.0)
		{
			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}

	// Compute a criterion value using the information of neighbor2
	if ( gl_FragCoord.s-stepSize >= 0)
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s-stepSize,gl_FragCoord.t  	    ) ).rgb;
		if (neighborInformation.z > 0.0)
		{

			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}
	// Compute a criterion value using the information of neighbor3
	if ( (gl_FragCoord.s-stepSize >= 0) && (gl_FragCoord.t-stepSize >= 0) )
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s-stepSize,gl_FragCoord.t-stepSize ) ).rgb;
		if (neighborInformation.z > 0.0)
		{

			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}
	// Compute a criterion value using the information of itself

	neighborInformation = texture2DRect ( primary, gl_FragCoord.st ).rgb;
	if (neighborInformation.z > 0.0)
	{

		distance = cutawaySurface(current_position, neighborInformation.xyz);
		if (distance > min_distance)
		{
			min_distance = distance;
			NearestCoord = neighborInformation.xyz;
		}
	}

	// Compute a criterion value using the information of neighbor4
	if ( (gl_FragCoord.t-stepSize >= 0) )
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s    	   ,gl_FragCoord.t-stepSize ) ).rgb;
		if (neighborInformation.z > 0.0)
		{

			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}
	// Compute a criterion value using the information of neighbor5
	if ( (gl_FragCoord.s+stepSize < viewport.x) && (gl_FragCoord.t-stepSize >= 0) )
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s+stepSize,gl_FragCoord.t-stepSize ) ).rgb;
		if (neighborInformation.z > 0.0)
		{

			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}
	// Compute a criterion value using the information of neighbor6
	if ( gl_FragCoord.s+stepSize < viewport.x )
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s+stepSize,gl_FragCoord.t        ) ).rgb;
		if (neighborInformation.z > 0.0)
		{

			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;
			}
		}
	}
	// Compute a criterion value using the information of neighbor7
	if ( (gl_FragCoord.s+stepSize < viewport.x) && (gl_FragCoord.t+stepSize < viewport.y) )
	{
		neighborInformation = texture2DRect ( primary, vec2(gl_FragCoord.s+stepSize,gl_FragCoord.t+stepSize ) ).rgb;
		if (neighborInformation.z > 0.0)
		{

			distance = cutawaySurface(current_position, neighborInformation.xyz);
			if (distance > min_distance)
			{
				min_distance = distance;
				NearestCoord = neighborInformation.xyz;

			}
		}
	}

	if (min_distance <= 0.0)
		discard;

	fragmentColor = vec4(NearestCoord.xyz,distance);


}
