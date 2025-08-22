#version 330 core

out vec4 FragColor;

in vec4 position;
in vec3 normal;
in vec2 tex;

void main () {
    FragColor = vec4(normal, 1.0);
}