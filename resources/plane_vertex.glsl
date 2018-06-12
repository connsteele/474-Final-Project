#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in int vertimat;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
uniform mat4 Manim[200]; //get the matrices of the bones

void main()
{
	mat4 Ma = Manim[vertimat];
	vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;
	vec4 tpos =  M * vec4(vertPos, 1.0);
	//vec4 tpos =  Ma * vec4(vertPos, 1.0);
	//tpos.x = Ma[3][0]; //these pos statements fix monster hands bone issue
    //tpos.y = Ma[3][1];
    //tpos.z = Ma[3][2];
	vertex_pos = tpos.xyz;
	gl_Position = P * V *  tpos;
	vertex_tex = vertTex;
}
