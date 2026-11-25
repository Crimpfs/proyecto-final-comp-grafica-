#include "Pasillo.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "Math3D.h"

namespace
{
    const float ANCHO = 6.0f;
    const float LARGO = 24.0f;
    const float ALTO = 3.2f;
    const float ESPESOR = 0.18f;
    const float MEDIO_LARGO = LARGO / 2.0f;

    Mesh crearCubo()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Cada cara tiene cuatro vertices para conservar normales planas.
        const math3d::vec3 caras[6][4] = {
            { {-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f} },
            { { 0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f} },
            { { 0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f, 0.5f} },
            { {-0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f,-0.5f} },
            { {-0.5f, 0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, { 0.5f, 0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f} },
            { {-0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f, 0.5f}, {-0.5f,-0.5f, 0.5f} }
        };
        const math3d::vec3 normales[6] = {
            {0,0,1}, {0,0,-1}, {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}
        };
        const math3d::vec2 uv[4] = {
            {0,0}, {1,0}, {1,1}, {0,1}
        };

        for (int cara = 0; cara < 6; ++cara)
        {
            unsigned int base = static_cast<unsigned int>(vertices.size());
            for (int j = 0; j < 4; ++j)
            {
                Vertex v;
                v.Position = caras[cara][j];
                v.Normal = normales[cara];
                v.TexCoords = uv[j];
                vertices.push_back(v);
            }
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
            indices.push_back(base + 0);
        }

        Mesh malla;
        malla.crear(vertices, indices);
        return malla;
    }
}

Pasillo::Pasillo() : bancaCargada(false) {}

bool Pasillo::init()
{
    cubo = crearCubo();
    // OBJ de una banca incluido en la carpeta modelos.
    // Se busca desde raiz o desde bin/Debug y se convierte a VAO/VBO/EBO.
    const std::string rutas[] = {"modelos/banca.obj", "../modelos/banca.obj", "../../modelos/banca.obj", "../../../modelos/banca.obj"};
    std::string rutaBanca = rutas[0];
    for (std::size_t i = 0; i < sizeof(rutas)/sizeof(rutas[0]); ++i)
    {
        std::ifstream archivo(rutas[i].c_str(), std::ios::binary);
        if (archivo.good()) { rutaBanca = rutas[i]; break; }
    }
    bancaCargada = bancaOBJ.cargar(rutaBanca);
    if (!bancaCargada)
        std::cout << "Aviso: no se cargo modelos/banca.obj; se dibujara una banca geometrica.\n";
    return cubo.VAO != 0;
}

void Pasillo::dibujarCaja(const Shader& shader, const math3d::vec3& posicion,
                          const math3d::vec3& escala, const math3d::vec3& color,
                          float rotacionY) const
{
    math3d::mat4 modelo(1.0f);
    modelo = math3d::translate(modelo, posicion);
    if (rotacionY != 0.0f)
        modelo = math3d::rotate(modelo, rotacionY, math3d::vec3(0,1,0));
    modelo = math3d::scale(modelo, escala);

    shader.setMat4("model", modelo);
    shader.setVec3("objectColor", color);
    cubo.dibujar();
}

void Pasillo::dibujarPuerta(const Shader& shader, int lado,
                            float centroZ, float angulo) const
{
    const float anchoPuerta = 1.40f;
    const float altoPuerta = 2.20f;
    const float x = lado * (ANCHO / 2.0f);
    const float inicioZ = centroZ - anchoPuerta / 2.0f;
    const float espesorHoja = 0.075f;
    const math3d::vec3 madera(0.28f, 0.105f, 0.045f);
    const math3d::vec3 maderaClara(0.38f, 0.17f, 0.075f);

    // Marco de la puerta.
    float marcoX = x + (lado < 0 ? 0.07f : -0.07f);
    dibujarCaja(shader, math3d::vec3(marcoX, altoPuerta/2.0f, inicioZ),
        math3d::vec3(0.12f, altoPuerta, 0.10f), madera);
    dibujarCaja(shader, math3d::vec3(marcoX, altoPuerta/2.0f, inicioZ+anchoPuerta),
        math3d::vec3(0.12f, altoPuerta, 0.10f), madera);
    dibujarCaja(shader, math3d::vec3(marcoX, altoPuerta, centroZ),
        math3d::vec3(0.12f, 0.10f, anchoPuerta), madera);

    // Hoja de puerta con pivote en una bisagra.
    math3d::mat4 modelo(1.0f);
    modelo = math3d::translate(modelo, math3d::vec3(x, 0.0f, inicioZ));
    float signo = (lado < 0) ? -1.0f : 1.0f;
    modelo = math3d::rotate(modelo, signo * angulo, math3d::vec3(0,1,0));
    modelo = math3d::translate(modelo, math3d::vec3(0.0f, altoPuerta/2.0f, anchoPuerta/2.0f));
    modelo = math3d::scale(modelo, math3d::vec3(espesorHoja, altoPuerta-0.08f, anchoPuerta-0.08f));
    shader.setMat4("model", modelo);
    shader.setVec3("objectColor", maderaClara);
    cubo.dibujar();

    // Panel decorativo sobre la hoja (mismo pivote y giro).
    math3d::mat4 panel(1.0f);
    panel = math3d::translate(panel, math3d::vec3(x, 0.0f, inicioZ));
    panel = math3d::rotate(panel, signo * angulo, math3d::vec3(0,1,0));
    panel = math3d::translate(panel, math3d::vec3(0.045f, 1.10f, anchoPuerta/2.0f));
    panel = math3d::scale(panel, math3d::vec3(0.012f, 0.70f, 0.82f));
    shader.setMat4("model", panel);
    shader.setVec3("objectColor", madera);
    cubo.dibujar();
}

