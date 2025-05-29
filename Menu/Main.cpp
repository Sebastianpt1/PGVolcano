#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Opciones del menú
enum MenuOption { START = 0, CREDITS = 1, EXIT = 2 };
MenuOption selectedOption = START;

// Manejo de entradas
void processInput(GLFWwindow* window)
{
	static bool upPressed = false, downPressed = false, enterPressed = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!upPressed) {
			selectedOption = (MenuOption)((selectedOption + 2) % 3);  // arriba
			upPressed = true;
		}
	}
	else upPressed = false;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!downPressed) {
			selectedOption = (MenuOption)((selectedOption + 1) % 3);  // abajo
			downPressed = true;
		}
	}
	else downPressed = false;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		if (!enterPressed) {
			enterPressed = true;
			if (selectedOption == START)
				std::cout << "Iniciar simulación...\n";
			else if (selectedOption == CREDITS)
				std::cout << "Mostrar créditos...\n";
			else if (selectedOption == EXIT)
				glfwSetWindowShouldClose(window, true);
		}
	}
	else enterPressed = false;
}

// Dibujar un "botón"
void drawRectangle(float x, float y, float w, float h, bool selected)
{
	float r = selected ? 1.0f : 0.5f;
	float g = selected ? 1.0f : 0.5f;
	float b = selected ? 0.0f : 0.5f;

	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y - h);
	glVertex2f(x, y - h);
	glEnd();
}

// Dibujar el menú visualmente
void renderMenu()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity(); // importante para coordenadas 2D
	drawRectangle(-0.5f, 0.4f, 1.0f, 0.2f, selectedOption == START);   // Iniciar Simulación
	drawRectangle(-0.5f, 0.1f, 1.0f, 0.2f, selectedOption == CREDITS); // Créditos
	drawRectangle(-0.5f, -0.2f, 1.0f, 0.2f, selectedOption == EXIT);   // Salir
}

int main()
{
	// Inicializar GLFW
	if (!glfwInit()) {
		std::cerr << "Error al iniciar GLFW\n";
		return -1;
	}

	// OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// USAR PERFIL DE COMPATIBILIDAD para funciones legacy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// Crear ventana
	GLFWwindow* window = glfwCreateWindow(800, 600, "Menu Principal", NULL, NULL);
	if (!window) {
		std::cerr << "Error al crear ventana GLFW\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Inicializar GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Error al inicializar GLAD\n";
		return -1;
	}

	// Configurar viewport y proyección 2D
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Fondo gris oscuro

	// Bucle principal
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