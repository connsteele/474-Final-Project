#version 410 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in int vertimat;
//layout(location = 2) in vec3 vertNor;
//layout(location = 3) in vec3 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec3 vertex_normal;
out vec3 vertex_tex;
uniform mat4 Manim[200];
void main()
{
	//vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;
	mat4 Ma = Manim[vertimat];
	vec4 pos = Ma*vec4(vertPos,1.0);
	pos.x = Ma[3][0]; //these pos statements fix monster hands bone issue
    pos.y = Ma[3][1];
    pos.z = Ma[3][2];
	gl_Position = P * V * M * pos;
	//vertex_tex = vertTex;
	//vertex_pos = pos.xyz;
}
