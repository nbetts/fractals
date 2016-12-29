#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in Data {
  vec4 position;
  vec3 normal;
  vec4 colour;
} fragment;

out vec4 colour;

uniform Material material;
uniform Light light;
uniform vec4 viewPosition;

void main()
{
  vec3 lightDirection;

  if (light.position.w == 0.0f) {
    lightDirection = vec3(normalize(light.position));
  } else {
    lightDirection = vec3(normalize(light.position - fragment.position));
  }
  
  vec3 viewDirection = vec3(normalize(viewPosition - fragment.position));
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);
  
  float diffuseStrength = max(dot(fragment.normal, lightDirection), 0.0f);
  float specularStrength = pow(max(dot(fragment.normal, halfwayDirection),
                                   0.0f), material.shininess);

  vec3 ambient  = light.ambient * material.ambient;
  vec3 diffuse  = light.diffuse * (material.diffuse * diffuseStrength);
  vec3 specular = light.specular * (material.specular * specularStrength);

  colour = fragment.colour * vec4(ambient + diffuse + specular, 1.0f);
}