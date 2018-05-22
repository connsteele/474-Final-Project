#version 410 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in unsigned int vertimat;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
uniform mat4 Manim[200];
void main()
{
	
	mat4 Ma = Manim[vertimat];
	vec4 pos = Ma*vec4(vertPos,1.0);
	gl_Position = P * V * M * pos;
	
}
