#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lavaTexture;
uniform float time;

void main() {
    vec2 uv = TexCoords;
    uv.y -= time * 0.2;  // Desplaza la lava
    vec3 color = texture(lavaTexture, uv).rgb;
    FragColor = vec4(color, 1.0);
}