#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <glm.hpp>
#include <vector>

bool meshLoader(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<unsigned int> & out_vertex_indices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals);

#endif
