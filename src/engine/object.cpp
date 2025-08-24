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
    std::stringstream error_ss, warning_ss, info_ss;
    const char caller[] = "object::_load(...)";

    std::ifstream file(fpath);
    if (!file.is_open())
    {
        error_ss << "Failed to open file at " << fpath << ".";
        throw FailedToParseException(error_ss.str(), caller);
    }

    std::vector<float> positions; // v
    std::vector<float> normals;   // vn
    std::vector<float> texcoords; // vt

    std::map<std::tuple<int, int, int>, unsigned int> vertex_map;
    std::vector<unsigned int> indices;
    std::vector<vertex> vertices;

    std::string line_raw, line;
    size_t lineno = 0;

    while (std::getline(file, line_raw))
    {
        lineno++;

        auto hash = line_raw.find('#');
        if (hash != std::string::npos)
            line_raw = line_raw.substr(0, hash);

        line = trim(line_raw);
        if (line.empty())
            continue;

        // tokenize (skip empties)
        std::vector<std::string> tokens;
        std::stringstream liness(line);
        std::string tok;
        while (std::getline(liness, tok, ' '))
        {
            if (!tok.empty())
                tokens.push_back(tok);
        }
        if (tokens.empty())
            continue;

        std::string t0 = tokens.at(0);

        // vertex positions
        if (t0 == "v")
        {
            if (tokens.size() < 4)
            {
                warning_ss << "invalid vertex at line " << lineno;
                printer::print_warning(warning_ss.str(), caller);
                warning_ss.str(std::string());
                continue;
            }
            float x = std::stof(tokens[1]);
            float y = std::stof(tokens[2]);
            float z = std::stof(tokens[3]);
            positions.insert(positions.end(), {x, y, z});
        }
        // texture coords
        else if (t0 == "vt")
        {
            if (tokens.size() < 3)
            {
                warning_ss << "invalid texcoord at line " << lineno;
                printer::print_warning(warning_ss.str(), caller);
                warning_ss.str(std::string());
                continue;
            }
            float u = std::stof(tokens[1]);
            float v = std::stof(tokens[2]);
            texcoords.insert(texcoords.end(), {u, v});
        }
        // normals
        else if (t0 == "vn")
        {
            if (tokens.size() < 4)
            {
                warning_ss << "invalid normal at line " << lineno;
                printer::print_warning(warning_ss.str(), caller);
                warning_ss.str(std::string());
                continue;
            }
            vec3<float> n(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
            n.normalize();
            normals.insert(normals.end(), {n.x, n.y, n.z});
        }
        // faces
        else if (t0 == "f")
        {
            if (tokens.size() < 4)
            {
                warning_ss << "face with fewer than 3 vertices ignored. (line " << lineno << ")";
                printer::print_warning(warning_ss.str(), caller);
                warning_ss.str(std::string());
                continue;
            }

            // this bit is just for triangulating any n-gon
            for (size_t i = 1; i + 2 < tokens.size(); i++)
            {
                std::array<std::string, 3> tri = {tokens[1], tokens[i + 1], tokens[i + 2]};

                for (size_t k = 0; k < 3; k++)
                {
                    std::stringstream tmp_sstream(tri[k]);
                    std::string tmp_tok;
                    std::vector<std::string> vertex_toks;
                    while (std::getline(tmp_sstream, tmp_tok, '/'))
                        vertex_toks.push_back(tmp_tok);

                    int v = 0, vt = 0, vn = 0;
                    if (vertex_toks.size() >= 1 && !vertex_toks[0].empty())
                        v = std::stoi(vertex_toks[0]);
                    if (vertex_toks.size() >= 2 && !vertex_toks[1].empty())
                        vt = std::stoi(vertex_toks[1]);
                    if (vertex_toks.size() >= 3 && !vertex_toks[2].empty())
                        vn = std::stoi(vertex_toks[2]);

                    // handle negatives
                    if (v < 0)
                        v = (positions.size() / 3) + v + 1;
                    if (vt < 0)
                        vt = (texcoords.size() / 2) + vt + 1;
                    if (vn < 0)
                        vn = (normals.size() / 3) + vn + 1;

                    std::tuple<int, int, int> key(v, vt, vn);
                    auto it = vertex_map.find(key);

                    if (it == vertex_map.end())
                    {
                        vertex vert{};
                        if (v > 0)
                            vert.pos = {positions[(v - 1) * 3 + 0], positions[(v - 1) * 3 + 1], positions[(v - 1) * 3 + 2]};
                        if (vt > 0)
                            vert.texcoord = {texcoords[(vt - 1) * 2 + 0], texcoords[(vt - 1) * 2 + 1]};
                        if (vn > 0)
                            vert.normal = {normals[(vn - 1) * 3 + 0], normals[(vn - 1) * 3 + 1], normals[(vn - 1) * 3 + 2]};

                        unsigned int newIndex = static_cast<unsigned int>(vertices.size());
                        vertices.push_back(vert);
                        vertex_map[key] = newIndex;
                        indices.push_back(newIndex);
                    }
                    else
                    {
                        indices.push_back(it->second);
                    }
                }
            }
        }
        else
        {
            info_ss << t0 << " is unsupported (line " << lineno << ")";
            printer::print_info(info_ss.str(), caller);
            info_ss.str(std::string());
            continue;
        }
    }

    // build interleaved buffer
    std::vector<float> mesh;
    mesh.reserve(vertices.size() * 8);
    for (const auto &v : vertices)
    {
        mesh.insert(mesh.end(),
                    {v.pos.x, v.pos.y, v.pos.z,
                     v.normal.x, v.normal.y, v.normal.z,
                     v.texcoord.x, v.texcoord.y});
    }

    this->i_count = indices.size();

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);                   // pos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float))); // texcoord

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void object::render(mat4 projection, mat4 view)
{
    this->s.use();

    this->s.uset_mat4("projection", projection);
    this->s.uset_mat4("view", view);
    this->s.uset_mat4("model", model);

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->i_count, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0); // just unbind it
    glUseProgram(0);
}
