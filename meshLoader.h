#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <glm.hpp>
#include <vector>

bool meshLoader(const char * path, std::vector<glm::vec3> & out_vertices);

#endif
