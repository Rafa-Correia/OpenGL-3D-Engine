#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <tuple>
#include <map>
#include <array>

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

#include "engine/shader.hpp"
#include "engine/shader_store.hpp"

typedef struct vertex_
{
    vec3<float> pos;
    vec3<float> normal;
    vec2<float> texcoord;
} vertex;

class object
{
public:
    object(std::string fpath, std::string shader_name) : wavefront_fpath(fpath), shader_name(shader_name)
    {
        shader_store *instance = shader_store::get_instance();
        this->s = instance->get_shader(this->shader_name.c_str());

        this->_load(fpath);
    }
    void render(mat4 projection, mat4 view);

private:
    std::string wavefront_fpath;
    bool loaded = false;
    std::string shader_name;
    shader s;
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