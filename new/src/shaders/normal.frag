#version 330 core

in Data {
  vec4 colour;
} fragment;

out vec4 colour;

void main()
{
  colour = fragment.colour;
}