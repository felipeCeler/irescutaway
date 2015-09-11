#version 420
	 
in vec4 ex_Color;
in vec3 normalDirection;
in vec4 vert;
in mat3 lightRotationMatrix;

out vec4 out_Color;

uniform mat3 lightViewMatrix;
	 
void main(void)
{

	vec3 lightDirection = lightViewMatrix * vec3(0.0, 0.0, 1.0);
	lightDirection = normalize(lightDirection);
	
	vec3 lightReflection = reflect(-lightDirection, normalDirection);
	vec3 eyeDirection = -normalize(vert.xyz);
	float shininess = 100.0;

	vec4 ambientLight = ex_Color * 0.5;
	vec4 diffuseLight = ex_Color * 0.4 * max(dot(lightDirection, normalDirection),0.0);
	vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection,eyeDirection),shininess),0.0);

	out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz,1.0);

}
