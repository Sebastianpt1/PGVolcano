#pragma once
#ifndef PERSONAJE_CLASS_H
#define PERSONAJE_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

class Personaje
{
public:
    glm::vec3* playerPosition;
    float* playerVelocityY;
    bool* isOnGround;
    bool modoCreativo = false;

    // Constructor
    Personaje(glm::vec3* pos, float* velY, bool* ground) {
        playerPosition = pos;
        playerVelocityY = velY;
        isOnGround = ground;
    }

    // Cambiar modo
    void toggleModo() {
        modoCreativo = !modoCreativo;
        std::cout << "Modo creativo: " << (modoCreativo ? "ACTIVADO" : "DESACTIVADO") << std::endl;
    }

    // Actualizar física (gravedad o libre)
    void updateFisica(float deltaTime, const std::vector<Triangle>& terreno) {
        if (!modoCreativo) {
            float terrenoY = getAlturaDesdeTerreno(*playerPosition, terreno);
            if (terrenoY != -INFINITY) {
                *playerVelocityY -= 13.0f * deltaTime;
                playerPosition->y += *playerVelocityY * deltaTime;

                float offsetSuelo = 0.5f;
                if (playerPosition->y < terrenoY + offsetSuelo) {
                    playerPosition->y = terrenoY + offsetSuelo;
                    *playerVelocityY = 0.0f;
                    *isOnGround = true;
                }
            }
        }
        else {
            *playerVelocityY = 0.0f;
            *isOnGround = true;
        }
    }

    // Controles
    void controles(GLFWwindow* window, Camera& camera, float deltaTime) {
        glm::vec3 forward = glm::normalize(camera.Orientation);
        glm::vec3 right = glm::normalize(glm::cross(forward, camera.Up));
        float speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 13.0f : 7.5f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) *playerPosition += forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) *playerPosition -= forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) *playerPosition -= right * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) *playerPosition += right * speed * deltaTime;

        if (modoCreativo) {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) playerPosition->y += speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) playerPosition->y -= speed * deltaTime;
        }
        else {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && *isOnGround) {
                *playerVelocityY = 9.0f;
                *isOnGround = false;
            }
        }
    }
};

#endif
