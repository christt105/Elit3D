#version 330 core

/*
Special thanks to the post (https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders) author: Martin Prazak (http://www.mprazak.info/)
and the post (http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/) author: Marie-Eve Dubé (https://github.com/bugztroll)
*/

in vec3 nearPoint;
in vec3 farPoint;

uniform mat4 view;
uniform mat4 projection;

out vec4 FragColor;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;

    if (grid.x > 0.999 && grid.y > 0.999) {
            discard;
    }
    else {
        vec3 mainaxis = vec3(0.0, 0.0, 0.0);

        // z axis
        if(fragPos3D.x > -0.02 && fragPos3D.x < 0.02)
            mainaxis = vec3(0.0, 0.0, 1.0);
        // x axis
        if(fragPos3D.z > -0.02 && fragPos3D.z < 0.02)
            mainaxis = vec3(1.0, 0.0, 0.0);

        return vec4(mainaxis.xyz, 1.0);
    }
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);

    float clip_space_depth = clip_space_pos.z / clip_space_pos.w;

	float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;

	float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;

	return depth;
}

void main()
{   
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);
        
    FragColor = (grid(fragPos3D, 1))* float(t > 0);
}