#ifndef LAVASTREAM_H
#define LAVASTREAM_H

#include <glm/glm.hpp>
#include <vector>

class LavaStream {
public:
    LavaStream(glm::vec3 inicio, glm::vec3 direccion, float largo, float ancho, int segmentos);

    void update(float tiempo);
    void draw(unsigned int shaderID);
    void activar();
    void reset();
    
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 origen;
    glm::vec3 dir;
    float longitud;
    float ancho;
    float alturaInferior = 50.0f; // Espesor del flujo de lava
    float alturaSuperior = 20.0f;
    float progreso = 0.0f;          // 0.0 (no se ha movido) a 1.0 (completo)
    float velocidadDescenso = 0.05f; // velocidad de avance por segundo
    bool activo = false;            // se activa cuando la erupción llega al máximo
    float timeAcumulado = 0.0f;

    int segmentos;

    unsigned int VAO = 0, VBO = 0, EBO = 0; // <-- INICIALIZA en 0

    void generarMalla();
};

#endif