#version 430

in vec3 v2fColor;

layout( location = 1 ) uniform vec3 uAmbientColor;

layout( location = 0 ) out vec3 oColor;

void main()
{
	float kAmbientStrength = 0.1;
	vec3 ambientLight = uAmbientColor * kAmbientStrength;

	oColor = v2fColor * ambientLight;
}
