#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in Data {
  vec3 position;
  vec3 normal;
  vec3 colour;
} vData[];

out Data {
  vec3 position;
  vec3 normal;
  vec3 colour;
} gData;

const float MAGNITUDE = 0.2f;
// uniform float magnitude;

void generateNormalLine(int index)
{
  gl_Position = gl_in[index].gl_Position;

  gData.position = vData[index].position;
  gData.normal = vData[index].normal;
  gData.colour = vData[index].colour;
  EmitVertex();

  gl_Position = gl_in[index].gl_Position +
                vec4(vData[index].normal, 0.0f) * MAGNITUDE;

  gData.position = vData[index].position;
  gData.normal = vData[index].normal;
  gData.colour = vData[index].colour;
  EmitVertex();

  EndPrimitive();
}

void main()
{
  for (int i = 0; i < 3; i++) {
    generateNormalLine(i);
  }
}