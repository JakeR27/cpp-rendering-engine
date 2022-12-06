#version 430

const float kAmbientStrength = 0.1;
const float kDiffuseStrength = 0.3;
const float kSpecularStrength = 0.5;
const float kShininess = 200.0;
#define POINT_LIGHT_COUNT 2

struct pointLight {
	vec3 position;
	vec3 color;
};

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPosition;

layout( location = 1 ) uniform pointLight uPointLightData[POINT_LIGHT_COUNT];
layout( location = 5 ) uniform vec3 uCameraDirection;

layout( location = 0 ) out vec3 oColor;


vec3 calculate_pointLight_contribution(pointLight light) {
	vec3 ambientLightContribution = light.color * kAmbientStrength;

	vec3 lightDirection = normalize(light.position - v2fPosition);
	float diffuseStrength = max(dot(v2fNormal, lightDirection), 0.0);

	vec3 diffuseLightContribution = light.color * diffuseStrength * kDiffuseStrength;

	vec3 reflectionDirection = reflect(-lightDirection, v2fNormal);
	float specularStrength = pow(max(dot(uCameraDirection, reflectionDirection), 0.0), kShininess);

	vec3 specularLightContribution = light.color * specularStrength * kSpecularStrength;

	return (ambientLightContribution + diffuseLightContribution + specularLightContribution);

}

vec3 pointLightContribution() {
	vec3 lightingOutput;
	for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
		lightingOutput += calculate_pointLight_contribution(uPointLightData[i]);
	}
	return lightingOutput;
}

void main()
{
	oColor = pointLightContribution() * v2fColor;
}

