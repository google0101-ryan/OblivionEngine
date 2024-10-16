#version 450

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

<<<<<<< HEAD
layout (binding = 1) uniform sampler2D albedo;
=======
layout (binding = 1, set = 0) uniform sampler2D albedo;
>>>>>>> 404aac2112e18fa1657331fb0aab7f36e30d0fdc

void main() {
    outColor = texture(albedo, TexCoord);
}