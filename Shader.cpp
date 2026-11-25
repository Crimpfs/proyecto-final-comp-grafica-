#include "Shader.h"
#include "Math3D.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader() : ID(0) {}

static std::string leerArchivo(const std::string& ruta)
{
    std::ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir el shader: " << ruta << '\n';
        return "";
    }
    std::stringstream contenido;
    contenido << archivo.rdbuf();
    return contenido.str();
}

GLuint Shader::compilar(GLenum tipo, const std::string& codigo)
{
    GLuint shader = glCreateShader(tipo);
    const char* fuente = codigo.c_str();
    glShaderSource(shader, 1, &fuente, NULL);
    glCompileShader(shader);

    GLint correcto = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &correcto);
    if (!correcto)
    {
        char mensaje[2048];
        glGetShaderInfoLog(shader, sizeof(mensaje), NULL, mensaje);
        std::cerr << "Error compilando shader:\n" << mensaje << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::cargar(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string codigoVertex = leerArchivo(vertexPath);
    std::string codigoFragment = leerArchivo(fragmentPath);
    if (codigoVertex.empty() || codigoFragment.empty()) return false;

    GLuint vertex = compilar(GL_VERTEX_SHADER, codigoVertex);
    GLuint fragment = compilar(GL_FRAGMENT_SHADER, codigoFragment);
    if (!vertex || !fragment)
    {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        return false;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    GLint correcto = GL_FALSE;
    glGetProgramiv(ID, GL_LINK_STATUS, &correcto);
    if (!correcto)
    {
        char mensaje[2048];
        glGetProgramInfoLog(ID, sizeof(mensaje), NULL, mensaje);
        std::cerr << "Error enlazando shaders:\n" << mensaje << '\n';
        glDeleteProgram(ID);
        ID = 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return ID != 0;
}

void Shader::usar() const
{
    glUseProgram(ID);
}

void Shader::setMat4(const std::string& nombre, const math3d::mat4& matriz) const
{
    GLint ubicacion = glGetUniformLocation(ID, nombre.c_str());
    glUniformMatrix4fv(ubicacion, 1, GL_FALSE, math3d::value_ptr(matriz));
}

void Shader::setVec3(const std::string& nombre, const math3d::vec3& valor) const
{
    GLint ubicacion = glGetUniformLocation(ID, nombre.c_str());
    glUniform3fv(ubicacion, 1, math3d::value_ptr(valor));
}

void Shader::liberar()
{
    if (ID) glDeleteProgram(ID);
    ID = 0;
}
