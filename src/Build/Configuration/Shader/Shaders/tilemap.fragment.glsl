#version 330 core

in vec2 TexCoord;

uniform sampler2D tileAtlas;
uniform sampler2D tilemap;

uniform ivec2 ntilesMap;
uniform ivec2 ntilesAtlas;

uniform bool useTransparent = false;
uniform vec3 transparentColor;

uniform int max_columns = 1;

uniform bool tilemap_selected = false;

uniform float alpha = 1.0;

out vec4 FragColor;

void main()
{   
    if (tilemap_selected) {
        //Get the pixel information from the texture of the map that indicates de column and the row of the tileset
        vec4 mapId = texture(tilemap, TexCoord) * 255.0; // * 255 because it returns a normalized value [0, 1]

        /*
            For TileAtlas higher than 255 rows I use also the Blue value.
            In order to do that, we use the Euclidean division. We want to store values greater than 255 in two numbers from 0 to 255.

            Here there is the equation:

            X = A * 256 + B

            If we use A and B for the Y position, we can have 65.535 rows, and a max number of tiles of more than 16.700.000.
            However I reserve Green value of 255 as an indicator of not to draw the tile, and who knows if I will give it another utility.
        */
        if (mapId.g == 255) {
            vec3 colorEmptyTile = vec3(0.9, 0.9, 0.9);
            FragColor = vec4(colorEmptyTile.xyz, alpha);
            return;
        }
        else if (mapId.g == 254) {
            discard;
        }

        int id = int(mapId.x) * 255 + int(mapId.z);

        vec2 colRow;
        colRow.x = id % max_columns; //TODO: margin and spacing
        colRow.y = id / max_columns;

        //Calculate the pixel of the tile we want to paint
        vec2 pixel = TexCoord * ntilesMap;
        vec2 tilePos = pixel - floor(pixel);

        //Calculate the position of the atlas
        vec2 atlasPos = (tilePos + colRow.xy) / ntilesAtlas;

        if (useTransparent) {
            vec4 color = texture(tileAtlas, atlasPos);
            if (color.xyz == transparentColor) {
                discard;
            }
            else {
                FragColor = vec4(color.xyz, alpha);
            }
        }
        else {
            FragColor = vec4(texture(tileAtlas, atlasPos).xyz, alpha);
        }
    }
    else{
        FragColor = vec4(0.9, 0.9,0.9, alpha);
    }
}