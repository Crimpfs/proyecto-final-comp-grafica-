// =======================================================================
// PROYECTO MIJAEL - PASILLO 3D EN OPENGL MODERNO
// Adaptacion del JLPERALTA30.cbp del profesor (JLPERALTA40.rar).
// GLFW + GLEW + OpenGL 3.3 Core + GLSL + VAO/VBO/EBO.
// Math3D.h sustituye SOLO GLM matematica, ausente en las dependencias.
// =======================================================================
#include <GL/glew.h>      // GLEW debe incluirse ANTES de GLFW.
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>

#include "Shader.h"
#include "Pasillo.h"
#include "Math3D.h"

const int ANCHO_VENTANA = 1280;
const int ALTO_VENTANA = 720;

static int anchoFramebuffer = ANCHO_VENTANA;
static int altoFramebuffer = ALTO_VENTANA;
static math3d::vec3 posicionCamara(0.0f, 1.65f, 9.0f);
static math3d::vec3 frenteCamara(0.0f, 0.0f, -1.0f);
static math3d::vec3 arribaCamara(0.0f, 1.0f, 0.0f);
static const math3d::vec3 arribaMundo(0.0f, 1.0f, 0.0f);

static float yaw = -90.0f;
static float pitch = 0.0f;
static float ultimoMouseX = ANCHO_VENTANA / 2.0f;
static float ultimoMouseY = ALTO_VENTANA / 2.0f;
static bool primerMouse = true;
static float campoVision = 60.0f;
static float deltaTime = 0.0f;
static float tiempoAnterior = 0.0f;

// Estado de las puertas (O para abrir/cerrar).
static float anguloPuertas = 0.85f;
static float objetivoPuertas = 0.85f;
static bool teclaOAnterior = false;

static bool dibujarAlambre = false;
static bool teclaMAnterior = false;

static std::string rutaRecurso(const std::string& nombre)
{
    // Los recursos pueden encontrarse desde la carpeta del proyecto
    // o desde bin/Debug y bin/Release, segun CodeBlocks.
    const std::string rutas[] = {
        nombre,
        "../" + nombre,
        "../../" + nombre,
        "../../../" + nombre
    };
    for (unsigned int i = 0; i < sizeof(rutas)/sizeof(rutas[0]); ++i)
    {
        std::ifstream archivo(rutas[i].c_str(), std::ios::binary);
        if (archivo.good()) return rutas[i];
    }
    return nombre;
}

static void actualizarDireccionCamara()
{
    math3d::vec3 d;
    d.x = std::cos(math3d::radians(yaw)) * std::cos(math3d::radians(pitch));
    d.y = std::sin(math3d::radians(pitch));
    d.z = std::sin(math3d::radians(yaw)) * std::cos(math3d::radians(pitch));
    frenteCamara = math3d::normalize(d);
}

// Equivale a redimensionar() del ejemplo del profesor.
static void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    anchoFramebuffer = (width > 0) ? width : 1;
    altoFramebuffer = (height > 0) ? height : 1;
    glViewport(0, 0, anchoFramebuffer, altoFramebuffer);
}

static void mouse_callback(GLFWwindow*, double xpos, double ypos)
{
    if (primerMouse)
    {
        ultimoMouseX = static_cast<float>(xpos);
        ultimoMouseY = static_cast<float>(ypos);
        primerMouse = false;
    }
    float diferenciaX = static_cast<float>(xpos) - ultimoMouseX;
    float diferenciaY = ultimoMouseY - static_cast<float>(ypos);
    ultimoMouseX = static_cast<float>(xpos);
    ultimoMouseY = static_cast<float>(ypos);

    const float sensibilidad = 0.10f;
    yaw += diferenciaX * sensibilidad;
    pitch += diferenciaY * sensibilidad;
    if (pitch > 85.0f) pitch = 85.0f;
    if (pitch < -85.0f) pitch = -85.0f;
    actualizarDireccionCamara();
}

static void scroll_callback(GLFWwindow*, double, double yoffset)
{
    campoVision -= static_cast<float>(yoffset);
    if (campoVision < 30.0f) campoVision = 30.0f;
    if (campoVision > 85.0f) campoVision = 85.0f;
}

