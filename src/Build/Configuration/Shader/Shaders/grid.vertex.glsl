#version 330 core

/*
Special thanks to the post (https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders) author: Martin Prazak (http://www.mprazak.info/)
and the post (http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/) author: Marie-Eve Dubé (https://github.com/bugztroll)
*/

vec3 gridPlane[6] = vec3[] (
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, -1, 0), vec3(1, 1, 0)
);

uniform mat4 view;
uniform mat4 projection;

out vec3 nearPoint;
out vec3 farPoint;

vec3 UnprojectPoint(float x, float y, float z, mat4 viewmat, mat4 proj) {
    mat4 viewInv = inverse(viewmat);
    mat4 projInv = inverse(proj);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
   vec3 p = gridPlane[gl_VertexID];
   nearPoint = UnprojectPoint(p.x, p.y, 0.0, view, projection).xyz; // unprojecting on the near plane
   farPoint = UnprojectPoint(p.x, p.y, 1.0, view, projection).xyz; // unprojecting on the far plane
   gl_Position = vec4(p, 1.0);
}