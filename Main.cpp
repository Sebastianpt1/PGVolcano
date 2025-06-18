#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"
#include "Collision.h"
#include "Menu.h"  // NUEVO: menú modular
#include "Personaje.h" // personaje controles creativos


// Cubo para representar al jugador
float cubeVertices[] = {
    // posiciones        
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // Inicializar GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    unsigned int width = 1280;
    unsigned int height = 800;

    GLFWwindow* window = glfwCreateWindow(width, height, "Simulacion Volcan", NULL, NULL);
    if (!window) {
        std::cout << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);

    gladLoadGL();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Crear menú
    AppState estadoActual = MENU;
    Menu menu(width, height);
    bool modelosCargados = false;

    // Objetos de simulación
    Shader shaderProgram("default.vert", "default.frag");
    Camera camera(width, height, glm::vec3(0.0f, 280.0f, 8.0f));
    Model model1(""), model2("");
    glm::vec3 playerPosition = glm::vec3(0.0f, 280.0f, 8.0f);
    float playerVelocityY = 0.0f;
    bool isOnGround = false;


    //funcion personaje creativo
    Personaje personaje(&playerPosition, &playerVelocityY, &isOnGround);


    //VAO Y VBO DEL CUBO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::string parentDir = fs::current_path().parent_path().string();
    std::string modelPath1 = parentDir + "/PGVolcano/Models/fumo/scene.gltf";
    std::string modelPath2 = parentDir + "/PGVolcano/Models/fuji/scene.gltf";

    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 1480.0f, -20.5f);
    glm::vec3 ambientColor = glm::vec3(0.12f, 0.15f, 0.25f);

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.07f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (estadoActual == MENU) {
            menu.Draw(window);

            if (menu.IsStartClicked()) {
                estadoActual = SIMULACION;
            }
            if (menu.IsExitClicked()) {
                glfwSetWindowShouldClose(window, true);
            }
        }
        else if (estadoActual == SIMULACION) {
            if (!modelosCargados) {
                model1 = Model(modelPath1.c_str());
                model2 = Model(modelPath2.c_str());

                for (auto& tri : model2.collisionTriangles) {
                    tri.v0 *= 3.5f; tri.v1 *= 3.5f; tri.v2 *= 3.5f;
                }
                glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                for (auto& tri : model2.collisionTriangles) {
                    tri.v0 = glm::vec3(rot * glm::vec4(tri.v0, 1.0f));
                    tri.v1 = glm::vec3(rot * glm::vec4(tri.v1, 1.0f));
                    tri.v2 = glm::vec3(rot * glm::vec4(tri.v2, 1.0f));
                }

                modelosCargados = true;
            }

            camera.Inputs(window);

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            camera.updateMatrix(45.0f, 0.1f, 10000.0f);

            float terrenoY = getAlturaDesdeTerreno(playerPosition, model2.collisionTriangles);
            if (terrenoY != -INFINITY) {
                playerVelocityY -= 13.0f * deltaTime;
                playerPosition.y += playerVelocityY * deltaTime;

                float offsetSuelo = 0.5f;
                if (playerPosition.y < terrenoY + offsetSuelo) {
                    playerPosition.y = terrenoY + offsetSuelo;
                    playerVelocityY = 0.0f;
                    isOnGround = true;
                }
            }




            //controles del personaje/////////////
            glm::vec3 forward = glm::normalize(camera.Orientation);
            glm::vec3 right = glm::normalize(glm::cross(forward, camera.Up));
            float speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 130.0f : 7.5f;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) playerPosition += forward * speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) playerPosition -= forward * speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) playerPosition -= right * speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) playerPosition += right * speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround) {
                playerVelocityY = 9.0f;
                isOnGround = false;
            }

            camera.Position = playerPosition + glm::vec3(0.0f, 2.0f, 0.0f);


            // Cambiar modo con tecla 9
            static bool tecla9Presionada = false;
            if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !tecla9Presionada) {
                personaje.toggleModo();
                tecla9Presionada = true;
            }
            if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
                tecla9Presionada = false;
            }

            // Fisica (gravedad o creativo)
            personaje.updateFisica(deltaTime, model2.collisionTriangles);

            // Movimiento
            personaje.controles(window, camera, deltaTime);

            // Posicionar cámara
            camera.Position = playerPosition + glm::vec3(0.0f, 2.0f, 0.0f);



            shaderProgram.Activate();
            glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.r, lightColor.g, lightColor.b, lightColor.a);
            glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glUniform3f(glGetUniformLocation(shaderProgram.ID, "ambientColor"), ambientColor.x, ambientColor.y, ambientColor.z);
            camera.Matrix(shaderProgram, "camMatrix");

            glm::mat4 modelMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 520.0f, 0.0f));
            modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix1));
            model1.Draw(shaderProgram);
          
            glm::mat4 modelMatrix2 = glm::scale(glm::mat4(1.0f), glm::vec3(3.5f));
            modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
            model2.Draw(shaderProgram);




            //cubo(personaje///////
            glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), playerPosition);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
            glUniform1f(glGetUniformLocation(shaderProgram.ID, "alphaOverride"), 0.3f);
            glDepthMask(GL_FALSE);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(GL_TRUE);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
