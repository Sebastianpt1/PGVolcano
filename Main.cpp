#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <irrKlang/includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"
#include "Collision.h"
#include "Personaje.h"
#include "Vertices.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lavaY = 400.5f;            // Altura inicial de la lava (misma que usamos en translate)
bool erupcionActiva = false;     // Estado de erupción
float alturaMaxima = 500.0f;     // Altura hasta la que subirá la lava
float velocidadLava = 10.0f;     // Qué tan rápido sube por segundo
struct ParticulaExplosion {
    glm::vec3 posicion;
    glm::vec3 velocidad;
    float vida;
};
const float WALK_SPEED = 7.0f;
const float SPRINT_SPEED = 13.0f;
const float GRAVITY = 9.81f;  // física realista
const float JUMP_VELOCITY = 3.0f;  



std::vector<ParticulaExplosion> particulasLava;
const int MAX_EXPLOSION = 500;
bool explotar = false;


unsigned int width = 1920, height = 1080;
int botonAncho = 460;
int botonAlto = 90;

int btnIniciarX = 180;
int btnIniciarY = 645;

int btnCreditosX = 1280;
int btnCreditosY = 645;

int btnSalirX = 720;
int btnSalirY = 440;

unsigned int skyboxVAO, skyboxVBO, cubemapTexture;
bool mostrarMenu = true;
unsigned int menuTexture;

unsigned int lavaVAO, lavaVBO, lavaEBO, lavaTexture;
std::vector<float> esferaVertices;
std::vector<unsigned int> esferaIndices;

unsigned int esferaVAO, esferaVBO, esferaEBO;
Shader* lavaShader = nullptr;
unsigned int particleVAO, particleVBO;
Shader* particleShader = nullptr;
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

unsigned int texturaCreditos;

bool clicDentroDeBoton(double mouseX, double mouseY, float x, float y, float ancho, float alto, int ventanaAltura) {
    float mouseYInvertido = ventanaAltura - static_cast<float>(mouseY);

    // Compensación pequeña en Y (ajusta según necesites)
    mouseYInvertido -= 30;  // Aumenta si el clic está demasiado bajo

    return mouseX >= x && mouseX <= x + ancho &&
        mouseYInvertido >= y && mouseYInvertido <= y + alto;
}




std::vector<unsigned int> menuFrames;
float frameTimer = 0.0f;
int currentAnimFrame = 0;
float frameDuration = 0.1f; // Cambia de imagen cada 0.1 segundos

void cargarFramesAnimacionMenu(const std::string& carpeta, int total) {
    for (int i = 1; i <= total; ++i) {
        std::string path = carpeta + "/Frame" + std::to_string(i) + ".jpg";
        std::cout << "Cargando frame: " << path << std::endl;
        menuFrames.push_back(cargarTexturaMenu(path));
    }
}

float fadeAlpha = 0.0f;
bool transicionEntrando = true;  // true = fade-in, false = fade-out

enum AppState {
    ESTADO_MENU,
    ESTADO_CREDITOS,
    ESTADO_AYUDA,
    ESTADO_SIMULACION
};

