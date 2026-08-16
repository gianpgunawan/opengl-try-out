#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 mvc;

out vec3 color;
out vec2 texCoord;

void main() {
   vec4 clip = mvc * aPos;
   gl_Position = clip;
   texCoord = aTexCoord;
};
