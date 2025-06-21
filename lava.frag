#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D lavaTexture;
uniform float time;

void main()
{
    vec2 uv = TexCoords + vec2(time * 0.1, time * 0.05);  // animación simple
    vec3 color = texture(lavaTexture, uv).rgb;
    FragColor = vec4(color, 1.0);
}
