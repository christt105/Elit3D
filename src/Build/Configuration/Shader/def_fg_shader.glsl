#version 330 core

in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D ourTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(ourTexture, TexCoord);// * vec4(color, 1.0);
}