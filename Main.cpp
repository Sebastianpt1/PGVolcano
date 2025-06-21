#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"
#include "Collision.h"
#include "Personaje.h"
#include "Vertices.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
unsigned int skyboxVAO, skyboxVBO, cubemapTexture;
bool mostrarMenu = true;
unsigned int menuTexture;

unsigned int lavaVAO, lavaVBO, lavaEBO, lavaTexture;
Shader* lavaShader = nullptr;
std::vector<float> circleVertices;
std::vector<unsigned int> circleIndices;



void procesarInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Error cargando cubemap: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int cargarTexturaMenu(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Parámetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Error al cargar la textura del menú: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    unsigned int width = 845, height = 480;
    GLFWwindow* window = glfwCreateWindow(width, height, "Simulacion Volcan", NULL, NULL);
    if (!window) {
        std::cout << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);

    gladLoadGL();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    float quadVertices[] = {
        // posiciones   // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Posición (location = 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TexCoords (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // <-- MUY IMPORTANTE


    Shader shaderProgram("default.vert", "default.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");

    Camera camera(width, height, glm::vec3(0.0f, 280.0f, 8.0f));

    glm::vec3 playerPosition = glm::vec3(0.0f, 280.0f, 8.0f);
    float playerVelocityY = 0.0f;
    bool isOnGround = false;
    Personaje personaje(&playerPosition, &playerVelocityY, &isOnGround);

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * sizeof(float), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::string parentDir = fs::current_path().parent_path().string();
    Model model1((parentDir + "/PGVolcano/Models/fumo/scene.gltf").c_str());
    Model model2((parentDir + "/PGVolcano/Models/fuji/scene.gltf").c_str());
    Shader menuShader("menu.vert", "menu.frag");
    std::string menuPath = parentDir + "/PGVolcano/Textures/MenuFrames/ezgif-frame-001.jpg";
    menuTexture = cargarTexturaMenu(menuPath);

    //PARA LA LAVA
    int numSegments = 50;
    float radius = 150.0f;

    // Centro
    circleVertices.push_back(0.0f); // x
    circleVertices.push_back(0.0f); // y
    circleVertices.push_back(0.0f); // z
    circleVertices.push_back(0.5f); // u
    circleVertices.push_back(0.5f); // v

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * 3.1415926f * i / numSegments;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        circleVertices.push_back(x);
        circleVertices.push_back(0.0f);
        circleVertices.push_back(z);
        circleVertices.push_back((x / (2.0f * radius)) + 0.5f); // u
        circleVertices.push_back((z / (2.0f * radius)) + 0.5f); // v

        if (i > 0) {
            circleIndices.push_back(0);
            circleIndices.push_back(i);
            circleIndices.push_back(i + 1);
        }
    }

    glGenVertexArrays(1, &lavaVAO);
    glGenBuffers(1, &lavaVBO);
    glGenBuffers(1, &lavaEBO);
    glBindVertexArray(lavaVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lavaVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), &circleVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lavaEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, circleIndices.size() * sizeof(unsigned int), &circleIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    lavaShader = new Shader("lava.vert", "lava.frag");

    std::string lavaPath = parentDir + "/PGVolcano/lava.jpg";
    int w, h, ch;
    unsigned char* lavaData = stbi_load(lavaPath.c_str(), &w, &h, &ch, 0);
    glGenTextures(1, &lavaTexture);
    glBindTexture(GL_TEXTURE_2D, lavaTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, lavaData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(lavaData);

    for (auto& tri : model2.collisionTriangles) {
        tri.v0 *= 3.5f; tri.v1 *= 3.5f; tri.v2 *= 3.5f;
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        tri.v0 = glm::vec3(rot * glm::vec4(tri.v0, 1.0f));
        tri.v1 = glm::vec3(rot * glm::vec4(tri.v1, 1.0f));
        tri.v2 = glm::vec3(rot * glm::vec4(tri.v2, 1.0f));
    }

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxVerticesCount * sizeof(float), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    std::vector<std::string> faces = {
        parentDir + "/PGVolcano/Textures/Skybox/Daylight Box_Right.bmp",
        parentDir + "/PGVolcano/Textures/Skybox/Daylight Box_Left.bmp",
        parentDir + "/PGVolcano/Textures/Skybox/Daylight Box_Top.bmp",
        parentDir + "/PGVolcano/Textures/Skybox/Daylight Box_Bottom.bmp",
        parentDir + "/PGVolcano/Textures/Skybox/Daylight Box_Front.bmp",
        parentDir + "/PGVolcano/Textures/Skybox/Daylight Box_Back.bmp"
    };
    cubemapTexture = loadCubemap(faces);

    while (!glfwWindowShouldClose(window)) {

        if (mostrarMenu) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                mostrarMenu = false;
                continue;
            }

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST); // <-- ESTA LÍNEA NUEVA

            menuShader.Activate();
            glBindVertexArray(quadVAO);
            glBindTexture(GL_TEXTURE_2D, menuTexture);
            glUniform1i(glGetUniformLocation(menuShader.ID, "menuTexture"), 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glEnable(GL_DEPTH_TEST); // <-- Reactiva después

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        glClearColor(0.05f, 0.07f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        procesarInput(window);
        camera.Inputs(window);
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

        glm::vec3 forward = glm::normalize(camera.Orientation);
        glm::vec3 right = glm::normalize(glm::cross(forward, camera.Up));
        float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 130.0f : 7.5f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) playerPosition += forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) playerPosition -= forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) playerPosition -= right * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) playerPosition += right * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround) {
            playerVelocityY = 9.0f;
            isOnGround = false;
        }

        camera.Position = playerPosition + glm::vec3(0.0f, 2.0f, 0.0f);

        static bool tecla9Presionada = false;
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !tecla9Presionada) {
            personaje.toggleModo();
            tecla9Presionada = true;
        }
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
            tecla9Presionada = false;
        }

        personaje.updateFisica(deltaTime, model2.collisionTriangles);
        personaje.controles(window, camera, deltaTime);

        shaderProgram.Activate();
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), 0.5f, 1480.0f, -20.5f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "ambientColor"), 0.12f, 0.15f, 0.25f);
        camera.Matrix(shaderProgram, "camMatrix");

        glm::mat4 modelMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 520.0f, 0.0f));
        modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix1));
        model1.Draw(shaderProgram);

        glm::mat4 modelMatrix2 = glm::scale(glm::mat4(1.0f), glm::vec3(3.5f));
        modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
        model2.Draw(shaderProgram);

        lavaShader->Activate();
        camera.Matrix(*lavaShader, "camMatrix");

        glm::mat4 lavaModel = glm::translate(glm::mat4(1.0f), glm::vec3(1400.0f, 400.5f, 2200.0f)); // dentro del cráter
        lavaModel = glm::scale(lavaModel, glm::vec3(5.0f));
        glUniformMatrix4fv(glGetUniformLocation(lavaShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(lavaModel));
        glUniform1f(glGetUniformLocation(lavaShader->ID, "time"), glfwGetTime());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lavaTexture);
        glUniform1i(glGetUniformLocation(lavaShader->ID, "lavaTexture"), 0);

        glBindVertexArray(lavaVAO);
        glDrawElements(GL_TRIANGLES, circleIndices.size(), GL_UNSIGNED_INT, 0);


        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), playerPosition);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "alphaOverride"), 0.3f);
        glDepthMask(GL_FALSE);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.Activate();
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f), camera.Orientation, camera.Up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 10000.0f);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}