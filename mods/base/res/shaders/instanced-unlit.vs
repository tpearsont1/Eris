#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 offset;

out vec2 UV;
uniform mat4 MVP;

void main() {
  gl_Position = MVP * vec4(position + offset, 1);
  UV = vertexUV;
}
