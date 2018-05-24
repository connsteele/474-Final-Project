#version 410 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;
uniform vec2 to;
uniform vec2 to2;
uniform float t;
uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
vec3 n = normalize(vertex_normal);
vec3 lp=vec3(10,-20,-100);
vec3 ld = normalize(vertex_pos - lp);
float diffuse = dot(n,ld);

//vec4 tcol = texture(tex, vertex_tex);
vec4 tcol = texture(tex, vec2((vertex_tex.x) + to.x, (vertex_tex.y) + to.y));
vec4 tcol2 = texture(tex, vec2((vertex_tex.x) + to2.x, (vertex_tex.y) + to2.y));

vec4 interpolatedtcol = (tcol2 * t) + (tcol * (1 - t));
color = interpolatedtcol;



}
