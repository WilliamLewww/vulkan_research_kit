#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) flat in int inMaterialPropertiesIndex;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform Camera {
  mat4 viewMatrix;
  mat4 projectionMatrix;
}
camera;

layout(set = 0, binding = 1) uniform Scene { uint lightCount; }
scene;

layout(set = 0, binding = 2) uniform Light {
  vec4 position;
  vec4 direction;
  int type;
}
lights[16];

layout(set = 0, binding = 3) buffer MaterialPropertiesBuffer {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float transmittance[4];
  float emission[4];
  float shininess;
  float ior;
  float dissolve;
  int illum;

  int ambientTextureIndex;
  int diffuseTextureIndex;
  int specularTextureIndex;
}
materialPropertiesBuffer[32];

layout(set = 0, binding = 4) uniform sampler imageSampler;

layout(set = 0, binding = 5) uniform texture2D textures[32];

void main() {
  vec3 color = vec3(0.0, 0.0, 0.0);

  if (materialPropertiesBuffer[inMaterialPropertiesIndex].diffuseTextureIndex ==
      -1) {
    color =
        vec3(materialPropertiesBuffer[inMaterialPropertiesIndex].diffuse[0],
             materialPropertiesBuffer[inMaterialPropertiesIndex].diffuse[1],
             materialPropertiesBuffer[inMaterialPropertiesIndex].diffuse[2]);
  } else {
    color =
        texture(sampler2D(
                    textures[materialPropertiesBuffer[inMaterialPropertiesIndex]
                                 .diffuseTextureIndex],
                    imageSampler),
                inTextureCoordinate)
            .xyz;
  }

  for (uint x = 0; x < scene.lightCount; x++) {
    if (lights[x].type == 1) {
      vec4 surfaceToLight =
          normalize(lights[x].position - vec4(inPosition, 1.0));
      color = color * dot(vec4(inNormal, 0.0), surfaceToLight);
    }
  }

  outColor = vec4(color, 1.0);
}
