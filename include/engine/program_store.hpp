#ifndef PROGRAM_STORE_HPP
#define PROGRAM_STORE_HPP

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#include "external/tinyxml2.h"
#include "helper/printer.hpp"

struct program_standin
{
    const char *name = nullptr;
    GLuint program = 0;
    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    bool is_program_compiled = false;
};

class program_store
{
public:
    static program_store *get_instance();
    void load(const char *config_path);

    void get_program(const char *program_name);
    void del_program(const char *);

private:
    bool has_loaded = false;

    std::vector<struct program_standin> programs;

    program_store();
    static program_store *instance;
};

#endif