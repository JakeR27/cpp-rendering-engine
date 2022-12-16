#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad.h>

#include <vector>
#include "optional"

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"
#include <string>

struct Vertex {
	std::vector<Vec3f> Position;
	std::vector<Vec3f> Normal;
	std::vector<Vec2f> TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
};

// names come from rapidobj attributes
// https://github.com/guybrush77/rapidobj#mesh
struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;

	size_t size;
};

SimpleMeshData concatenate( SimpleMeshData, SimpleMeshData const& );

GLuint create_vao( SimpleMeshData const&, std::optional<GLuint> = std::nullopt );

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
