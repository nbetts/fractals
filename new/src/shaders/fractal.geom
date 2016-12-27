#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in Data {
  vec3 position;
  vec3 normal;
  vec3 colour;
} vertices[];

out Data {
  vec3 position;
  vec3 normal;
  vec3 colour;
} fragment;

const float MAGNITUDE = 0.2f;
// uniform float magnitude;

void generateNormalLine(int index)
{
  gl_Position = gl_in[index].gl_Position;

  fragment.position = vertices[index].position;
  fragment.normal = vertices[index].normal;
  fragment.colour = vertices[index].colour;
  EmitVertex();

  gl_Position = gl_in[index].gl_Position +
                vec4(vertices[index].normal, 0.0f) * MAGNITUDE;

  fragment.position = vertices[index].position;
  fragment.normal = vertices[index].normal;
  fragment.colour = vertices[index].colour;
  EmitVertex();

  EndPrimitive();
}

void main()
{
  for (int i = 0; i < gl_in.length(); i++) {
    generateNormalLine(i);
  }
}