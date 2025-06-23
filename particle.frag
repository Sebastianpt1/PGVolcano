#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D lavaTexture;

void main() {
    vec3 glowColor = vec3(1.0, 0.2, 0.0); // rojo-anaranjado intenso
    float intensity = 1.5;

    vec4 texColor = texture(lavaTexture, TexCoords);

    // Combinar textura con el color de brillo
    vec3 finalColor = texColor.rgb * glowColor * intensity;

    // Puedes descartar píxeles muy oscuros si quieres forma más definida
    if (length(finalColor) < 0.05)
        discard;

    FragColor = vec4(finalColor, texColor.a); // Usa alpha original
}