#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.005; // Gravity
float speed = 2.5; // Allows to control the speed of the explosion


void main()
{
    // Let time cycle every four seconds
	float particle_id = color.r;
	float phase = particle_id * 80;
	float time = timer + phase;
    float t = time - 80.0 * floor(time / 80);
	float t2 = timer - 80.0 * floor(timer / 80);

    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 1.0);

	float offset = 0;
	if (norm.x < 0) offset = 3.14159;

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += (t / 20) * sin(t + offset + t2);
    position.y -= t/2;
    position.z += (t / 20) * cos(t + offset + t2);
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    //vertex_color = color.rgb; // Color defined during the construction of the particles
    vertex_color = object_color; // Uniform color 
    //vertex_color = vec3(t, 0.0, 1-t);
    //vertex_color = vec3(1.0, 1-t, 0.0);

    // Forward time step to geometry shader
    timestep = t;
}
