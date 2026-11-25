#include "Mesh.h"
#include <cstddef>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), cantidadIndices(0) {}

bool Mesh::crear(const std::vector<Vertex>& vertices,
                 const std::vector<unsigned int>& indices)
{
    if (vertices.empty() || indices.empty()) return false;

    cantidadIndices = static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // location 0: posicion XYZ
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // location 1: normal XYZ
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // location 2: coordenadas de textura UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // El EBO queda asociado al VAO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}


void Mesh::dibujar() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, cantidadIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::liberar()
{
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    VAO = VBO = EBO = 0;
    cantidadIndices = 0;
}
