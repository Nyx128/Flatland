#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout( push_constant ) uniform Push{
    mat2 matrix;
    vec2 position;
    vec3 color;
} push;

layout(binding = 0) uniform GlobalUbo{
    vec4 color;
}globalUbo;

void main() {
    gl_Position = vec4(push.matrix * inPosition, 0.0, 1.0) + vec4(push.position, 0.0, 0.0);
    fragColor = globalUbo.color;
    fragTexCoord = texCoord;
}