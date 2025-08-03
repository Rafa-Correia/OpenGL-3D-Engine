#include "engine/program_store.hpp"

program_store *program_store::instance = nullptr;

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
    if (this->has_loaded)
    {
        printer::print_warning("Store has loaded all shaders already!");
        return;
    }

    // todo - actually parse config file with shader paths and all that
}