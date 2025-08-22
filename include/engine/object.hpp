#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <tuple>
#include <map>

#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#include "helper/printer.hpp"

#include "math/vec3.hpp"
#include "math/vec2.hpp"
#include "math/mat4.hpp"

typedef struct vertex_
{
    vec3<float> pos;
    vec3<float> normal;
    vec2<float> texcoord;
} vertex;

class object
{
public:
    object(std::string fpath, GLuint shader_program) : fpath(fpath), shader_program(shader_program) { this->_load(fpath); }
    void render(mat4 projection, mat4 view);

private:
    std::string fpath;
    bool loaded = false;
    GLuint shader_program = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;
    size_t i_count = 0;
    mat4 model = mat4::Identity();
    void _load(std::string &fpath);
};

class FailedToParseException : public std::exception
{
public:
    std::string caller;
    FailedToParseException(std::string what_msg, std::string caller) : what_msg(what_msg), caller(caller) {};

    const char *what() const noexcept
    {
        return what_msg.c_str();
    }

private:
    std::string what_msg;
};

#endif