AppState estadoActual = ESTADO_MENU;



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    ISoundEngine* motorSonido = createIrrKlangDevice();
    if (!motorSonido) {
        std::cout << "No se pudo inicializar irrKlang.\n";
        return -1;
    }
    unsigned int width = 845, height = 480;
    GLFWwindow* window = glfwCreateWindow(width, height, "Simulacion Volcan", NULL, NULL);
    if (!window) {
        std::cout << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    float escalaX = (float)width / 1920.0f;
    float escalaY = (float)height / 1080.0f;

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
 
    cargarFramesAnimacionMenu(parentDir + "/PGVolcano/Textures/MenuFrames", 50); 
    texturaCreditos = cargarTexturaMenu(parentDir + "/PGVolcano/Textures/MenuFrames/creditos.png");
    unsigned int texturaAyuda;
    texturaAyuda = cargarTexturaMenu(parentDir + "/PGVolcano/Textures/MenuFrames/ayuda.png");


    ISoundEngine* engine = createIrrKlangDevice();
    ISound* sonidoErupcion = nullptr;
    ISound* musicaMenu = nullptr;
    ISound* musicaSimulacion = nullptr;

    if (!engine)
    {
        std::cout << "No se pudo inicializar irrKlang" << std::endl;
        return -1;
    }

    //PARA LA LAVA
    int numSegments = 50;
    float radius = 120.0f;

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

    int sectores = 20;
    int stacks = 20;
    float radio = 1.0f;

    for (int i = 0; i <= stacks; ++i) {
        float v = float(i) / stacks;
        float phi = v * glm::pi<float>();

        for (int j = 0; j <= sectores; ++j) {
            float u = float(j) / sectores;
            float theta = u * 2.0f * glm::pi<float>();

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            esferaVertices.push_back(x * radio);
            esferaVertices.push_back(y * radio);
            esferaVertices.push_back(z * radio);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectores; ++j) {
            int first = i * (sectores + 1) + j;
            int second = first + sectores + 1;

            esferaIndices.push_back(first);
            esferaIndices.push_back(second);
            esferaIndices.push_back(first + 1);

            esferaIndices.push_back(second);
            esferaIndices.push_back(second + 1);
            esferaIndices.push_back(first + 1);
        }
    }

    glGenVertexArrays(1, &esferaVAO);
    glGenBuffers(1, &esferaVBO);
    glGenBuffers(1, &esferaEBO);

    glBindVertexArray(esferaVAO);

    glBindBuffer(GL_ARRAY_BUFFER, esferaVBO);
    glBufferData(GL_ARRAY_BUFFER, esferaVertices.size() * sizeof(float), &esferaVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esferaEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, esferaIndices.size() * sizeof(unsigned int), &esferaIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_EXPLOSION * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW); // reserva

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    particleShader = new Shader("particle.vert", "particle.frag");

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
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (estadoActual == ESTADO_MENU) {
            frameTimer += deltaTime;
            if (frameTimer >= frameDuration) {
                frameTimer = 0.0f;
                currentAnimFrame = (currentAnimFrame + 1) % menuFrames.size();
            }

            // Transición fade
            if (transicionEntrando) {
                fadeAlpha += deltaTime * 1.5f;
                if (fadeAlpha >= 1.0f) fadeAlpha = 1.0f;
            }
            else {
                fadeAlpha -= deltaTime * 1.5f;
                if (fadeAlpha <= 0.0f) {
                    fadeAlpha = 0.0f;
                    estadoActual = ESTADO_SIMULACION;
                }
            }

            static bool mouseLiberado = true;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseLiberado && transicionEntrando) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                if (clicDentroDeBoton(mouseX, mouseY, btnIniciarX * escalaX, btnIniciarY * escalaY, botonAncho * escalaX, botonAlto * escalaY, height)) {
                    motorSonido->play2D("media/click.mp3", false);

                    std::cout << "Iniciar simulación\n";
                    transicionEntrando = false; // activa fade-out
                }
                else if (clicDentroDeBoton(mouseX, mouseY, btnCreditosX * escalaX, btnCreditosY * escalaY, botonAncho * escalaX, botonAlto * escalaY, height)) {
                    motorSonido->play2D("media/click.mp3", false);

                    std::cout << "Ver créditos\n";
                    estadoActual = ESTADO_CREDITOS;
                    fadeAlpha = 0.0f;
                    transicionEntrando = true;
                }
                else if (clicDentroDeBoton(mouseX, mouseY, btnSalirX * escalaX, btnSalirY * escalaY, botonAncho * escalaX, botonAlto * escalaY, height)) {
                    motorSonido->play2D("media/click.mp3", false);
                    std::cout << "Salir\n";
                    glfwSetWindowShouldClose(window, true);
                }

                mouseLiberado = false;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                mouseLiberado = true;
            }

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            menuShader.Activate();
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, menuFrames[currentAnimFrame]);
            glUniform1i(glGetUniformLocation(menuShader.ID, "menuTexture"), 0);
            glUniform1f(glGetUniformLocation(menuShader.ID, "fadeAlpha"), fadeAlpha); // NUEVO
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glEnable(GL_DEPTH_TEST);
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        else if (estadoActual == ESTADO_CREDITOS) {
            // Fade-in / fade-out
            if (transicionEntrando) {
                fadeAlpha += deltaTime * 1.5f;
                if (fadeAlpha >= 1.0f) fadeAlpha = 1.0f;
            }
            else {
                fadeAlpha -= deltaTime * 1.5f;
                if (fadeAlpha <= 0.0f) {
                    fadeAlpha = 0.0f;
                    estadoActual = ESTADO_MENU;
                    transicionEntrando = true;
                }
            }

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            menuShader.Activate();
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturaCreditos);
            glUniform1i(glGetUniformLocation(menuShader.ID, "menuTexture"), 0);
            glUniform1f(glGetUniformLocation(menuShader.ID, "fadeAlpha"), fadeAlpha);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glEnable(GL_DEPTH_TEST);

            // Detectar botón "Volver" y "Ayuda"
            static bool mouseLiberado = true;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseLiberado && transicionEntrando) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                // Botón Volver (abajo izquierda)
                int volverX = 60;
                int volverY = 50;
                int volverAncho = 230;
                int volverAlto = 80;


                if (clicDentroDeBoton(mouseX, mouseY, volverX * escalaX, volverY * escalaY, volverAncho * escalaX, volverAlto * escalaY, height)) {
                    motorSonido->play2D("media/click.mp3", false);
                    std::cout << "Volviendo al menú\n";
                    transicionEntrando = false;
                }

                // Botón Ayuda (abajo derecha)
                int ayudaAncho = 230;
                int ayudaAlto = 80;
                int ayudaX = 1920 - volverX - ayudaAncho;  // 1920 = ancho de la imagen base
                int ayudaY = volverY; // misma altura


                if (clicDentroDeBoton(mouseX, mouseY, ayudaX * escalaX, ayudaY * escalaY, ayudaAncho * escalaX, ayudaAlto * escalaY, height)) {
                    motorSonido->play2D("media/click.mp3", false);
                    std::cout << "Abriendo ayuda\n";
                    estadoActual = ESTADO_AYUDA;
                    fadeAlpha = 0.0f;
                    transicionEntrando = true;
                }

                mouseLiberado = false;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                mouseLiberado = true;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }
        else if (estadoActual == ESTADO_AYUDA) {
            if (transicionEntrando) {
                fadeAlpha += deltaTime * 1.5f;
                if (fadeAlpha >= 1.0f) fadeAlpha = 1.0f;
            }
            else {
                fadeAlpha -= deltaTime * 1.5f;
                if (fadeAlpha <= 0.0f) {
                    fadeAlpha = 0.0f;
                    estadoActual = ESTADO_CREDITOS;
                    transicionEntrando = true;
                }
            }

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            menuShader.Activate();
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturaAyuda);
            glUniform1i(glGetUniformLocation(menuShader.ID, "menuTexture"), 0);
            glUniform1f(glGetUniformLocation(menuShader.ID, "fadeAlpha"), fadeAlpha);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glEnable(GL_DEPTH_TEST);

            // Botón Volver
            static bool mouseLiberado = true;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseLiberado && transicionEntrando) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                int volverX = 60;
                int volverY = 50;
                int volverAncho = 230;
                int volverAlto = 80;


                if (clicDentroDeBoton(mouseX, mouseY, volverX * escalaX, volverY * escalaY, volverAncho * escalaX, volverAlto * escalaY, height)) {
                    motorSonido->play2D("media/click.mp3", false);
                    std::cout << "Volviendo a créditos desde ayuda\n";
                    transicionEntrando = false; // activa fade-out
                }

                mouseLiberado = false;
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                mouseLiberado = true;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }


        static bool mouseLocked = false;
        static bool ctrlPressed = false;
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !ctrlPressed) {
            ctrlPressed = true;
            mouseLocked = !mouseLocked;
            if (mouseLocked) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                camera.firstClick = true;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
            ctrlPressed = false;
        }


        // Y después ya el resto normal:
        procesarInput(window);
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 10000.0f);


        glClearColor(0.05f, 0.07f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // Detectar tecla X para iniciar erupción
        static bool xPresionada = false;
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !xPresionada) {
            xPresionada = true;
            if (sonidoErupcion && !sonidoErupcion->isFinished()) {
                sonidoErupcion->stop();  // Detiene la reproducción anterior
            }
            sonidoErupcion = engine->play2D("media/erupcion.mp3", false, false, true);

            explotar = true;
            lavaY = 400.5f;         // Reiniciar lava
            erupcionActiva = true;  // Activar subida
            particulasLava.clear();

            for (int i = 0; i < 800; ++i) {
                float angle = glm::radians(static_cast<float>(rand() % 360));
                float elevation = ((rand() % 100) / 100.0f) * glm::radians(60.0f);
                float speed = 70.0f + static_cast<float>(rand() % 60);

                glm::vec3 dir = glm::vec3(
                    cos(angle) * cos(elevation),
                    sin(elevation) * 3.5f,
                    sin(angle) * cos(elevation)
                );

                float r = 7.5f;
                float offsetX = cos(angle) * r;
                float offsetZ = sin(angle) * r;

                ParticulaExplosion p;
                p.posicion = glm::vec3(1400.0f + offsetX, lavaY + 130.0f, 2200.0f + offsetZ);
                p.velocidad = dir * speed;
                p.vida = 4.0f + static_cast<float>(rand() % 100) / 50.0f;

                particulasLava.push_back(p);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) {
            xPresionada = false;
        }

        // ⬇️ BLOQUE QUE SUBE LA LAVA
        if (erupcionActiva) {
            lavaY += velocidadLava * deltaTime;
            if (lavaY >= alturaMaxima) {
                lavaY = alturaMaxima;
                erupcionActiva = false;
            }
        }

        // ⬇️ BLOQUE QUE ACTUALIZA LAS PARTÍCULAS
        if (explotar) {
            for (auto& p : particulasLava) {
                if (p.vida > 0.0f) {
                    p.posicion += p.velocidad * deltaTime;
                    p.velocidad.y -= 20.0f * deltaTime; // gravedad
                    p.vida -= deltaTime;
                }
            }
        }

        lastFrame = currentFrame;


        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");

        glBindVertexArray(esferaVAO);

        for (auto& p : particulasLava) {
            if (p.vida > 0.0f) {
                glm::mat4 modelo = glm::translate(glm::mat4(1.0f), p.posicion);
                modelo = glm::scale(modelo, glm::vec3(20.0f)); // Cambia 20.0f si quieres más grandes
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelo));
                glDrawElements(GL_TRIANGLES, esferaIndices.size(), GL_UNSIGNED_INT, 0);
            }
        }

        glBindVertexArray(0);
        

        procesarInput(window);
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 10000.0f);

        float terrenoY = getAlturaDesdeTerreno(playerPosition, model2.collisionTriangles);
        if (terrenoY != -INFINITY) {
            playerVelocityY -= GRAVITY * deltaTime;
            playerPosition.y += playerVelocityY * deltaTime;

            float offsetSuelo = 0.001f;
            if (playerPosition.y < terrenoY + offsetSuelo) {
                playerPosition.y = terrenoY + offsetSuelo;
                playerVelocityY = 0.0f;
                isOnGround = true;
            }
        }
        //primer click del salto muy rapido
        static bool spacePressed = false;

        glm::vec3 forward = glm::normalize(camera.Orientation);
        glm::vec3 right = glm::normalize(glm::cross(forward, camera.Up));
        float baseSpeed = personaje.modoCreativo ? 45.0f : WALK_SPEED;
        float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? baseSpeed * 2.0f : baseSpeed;


        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) playerPosition += forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) playerPosition -= forward * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) playerPosition -= right * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) playerPosition += right * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround && !spacePressed) {
            playerVelocityY = JUMP_VELOCITY;
            isOnGround = false;
            spacePressed = true;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }
        //tamaño del personaje(ubicacion de la camara en el personaje)
        camera.Position = playerPosition + glm::vec3(0.0f, 3.0f, 0.0f);

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

        glm::mat4 lavaModel = glm::translate(glm::mat4(1.0f), glm::vec3(1400.0f, lavaY, 2200.0f)); // dentro del cráter
        lavaModel = glm::scale(lavaModel, glm::vec3(5.0f));
        glUniformMatrix4fv(glGetUniformLocation(lavaShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(lavaModel));
        glUniform1f(glGetUniformLocation(lavaShader->ID, "time"), glfwGetTime());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lavaTexture);
        glUniform1i(glGetUniformLocation(lavaShader->ID, "lavaTexture"), 0);

        glBindVertexArray(lavaVAO);
        glDrawElements(GL_TRIANGLES, circleIndices.size(), GL_UNSIGNED_INT, 0);

        if (explotar) {
            glDisable(GL_DEPTH_TEST);     // Que no se oculten por el terreno
            glEnable(GL_PROGRAM_POINT_SIZE); // Permite tamaños grandes

            particleShader->Activate();
            camera.Matrix(*particleShader, "camMatrix");

            glBindVertexArray(esferaVAO);  // si estás usando VAO de partículas

            glPointSize(10.0f); // Aumenta si lo deseas

            for (auto& p : particulasLava) {
                if (p.vida > 0.0f) {
                    glm::mat4 modelParticula = glm::translate(glm::mat4(1.0f), p.posicion);
                    modelParticula = glm::scale(modelParticula, glm::vec3(8.0f)); // tamaño esfera
                    glUniformMatrix4fv(glGetUniformLocation(particleShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(modelParticula));
                    glDrawElements(GL_TRIANGLES, esferaIndices.size(), GL_UNSIGNED_INT, 0);
                }
            }

            glEnable(GL_DEPTH_TEST);
        }

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