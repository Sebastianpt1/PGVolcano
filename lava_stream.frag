#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lavaTexture;
uniform float time;

void main() {
    vec2 uv = TexCoords;
    uv.y -= time * 0.5;   // más rápido
uv.x += sin(time * 0.3) * 0.05;  // un leve "temblor"
    vec3 color = texture(lavaTexture, uv).rgb;
    FragColor = vec4(color, 1.0);
}