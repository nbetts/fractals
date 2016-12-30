#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in Data {
  vec4 position;
  vec3 normal;
  vec4 colour;
  vec4 vNormal;
} vertices[];

out Data {
  vec4 colour;
} fragment;

uniform float normalLength;

void main()
{
  for (int i = 0; i < gl_in.length(); i++) {
    vec4 oppositeColour = 1.0f - vertices[i].colour;

    gl_Position = gl_in[i].gl_Position + (vertices[i].vNormal * normalLength);
    fragment.colour = oppositeColour;
    EmitVertex();

    gl_Position = gl_in[i].gl_Position;
    oppositeColour.w = 1.0f;
    fragment.colour = oppositeColour;
    EmitVertex();

    EndPrimitive();
  }

}