#version 450

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D albedo;

void main() {
    outColor = texture(albedo, TexCoord);
}