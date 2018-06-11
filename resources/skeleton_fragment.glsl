#version 410 core
out vec4 color;
in vec3 vertex_pos;
in vec3 vertex_normal;
in vec3 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
	color = vec4(1, 0, .2, 1);
}
