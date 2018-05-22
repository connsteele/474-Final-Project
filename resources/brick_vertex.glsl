#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec2 vertex_tex;
out vec3 vertex_nor;
out vec3 vertex_pos;

void main()
{
	gl_Position = P * V * M * vec4(vertPos, 1.0);
    vertex_tex = vertTex;
    vertex_nor = vec4(M * vec4(vertNor, 0)).xyz;
    vertex_pos = vec4(M * vec4(vertPos, 1)).xyz;
}
