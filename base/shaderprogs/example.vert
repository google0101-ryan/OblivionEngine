#version 450

layout(location = 0) out vec2 TexCoord;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout (binding = 0) uniform UboView 
{
	mat4 projection;
	mat4 view;
} ubo;

layout (push_constant) uniform Model
{
	mat4 model;
} model;

void main() {
    vec4 pos = vec4(inPosition, 1.0);

	gl_Position = ubo.projection * ubo.view * model.model * pos;
	TexCoord = inTexCoord;
}