void Pasillo::dibujarParedLateral(const Shader& shader, int lado,
                                  float centroPuerta, float anguloPuerta) const
{
    const float x = lado * (ANCHO / 2.0f);
    const float anchoPuerta = 1.40f;
    const float altoPuerta = 2.20f;
    const float inicio = centroPuerta - anchoPuerta/2.0f;
    const float fin = centroPuerta + anchoPuerta/2.0f;
    const math3d::vec3 pared(0.80f, 0.77f, 0.68f);
    const math3d::vec3 franja(0.67f, 0.63f, 0.51f);

    // Dos tramos de pared dejan un vano real para pasar a cada ambiente.
    const float tramos[2][2] = {{-MEDIO_LARGO, inicio}, {fin, MEDIO_LARGO}};
    for (int i = 0; i < 2; ++i)
    {
        float longitud = tramos[i][1] - tramos[i][0];
        float centro = (tramos[i][0] + tramos[i][1]) / 2.0f;
        if (longitud <= 0.0f) continue;
        dibujarCaja(shader, math3d::vec3(x, ALTO/2.0f, centro),
            math3d::vec3(ESPESOR, ALTO, longitud), pared);
        float caraInterior = x + (lado < 0 ? 0.105f : -0.105f);
        dibujarCaja(shader, math3d::vec3(caraInterior, 0.58f, centro),
            math3d::vec3(0.025f, 1.16f, longitud), franja);
    }

    // Tramo superior sobre el vano.
    dibujarCaja(shader, math3d::vec3(x, (altoPuerta+ALTO)/2.0f, centroPuerta),
        math3d::vec3(ESPESOR, ALTO-altoPuerta, anchoPuerta), pared);

    dibujarPuerta(shader, lado, centroPuerta, anguloPuerta);
}

void Pasillo::dibujarAmbienteIzquierdo(const Shader& shader) const
{
    const math3d::vec3 pared(0.80f,0.77f,0.68f);
    const math3d::vec3 piso(0.44f,0.43f,0.40f);
    const math3d::vec3 techo(0.86f,0.86f,0.83f);
    const math3d::vec3 franja(0.67f,0.63f,0.51f);

    // Ambiente 2: aula a la izquierda, conectada mediante una puerta.
    dibujarCaja(shader, math3d::vec3(-6.0f,-0.12f,5.0f), math3d::vec3(6.0f,0.24f,8.0f), piso);
    dibujarCaja(shader, math3d::vec3(-6.0f,3.34f,5.0f), math3d::vec3(6.0f,0.28f,8.0f), techo);
    dibujarCaja(shader, math3d::vec3(-9.0f,1.60f,5.0f), math3d::vec3(ESPESOR,ALTO,8.0f), pared);
    dibujarCaja(shader, math3d::vec3(-6.0f,1.60f,1.0f), math3d::vec3(6.0f,ALTO,ESPESOR), pared);
    dibujarCaja(shader, math3d::vec3(-6.0f,1.60f,9.0f), math3d::vec3(6.0f,ALTO,ESPESOR), pared);
    dibujarCaja(shader, math3d::vec3(-8.88f,0.58f,5.0f), math3d::vec3(0.025f,1.16f,7.8f), franja);

    // Mesa del aula.
    const math3d::vec3 madera(0.36f,0.20f,0.11f);
    dibujarCaja(shader, math3d::vec3(-6.1f,0.82f,4.2f), math3d::vec3(1.5f,0.12f,0.8f), madera);
    for (float x : {-6.70f,-5.50f})
        for (float z : {3.9f,4.5f})
            dibujarCaja(shader, math3d::vec3(x,0.40f,z), math3d::vec3(0.09f,0.80f,0.09f), madera);
}

