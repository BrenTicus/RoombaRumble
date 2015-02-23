#version 330 core

// Output
layout (location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 uv;
} fs_in;

// Material properties
uniform vec3 diffuse_albedo = vec3(0.5, 0.5, 0.5);
uniform vec3 specular_albedo = vec3(0.5);
uniform float specular_power = 85.0;
uniform vec3 ambient = vec3(0.0, 0.0, 0.0);

// Texture Mapping
uniform sampler2D texObject;

void main(void)
{
    // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);

    // Calculate R locally
    vec3 R = reflect(-L, N);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
    diffuse *= vec3(texture(texObject, fs_in.uv));
    vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;

    // Write final color to the framebuffer
    color = vec4(ambient + diffuse + specular, 1.0);
}
