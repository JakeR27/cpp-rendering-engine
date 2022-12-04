#version 430

layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec3 iColor;

out vec3 v2fColor;

void main()
{
	v2fColor = iColor;
	gl_Position = vec4( iPosition.xyz, 1.0);
}
