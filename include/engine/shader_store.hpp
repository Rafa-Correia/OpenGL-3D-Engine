#ifndef SHADER_STORE_HPP
#define SHADER_STORE_HPP

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>

#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#include "external/tinyxml2.h"
#include "helper/printer.hpp"

#include "engine/shader.hpp"

class shader_store
{
public:
    static shader_store *get_instance();
    void load(const char *config_path);

    shader get_shader(const char *shader_name);
    void del_program(const char *);

private:
    bool has_loaded = false;

    std::unordered_map<std::string, shader> shaders;

    shader_store() {};
    static shader_store *instance;
};

class FailedToLoadException : public std::exception
{
public:
    FailedToLoadException(std::string what_msg, std::string caller = "") : what_msg(what_msg), caller(caller) {};

    const char *what() const noexcept
    {
        return what_msg.c_str();
    }

private:
    std::string what_msg;
    std::string caller;
};

#endif