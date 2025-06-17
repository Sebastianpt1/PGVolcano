#pragma once
#ifndef MENU_H
#define MENU_H

#include <GLFW/glfw3.h>

enum AppState {
    MENU,
    SIMULACION
};

class Menu {
public:
    Menu(int width, int height);
    void Draw(GLFWwindow* window);
    bool IsStartClicked() const;
    bool IsExitClicked() const;

private:
    int width, height;
    bool startClicked, exitClicked;

    void drawRect(float x, float y, float w, float h);
    void setup2D();
};

#endif

