#ifndef CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
#define CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B

// This defines the vertex data for a colored unit cube.

constexpr float const kCubePositions[] = {
	+1.f, +1.f, -1.f,
	-1.f, +1.f, -1.f,
	-1.f, +1.f, +1.f,
	+1.f, +1.f, -1.f,
	-1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	+1.f, -1.f, +1.f,
	+1.f, +1.f, +1.f,
	-1.f, +1.f, +1.f,
	+1.f, -1.f, +1.f,
	-1.f, +1.f, +1.f,
	-1.f, -1.f, +1.f,

	-1.f, -1.f, +1.f,
	-1.f, +1.f, +1.f,
	-1.f, +1.f, -1.f,
	-1.f, -1.f, +1.f,
	-1.f, +1.f, -1.f,
	-1.f, -1.f, -1.f,

	-1.f, -1.f, -1.f,
	+1.f, -1.f, -1.f,
	+1.f, -1.f, +1.f,
	-1.f, -1.f, -1.f,
	+1.f, -1.f, +1.f,
	-1.f, -1.f, +1.f,

	+1.f, -1.f, -1.f,
	+1.f, +1.f, -1.f,
	+1.f, +1.f, +1.f,
	+1.f, -1.f, -1.f,
	+1.f, +1.f, +1.f,
	+1.f, -1.f, +1.f,

	-1.f, -1.f, -1.f,
	-1.f, +1.f, -1.f,
	+1.f, +1.f, -1.f,
	-1.f, -1.f, -1.f,
	+1.f, +1.f, -1.f,
	+1.f, -1.f, -1.f,
};

constexpr float const kCubeColors[] = {
	// top face
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	// north (front) face
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	// east (right) face
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	// bottom face
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	// west (left) face
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	//south (back) face
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f
};

constexpr float const kCubeTextureCoord[] = {
	// top face
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f
};

static_assert( sizeof(kCubeColors) == sizeof(kCubePositions),
	"Size of cube colors and cube positions do not match. Both are 3D vectors."
);

#endif // CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
