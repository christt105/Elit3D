#version 330 core

in vec2 TexCoord;

uniform sampler2D TextureAtlas;
uniform sampler2D TextureMap;

uniform ivec2 tileSelected;
uniform ivec2 ntilesAtlas;

uniform vec2 sizeMap;
uniform vec2 mousePos;

uniform int tool;	// 0: Brush | 1: Eraser ...
uniform bool locked;
uniform int brushSize = 1;
uniform int brushShape = 0;

out vec4 FragColor;

float minRangeBorder = 0.025;

//Color indicators
vec4 orange = vec4(0.9, 0.470, 0.0, 1.0);
vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
vec4 green = vec4(0.0, 1.0, 0.0, 1.0);

uniform bool loco = true;

void DrawTexturedTile() {
	//Calculate the position of the atlas
			vec2 coord = TexCoord * brushSize;
			coord = coord - floor(coord);
			vec2 atlasPos = (coord + tileSelected) / ntilesAtlas;
			if (locked) {
				if (tool == 0 || tool == 2) {
					if (tileSelected.x == -1.0) {
						discard;
					}
					else {
						FragColor = vec4(texture(TextureAtlas, atlasPos).xyz, 0.75) * red;
					}
				}
				else {
					FragColor = vec4(red.xyz, 0.2);
				}
			}
			else {
				if (tool == 0 || tool == 2) {
					if (tileSelected.x == -1.0) {
						discard;
					}
					else {
						FragColor = vec4(texture(TextureAtlas, atlasPos).xyz, 0.75) /* + vec4(0.9, 0.470, 0.0, 0.2)*/;
					}
				}
				else {
					FragColor = vec4(orange.xyz, 0.2);
				}
			}
}

void main()
{
	if(loco){
		vec2 mapPos = mousePos;
		if(mousePos.x < sizeMap.x-1)
			mapPos.x += 1.0;
		if(mousePos.y < sizeMap.y-1)
			mapPos.y += 1.0;
		vec4 tileOnMouse = texture(TextureMap, mapPos/sizeMap);
		vec4 tile = texture(TextureMap, TexCoord);
		if(tileOnMouse == tile){
			FragColor = vec4(green.xyz, 0.5);
		}
		else{
			FragColor = vec4(orange.xyz, 0.5);
		}
	}
	else{
	if (brushShape == 0) {
  		minRangeBorder = minRangeBorder / brushSize;
		if (TexCoord.x < minRangeBorder || TexCoord.x > 1.0 - minRangeBorder || TexCoord.y < minRangeBorder || TexCoord.y > 1.0 - minRangeBorder) {
			if (tool == 0 || tool == 2 || tool == 3) {
				if (locked) {
					FragColor = red;
				}
				else {
					FragColor = green;
				}
    		}
			else {
				FragColor = orange;
			}
		}
		else {
			if(tool != 3)
				DrawTexturedTile();
		}
	}
	else {
		float r = float(brushSize) * 0.5;
		vec2 uv = TexCoord;
		if (brushSize % 2 == 0) {
			uv -= 0.5;
		}
		uv *= float(brushSize);

		vec2 p = floor(uv) + 0.5;
		if (brushSize % 2 != 0) {
			p -= 0.5 * float(brushSize);
		}

		if (p.x*p.x+p.y*p.y <= r*r) {
			DrawTexturedTile();
		}
		else {
			discard;
		}
	}
	}
}