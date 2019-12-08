// Renders texture and applies lighting on top

#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map;

// Material attributes (constants)
uniform vec3 light_direction = vec3(-1.0, -0.7, -1.0); //direction of the directional light
uniform vec4 ambient_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 diffuse_color = vec4(0.6, 0.6, 0.6, 1.0);
uniform vec4 specular_color = vec4(0.7, 0.7, 0.7, 1.0);
float phong_exponent = 7.0;
uniform float Ia = 0.4; // Ambient light amount


void main() 
{
    vec3 N, // Interpolated normal for fragment
         L,
         V, // View direction
		 R;

	// Compute Lambertian term Id
	N = normalize(normal_interp);

	L = -normalize(light_direction);

	float Id = max(dot(N, L), 0.0);
    
	// Reflection
	// R = -L + 2 * (L . N)N
	R = -L + 2 * (dot(L, N)) * N;
        
	float spec_angle_cos = max(dot(V, R), 0.0);
	float Is = pow(spec_angle_cos, phong_exponent);

    // Retrieve texture value
    vec4 pixel = texture(texture_map, uv_interp);

    // Assign illumination to the fragment
	// Lighting is multiplied with the ambient color
    gl_FragColor = pixel * Ia + Id * diffuse_color;

	//gl_FragColor = vec4(1.0) * sign(N.z);

}
