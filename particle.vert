#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform mat4 model;

out vec2 TexCoords;

void main() {
    gl_Position = camMatrix * model * vec4(aPos, 1.0);

    // Genera UV simples a partir de posición — suficiente para esferas
    TexCoords = aPos.xy * 0.5 + 0.5;
}