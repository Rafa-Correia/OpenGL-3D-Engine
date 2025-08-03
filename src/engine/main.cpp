/**
 * Hey! This is the main source file for the engine!
 *
 * This whole project is just my way of testing my graphics programming
 * capabilities. I want to make a 3D engine from the ground up, like I did
 * during my semester with the Graphics Computing course, except this time
 * I would not be using the OpenGL Fixed Function Pipeline, and instead I'd
 * like to use shaders! That way I'd also get familiar with GLSL.
 *
 * I also want to try and build a 3D scene, both the file format and the parsing.
 * Above that, I'd like to be able to parse at least .obj files, if not other
 * formats in the future.
 *
 * We'll see how this goes.
 */

#include <iostream>
#include <string>
#include <sstream>

#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#include "engine/program_store.hpp"

GLuint VAO, VBO, EBO;
GLuint shader_program_g;

/**
 * So now like, you'd load a config file with paths to all shaders and stuff.
 * I don't have that setup yet. So like, the shader source (very basic) will
 * stay here.
 */

const float vertices[] = {
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f};

const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3};

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program_g);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glutSwapBuffers();
    return;
}

void idle()
{
    return;
}

int main(int argc, char **argv)
{
    /**
     * For now, I will use hardcoded values.
     *
     *   * CHANGE LATER TO ACCEPT A SCENE FILE AS AN ENTRY POINT! *
     *   * MAYBE A CONFIGURATION FILE INSTEAD OF A SCENE, WE'LL SEE *
     */

    int win_width = 200;
    int win_height = 200;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("test");

    glutDisplayFunc(render);
    glutIdleFunc(idle);

#ifndef __APPLE__
    glewInit();
#endif

    program_store *instance = program_store::get_instance();
    instance->load("./config/shaders.xml");

    shader_program_g = instance->get_program("test");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glutMainLoop();

    return 0;
}