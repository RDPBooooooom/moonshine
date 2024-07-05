#version 450

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 normal;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out vec3 vertexPos;
layout (location = 3) out vec3 vertexNormalWorldSpace;

layout(push_constant) uniform Push {
    mat4 modelMatrix; 
    mat4 tangentToWorld;
} push;

void main() {
    gl_Position = ubo.proj * ubo.view * push.modelMatrix * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    vertexPos = (push.modelMatrix * vec4(inPosition, 1.0)).xyz;
    vertexNormalWorldSpace = mat3(push.tangentToWorld) * normal;
}