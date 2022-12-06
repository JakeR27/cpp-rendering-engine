#version 430

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPosition;

layout( location = 1 ) uniform vec3 uLightColor;
layout( location = 2 ) uniform vec3 uLightPosition;

layout( location = 0 ) out vec3 oColor;

void main()
{
	//ambient lighting
	float kAmbientStrength = 0.1;
	vec3 ambientLight = uLightColor * kAmbientStrength;

	// diffuse lighting
	float kDiffuseStrength = 0.3;
	vec3 lightDir = normalize(uLightPosition - v2fPosition);
	float diffuseStrength = max(dot(v2fNormal, lightDir), 0.0) * kDiffuseStrength;
	vec3 diffuseLight = uLightColor * diffuseStrength;

	oColor = (ambientLight + diffuseLight) * v2fColor;
}
