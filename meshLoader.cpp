#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include <glm.hpp>

bool meshLoader(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<unsigned int> & out_vertex_indices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals)
{
	printf("Loading obj file %s........\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		std::cout << "Can't open the file!" << std::endl;
		return false;
	}

	while (1) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f \n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f \n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f \n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d %d %d %d %d %d %d %d %d \n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				std::cout << "File cannot be read :(" << std::endl;
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
			
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = vertexIndices[i];
		glm::vec2 uv = temp_vertices[uvIndex - 1];
		out_uvs.push_back(uv);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_vertices[normalIndex - 1];
		out_normals.push_back(normal);
	}

	out_vertex_indices = vertexIndices;

	fclose(file);
	std::cout << "File has been read" << std::endl;
	return true;
}