void Pasillo::dibujarAmbienteDerecho(const Shader& shader) const
{
    const math3d::vec3 pared(0.80f,0.77f,0.68f);
    const math3d::vec3 piso(0.44f,0.43f,0.40f);
    const math3d::vec3 techo(0.86f,0.86f,0.83f);
    const math3d::vec3 franja(0.67f,0.63f,0.51f);

    // Ambiente 3: aula a la derecha, conectada mediante otra puerta.
    dibujarCaja(shader, math3d::vec3(6.0f,-0.12f,-5.0f), math3d::vec3(6.0f,0.24f,8.0f), piso);
    dibujarCaja(shader, math3d::vec3(6.0f,3.34f,-5.0f), math3d::vec3(6.0f,0.28f,8.0f), techo);
    dibujarCaja(shader, math3d::vec3(9.0f,1.60f,-5.0f), math3d::vec3(ESPESOR,ALTO,8.0f), pared);
    dibujarCaja(shader, math3d::vec3(6.0f,1.60f,-1.0f), math3d::vec3(6.0f,ALTO,ESPESOR), pared);
    dibujarCaja(shader, math3d::vec3(6.0f,1.60f,-9.0f), math3d::vec3(6.0f,ALTO,ESPESOR), pared);
    dibujarCaja(shader, math3d::vec3(8.88f,0.58f,-5.0f), math3d::vec3(0.025f,1.16f,7.8f), franja);

    // Una banca OBJ se carga con el lector del profesor.
    if (bancaCargada)
    {
        math3d::mat4 modelo(1.0f);
        modelo = math3d::translate(modelo, math3d::vec3(6.2f,0.0f,-5.0f));
        bancaOBJ.dibujar(shader, modelo);
    }
}

void Pasillo::dibujarSalida(const Shader& shader) const
{
    const float z = -MEDIO_LARGO;
    const math3d::vec3 pared(0.80f,0.77f,0.68f);
    const math3d::vec3 madera(0.25f,0.10f,0.045f);
    const math3d::vec3 vidrio(0.30f,0.57f,0.65f);
    const math3d::vec3 senal(0.10f,0.45f,0.20f);

    // Muro del fondo con abertura central acristalada.
    dibujarCaja(shader, math3d::vec3(-2.15f,1.60f,z), math3d::vec3(1.70f,ALTO,ESPESOR), pared);
    dibujarCaja(shader, math3d::vec3( 2.15f,1.60f,z), math3d::vec3(1.70f,ALTO,ESPESOR), pared);
    dibujarCaja(shader, math3d::vec3(0.0f,2.70f,z), math3d::vec3(2.60f,1.0f,ESPESOR), pared);

    // Marco de la puerta acristalada.
    for (float x : {-1.30f,0.0f,1.30f})
        dibujarCaja(shader, math3d::vec3(x,1.10f,z+0.08f), math3d::vec3(0.10f,2.20f,0.12f), madera);
    dibujarCaja(shader, math3d::vec3(0.0f,2.20f,z+0.08f), math3d::vec3(2.60f,0.10f,0.12f), madera);
    dibujarCaja(shader, math3d::vec3(0.0f,0.06f,z+0.08f), math3d::vec3(2.60f,0.12f,0.12f), madera);

    // Paneles azulados que representan el vidrio.
    dibujarCaja(shader, math3d::vec3(-0.65f,1.13f,z+0.10f), math3d::vec3(1.18f,2.05f,0.035f), vidrio);
    dibujarCaja(shader, math3d::vec3( 0.65f,1.13f,z+0.10f), math3d::vec3(1.18f,2.05f,0.035f), vidrio);
    dibujarCaja(shader, math3d::vec3(0.0f,2.55f,z+0.18f), math3d::vec3(0.72f,0.22f,0.08f), senal);
}

