#version 130

// Attributes passed from the vertex shader
in vec3 vertex_position;
in vec2 vertex_uv;
in mat3 TBN_mat;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map; // Normal map

// Material attributes (constants)
uniform vec4 object_color = vec4(0.49, 0.36, 0.20, 1.0);


void main() 
{

    // Blinn-Phong shading

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

    // Get substitute normal in tangent space from the normal map
    vec2 coord = vertex_uv;
    coord.y = 1.0 - coord.y;
    N = normalize(texture2D(texture_map, coord).rgb*2.0 - 1.0);

    // Work in tangent space by multiplying our vectors by TBN_mat    
    // Get light direction
    L = TBN_mat * (light_pos - vertex_position);
    L = normalize(L);
    
    // Compute diffuse lighting intensity
    float lambertian = max(dot(N, L), 0.0);

    // Get view direction
    //V = TBN_mat * (eye_position - vertex_position);
    V = TBN_mat * (- vertex_position); // We already applied the view matrix, so the camera is at the origin
    V = normalize(V);
    
    // Blinn-Phong specular component
    //H = 0.5*(V + L);
    H = (V + L);
    H = normalize(H);
    
    float spec_angle = max(dot(N, H), 0.0);
    float specular = pow(spec_angle, 128.0);
        
    // Assume all components have the same color but with different weights
    float ambient = 0.4;
    if (gl_FrontFacing){
        gl_FragColor = (0.25*ambient + 0.5*lambertian + 1.0*specular)*object_color;
    } else {
        gl_FragColor = object_color;
    }
}
