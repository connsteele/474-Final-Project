#version 330 core
out vec4 color;
in vec3 vertNor;
in vec2 vertex_tex;
in vec3 vertex_nor;
in vec3 vertex_pos;

uniform vec3 camPos;
uniform sampler2D tex1;
uniform sampler2D tex2;


void main()
{
	vec4 tcol = texture(tex1, vertex_tex);
    color = tcol;
    // diffuse lighting
    vec3 n = normalize(vertex_nor);
    vec3 lp = vec3(150, 300, 100);
    vec3 ld = normalize(lp - vertex_pos);
    float diffuse = dot(n, ld);
    diffuse = clamp(diffuse, 1, 1.5);
    color *= diffuse;

    // specular lighting
    vec3 cd = normalize(camPos - vertex_pos);
    vec3 h = normalize(cd + ld);
    float specular = dot(h, n);
    specular = clamp(specular, 0, 0.5);
    specular = pow(specular, 20);
    color += specular;
    color.a = 1;
}
