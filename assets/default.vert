#version 430

layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;

layout( location = 0 ) uniform mat4 projection;

out vec3 v2fColor;
out vec3 v2fNormal;
out vec3 v2fPosition;

void main()
{
	v2fColor = iColor; 
	v2fNormal = normalize(iNormal);
	v2fPosition = iPosition;
	gl_Position = projection * vec4( iPosition.xyz, 1.0);
}
