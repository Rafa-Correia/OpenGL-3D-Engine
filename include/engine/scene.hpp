#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "external/tinyxml2.h"
#include "math/vec3.hpp"

typedef struct obj_offset_pair_
{
    vec3<float> offset;
    // object of class "object" would go here
} oo_pair;

class scene
{
public:
    scene(const char *spath) { this->load(spath); };
    ~scene();

private:
    std::vector<oo_pair> objects;
    void load(const char *spath);
};

#endif