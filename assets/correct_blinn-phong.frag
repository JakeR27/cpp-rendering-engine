#version 430

const float kA = 0.2;
const float kD = 0.5;
const float kS = 0.2;
const float kAlphaPrime = 4;
const float kAlpha = 4 * kAlphaPrime;
const float kPI = 3.1415926;
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

	vec3 L = normalize(light.position - v2fPosition);
	vec3 V = normalize(uCameraPosition - v2fPosition);
	vec3 N = normalize(v2fNormal);
	vec3 H = normalize(L + V);
	vec3 R = normalize((2 * dot(L, N) * N) - L);

	float dist = distance(v2fPosition, light.position);
	float factorTerm = max( dot(N, L), 0 );
	float specularTerm = max( dot(H, N), 0 );
	//float specularTerm = max( dot(R, V), 0 );

	//ambient
	float ambient = kA;

	//diffuse
	float diffuse = kD / kPI;

	//specular
	float specular = kS * ((kAlphaPrime + 2) / 8) * pow( specularTerm, kAlphaPrime);

	//distance falloff
	float falloff = 1 / (dist * dist);

	return light.color * falloff * (ambient + (factorTerm * (diffuse + specular)));

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

