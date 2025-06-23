#include "LavaStream.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

LavaStream::LavaStream(glm::vec3 inicio, glm::vec3 direccion, float largo, float ancho, int segmentos)
    : origen(inicio), dir(glm::normalize(direccion)), longitud(largo), ancho(ancho), segmentos(segmentos)
{
    activo = false;
    progreso = 0.0f;
    VAO = VBO = EBO = 0;
}

void LavaStream::generarMalla() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);

    vertices.clear();
    indices.clear();

    glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0, 1, 0)));

    for (int i = 0; i <= segmentos; ++i) {
        float t = (float)i / segmentos;
        float realT = glm::min(t, progreso);

        glm::vec3 center = origen + dir * (realT)*longitud;
        center.y -= t * 30.0f;

        glm::vec3 left = center - right * ancho * 0.5f;
        glm::vec3 rightV = center + right * ancho * 0.5f;

        // Superiores
        vertices.push_back(left.x); vertices.push_back(left.y + alturaSuperior); vertices.push_back(left.z); vertices.push_back(0.0f); vertices.push_back(t);
        vertices.push_back(rightV.x); vertices.push_back(rightV.y + alturaSuperior); vertices.push_back(rightV.z); vertices.push_back(1.0f); vertices.push_back(t);
        // Inferiores
        vertices.push_back(left.x); vertices.push_back(left.y - alturaInferior); vertices.push_back(left.z); vertices.push_back(0.0f); vertices.push_back(t);
        vertices.push_back(rightV.x); vertices.push_back(rightV.y - alturaInferior); vertices.push_back(rightV.z); vertices.push_back(1.0f); vertices.push_back(t);
    }

    // ======== CARAS ENTRE SEGMENTOS =========
    for (int i = 0; i < segmentos; ++i) {
        int base = i * 4;

        // Cara superior
        indices.push_back(base);     indices.push_back(base + 1); indices.push_back(base + 4);
        indices.push_back(base + 1); indices.push_back(base + 5); indices.push_back(base + 4);

        // Cara inferior
        indices.push_back(base + 2); indices.push_back(base + 6); indices.push_back(base + 3);
        indices.push_back(base + 3); indices.push_back(base + 6); indices.push_back(base + 7);

        // Cara lateral izquierda
        indices.push_back(base);     indices.push_back(base + 4); indices.push_back(base + 2);
        indices.push_back(base + 2); indices.push_back(base + 4); indices.push_back(base + 6);

        // Cara lateral derecha
        indices.push_back(base + 1); indices.push_back(base + 3); indices.push_back(base + 5);
        indices.push_back(base + 3); indices.push_back(base + 7); indices.push_back(base + 5);
    }

    // ======== TAPAS =========
    int i0 = 0;
    indices.push_back(i0);     indices.push_back(i0 + 2); indices.push_back(i0 + 1);
    indices.push_back(i0 + 1); indices.push_back(i0 + 2); indices.push_back(i0 + 3);

    int ilast = segmentos * 4;
    indices.push_back(ilast);     indices.push_back(ilast + 1); indices.push_back(ilast + 2);
    indices.push_back(ilast + 1); indices.push_back(ilast + 3); indices.push_back(ilast + 2);

    // Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}


void LavaStream::update(float deltaTime) {
    if (activo && progreso < 1.0f) {
        progreso += velocidadDescenso * deltaTime;
        if (progreso > 1.0f) progreso = 1.0f;
        generarMalla();
    }
}

void LavaStream::draw(unsigned int shaderID) {
    if (!activo || progreso == 0.0f) return;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void LavaStream::activar() {
    activo = true;
    progreso = 0.0f;
    generarMalla();
}

void LavaStream::reset() {
    activo = false;
    progreso = 0.0f;
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    VAO = VBO = EBO = 0;
}