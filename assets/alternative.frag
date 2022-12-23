#version 430

//const float kA = 0.8;
//const float kD = 0.8;
//const float kS = 0.8;
//const float kAlphaPrime = 4;

const float kPI = 3.1415926;
#define POINT_LIGHT_COUNT 3

struct pointLight {
	vec3 position;
	vec3 color;
	float brightness;
};

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPosition;
in vec2 v2fTexCoord;

layout ( location = 2 ) uniform vec3 uCameraPosition;
layout ( location = 3 ) uniform mat4 uMaterialData;
layout ( location = 4 ) uniform pointLight uPointLightData[POINT_LIGHT_COUNT];
uniform sampler2D uTexture;

layout ( location = 0 ) out vec4 oColor;

vec3 kA = uMaterialData[0].xyz;
vec3 kD = uMaterialData[1].xyz;
vec3 kS = uMaterialData[2].xyz;
vec3 kE = uMaterialData[3].xyz;
float kAlphaPrime = uMaterialData[3].w;
float kAlpha = 4 * kAlphaPrime;

float orenNayarDiffuse(
  vec3 lightDirection,
  vec3 viewDirection,
  vec3 surfaceNormal,
  float roughness,
  float albedo) {
  
  float LdotV = dot(lightDirection, viewDirection);
  float NdotL = dot(lightDirection, surfaceNormal);
  float NdotV = dot(surfaceNormal, viewDirection);

  float s = LdotV - NdotL * NdotV;
  float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

  float sigma2 = roughness * roughness;
  float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
  float B = 0.45 * sigma2 / (sigma2 + 0.09);

  return albedo * max(0.0, NdotL) * (A + B * s / t) / kPI;
}

float beckmannDistribution(float x, float roughness) {
  float NdotH = max(x, 0.0001);
  float cos2Alpha = NdotH * NdotH;
  float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
  float roughness2 = roughness * roughness;
  float denom = 3.141592653589793 * roughness2 * cos2Alpha * cos2Alpha;
  return exp(tan2Alpha / roughness2) / denom;
}

float cookTorranceSpecular(
  vec3 lightDirection,
  vec3 viewDirection,
  vec3 surfaceNormal,
  float roughness,
  float fresnel) {

  float VdotN = max(dot(viewDirection, surfaceNormal), 0.0);
  float LdotN = max(dot(lightDirection, surfaceNormal), 0.0);

  //Half angle vector
  vec3 H = normalize(lightDirection + viewDirection);

  //Geometric term
  float NdotH = max(dot(surfaceNormal, H), 0.0);
  float VdotH = max(dot(viewDirection, H), 0.000001);
  float x = 2.0 * NdotH / VdotH;
  float G = min(1.0, min(x * VdotN, x * LdotN));
  
  //Distribution term
  float D = beckmannDistribution(NdotH, roughness);

  //Fresnel term
  float F = pow(1.0 - VdotN, fresnel);

  //Multiply terms and done
  return  G * F * D / max(3.14159265 * VdotN * LdotN, 0.000001);
}

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
	vec3 ambient = kA + (light.brightness * 0);

	//diffuse
	vec3 diffuse = kD / kPI;

	//specular
	vec3 specular = kS * ((kAlphaPrime + 2) / 8) * pow( specularTerm, kAlphaPrime);

	//distance falloff
	float falloff = 1 / (dist * dist);

	vec3 blinnPhong = light.color * falloff * (ambient + (factorTerm * (diffuse + specular)));

	const float p = 0.3;

	vec3 ambient2 = kA;
	vec3 diffuse2 = kD * orenNayarDiffuse(L, V, v2fNormal, kAlphaPrime / 64.0, 0.5);
	vec3 specular2 = kS * cookTorranceSpecular(L, V, v2fNormal, kAlphaPrime / 64.0, 0.5);

	vec3 cookTorrance = light.color * falloff * (ambient2 + diffuse2 + specular2);

	return (blinnPhong * 0) + (cookTorrance * 1);

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
	// full blinn-phong
	oColor = (vec4(v2fTexCoord.xy, 0.0, 0.0) *0)+ (texture(uTexture, v2fTexCoord) *1 ) * (1 *vec4(((pointLightContribution() * v2fColor) + (kE * v2fColor)), 1.0));
	
	// normals debug view
	// oColor = (vec4(v2fTexCoord.xy, 0.0, 0.0) *0)+ (texture(uTexture, v2fTexCoord) *0 ) + (1 *vec4(((pointLightContribution() * 0 + normalize(v2fNormal)) + (kE * v2fColor)), 1.0));
	// * 0) + normalize(v2fNormal);
	//oColor = normalize(v2fNormal);
}

