#version 330 core

out vec4 FragColor;

void main()
{
    // Coordenadas del fragmento dentro del punto (0-1)
    vec2 coord = gl_PointCoord * 2.0 - 1.0; // va de -1 a 1
    float dist = dot(coord, coord);

    // Descarta fragmentos fuera del círculo
    if (dist > 1.0)
        discard;

    // Lava anaranjada
    FragColor = vec4(1.0, 0.4, 0.0, 1.0);
}
