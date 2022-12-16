#ifndef LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
#define LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F

#include "simple_mesh.hpp"

SimpleMeshData load_wavefront_obj( char const* aPath );

std::vector<SimpleMeshData> load_wavefront_multi_obj( char const* aPath );

TexturedMeshData load_wavefront_obj_textured( char const* aPath );

std::vector<TexturedMeshData> load_wavefront_multi_obj_textured( char const* aPath );

Texture load_texture( char const* aPath );

std::vector<Texture> load_texture_multi( char const* aPath );

#endif // LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
