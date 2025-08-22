#ifndef PROGRAM_STORE_HPP
#define PROGRAM_STORE_HPP

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

#define LOG_LENGTH 512

typedef struct program_standin__ // this is only used until shader class is created, afterwards, it will merely have a name for lookup (idk how to) and a pointer to the shader instance
{
    GLuint program = 0;
} program_standin;

class program_store
{
public:
    static program_store *get_instance();
    void load(const char *config_path);

    GLuint get_program(const char *program_name);
    void del_program(const char *);

private:
    bool has_loaded = false;

    std::unordered_map<std::string, program_standin> programs; // after creating a shader class, use it here

    program_store();
    static program_store *instance;

    static inline GLuint compile_shader(GLenum shader_type, const char *shader_source);
    static inline GLuint create_shader_program(const char *vertex_source, const char *fragment_source);
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