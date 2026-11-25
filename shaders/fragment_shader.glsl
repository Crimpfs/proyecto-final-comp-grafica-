#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightDirection;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 luz = normalize(-lightDirection);

    // Iluminacion ambiental + difusa sencilla.
    // abs permite iluminar tambien las caras interiores del pasillo.
    float difusa = abs(dot(normal, luz));
    vec3 ambiental = 0.62 * objectColor;
    vec3 difusaColor = 0.38 * difusa * objectColor;

    FragColor = vec4(ambiental + difusaColor, 1.0);

    
}


