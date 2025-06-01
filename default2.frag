#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 ambientColor;       // Luz global suave
uniform sampler2D texture_diffuse1;

void main()
{
    // Aplicar luz ambiente directamente
    vec3 ambient = ambientColor;

    // Obtener color base de la textura
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    // Combinar
    vec3 finalColor = ambient * texColor.rgb;

    FragColor = vec4(finalColor, texColor.a);
}
