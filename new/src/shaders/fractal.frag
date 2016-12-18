#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 outPosition;
in vec3 outNormal;
in vec3 outColor;

out vec4 color;

uniform Material material;
uniform Light light;
uniform vec3 viewPosition;
uniform vec3 lightPosition;

void main()
{
  vec3 normal = normalize(outNormal);

  // color = vec4((normal + vec3(1.0f)) / 2.0f, 1.0f);


  vec3 lightDirection = normalize(lightPosition - outPosition);
  vec3 viewDirection = normalize(viewPosition - outPosition);
  vec3 reflectDirection = reflect(-lightDirection, normal);
  
  float diffuseStrength = max(dot(normal, lightDirection), 0.0f);
  float specularStrength = pow(max(dot(viewDirection, reflectDirection),
                                   0.0f), material.shininess);

  vec3 ambient  = light.ambient * material.ambient;
  vec3 diffuse  = light.diffuse * (material.diffuse * diffuseStrength);
  vec3 specular = light.specular * (material.specular * specularStrength);

  color = vec4(outColor * (ambient + diffuse + specular), 1.0f);
}