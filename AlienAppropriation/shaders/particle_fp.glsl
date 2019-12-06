#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;

void main (void)
{
    // Very simple fragment shader, but we can do anything we want here
    // We could apply a texture to the particle, illumination, etc.
	vec4 new_color = vec4(0.0, 1.0, 1.0, 0.0);
    gl_FragColor = new_color;
}
