#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include "Math3D.h"
#include <string>

class Shader
{
public:
    GLuint ID;

    Shader();
    bool cargar(const std::string& vertexPath, const std::string& fragmentPath);
    void usar() const;
    void setMat4(const std::string& nombre, const math3d::mat4& matriz) const;
    void setVec3(const std::string& nombre, const math3d::vec3& valor) const;
    void liberar();

private:
    GLuint compilar(GLenum tipo, const std::string& codigo);
};

#endif
