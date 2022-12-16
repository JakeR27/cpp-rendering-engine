#include "loadobj.hpp"

#include <stb_image.h>
#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

SimpleMeshData load_wavefront_obj(char const* aPath)
{
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if(result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());
	
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines
	// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do
	// this for us.
	rapidobj::Triangulate(result);
	
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle
	// soup, ignoring the indexing information that the OBJ file contains.
	SimpleMeshData ret;
	
	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});


			// Always triangles, so we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			// Just replicate the material ambient color for each vertex... 
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});

		}
	}
	ret.size = ret.positions.size();
	return ret;
}

std::vector<SimpleMeshData> load_wavefront_multi_obj(char const* aPath)
{
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if(result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());
	
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines
	// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do
	// this for us.
	rapidobj::Triangulate(result);
	
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle
	// soup, ignoring the indexing information that the OBJ file contains.
	std::vector<SimpleMeshData> ret_multi;
	
	for (auto const& shape : result.shapes)
	{
		SimpleMeshData ret;
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			// Always triangles, so we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			// Just replicate the material ambient color for each vertex... 
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});

		}
		ret.size = ret.positions.size();
		ret_multi.push_back(ret);
	}
	
	return ret_multi;
}

TexturedMeshData load_wavefront_obj_textured(char const* aPath)
{
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if(result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());
	
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines
	// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do
	// this for us.
	rapidobj::Triangulate(result);
	
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle
	// soup, ignoring the indexing information that the OBJ file contains.
	SimpleMeshData ret;
	TexturedMeshData ret_textured;
	
	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			ret_textured.textureCoords.emplace_back(Vec3f{
				result.attributes.texcoords[idx.texcoord_index * 3 + 0],
				result.attributes.texcoords[idx.texcoord_index * 3 + 1],
				result.attributes.texcoords[idx.texcoord_index * 3 + 2]
			});


			// Always triangles, so we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			// Just replicate the material ambient color for each vertex... 
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});

		}
	}
	ret.size = ret.positions.size();
	ret_textured.meshData = ret;
	return ret_textured;
}

std::vector<TexturedMeshData> load_wavefront_multi_obj_textured(char const* aPath)
{
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if(result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());
	
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines
	// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do
	// this for us.
	rapidobj::Triangulate(result);
	
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle
	// soup, ignoring the indexing information that the OBJ file contains.
	std::vector<TexturedMeshData> ret_multi;
	
	for (auto const& shape : result.shapes)
	{
		SimpleMeshData ret;
		TexturedMeshData ret_textures;
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			ret_textures.textureCoords.emplace_back(Vec3f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1],
				result.attributes.texcoords[idx.texcoord_index * 2 + 2],
			});

			// Always triangles, so we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			// Just replicate the material ambient color for each vertex... 
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});

		}
		ret.size = ret.positions.size();
		ret_textures.meshData = ret;
		ret_multi.push_back(ret_textures);
	}
	
	return ret_multi;
}

Texture load_texture( char const* aPath, int index = 0)
{
	// Ask rapidobj to load the requested file
	auto const result = rapidobj::ParseFile(aPath);
	if(result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());

	std::string const text_name = "assets/" + result.materials[index].diffuse_texname;

	if (text_name == "assets/") return Texture{0, "NONE"};

	unsigned int woodTexture;
	glGenTextures(1, &woodTexture);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load wooden container texture
	int woodTextureWidth, woodTextureHeight, woodTextureNRChannels;

	unsigned char* woodTextureData = stbi_load(text_name.c_str(), &woodTextureWidth, &woodTextureHeight, &woodTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, woodTextureWidth, woodTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, woodTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(woodTextureData);

	return Texture {woodTexture, "GL_TEXTURE_2D"};
}

std::vector<Texture> load_texture_multi( char const* aPath )
{
	std::vector<Texture> textures;
	auto const result = rapidobj::ParseFile(aPath);
	if(result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());

	for (int i = 0; i < result.materials.size(); i++)
	{
		textures.push_back(load_texture(aPath, i));
	}
	return textures;
}