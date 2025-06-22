#version 330 core
out vec4 FragColor;

void main() {
    vec3 glowColor = vec3(1.0, 0.2, 0.0); // rojo-anaranjado intenso
    float intensity = 1.5;
    FragColor = vec4(glowColor * intensity, 1.0);
}