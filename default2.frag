#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 ambientColor;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

uniform sampler2D texture_diffuse1;

void main()
{
    // Ambiente tenue
    vec3 ambient = ambientColor * 0.5;

    // Normal, dirección a la luz
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // Atenuación (luz pierde fuerza con la distancia)
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (0.01 * distance * distance); // puedes ajustar

    vec3 diffuse = diff * lightColor.rgb * attenuation;

    // Especular
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * lightColor.rgb * attenuation;

    // Color de textura
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    // Resultado final
    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = lighting * texColor.rgb;

    FragColor = vec4(finalColor, texColor.a);
}
