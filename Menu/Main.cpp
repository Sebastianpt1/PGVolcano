#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm> // necesario para std::min

enum MenuOption { START = 0, CREDITS = 1, EXIT = 2 };
MenuOption selectedOption = START;

void processInput(GLFWwindow* window)
{
	static bool upPressed = false, downPressed = false, enterPressed = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !upPressed) {
		selectedOption = (MenuOption)((selectedOption + 2) % 3);
		upPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		upPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !downPressed) {
		selectedOption = (MenuOption)((selectedOption + 1) % 3);
		downPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
		downPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !enterPressed) {
		enterPressed = true;
		if (selectedOption == START)
			std::cout << "Iniciar simulación...\n";
		else if (selectedOption == CREDITS)
			std::cout << "Mostrar créditos...\n";
		else if (selectedOption == EXIT)
			glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
		enterPressed = false;
	}
}

void drawButton(float x, float y, float w, float h, MenuOption option)
{
	float time = glfwGetTime();
	float pulse = 0.7f + 0.3f * std::sin(time * 6.0f); // brillo dinámico

	// Color base por botón
	float r = 0.5f, g = 0.5f, b = 0.5f;
	if (option == START) {
		r = 1.0f; g = 1.0f; b = 0.0f; // amarillo
	}
	else if (option == CREDITS) {
		r = 0.0f; g = 0.8f; b = 0.0f; // verde
	}
	else if (option == EXIT) {
		r = 0.9f; g = 0.0f; b = 0.0f; // rojo
	}

	// Brillo si está seleccionado
	if (selectedOption == option) {
		r = std::min(r * pulse + 0.2f, 1.0f);
		g = std::min(g * pulse + 0.2f, 1.0f);
		b = std::min(b * pulse + 0.2f, 1.0f);
	}

	// Botón (relleno)
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y - h);
	glVertex2f(x, y - h);
	glEnd();

	// Borde del botón
	glLineWidth(3.0f);
	glColor3f(1.0f, 1.0f, 1.0f); // blanco
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y - h);
	glVertex2f(x, y - h);
	glEnd();
}

// Letras simuladas
void drawI(float x, float y, float size)
{
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + size * 0.1f, y);
	glVertex2f(x + size * 0.1f, y - size);
	glVertex2f(x, y - size);
	glEnd();
}

void drawC(float x, float y, float size)
{
	float thick = size * 0.1f;
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + thick, y);
	glVertex2f(x + thick, y - size);
	glVertex2f(x, y - size);

	glVertex2f(x, y);
	glVertex2f(x + size * 0.6f, y);
	glVertex2f(x + size * 0.6f, y - thick);
	glVertex2f(x, y - thick);

	glVertex2f(x, y - size);
	glVertex2f(x + size * 0.6f, y - size);
	glVertex2f(x + size * 0.6f, y - size + thick);
	glVertex2f(x, y - size + thick);
	glEnd();
}

void drawS(float x, float y, float size)
{
	float thick = size * 0.1f;
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + size * 0.6f, y);
	glVertex2f(x + size * 0.6f, y - thick);
	glVertex2f(x, y - thick);

	glVertex2f(x, y - size / 2.0f - thick / 2);
	glVertex2f(x + size * 0.6f, y - size / 2.0f - thick / 2);
	glVertex2f(x + size * 0.6f, y - size / 2.0f + thick / 2);
	glVertex2f(x, y - size / 2.0f + thick / 2);

	glVertex2f(x, y - size);
	glVertex2f(x + size * 0.6f, y - size);
	glVertex2f(x + size * 0.6f, y - size + thick);
	glVertex2f(x, y - size + thick);

	glVertex2f(x, y - thick);
	glVertex2f(x + thick, y - thick);
	glVertex2f(x + thick, y - size / 2.0f + thick / 2);
	glVertex2f(x, y - size / 2.0f + thick / 2);

	glVertex2f(x + size * 0.6f - thick, y - size / 2.0f - thick / 2);
	glVertex2f(x + size * 0.6f, y - size / 2.0f - thick / 2);
	glVertex2f(x + size * 0.6f, y - size + thick);
	glVertex2f(x + size * 0.6f - thick, y - size + thick);
	glEnd();
}

void renderMenu()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	drawButton(-0.5f, 0.4f, 1.0f, 0.2f, START);
	drawButton(-0.5f, 0.1f, 1.0f, 0.2f, CREDITS);
	drawButton(-0.5f, -0.2f, 1.0f, 0.2f, EXIT);

	glColor3f(1.0f, 1.0f, 1.0f); // letras blancas
	drawI(-0.05f, 0.3f, 0.15f);
	drawC(-0.05f, 0.0f, 0.15f);
	drawS(-0.05f, -0.3f, 0.15f);
}

int main()
{
	if (!glfwInit()) {
		std::cerr << "Error al iniciar GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Menu Principal", NULL, NULL);
	if (!window) {
		std::cerr << "Error al crear ventana GLFW\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Error al inicializar GLAD\n";
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f, 0.2f, 0.4f, 1.0f); // fondo azul

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		renderMenu();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
