#include "engine/program_store.hpp"

program_store *program_store::instance = nullptr;

program_store::program_store()
{
    return;
}

program_store *program_store::get_instance()
{
    if (program_store::instance != nullptr)
    {
        return program_store::instance;
    }

    program_store::instance = new program_store();
    return program_store::instance;
}

void program_store::load(const char *config_path)
{
    const char caller[] = "program_store::load(...)";

    if (this->has_loaded)
    {
        printer::print_warning("Store has loaded all shaders already!");
        return;
    }

    std::stringstream error_ss;
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path) != tinyxml2::XML_SUCCESS)
    {
        error_ss << "Failed to load file: " << config_path << std::endl;
        printer::print_exception(error_ss.str(), caller);
        throw FailedToLoadException(error_ss.str());
    }

    tinyxml2::XMLElement *root = doc.RootElement();
    if (!root)
    {
        error_ss << "Failed to get root element." << std::endl;
        printer::print_exception(error_ss.str(), caller);
        throw FailedToLoadException(error_ss.str());
    }
    if (std::string(root->Value()) != "root")
    {
        error_ss << "Root element (" << root->Value() << ") is not valid." << std::endl;
        printer::print_exception(error_ss.str(), caller);
        throw FailedToLoadException(error_ss.str());
    }

    tinyxml2::XMLElement *program_elem = root->FirstChildElement("program");
    if (!program_elem)
    {
        error_ss << "No program element found!" << std::endl;
        printer::print_exception(error_ss.str(), caller);
        throw FailedToLoadException(error_ss.str());
    }
    while (program_elem)
    {
        if (!program_elem->Attribute("name"))
        {
            error_ss << "Program element doesn't have name attribute." << std::endl;
            printer::print_exception(error_ss.str(), caller);
            throw FailedToLoadException(error_ss.str());
        }
        if (std::string(program_elem->Value()) != "program")
        {
            error_ss << "Invalid element (not program) found!" << std::endl;
            printer::print_exception(error_ss.str(), caller);
            throw FailedToLoadException(error_ss.str());
        }

        program_standin program;

        tinyxml2::XMLElement *vertex_elem = program_elem->FirstChildElement("vertex"), *fragment_elem = program_elem->FirstChildElement("fragment");
        if (!vertex_elem || !fragment_elem)
        {
            error_ss << "Program is missing one or both shader sources." << std::endl;
            printer::print_exception(error_ss.str(), caller);
            throw FailedToLoadException(error_ss.str());
        }
        const char *vertex_path = vertex_elem->Attribute("path"), *fragment_path = fragment_elem->Attribute("path");
        if (!vertex_path || !fragment_path)
        {
            error_ss << "Couldn't find path to one or both shader sources." << std::endl;
            printer::print_exception(error_ss.str(), caller);
            throw FailedToLoadException(error_ss.str());
        }

        std::ifstream vertex_file(vertex_path), fragment_file(fragment_path);
        std::ostringstream oss_v, oss_f;
        oss_v << vertex_file.rdbuf();
        oss_f << fragment_file.rdbuf();

        program.program = create_shader_program(oss_v.str().c_str(), oss_f.str().c_str());

        this->programs[program_elem->Attribute("name")] = program;
        program_elem = program_elem->NextSiblingElement("program");
    }
}

GLuint program_store::get_program(const char *program_name)
{
    auto it = this->programs.find(program_name);
    if (it == this->programs.end())
    {
        return 0;
    }
    else
    {
        const program_standin &res = it->second;
        return res.program;
    }
}

GLuint program_store::compile_shader(GLenum shader_type, const char *shader_source)
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
GLuint program_store::create_shader_program(const char *vertex_source, const char *fragment_source)
{
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    // check if program link success
    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char program_log[LOG_LENGTH];
        glGetProgramInfoLog(shader_program, LOG_LENGTH, nullptr, program_log);

        std::cout << "Failed to link shader program: " << program_log << std::endl;
        return 0;
    }

    return shader_program;
}