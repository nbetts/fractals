#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in Data {
  vec4 position;
  vec3 normal;
  vec4 colour;
  vec4 vNormal;
} vertices[];

out Data {
  vec4 position;
  vec3 normal;
  vec4 colour;
  noperspective vec3 wireframeDistance;
} fragment;

void shadeVertex(int index)
{
  gl_Position = gl_in[index].gl_Position;
  fragment.position = vertices[index].position;
  fragment.normal = vertices[index].normal;
  fragment.colour = vertices[index].colour;
  fragment.wireframeDistance = vec3(0.0f);
  fragment.wireframeDistance[index] = 1.0f;

  EmitVertex();
}

void main()
{
  for (int i = 0; i < gl_in.length(); i++) {
    shadeVertex(i);
  }

  EndPrimitive();
}