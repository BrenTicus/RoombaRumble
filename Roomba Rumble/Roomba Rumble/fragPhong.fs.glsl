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
uniform bool visible = true;

// Texture Mapping
uniform sampler2D texObject;

void main(void)
{
	if (visible)
	{
		// Normalize the incoming N, L and V vectors
		vec3 N = normalize(fs_in.N);
		vec3 L = normalize(fs_in.L);
		vec3 V = normalize(fs_in.V);

		// Calculate R locally
		vec3 R = reflect(-L, N);

		// Compute the diffuse and specular components for each fragment
		vec3 diffuse;
		if (abs(dot(L, N)) > 0.0)
		{
			diffuse = max(abs(dot(L, N)), 0.5) * diffuse_albedo * vec3(texture(texObject, fs_in.uv));
		}
		else diffuse = vec3(0.0, 0.0, 0.0);
		vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;

		// Write final color to the framebuffer
		color = vec4(ambient + diffuse + specular, 1.0);
	}
	else color = vec4(0.8, 0.8, 0.8, 1.0);
}
