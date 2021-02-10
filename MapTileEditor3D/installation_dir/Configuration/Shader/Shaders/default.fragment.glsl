#version 330 core

in vec2 TexCoord;

in vec3 FragPos;
in vec3 Normal;

uniform sampler2D Texture;
uniform bool useTexture = false;

uniform vec3 lightPos = vec3(0.0, 0.0, 0.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(1.0, 1.0, 1.0);

out vec4 FragColor;
out vec3 normal;

void main() {
  // ambient
  float ambientStrength = 0.01;
  vec3 ambient = ambientStrength * lightColor;

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  vec3 result = (ambient + diffuse) * objectColor;
  if (useTexture) {
    FragColor = texture(Texture, TexCoord);
  } 
  else {
    FragColor = vec4(result, 1.0);
  }
}