// Equivalente a processInput() del proyecto JLPERALTA40.
static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float velocidad = 4.0f * deltaTime;
    const math3d::vec3 adelante = math3d::normalize(
        math3d::vec3(frenteCamara.x, 0.0f, frenteCamara.z));
    const math3d::vec3 derecha = math3d::normalize(math3d::cross(adelante, arribaMundo));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        posicionCamara += adelante * velocidad;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        posicionCamara -= adelante * velocidad;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        posicionCamara -= derecha * velocidad;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        posicionCamara += derecha * velocidad;

    // Camara libre para inspeccionar la geometria.
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        posicionCamara.y += velocidad;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        posicionCamara.y -= velocidad;

    const bool teclaO = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
    if (teclaO && !teclaOAnterior)
        objetivoPuertas = (objetivoPuertas < 0.5f) ? 0.85f : 0.0f;
    teclaOAnterior = teclaO;

    const bool teclaM = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
    if (teclaM && !teclaMAnterior)
    {
        dibujarAlambre = !dibujarAlambre;
        glPolygonMode(GL_FRONT_AND_BACK, dibujarAlambre ? GL_LINE : GL_FILL);
    }
    teclaMAnterior = teclaM;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        posicionCamara = math3d::vec3(0.0f, 1.65f, 9.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        actualizarDireccionCamara();
    }
}

// Equivalente a animate() del ejemplo del profesor.
static void animate()
{
    const float diferencia = objetivoPuertas - anguloPuertas;
    const float paso = 1.5f * deltaTime;
    if (std::fabs(diferencia) <= paso)
        anguloPuertas = objetivoPuertas;
    else
        anguloPuertas += (diferencia > 0.0f ? paso : -paso);
}

// Equivale a graficar() del proyecto del profesor, sin glBegin/glEnd.
static void graficar(Shader& shader, const Pasillo& entorno)
{
    glClearColor(0.78f, 0.82f, 0.84f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.usar();

    const math3d::mat4 vista = math3d::lookAt(
        posicionCamara, posicionCamara + frenteCamara, arribaCamara);
    const math3d::mat4 proyeccion = math3d::perspective(
        math3d::radians(campoVision),
        static_cast<float>(anchoFramebuffer) / static_cast<float>(altoFramebuffer),
        0.1f, 100.0f);

    shader.setMat4("view", vista);
    shader.setMat4("projection", proyeccion);
    shader.setVec3("lightDirection", math3d::normalize(
        math3d::vec3(-0.35f, -1.0f, -0.25f)));
    entorno.graficar(shader, anguloPuertas);
}

int main()
{
    // 1. Crear ventana y contexto OpenGL con GLFW (como el RAR JLPERALTA40).
    if (!glfwInit())
    {
        std::cerr << "No fue posible inicializar GLFW.\n";
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        ANCHO_VENTANA, ALTO_VENTANA,
        "Proyecto Final CG - Mijael - Pasillo 3D", NULL, NULL);
    if (!window)
    {
        std::cerr << "No fue posible crear el contexto OpenGL 3.3 Core.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // 2. Inicializar GLEW DESPUES de activar el contexto.
    glewExperimental = GL_TRUE;
    const GLenum estado = glewInit();
    if (estado != GLEW_OK)
    {
        std::cerr << "Error GLEW: " << glewGetErrorString(estado) << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glGetError(); // Limpia error espurio de algunas versiones de GLEW/Core.

    std::cout << "Version OpenGL: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Tarjeta grafica: " << glGetString(GL_RENDERER) << '\n';

    // 3. Callbacks de ventana y mouse.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetFramebufferSize(window, &anchoFramebuffer, &altoFramebuffer);
    framebuffer_size_callback(window, anchoFramebuffer, altoFramebuffer);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // Visualizar el interior de habitaciones.

    // 4. Preparar shaders y geometria (VAO, VBO, EBO).
    Shader shader;
    if (!shader.cargar(rutaRecurso("shaders/vertex_shader.glsl"),
                       rutaRecurso("shaders/fragment_shader.glsl")))
    {
        std::cerr << "Comprueba la carpeta shaders.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    Pasillo entorno;
    if (!entorno.init())
    {
        std::cerr << "Error al crear el pasillo.\n";
        shader.liberar();
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::cout << "\n======= ESCENARIO DE MIJAEL =======\n";
    std::cout << "WASD: Moverse | Mouse: Mirar | Rueda: Zoom\n";
    std::cout << "O: Abrir/cerrar puertas | M: Alambre/Solido\n";
    std::cout << "ESPACIO/CTRL: Subir/bajar | R: Reiniciar camara | ESC: Salir\n";
    std::cout << "Medidas del escenario: PROVISIONALES.\n\n";

    // 5. Bucle principal de GLFW: reemplaza a glutMainLoop().
    while (!glfwWindowShouldClose(window))
    {
        const float actual = static_cast<float>(glfwGetTime());
        deltaTime = actual - tiempoAnterior;
        tiempoAnterior = actual;
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        processInput(window);
        animate();
        graficar(shader, entorno);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    entorno.liberar();
    shader.liberar();
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
