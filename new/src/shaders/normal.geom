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

const vec4 purple = vec4(0.25f, 0.0f, 1.0f, 1.0f);
const vec4 lightPurple  = vec4(0.5f, 0.5f, 1.0f, 0.0f);
uniform float normalLength;

void main()
{
  for (int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    fragment.colour = purple;
    EmitVertex();

    gl_Position = gl_in[i].gl_Position + (vertices[i].vNormal * normalLength);
    fragment.colour = lightPurple;
    EmitVertex();
    
    EndPrimitive();
  }

}