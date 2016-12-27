#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 colour;

out Data {
  vec3 position;
  vec3 normal;
  vec3 colour;
} vData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);

  vData.position = vec3(model * vec4(position, 1.0f));
  vData.normal = mat3(transpose(inverse(model))) * normal;
  vData.colour = colour;
}