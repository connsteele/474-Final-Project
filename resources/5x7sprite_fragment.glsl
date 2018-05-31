#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex;
uniform sampler2D tex2;

//offset uniforms
uniform vec2 offset1;
uniform vec2 offset2; 
//interpolation uniform
uniform float t;

void main()
{
	vec3 n = normalize(vertex_normal);
	vec3 lp = vec3(10,-20,-100);
	vec3 ld = normalize(vertex_pos - lp);
	float diffuse = dot(n,ld);

	//set up the 2x textures to interpolate between
	vec4 tcol = texture(tex, vec2((vertex_tex.x / 5) + offset1.x, (vertex_tex.y / 7) + offset1.y ) ); //need a tcol2, then interpolate btwn these two
	vec4 tcol2 = texture(tex, vec2((vertex_tex.x / 5) + offset2.x, (vertex_tex.y / 7) + offset2.y ) );

	//interpolate between the textures
	color = (tcol2 * t) + (tcol * (1 - t)); // or flip tcols


}
