#version 330 core

in vec3 texcoords;
uniform samplerCube cube_texture;
out vec4 colour;

void main () {
	colour = texture (cube_texture, texcoords);
}