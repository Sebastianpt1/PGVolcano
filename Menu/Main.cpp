#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

void drawRectangle(float x, float y, float w, float h, MenuOption option)
{
	if (option == START)
		glColor3f(1.0f, 1.0f, 0.0f);  // Amarillo
	else if (option == CREDITS)
		glColor3f(0.0f, 0.8f, 0.0f);  // Verde
	else if (option == EXIT)
		glColor3f(0.9f, 0.0f, 0.0f);  // Rojo

	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y - h);
	glVertex2f(x, y - h);
	glEnd();
}

// Letras
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

	drawRectangle(-0.5f, 0.4f, 1.0f, 0.2f, START);
	drawRectangle(-0.5f, 0.1f, 1.0f, 0.2f, CREDITS);
	drawRectangle(-0.5f, -0.2f, 1.0f, 0.2f, EXIT);

	glColor3f(1.0f, 1.0f, 1.0f); // Letras blancas
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

	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);  // Azul profundo

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
