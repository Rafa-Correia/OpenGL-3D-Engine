#include "engine/shader_store.hpp"

shader_store *shader_store::instance = nullptr;

shader_store *shader_store::get_instance()
{
    if (shader_store::instance != nullptr)
    {
        return shader_store::instance;
    }

    shader_store::instance = new shader_store();
    return shader_store::instance;
}

void shader_store::load(const char *config_path)
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

        shader new_shader = shader(std::string(vertex_path), std::string(fragment_path));

        this->shaders[program_elem->Attribute("name")] = new_shader;
        program_elem = program_elem->NextSiblingElement("program");
    }
}

shader shader_store::get_shader(const char *shader_name)
{
    auto it = this->shaders.find(shader_name);
    if (it == this->shaders.end())
    {
        throw FailedToLoadException("ACtually didnt fail to load jsut didnt find shader");
    }
    else
    {
        shader res = it->second;
        return res;
    }
}