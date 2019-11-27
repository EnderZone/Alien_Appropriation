// There are two lights: A red directional light from the left, and a blue point light from the right
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
vec4 ambient_color = vec4(1.0, 1.0, 1.0, 1.0);
vec4 diffuse_color_1 = vec4(0.0, 0.0, 0.7, 1.0);
vec4 specular_color_1 = vec4(0.0, 0.0, 1.0, 1.0);
float phong_exponent_1 = 64.0;


vec3 light_direction = vec3(1.0, -1.0, -1.0); //direction of the directional light
vec4 diffuse_color_2 = vec4(0.2, 0.2, 0.2, 1.0);
vec4 specular_color_2 = vec4(0.7, 0.7, 0.7, 1.0);
float phong_exponent_2 = 7.0;
float Ia = 0.6; // Ambient light amount


void main() 
{
    vec3 N, // Interpolated normal for fragment
         L1, // Light-source direction
		 L2,
         V, // View direction
		 R1,  // Reflection vector
		 R2;

	
    // Compute Lambertian term Id
    N = normalize(normal_interp);

	//Light source 1 (blue)
		L1 = (light_pos - position_interp);
		L1 = normalize(L1);

		float Id1 = max(dot(N, L1), 0.0);
    
		// Compute specular term Is for Phong shading
		// V = (eye_position - position_interp);
		V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
		V = normalize(V);

		// Reflection
		// R = -L + 2 * (L1 . N)N
		R1 = -L1 + 2 * (dot(L1, N)) * N;
        
		float spec_angle_cos1 = max(dot(V, R1), 0.0);
		float Is1 = pow(spec_angle_cos1, phong_exponent_1);

	//Light source 2 (red)
		L2 = -normalize(light_direction);

		float Id2 = max(dot(N, L2), 0.0);
    
		// Reflection
		// R = -L2 + 2 * (L2 . N)N
		R2 = -L2 + 2 * (dot(L2, N)) * N;
        
		float spec_angle_cos2 = max(dot(V, R2), 0.0);
		float Is2 = pow(spec_angle_cos2, phong_exponent_2);

    // Retrieve texture value
    vec4 pixel = texture(texture_map, uv_interp);

    // Assign illumination to the fragment
	// Lighting is multiplied with the ambient color
    gl_FragColor = pixel * Ia + Id2 * diffuse_color_2 + Is2 * specular_color_2;

}
