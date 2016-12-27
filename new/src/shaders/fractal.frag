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
  vec3 position;
  vec3 normal;
  vec3 colour;
} fragment;

out vec4 colour;

uniform Material material;
uniform Light light;
uniform vec4 viewPosition;

void main()
{
  vec3 normal = normalize(fragment.normal);
  vec4 position = vec4(fragment.position, 1.0f);
  vec3 lightDirection;

  if (light.position.w == 0.0f) {
    lightDirection = vec3(normalize(light.position));
  } else {
    lightDirection = vec3(normalize(light.position - position));
  }
  
  vec3 viewDirection = vec3(normalize(viewPosition - position));
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);
  
  float diffuseStrength = max(dot(normal, lightDirection), 0.0f);
  float specularStrength = pow(max(dot(normal, halfwayDirection),
                                   0.0f), material.shininess);

  vec3 ambient  = light.ambient * material.ambient;
  vec3 diffuse  = light.diffuse * (material.diffuse * diffuseStrength);
  vec3 specular = light.specular * (material.specular * specularStrength);

  colour = vec4(fragment.colour * (ambient + diffuse + specular), 1.0f);
}