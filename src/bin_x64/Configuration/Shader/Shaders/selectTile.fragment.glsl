#version 330 core

in vec2 TexCoord;

uniform sampler2D Texture;

uniform ivec2 tileSelected;
uniform ivec2 ntilesAtlas;

out vec4 FragColor;

float minRangeBorder = 0.05;

void main()
{
    //Calculate the position of the atlas
    vec2 atlasPos = (TexCoord + tileSelected) / ntilesAtlas;
    if (TexCoord.x < minRangeBorder || TexCoord.x > 1.0 - minRangeBorder || TexCoord.y < minRangeBorder || TexCoord.y > 1.0 - minRangeBorder) {
        FragColor = vec4(0.9, 0.470, 0.0, 1.0);
    }
    else {
        FragColor = vec4(texture(Texture, atlasPos).xyz, 0.75)/* + vec4(0.9, 0.470, 0.0, 0.2)*/;
    }
}