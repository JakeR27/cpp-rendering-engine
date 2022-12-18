#ifndef MESH_DATA_HEADER_FILE
#define MESH_DATA_HEADER_FILE

#include <vector>
#include "material.hpp"
#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

struct MeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;

	Material material; 

	size_t size;
};


#endif//MESH_DATA_HEADER_FILE
