#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include "Math3D.h"
#include <vector>

struct Vertex
{
    math3d::vec3 Position;
    math3d::vec3 Normal;
    math3d::vec2 TexCoords;
};

class Mesh
{
public:
    GLuint VAO, VBO, EBO;
    GLsizei cantidadIndices;

    Mesh();
    bool crear(const std::vector<Vertex>& vertices,
               const std::vector<unsigned int>& indices);
    void dibujar() const;
    void liberar();
};

#endif
