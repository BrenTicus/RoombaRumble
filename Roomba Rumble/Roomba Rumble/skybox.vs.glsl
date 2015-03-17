#version 330 core

in vec3 position;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
out vec3 texcoords;

void main () {
  texcoords = position;
  gl_Position = proj_matrix * mv_matrix * vec4 (position, 1.0);
}