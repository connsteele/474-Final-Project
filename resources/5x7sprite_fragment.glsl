#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

//two textures to interpolate between
uniform sampler2D tex;
uniform sampler2D tex2;

//offset uniforms
uniform vec2 offset1;
uniform vec2 offset2; 

//interpolation uniform
uniform float t;

//get team uniform for coloring the teams, color active unit
uniform int team;
uniform int activeUnit;


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

	//update color for teams
	if (team == 1) //red team
	{
		color *= vec4(1, .6, .6, 1);
		color  = clamp(color, 0, 1);
	}
	else if (team == 2) //blue team
	{
		color *= vec4(.6, 1, 1, 1);
		color  = clamp(color, 0, 1);
	}

	//update color if active unit
	if (activeUnit == 1)
	{
		color *= vec4(1.7, 1.7, 1.7, 1);
		color = clamp(color, 0, 1); //clamp the colors so they render correctly
	}


}
