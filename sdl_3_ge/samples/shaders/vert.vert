#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
    mat4 mvp;
} pushData;

void main() {
    fragColor = inColor;
    gl_Position = pushData.mvp * vec4(inPos, 1.0);
}