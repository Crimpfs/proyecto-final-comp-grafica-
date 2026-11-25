#include "ModeloOBJ.h"
#include <map>
#include <tuple>
#include <iostream>

ModeloOBJ::ModeloOBJ() {}

bool ModeloOBJ::cargar(const std::string& ruta)
{
    liberar();

    // Se reutiliza glmReadOBJ del profesor para leer vertices, caras,
    // normales, coordenadas UV y grupos del archivo Wavefront OBJ.
    std::string rutaMutable = ruta;
    GLMmodel* modelo = glmReadOBJ(&rutaMutable[0]);
    if (!modelo)
    {
        std::cerr << "No se pudo cargar el OBJ: " << ruta << '\n';
        return false;
    }


    // glmDraw() del lector antiguo usa glBegin/glEnd y NO se llama.
    // Solo se aprovechan los datos del modelo para construir buffers modernos.
    glmFacetNormals(modelo);
    glmVertexNormals(modelo, 90.0f);

    GLMgroup* grupo = modelo->groups;
    while (grupo)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::map<std::tuple<GLuint, GLuint, GLuint>, GLuint> mapaIndices;

        for (GLuint i = 0; i < grupo->numtriangles; ++i)
        {
            GLMtriangle* triangulo =
                &modelo->triangles[grupo->triangles[i]];

            for (int k = 0; k < 3; ++k)
            {
                GLuint vi = triangulo->vindices[k];
                GLuint ni = triangulo->nindices[k];
                GLuint ti = (modelo->numtexcoords > 0) ? triangulo->tindices[k] : 0;
                std::tuple<GLuint, GLuint, GLuint> clave(vi, ni, ti);

                std::map<std::tuple<GLuint, GLuint, GLuint>, GLuint>::iterator it =
                    mapaIndices.find(clave);

                if (it == mapaIndices.end())
                {
                    Vertex v;
                    v.Position = math3d::vec3(
                        modelo->vertices[3 * vi + 0],
                        modelo->vertices[3 * vi + 1],
                        modelo->vertices[3 * vi + 2]);

                    if (ni > 0 && ni <= modelo->numnormals)
                    {
                        v.Normal = math3d::vec3(
                            modelo->normals[3 * ni + 0],
                            modelo->normals[3 * ni + 1],
                            modelo->normals[3 * ni + 2]);
                    }
                    else
                    {
                        v.Normal = math3d::vec3(0.0f, 1.0f, 0.0f);
                    }

                    if (ti > 0 && ti <= modelo->numtexcoords)
                    {
                        v.TexCoords = math3d::vec2(
                            modelo->texcoords[2 * ti + 0],
                            modelo->texcoords[2 * ti + 1]);
                    }
                    else
                    {
                        v.TexCoords = math3d::vec2(0.0f, 0.0f);
                    }

                    GLuint nuevoIndice = static_cast<GLuint>(vertices.size());
                    vertices.push_back(v);
                    mapaIndices[clave] = nuevoIndice;
                    indices.push_back(nuevoIndice);
                }
                else
                {
                    indices.push_back(it->second);
                }
            }
        }

        if (!vertices.empty() && !indices.empty())
        {
            Parte parte;
            parte.color = math3d::vec3(0.55f, 0.32f, 0.18f);

            if (modelo->materials && grupo->material < modelo->nummaterials)
            {
                GLMmaterial& material = modelo->materials[grupo->material];
                parte.color = math3d::vec3(
                    material.diffuse[0],
                    material.diffuse[1],
                    material.diffuse[2]);
            }

            parte.malla.crear(vertices, indices);
            partes.push_back(parte);
        }

        grupo = grupo->next;
    }

    glmDelete(modelo);
    return !partes.empty();
}

void ModeloOBJ::dibujar(const Shader& shader, const math3d::mat4& modelo) const
{
    for (std::size_t i = 0; i < partes.size(); ++i)
    {
        shader.setMat4("model", modelo);
        shader.setVec3("objectColor", partes[i].color);
        partes[i].malla.dibujar();
    }
}

void ModeloOBJ::liberar()
{
    for (std::size_t i = 0; i < partes.size(); ++i)
        partes[i].malla.liberar();
    partes.clear();
}

bool ModeloOBJ::estaCargado() const
{
    return !partes.empty();
}