void Pasillo::dibujarArquitectura(const Shader& shader) const
{
    const math3d::vec3 piso(0.43f,0.42f,0.39f);
    const math3d::vec3 techo(0.87f,0.87f,0.84f);
    const math3d::vec3 viga(0.58f,0.60f,0.60f);
    const math3d::vec3 luminaria(0.98f,0.96f,0.82f);
    const math3d::vec3 junta(0.31f,0.30f,0.28f);

    // Piso y juntas que sugieren las losetas observadas en las fotos.
    dibujarCaja(shader, math3d::vec3(0.0f,-0.12f,0.0f), math3d::vec3(ANCHO,0.24f,LARGO), piso);
    for (float z=-11.0f; z<=11.0f; z+=1.5f)
        dibujarCaja(shader, math3d::vec3(0.0f,0.006f,z), math3d::vec3(ANCHO-0.08f,0.008f,0.012f), junta);
    for (float x=-2.0f; x<=2.0f; x+=1.0f)
        dibujarCaja(shader, math3d::vec3(x,0.006f,0.0f), math3d::vec3(0.012f,0.008f,LARGO-0.08f), junta);

    // Techo y vigas transversales.
    dibujarCaja(shader, math3d::vec3(0.0f,3.34f,0.0f), math3d::vec3(ANCHO,0.28f,LARGO), techo);
    for (float z=-10.0f; z<=10.0f; z+=4.0f)
        dibujarCaja(shader, math3d::vec3(0.0f,3.12f,z), math3d::vec3(ANCHO+0.10f,0.28f,0.30f), viga);

    // Luminarias rectangulares.
    for (float z=8.0f; z>=-10.0f; z-=4.5f)
        dibujarCaja(shader, math3d::vec3(0.0f,3.16f,z), math3d::vec3(0.42f,0.06f,1.20f), luminaria);
}

void Pasillo::dibujarVentanas(const Shader& shader) const
{
    const math3d::vec3 vidrio(0.38f,0.62f,0.68f);
    const math3d::vec3 marco(0.28f,0.13f,0.07f);

    for (int lado : {-1,1})
    {
        float x = lado * 2.88f;
        for (float z : {8.0f,0.0f,-8.0f})
        {
            dibujarCaja(shader, math3d::vec3(x,2.62f,z), math3d::vec3(0.035f,0.48f,1.25f), vidrio);
            dibujarCaja(shader, math3d::vec3(x,2.62f,z-0.65f), math3d::vec3(0.07f,0.55f,0.07f), marco);
            dibujarCaja(shader, math3d::vec3(x,2.62f,z+0.65f), math3d::vec3(0.07f,0.55f,0.07f), marco);
        }
    }
}

void Pasillo::dibujarMobiliario(const Shader& shader) const
{
    // Tacho de basura azul verdoso observado en las fotografias.
    const math3d::vec3 tacho(0.035f,0.30f,0.32f);
    const math3d::vec3 tapa(0.025f,0.18f,0.20f);
    dibujarCaja(shader, math3d::vec3(-2.25f,0.46f,6.7f), math3d::vec3(0.48f,0.88f,0.48f), tacho);
    dibujarCaja(shader, math3d::vec3(-2.25f,0.92f,6.7f), math3d::vec3(0.55f,0.10f,0.55f), tapa);

    // Banca de respaldo geometrico si el OBJ no se pudo cargar.
    if (!bancaCargada)
    {
        const math3d::vec3 madera(0.28f,0.13f,0.06f);
        dibujarCaja(shader, math3d::vec3(2.20f,0.52f,-2.0f), math3d::vec3(1.30f,0.14f,0.42f), madera);
        dibujarCaja(shader, math3d::vec3(2.20f,0.90f,-2.18f), math3d::vec3(1.30f,0.65f,0.10f), madera);
        for (float x : {1.72f,2.68f})
            dibujarCaja(shader, math3d::vec3(x,0.25f,-2.0f), math3d::vec3(0.09f,0.50f,0.09f), madera);
    }
}

void Pasillo::graficar(const Shader& shader, float anguloPuertas) const
{
    dibujarArquitectura(shader);
    dibujarParedLateral(shader,-1,5.0f,anguloPuertas);
    dibujarParedLateral(shader, 1,-5.0f,anguloPuertas);
    dibujarAmbienteIzquierdo(shader);
    dibujarAmbienteDerecho(shader);
    dibujarSalida(shader);
    dibujarVentanas(shader);
    dibujarMobiliario(shader);
}

void Pasillo::liberar()
{
    cubo.liberar();
    bancaOBJ.liberar();
}
