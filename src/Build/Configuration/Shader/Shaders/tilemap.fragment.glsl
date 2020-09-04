#version 330 core

in vec2 TexCoord;

uniform sampler2D tileAtlas;
uniform sampler2D tilemap;

uniform ivec2 ntilesMap;
uniform ivec2 ntilesAtlas;

out vec4 FragColor;

void main()
{   
    //Get the pixel information from the texture of the map that indicates de column and the row of the tileset
    vec4 colRow = texture(tilemap, TexCoord) * 255.0; // * 255 because it returns a normalized value [0, 1]

    /*
        For TileAtlas higher than 255 rows I use also the Blue value.
        In order to do that, we use the Euclidean division. We want to store values grater than 255 in two numbers from 0 to 255.

        Here there is the equation:

        X = A * 256 + B

        If we use A and B for the Y position, we can have 65.535 rows, and a max number of tiles of more than 16.700.000.
        However I reserve Green value of 255 as an indicator of not to draw the tile, and who knows if I will give it another utility.
    */
    if(colRow.y == 255) {
        discard;
    }

    colRow.y = colRow.y * 256 + colRow.z;

    //Calculate the pixel of the tile we want to paint
    vec2 pixel = TexCoord * ntilesMap;
    vec2 tilePos = pixel - floor(pixel);

    //Calculate the position of the atlas
    vec2 atlasPos = (tilePos + colRow.xy) / ntilesAtlas;

    FragColor = texture(tileAtlas, atlasPos);
}