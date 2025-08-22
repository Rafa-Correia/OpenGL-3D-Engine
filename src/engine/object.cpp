#include "engine/object.hpp"

std::string trim(const std::string &str, const std::string &whitespace = " \t")
{
    const auto str_begin = str.find_first_not_of(whitespace);
    if (str_begin == std::string::npos)
        return "";
    const auto str_end = str.find_last_not_of(whitespace);
    const auto str_range = str_end - str_begin + 1;

    return str.substr(str_begin, str_range);
}

void object::_load(std::string &fpath)
{
    // like, i want to keep these streams separate, but that might not be needed. The performance costs should be negligible
    std::stringstream error_ss;
    std::stringstream warning_ss;
    std::stringstream info_ss;

    const char caller[] = "object::_load(...)";
    std::ifstream file(fpath);
    if (!file.is_open())
    {
        error_ss << "Failed to open file at " << fpath << "." << std::endl;
        throw FailedToParseException(error_ss.str(), caller);
    }

    std::vector<float> positions; // each vertex will the a triplet of floats, so the number of vertices will just be vertices.size() / 3
    std::vector<float> normals;   // same as vertices
    std::vector<float> texcoords; // not the same! its actually 2 values for each vertex

    std::map<std::tuple<int, int, int>, unsigned int> vertex_map;
    std::vector<unsigned int> indices;
    std::vector<vertex> vertices;

    std::string line_raw;
    std::string line;
    size_t lineno = 0;
    size_t v_index = 0;

    while (std::getline(file, line_raw))
    {
        lineno++;
        std::vector<std::string> tokens;

        line = trim(line_raw);
        if (line.at(0) == '#')
            continue;

        std::string tok;
        std::stringstream liness(line);
        while (std::getline(liness, tok, ' '))
        {
            tokens.push_back(tok);
        }

        // damn, cant use switch here :(
        std::string t0 = tokens.at(0);
        if (t0 == "v" || t0 == "vn" || t0 == "vt")
        {
            std::string warn_msg = "";
            size_t expected_tokens = 0;
            std::vector<float> *current_vec = nullptr;
            if (t0 == "v")
            {
                expected_tokens = 3;
                warn_msg = "vertices with more or less than 3 dimensions are unsuported.";
                current_vec = &positions;
            }
            else if (t0 == "vn")
            {
                expected_tokens = 3;
                warn_msg = "normals with more or less than 3 dimensions are unsuported.";
                current_vec = &normals;
            }
            else
            {
                // vt
                expected_tokens = 2;
                warn_msg = "texture coordinates are strictly bidimensional.";
                current_vec = &texcoords;
            }

            if (tokens.size() != expected_tokens + 1)
            {
                warning_ss << warn_msg << " (line" << lineno << ")";
                printer::print_warning(warning_ss.str(), caller);
                warning_ss.str(std::string()); // clear warning buffer
                continue;
            }

            float *buf = (float *)malloc(sizeof(float) * expected_tokens);
            for (size_t i = 0; i < expected_tokens; i++)
            {
                float tmp;
                try
                {
                    tmp = std::stof(tokens.at(i + 1));
                }
                catch (std::exception &e)
                {
                    // for now we just ignore bad values. maybe shut the whole thing down later on?
                    warning_ss << "all coordinates must be valid floats. (line " << lineno << ")";
                    printer::print_warning(warning_ss.str(), caller);
                    warning_ss.str(std::string());
                    continue;
                }

                buf[i] = tmp;
            }

            if (t0 == "vn")
            {
                vec3<float> normalvec(buf[0], buf[1], buf[2]);
                normalvec.normalize();

                current_vec->push_back(normalvec.x);
                current_vec->push_back(normalvec.y);
                current_vec->push_back(normalvec.z);

                continue;
            }

            for (size_t i = 0; i < expected_tokens; i++)
                current_vec->push_back(buf[i]);
        }
        else if (t0 == "f")
        {
            if (tokens.size() != 4)
            {
                warning_ss << "only triangle faces are supported. (line" << lineno << ")";
                printer::print_warning(warning_ss.str());
                warning_ss.str(std::string());
                continue;
            }

            // lets like, assume that i always have vertex, normals and texcoords.
            // this isnt true! i should update this function to support all of that.
            for (size_t i = 0; i < 3; i++)
            {
                std::stringstream tmp_sstream(tokens.at(i + 1)); // used to tokenize each face token (vi/vni/vti)
                std::vector<std::string> vertex_toks;
                std::string tmp_tok;

                while (std::getline(tmp_sstream, tmp_tok, '/'))
                {
                    vertex_toks.push_back(tmp_tok);
                }

                int v_indices[3] = {0};
                for (size_t j = 0; j < 3; j++)
                {
                    try
                    {
                        std::cout << "t: " << vertex_toks.at(j) << std::endl;
                        v_indices[j] = std::stoi(vertex_toks.at(j));
                    }
                    catch (std::exception &e)
                    {
                        error_ss << "face has invalid indices. (line " << lineno << ")";
                        throw FailedToParseException(error_ss.str(), caller);
                    }
                }

                std::tuple<int, int, int> vertex_key(v_indices[0], v_indices[1], v_indices[2]);
                auto it = vertex_map.find(vertex_key);
                if (it == vertex_map.end())
                {
                    // vertex is new

                    vertex vertex_cur;

                    vertex_cur.pos = vec3<float>(positions.at(v_indices[0] * 3), positions.at(v_indices[0] * 3 + 1), positions.at(v_indices[0] * 3 + 2));
                    vertex_cur.texcoord = vec2<float>(texcoords.at(v_indices[1] * 2), texcoords.at(v_indices[1] * 2 + 1));
                    vertex_cur.normal = vec3<float>(normals.at(v_indices[2] * 3), normals.at(v_indices[2] * 3 + 1), normals.at(v_indices[2] * 3 + 2));

                    vertices.push_back(vertex_cur);
                    vertex_map[vertex_key] = v_index;
                    indices.push_back(v_index);

                    v_index++;
                }
                else
                {
                    // vertex exists
                    indices.push_back(it->second);
                }
            }
        }
        else
        {
            info_ss << t0 << " is unsuported for now. (line " << lineno << ")";
            printer::print_info(info_ss.str(), caller);
            info_ss.str(std::string());
            continue;
        }
    }

    // yay! the whole file should be parsed atm
    // now I just need to build the array, and actually load them into the GPU!
    std::vector<float> mesh;
    for (vertex v : vertices)
    {
        vec3<float> v_pos = v.pos;
        vec3<float> v_normal = v.normal;
        vec2<float> v_texcoord = v.texcoord;

        mesh.push_back(v_pos.x);
        mesh.push_back(v_pos.y);
        mesh.push_back(v_pos.z);

        mesh.push_back(v_normal.x);
        mesh.push_back(v_normal.y);
        mesh.push_back(v_normal.z);

        mesh.push_back(v_texcoord.x);
        mesh.push_back(v_texcoord.y);
    }

    const float *mesh_data = mesh.data();
    const unsigned int *indices_data = indices.data();

    this->i_count = indices.size();

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_data), mesh_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_data), indices_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0); // vertex position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)3); // vertex normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)6); // vertex texcoord

    glEnableVertexAttribArray(0);
}

void object::render(mat4 projection, mat4 view)
{
    glUseProgram(this->shader_program);
    GLint proj_loc = glGetUniformLocation(this->shader_program, "projection");
    GLint view_loc = glGetUniformLocation(this->shader_program, "view");
    GLint model_loc = glGetUniformLocation(this->shader_program, "model");

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model);

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->i_count, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0); // just unbind it
}
