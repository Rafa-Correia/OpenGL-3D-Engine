#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec4 position;
out vec3 normal;
out vec2 tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    position = projection * view * model * vec4(aPos, 1.0);

    vec4 normal_tmp = vec4(aNormal, 0.0);
    vec4 normal_res = projection * view * model * normal_tmp;
    normal = vec3(normal_res);

    tex = aTex;
}