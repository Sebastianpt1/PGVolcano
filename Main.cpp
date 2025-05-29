#include <filesystem>
namespace fs = std::filesystem;
//momos
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"

const unsigned int width = 800;
const unsigned int height = 800;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Modelo 3D Cargado", NULL, NULL);
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
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));

	// Ruta al modelo
	std::string parentDir = fs::current_path().parent_path().string();
	std::string modelPath = parentDir + "/OpengGL_Final/Modelos/minecraft-iron-golem/source/iron_golem.fbx";

	// Cargar modelo
	Model model(modelPath.c_str());
	std::cout << "Modelo cargado desde: " << modelPath << std::endl;

	// Iluminación
	glm::vec4 lightColor = glm::vec4(1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 1.0f, 0.5f);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);

		shaderProgram.Activate();
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.r, lightColor.g, lightColor.b, lightColor.a);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		camera.Matrix(shaderProgram, "camMatrix");

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

		model.Draw(shaderProgram);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
