#version 130

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec3 normal_interp;
out vec4 color_interp;
out vec2 uv_interp;
out vec3 light_pos;

// Material attributes (constants)
uniform vec3 light_position = vec3(0, 10, 0);



void main()
{
    // Transform vertex position
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

    // Transform vertex position without including projection
    position_interp = vec3(view_mat * world_mat * vec4(vertex, 1.0));

    // Transform normal
    normal_interp = vec3(normal_mat * vec4(normal, 0.0));

    // Transform light position to align with view
    light_pos = vec3(view_mat * vec4(light_position, 1.0));

    color_interp = vec4(color, 1.0);

    uv_interp = uv;
}
