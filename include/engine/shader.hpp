#ifndef SHADER_HPP
#define SHADER_HPP

#define LOG_LENGTH 512

#ifdef __APPLE__
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "math/vec2.hpp"
#include "math/vec3.hpp"
// probably include vec4 when done
#include "math/mat4.hpp"

class shader
{
public:
    shader(std::string &vertex_shader_path_, std::string &fragment_shader_path_)
    {
        this->vertex_shader_path = vertex_shader_path_;
        this->fragment_shader_path = fragment_shader_path_;

        this->v_shader_id = 0;
        this->f_shader_id = 0;
        this->shader_program_id = 0;

        this->load_();
    }

    shader() {}

    void use() { glUseProgram(this->shader_program_id); }

    // void uset_float(std::string uniform_name, float val);
    // void uset_vec2(std::string uniform_name, vec2<float> val);

    void uset_vec3(std::string uniform_name, vec3<float> val)
    {
        GLint u_loc = this->get_uniform_(uniform_name);
        glUniform3f(u_loc, val.x, val.y, val.z);
    }

    // void uset_vec4(std::string uniform_name, vec4<float> val);

    void uset_mat4(std::string uniform_name, mat4 val)
    {
        GLint u_loc = this->get_uniform_(uniform_name);
        glUniformMatrix4fv(u_loc, 1, GL_FALSE, val);
    }

private:
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    // for now I will only support these 2 types of shaders.
    // later on I can just add more if needed.
    GLuint v_shader_id;
    GLuint f_shader_id;
    GLuint shader_program_id;

    std::map<std::string, GLint> uniform_locations;

    GLint get_uniform_(std::string uniform_name_)
    {
        auto it = uniform_locations.find(uniform_name_);
        if (!(it == uniform_locations.end()))
        {
            // uniform exists already;
            return it->second;
        }
        else
        {
            GLint u_loc = glGetUniformLocation(this->shader_program_id, uniform_name_.c_str());
            uniform_locations[uniform_name_] = u_loc;
            return u_loc;
        }
    }

    GLuint compile_shader(GLenum shader_type, const char *shader_source)
    {
        GLuint shader = glCreateShader(shader_type);

        glShaderSource(shader, 1, &shader_source, nullptr);

        glCompileShader(shader);

        // check if compilation success
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char shader_log[LOG_LENGTH];
            glGetShaderInfoLog(shader, LOG_LENGTH, nullptr, shader_log);
            std::cout << "Failed to compile shader: " << shader_log << std::endl;

            return 0;
        }

        return shader;
    }

    /**
     * For now, only supports vertex and fragment!
     */
    void load_()
    {
        std::ifstream vertex_file(this->vertex_shader_path), fragment_file(this->fragment_shader_path);
        std::ostringstream oss_v, oss_f;
        oss_v << vertex_file.rdbuf();
        oss_f << fragment_file.rdbuf();

        this->v_shader_id = compile_shader(GL_VERTEX_SHADER, oss_v.str().c_str());
        this->f_shader_id = compile_shader(GL_FRAGMENT_SHADER, oss_f.str().c_str());

        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, this->v_shader_id);
        glAttachShader(shader_program, this->f_shader_id);

        glLinkProgram(shader_program);

        // check if program link success
        GLint success;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

        if (!success)
        {
            char program_log[LOG_LENGTH];
            glGetProgramInfoLog(shader_program, LOG_LENGTH, nullptr, program_log);

            std::cout << "Failed to link shader program: " << program_log << std::endl;
            return;
        }

        this->shader_program_id = shader_program;
    }
};

#endif