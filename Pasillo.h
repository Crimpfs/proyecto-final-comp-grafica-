#ifndef PASILLO_H
#define PASILLO_H

#include "Mesh.h"
#include "Shader.h"
#include "ModeloOBJ.h"

class Pasillo
{
public:
    Pasillo();
    bool init();
    void graficar(const Shader& shader, float anguloPuertas) const;
    void liberar();

private:
    Mesh cubo;
    ModeloOBJ bancaOBJ;
    bool bancaCargada;

    void dibujarCaja(const Shader& shader, const math3d::vec3& posicion,
                     const math3d::vec3& escala, const math3d::vec3& color,
                     float rotacionY = 0.0f) const;
    void dibujarParedLateral(const Shader& shader, int lado,
                             float centroPuerta, float anguloPuerta) const;
    void dibujarPuerta(const Shader& shader, int lado,
                       float centroZ, float angulo) const;
    void dibujarAmbienteIzquierdo(const Shader& shader) const;
    void dibujarAmbienteDerecho(const Shader& shader) const;
    void dibujarSalida(const Shader& shader) const;
    void dibujarArquitectura(const Shader& shader) const;
    void dibujarVentanas(const Shader& shader) const;
    void dibujarMobiliario(const Shader& shader) const;
};

#endif
