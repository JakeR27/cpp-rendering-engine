#version 430

const float kAmbientStrength = 0.1;
const float kDiffuseStrength = 0.3;
const float kSpecularStrength = 0.3;
const float kShininess = 32;
#define POINT_LIGHT_COUNT 2

struct pointLight {
	vec3 position;
	vec3 color;
};

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPosition;

layout ( location = 1 ) uniform pointLight uPointLightData[POINT_LIGHT_COUNT];
layout( location = 5 ) uniform vec3 uCameraPosition;

layout( location = 0 ) out vec3 oColor;


vec3 calculate_pointLight_contribution(pointLight light) {
	vec3 ambientLightContribution = light.color * kAmbientStrength;

	vec3 lightDirection = normalize(light.position - v2fPosition);
	float diffuseStrength = max(dot(v2fNormal, lightDirection), 0.0);

	vec3 diffuseLightContribution = light.color * diffuseStrength * kDiffuseStrength;

	vec3 reflectionDirection = reflect(-lightDirection, v2fNormal);
	vec3 viewDirection = normalize(uCameraPosition - v2fPosition);
	float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), kShininess);

	vec3 specularLightContribution = light.color * specularStrength * kSpecularStrength * 0.0f;

	float dist = distance(light.position, v2fPosition);
	float distanceContribution = 1.0 / (dist * dist);

	return (ambientLightContribution + diffuseLightContribution + specularLightContribution) * distanceContribution;

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
	//oColor = normalize(v2fNormal);
	oColor = pointLightContribution() * v2fColor;
}

