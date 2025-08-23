#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec3 Normal;
out vec3 FragPos;
out vec2 Tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    // world-space position
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);

    // normal matrix (inverse transpose of model)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * aNormal);

    Tex = aTex;

    // final clip space position
    gl_Position = projection * view * worldPos;
}
