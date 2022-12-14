#version 430

layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;
layout( location = 3 ) in vec2 iTexCoord;

layout ( location = 0 ) uniform mat4 projection;
layout ( location = 1 ) uniform mat4 modelTransform;

out vec3 v2fColor;
out vec3 v2fNormal;
out vec3 v2fPosition;
out vec2 v2fTexCoord;

void main()
{
	v2fColor = iColor; 
	v2fNormal = normalize(iNormal);
	v2fPosition = (modelTransform * vec4(iPosition.xyz, 1.0)).xyz;
	v2fTexCoord = iTexCoord;
	gl_Position = projection * vec4( iPosition.xyz, 1.0);
}
