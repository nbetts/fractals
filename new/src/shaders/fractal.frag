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
  noperspective vec3 wireframeDistance;
} fragment;

out vec4 colour;

uniform Material material;
uniform Light light;
uniform vec4 viewPosition;
uniform bool isWireframeEnabled;
uniform bool areFacesEnabled;

void main()
{
  vec3 lightDirection;
  bool wireframeEnabled = isWireframeEnabled;

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

  vec4 finalColour = fragment.colour * vec4(ambient + diffuse + specular, 1.0f);

  if (!areFacesEnabled) {
    wireframeEnabled = true;
  }

  if (wireframeEnabled) {
    vec3 a3 = smoothstep(vec3(0.0f), fwidth(fragment.wireframeDistance),
                         fragment.wireframeDistance);
    float lineWidth = min(min(a3.x, a3.y), a3.z);

    if (!areFacesEnabled) {
      colour = vec4(mix(finalColour.rgb, vec3(0.0f), lineWidth),
                    1.0f - lineWidth);
    } else {
      colour = vec4(mix(1.0f - finalColour.rgb, finalColour.rgb, lineWidth),
                    finalColour.a);
    }
  } else {
    colour = finalColour;
  }
}