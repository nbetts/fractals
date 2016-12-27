#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 colour;

out Data {
  vec3 position;
  vec3 normal;
  vec3 colour;
} vertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);

  vertex.position = vec3(model * vec4(position, 1.0f));
  vertex.normal = mat3(transpose(inverse(model))) * normal;
  vertex.colour = colour;
}