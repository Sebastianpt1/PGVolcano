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

// Cubo para representar al jugador
float cubeVertices[] = {
    // posiciones        
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
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

    // Tamaño de la ventana
    unsigned int width = 1280;
    unsigned int height = 800;

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(width, height, "Modelos 3D Cargados", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Centrar ventana
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    int xpos = (mode->width - width) / 2;
    int ypos = (mode->height - height) / 2;
    glfwSetWindowPos(window, xpos, ypos);

    std::cout << "Ruta actual: " << fs::current_path() << std::endl;

    gladLoadGL();
    glViewport(0, 0, width, height);

    // Cámara
    Camera camera(width, height, glm::vec3(0.0f, 280.0f, 8.0f));

    //vao y vbo del cubo(personaje)

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //sistema fisico del personaje
    glm::vec3 playerPosition = glm::vec3(0.0f, 280.0f, 8.0f);
    float playerVelocityY = 0.0f;
    bool isOnGround = false;

    // Shaders
    Shader shaderProgram("default.vert", "default.frag");


 

    // Ruta de modelos
    std::string parentDir = fs::current_path().parent_path().string();
    std::string modelPath1 = parentDir + "/PGVolcano/Models/fumo/scene.gltf";
    std::string modelPath2 = parentDir + "/PGVolcano/Models/fuji/scene.gltf";

    // Cargar modelos
    Model model1(modelPath1.c_str());
    Model model2(modelPath2.c_str());

    // Ajustar los triángulos del modelo 2 al escalado que tiene visualmente
    for (auto& tri : model2.collisionTriangles) {
        tri.v0 *= 3.5f;
        tri.v1 *= 3.5f;
        tri.v2 *= 3.5f;
    }
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    for (auto& tri : model2.collisionTriangles) {
        tri.v0 = glm::vec3(rot * glm::vec4(tri.v0, 1.0f));
        tri.v1 = glm::vec3(rot * glm::vec4(tri.v1, 1.0f));
        tri.v2 = glm::vec3(rot * glm::vec4(tri.v2, 1.0f));
    }


    std::cout << "Modelo 1 cargado desde: " << modelPath1 << std::endl;
    std::cout << "Modelo 2 cargado desde: " << modelPath2 << std::endl;

    // Iluminación
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 1480.0f, -20.5f);

    glEnable(GL_DEPTH_TEST);
    glm::vec3 ambientColor = glm::vec3(0.12f, 0.15f, 0.25f); // Azul claro, suave

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.05f, 0.07f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);

        // Cerrar con Esc
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        float currentFrame = glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        camera.updateMatrix(45.0f, 0.1f, 10000.0f);

        // Calcular altura del terreno bajo el jugador
        float terrenoY = getAlturaDesdeTerreno(playerPosition, model2.collisionTriangles);
        if (terrenoY == -INFINITY) {
            std::cout << "❌ No se detectó colisión bajo el jugador en ("
                << playerPosition.x << ", " << playerPosition.z << ")" << std::endl;
        }
        else {
            std::cout << "✅ Altura del terreno detectada: " << terrenoY << std::endl;
        }


        playerVelocityY -= 12.0f * deltaTime;
        playerPosition.y += playerVelocityY * deltaTime;

        const float alturaCubo = 1.0f; // scale.y
        const float offsetSuelo = alturaCubo / 2.0f;

        // Si cae sobre el terreno

        if (playerPosition.y < terrenoY + offsetSuelo) {
            playerPosition.y = terrenoY + offsetSuelo;
            playerVelocityY = 0.0f;
            isOnGround = true;
        }


        // Movimiento
        glm::vec3 forward = glm::normalize(camera.Orientation);
        glm::vec3 right = glm::normalize(glm::cross(forward, camera.Up));
        float speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 13.0f : 7.5f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            playerPosition += forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            playerPosition -= forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            playerPosition -= right * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            playerPosition += right * speed * deltaTime;

        // Salto
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround) {
            playerVelocityY = 9.0f;
            isOnGround = false;
        }

        // Actualizar posición de la cámara para que siga al jugador
        camera.Position = playerPosition + glm::vec3(0.0f, 0.7f, 0.0f);



        shaderProgram.Activate();
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.r, lightColor.g, lightColor.b, lightColor.a);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "ambientColor"), ambientColor.x, ambientColor.y, ambientColor.z);

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "ambientColor"),ambientColor.x, ambientColor.y, ambientColor.z);


        camera.Matrix(shaderProgram, "camMatrix");

        // Modelo 1 - fumo
        glm::mat4 modelMatrix1 = glm::mat4(1.0f);
        modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(60.0f, 520.0f, 0.0f));
        modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix1));
        model1.Draw(shaderProgram);

        // Modelo 2 - fuji
        glm::mat4 modelMatrix2 = glm::mat4(1.0f);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.0f, 0.0f, 0.0f));
        modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(3.5f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
        model2.Draw(shaderProgram);

        // Dibuja el cubo del jugador
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, playerPosition);
        cubeModel = glm::scale(cubeModel, glm::vec3(0.0f, 0.0f, 0.0f)); // altura personaje

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "alphaOverride"), 0.3f); // 0.0 = invisible, 1.0 = opaco

        glDepthMask(GL_FALSE);


        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}