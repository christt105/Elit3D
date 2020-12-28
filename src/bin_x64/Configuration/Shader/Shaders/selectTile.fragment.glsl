#version 330 core

in vec2 TexCoord;

uniform sampler2D Texture;

uniform ivec2 tileSelected;
uniform ivec2 ntilesAtlas;

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

void main()
{
	if (brushShape == 0) {
  		minRangeBorder = minRangeBorder / brushSize;
		//Calculate the position of the atlas
		vec2 coord = TexCoord * brushSize;
		coord = coord - floor(coord);
		vec2 atlasPos = (coord + tileSelected) / ntilesAtlas;
		if (TexCoord.x < minRangeBorder || TexCoord.x > 1.0 - minRangeBorder || TexCoord.y < minRangeBorder || TexCoord.y > 1.0 - minRangeBorder) {
			if (tool == 0) {
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
			if (locked) {
				if (tool == 0) {
					if (tileSelected.x == -1.0) {
						discard;
					}
					else {
						FragColor = vec4(texture(Texture, atlasPos).xyz, 0.75) * red;
					}
				}
				else {
					FragColor = vec4(red.xyz, 0.2);
				}
			}
			else {
				if (tool == 0) {
					if (tileSelected.x == -1.0) {
						discard;
					}
					else {
						FragColor = vec4(texture(Texture, atlasPos).xyz, 0.75) /* + vec4(0.9, 0.470, 0.0, 0.2)*/;
					}
				}
				else {
					FragColor = vec4(orange.xyz, 0.2);
				}
			}
		}
	}
	else {
		float r = float(brushSize) * 0.5;
		vec2 uv = (TexCoord);
		if (brushSize % 2 == 0) {
			uv -= 0.5;
		}
		else {

		}
		uv *= float(brushSize);
		vec2 p = floor(uv);
		
		vec2 middle = vec2(0.0, 0.0);
			p += 0.5;
		if (brushSize % 2 != 0) {
			p -= 0.5 * brushSize;
			float range = 0.5/brushSize;
			//middle -= vec2(range, range);
		}

		vec2 c = p - middle;
		if (c.x*c.x+c.y*c.y <= r*r) {
			FragColor = vec4(p.xy, r, 1.0);
		}
		else {
			discard;
		}
		/*vec2 uv = TexCoord * brushSize;
		vec2 l = vec2(0.45, 0.45);
		vec2 u = vec2(0.55, 0.55);
		if(uv.x >= (floor(uv) +l).x && uv.x <= (floor(uv) + u).x
			&&
			uv.y >= (floor(uv) + l).y && uv.y <= (floor(uv) + u).y)
			FragColor = red;
		else
			FragColor = green;*/
	}
}