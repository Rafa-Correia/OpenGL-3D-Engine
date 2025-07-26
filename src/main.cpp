/**
 * Hey! This is the main source file for the engine!
 * 
 * This whole project is just my way of testing my graphics programming
 * capabilities. I want to make a 3D engine from the ground up, like I did
 * during my semester with the Graphics Computing course, except this time
 * I would not be using the OpenGL Fixed Function Pipeline, and instead I'd
 * like to use shaders! That way I'd also get familiar with GLSL. 
 * 
 * I also want to try and build a 3D scene, both the format and the parsing.
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

float angle = 0;

void render() {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.9f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-0.9f, -0.9f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.9f, -0.9f);
    glEnd();

    glutSwapBuffers();
    return;
}

void idle() {
    return;
}

int main(int argc, char **argv) {
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
    
    glutMainLoop();

    return 0;
}