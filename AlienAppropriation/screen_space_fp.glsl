#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;

void main() 
{
    // wavering
    vec2 pos = uv0;
    pos.x = pos.x + 0.05*(sin(timer*5.0+8.0*pos.y));

    vec4 pixel = texture(texture_map, pos);
    
    gl_FragColor = pixel;
}
