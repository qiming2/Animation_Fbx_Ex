#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 n;
layout(location = 2) in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_IN {
	vec3 pos;
	vec3 n;
	vec2 uv;
} vs_out;

void main() {

	vs_out.pos = pos;
	vs_out.uv = uv;
	vs_out.n = normalize(mat3(view) * n);
	gl_Position = projection * view * model * vec4(pos, 1.0);
}



#type - delim

#version 330 core
out vec4 frag;

in VS_IN {
	vec3 pos;
	vec3 n;
	vec2 uv;
} vs_in;

void main() {
	frag = vec4(vs_in.uv.x, vs_in.uv.y,  1.0 - vs_in.uv.x - vs_in.uv.y, 1.0);
}