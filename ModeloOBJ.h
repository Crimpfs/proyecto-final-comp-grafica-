#ifndef MODELO_OBJ_H
#define MODELO_OBJ_H

#include <GL/glew.h>
#include "Math3D.h"
#include <string>
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include "glm.h" // Lector OBJ entregado en el proyecto del profesor.

class ModeloOBJ
{
public:
    ModeloOBJ();
    bool cargar(const std::string& ruta);
    void dibujar(const Shader& shader, const math3d::mat4& modelo) const;
    void liberar();
    bool estaCargado() const;

private:
    struct Parte
    {
        Mesh malla;
        math3d::vec3 color;
    };
    std::vector<Parte> partes;
};

#endif
