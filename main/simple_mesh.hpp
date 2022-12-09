#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad.h>

#include <vector>
#include "optional"

#include "../vmlib/vec3.hpp"

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;

	size_t size;
};

SimpleMeshData concatenate( SimpleMeshData, SimpleMeshData const& );


GLuint create_vao( SimpleMeshData const&, std::optional<GLuint> );

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
