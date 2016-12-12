#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 outPosition;
out vec3 outNormal;
out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);

  outPosition = vec3(model * vec4(position, 1.0f));
  outNormal = mat3(transpose(inverse(model))) * normal;
  outColor = color;
}