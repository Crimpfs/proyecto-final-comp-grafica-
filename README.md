# PROYECTO MIJAEL – JLPERALTA40 en CodeBlocks

## QUE SE ADAPTO

Se utilizo el contenido real de `JLPERALTA40.rar`:
- `JLPERALTA30.cbp` de CodeBlocks (mismo nombre y estructura del profesor).
- `dependencias/include/GL/glew.h` y `dependencias/include/GLFW/glfw3.h`.
- `dependencias/lib/libglew32.dll.a` y `dependencias/lib/libglfw3dll.a`.
- `dependencias/lib/glew32.dll` y `dependencias/lib/glfw3.dll`.

Se integraron las fuentes del proyecto anterior de Mijael: `Pasillo`, `Mesh`, `Shader`, `ModeloOBJ`, su `glm.cpp` lector OBJ y los archivos GLSL. El entorno contiene pasillo, dos ambientes laterales, muros con aperturas para las puertas, ventanas, marcos, vigas, techo, piso, banco y tacho. Las dimensiones son provisionales; deben ajustarse a mediciones reales.

**La biblioteca GLM matematica NO viene en JLPERALTA40.rar.** Para evitar instalarla o mezclar versiones, se incorporo `Math3D.h` con `vec2`, `vec3`, `mat4`, `translate`, `scale`, `rotate`, `lookAt` y `perspective`. Esta matematicas propias sustituyen las llamadas a `glm/glm.hpp` del proyecto antiguo. **`glm.h`/`glm.cpp` sigue siendo solamente el lector de OBJ** incluido en el proyecto Mijael original; no es la GLM matematica.

## REQUISITOS

- CodeBlocks con MinGW / GCC de **32 bits**. Las dependencias DLL y `.a` suministradas por JLPERALTA40 son de 32 bits.
- Controladores de video que permitan crear un contexto **OpenGL 3.3 Core**.
- No requiere MSYS2, GLEW/GLFW/GLM de C:\OpenGL, CMake ni bibliotecas descargadas por separado.

## COMO ABRIR Y COMPILAR

1. Descomprime **toda** la carpeta `Proyecto_Mijael_JLPERALTA40`, sin sacar `main.cpp` de ella.
2. En CodeBlocks ve a **File > Open...** y abre exactamente `JLPERALTA30.cbp` dentro de esa carpeta.
3. En **Settings > Compiler > Toolchain executables** verifica que tu CodeBlocks use **MinGW/GCC de 32 bits**. El compilador puede ser externo, pero debe ser compatible con las bibliotecas PE32 del profesor.
4. Selecciona la configuracion **Debug**.
5. Haz **Build > Clean** y luego **Build > Build and run (F9)**.
6. La salida se generara en `bin/Debug/MIJAEL_PASILLO.exe`.
7. Si compilas Release, el ejecutable estara en `bin/Release/MIJAEL_PASILLO.exe`.

**No abras el proyecto anterior `Mijael_OpenGL.cbp` ni el main.cpp del triangulo del RAR original.** Solo se debe compilar el `.cbp` que va dentro de este paquete.

## BIBLIOTECAS YA CONFIGURADAS EN EL .CBP

- `dependencias/include` para encabezados.
- `dependencias/lib` para bibliotecas de enlace.
- `-l:libglew32.dll.a` y `-l:libglfw3dll.a` para versiones dinamicas de GLEW y GLFW.
- Bibliotecas del sistema: `opengl32`, `winmm`, `kernel32`, `user32`, `gdi32`.
- Compilacion C++11, igual que en el proyecto JLPERALTA40 del profesor.

**DLLs:** `glew32.dll` y `glfw3.dll` ya estan copiadas a `bin/Debug` y `bin/Release`. NO coloques DLLs en System32. Si extraes solo el EXE fuera de su carpeta, deberas llevar contigo las DLLs.

## CONTROLES DEL PASILLO

| Tecla | Accion |
|---|---|
| W A S D | Caminar por el pasillo (camara libre) |
| Mouse | Mirar alrededor |
| Rueda | Cambiar campo visual (FOV) |
| O | Abrir y cerrar las dos puertas laterales |
| M | Cambiar entre caras solidas y alambre (wireframe) |
| ESPACIO / CTRL | Subir / bajar |
| R | Reiniciar la posicion de la camara |
| ESC | Salir |

## COMO ESTA DISTRIBUIDO EL CODIGO

- `main.cpp`: ventana GLFW, inicializacion GLEW, camara, teclado, loop y envio de matrices.
- `Pasillo.cpp/.h`: geometria de edificio y ambientes, mobiliario, puertas animadas.
- `Mesh.cpp/.h`: VAO, VBO, EBO/IBO, glVertexAttribPointer() y glDrawElements().
- `Shader.cpp/.h`: lectura, compilacion y enlace de Vertex y Fragment Shader GLSL.
- `shaders/*.glsl`: calculo de posicion clip y colores/iluminacion.
- `ModeloOBJ.cpp/.h`: conversion de OBJ a triangulos indexados para OpenGL moderno.
- `glm.cpp/.h`: lector OBJ original adaptado a C++ sin funciones de dibujo obsoletas.
- `Math3D.h`: operaciones matematicas propias de vectores y matrices.
- `modelos/banca.obj`: prueba de importacion OBJ, sin bibliotecas externas.

## SI APARECE UN ERROR

- `undefined reference to glfw...` o `__imp__glew...`: revisa que estes abriendo **este** `JLPERALTA30.cbp`. Sus opciones de Linker ya apuntan a las bibliotecas `.dll.a` incluidas. Verifica que no se agreguen bibliotecas de otra arquitectura desde una configuracion global de CodeBlocks.
- `skipping incompatible ...`: tu compilador probablemente es de 64 bits. Para este paquete usa MinGW de 32 bits.
- `glew32.dll was not found`: las DLLs deben permanecer junto a `MIJAEL_PASILLO.exe` en `bin/Debug` o `bin/Release`.
- No aparece escenario: revisa que exista la carpeta `shaders` junto al proyecto. `main.cpp` busca recursos tanto desde la raiz como desde subcarpetas de salida.
- Error de OpenGL 3.3 Core: actualiza el controlador grafico o verifica que el hardware soporte esa version de OpenGL.

## ALCANCE

Este proyecto implementa la reconstruccion geometrica y el renderizador moderno del pasillo. Todavia no incluye mediciones finales, texturas fotograficas, deteccion de colisiones, Ray Casting, inspector de vertices ni segunda ruta de evacuacion comprobada. Para lograr el trabajo final deben desarrollarse estos modulos posteriormente.

### VALIDACION REALIZADA

Se revisaron las bibliotecas originales (archivos de 32 bits) y se verifico que las bibliotecas de importacion incluyen los simbolos `glewInit`, `glewExperimental`, `glfwInit` y `glfwCreateWindow`. El codigo supero una revision de sintaxis C++11 utilizando declaraciones auxiliares de OpenGL, y se probaron numericamente las matrices Model/View/Projection y la lectura del modelo OBJ. No se pudo ejecutar la aplicacion Windows en este entorno Linux: la compilacion final y apertura de ventana deben verificarse en CodeBlocks con MinGW 32 bits.
