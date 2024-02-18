#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;

void main()
{
    FragColor = vec4(0.8f, 0.8f, 0.0f, 1.0f);
}
