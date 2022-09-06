#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout( push_constant ) uniform Push{
    mat2 matrix;
    vec2 position;
    vec3 color;
} push;

void main() {
    gl_Position = vec4(push.matrix * inPosition, 0.0, 1.0) + vec4(push.position, 0.0, 0.0);
    fragColor = inColor;
}