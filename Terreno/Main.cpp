#include <filesystem>
namespace fs = std::filesystem;
//momossad
//ola buenas
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"

int main()
{
    // Inicializar GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Obtener monitor principal y modo de video
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    unsigned int width = mode->width;
    unsigned int height = mode->height;

    // Crear ventana en modo pantalla completa
    GLFWwindow* window = glfwCreateWindow(width, height, "Modelos 3D Cargados", primaryMonitor, NULL);
    if (window == NULL)
    {
        std::cout << "No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    std::cout << "Ruta actual: " << fs::current_path() << std::endl;

    gladLoadGL();
    glViewport(0, 0, width, height);

    // Shaders
    Shader shaderProgram("default.vert", "default.frag");

    // Cámara
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 8.0f));

    // Ruta de modelos
    std::string parentDir = fs::current_path().parent_path().string();
    std::string modelPath1 = parentDir + "/Modelos/fumo/scene.gltf";
    std::string modelPath2 = parentDir + "/Modelos/fuji/scene.gltf";

    // Cargar modelos
    Model model1(modelPath1.c_str());
    Model model2(modelPath2.c_str());

    std::cout << "Modelo 1 cargado desde: " << modelPath1 << std::endl;
    std::cout << "Modelo 2 cargado desde: " << modelPath2 << std::endl;

    // Iluminación
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 180.0f, -20.5f);

    glEnable(GL_DEPTH_TEST);

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);

        // Cerrar con Esc
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        camera.updateMatrix(45.0f, 0.1f, 10000.0f);

        shaderProgram.Activate();
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.r, lightColor.g, lightColor.b, lightColor.a);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        camera.Matrix(shaderProgram, "camMatrix");

        // Modelo 1 - al centro
        glm::mat4 modelMatrix1 = glm::mat4(1.0f);
        modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(0.0f, 220.0f, 0.0f));
        modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix1));
        model1.Draw(shaderProgram);

        // Modelo 2 - desplazado a la derecha y escalado
        glm::mat4 modelMatrix2 = glm::mat4(1.0f);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(14.0f, 0.0f, 0.0f));
        modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.5f)); // escala opcional

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix2));
        model2.Draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
