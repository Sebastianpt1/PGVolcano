#include "Menu.h"
#include <GL/gl.h>  // Solo si estás usando OpenGL compatibility (no core profile)

Menu::Menu(int width, int height) : width(width), height(height) {
    startClicked = false;
    exitClicked = false;
}

void Menu::setup2D() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Menu::drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void Menu::Draw(GLFWwindow* window) {
    // Fondo claro para verificar que se está dibujando
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setup2D();

    float botonAncho = 300, botonAlto = 60;
    float xCentro = (width - botonAncho) / 2;
    float yInicio = height / 2 + 50;
    float ySalir = height / 2 - 50;

    // Dibujar botones
    glColor3f(0.7f, 0.2f, 0.2f); drawRect(xCentro, yInicio, botonAncho, botonAlto); // Iniciar
    glColor3f(0.2f, 0.2f, 0.7f); drawRect(xCentro, ySalir, botonAncho, botonAlto);  // Salir

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(height - mouseY); // invertir eje Y

    startClicked = false;
    exitClicked = false;

    if (mousePressed) {
        if (mx >= xCentro && mx <= xCentro + botonAncho) {
            if (my >= yInicio && my <= yInicio + botonAlto) {
                startClicked = true;
            }
            else if (my >= ySalir && my <= ySalir + botonAlto) {
                exitClicked = true;
            }
        }
    }
}

bool Menu::IsStartClicked() const {
    return startClicked;
}

bool Menu::IsExitClicked() const {
    return exitClicked;
}
