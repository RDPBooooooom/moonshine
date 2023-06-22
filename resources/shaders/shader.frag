#version 450

#define NR_POINT_LIGHTS 6

struct Material {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


layout (binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform fragUBO {
    Material material;
    vec3 viewPos;
    DirLight dirLight;
};

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 vertexPos;
layout (location = 3) in vec3 vertexNormalWorldSpace;

layout (location = 0) out vec4 outColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    //outColor = texture(texSampler, fragTexCoord);

    vec3 baseColor = texture(texSampler, fragTexCoord).rgb;
    // properties
    vec3 norm = normalize(vertexNormalWorldSpace);
    vec3 viewDir = normalize(viewPos - vertexPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //result += CalcPointLight(pointLights[i], norm, vertexPos, viewDir);

    outColor = vec4(baseColor * result, 1.0);
}

vec3 ambientReflection(float factor, vec3 lightColor) {
    return lightColor * factor;
}

vec3 diffuseReflection(float factor, vec3 lightColor, vec3 lightDirection, vec3 normal) {
    return lightColor * factor * clamp(dot(lightDirection, normal), 0.0001f, 1.0f);
}

vec3 specularReflectionBlinn(float factor, vec3 lightColor, float hardness, vec3 h, vec3 normal) {

    return lightColor * pow(clamp(dot(normal, h), 0.0001f, 1.0f), hardness) * factor;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    // specular shading
    vec3 h = normalize(viewDir + -lightDir);
    // combine results
    vec3 ambient = ambientReflection(material.ambient, light.ambient);
    vec3 diffuse = diffuseReflection(material.diffuse, light.diffuse, lightDir, normal);
    vec3 specular = specularReflectionBlinn(material.specular, light.specular, material.shininess, h, normal);
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (light.ambient == vec3(0, 0, 0) && light.diffuse == vec3(0, 0, 0) && light.specular == vec3(0, 0, 0)) return vec3(0, 0, 0);

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 h = normalize(viewDir + -lightDir);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = ambientReflection(material.ambient, light.ambient);
    vec3 diffuse = diffuseReflection(material.diffuse, light.diffuse, lightDir, normal);
    vec3 specular = specularReflectionBlinn(material.specular, light.specular, material.shininess, h, normal);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}