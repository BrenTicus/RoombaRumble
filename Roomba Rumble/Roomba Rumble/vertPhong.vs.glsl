#version 330 core

// Per-vertex inputs
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tcoords;

uniform mat4 mod_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

// Inputs from vertex shader
out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 uv;
} vs_out;

// Position of light
uniform vec3 light_pos = vec3(100.0, 100.0, -100.0);

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = mod_matrix * position;

    // Calculate normal in view-space
    vs_out.N = mat3(mod_matrix) * normal;

    // Calculate light vector
    vs_out.L = light_pos - P.xyz;

    // Calculate view vector
    vs_out.V = -P.xyz;

	// Pass along the texture coordinates
    vs_out.uv = tcoords.st;
	
    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * view_matrix * P;
}
