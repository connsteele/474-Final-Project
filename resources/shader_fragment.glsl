#version 410 core
out vec4 color;
in vec3 vertex_pos;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
color = vec4(1,1,1,1);
}
