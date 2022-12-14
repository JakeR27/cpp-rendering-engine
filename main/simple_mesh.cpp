#include "simple_mesh.hpp"

SimpleMeshData concatenate( SimpleMeshData aM, SimpleMeshData const& aN )
{
	aM.positions.insert( aM.positions.end(), aN.positions.begin(), aN.positions.end() );
	aM.colors.insert( aM.colors.end(), aN.colors.begin(), aN.colors.end() );
	return aM;
}

// VBO for positions and VBO for colours, combined into one VAO that we return
GLuint create_vao( SimpleMeshData const& aMeshData, std::optional<GLuint> VAO)
{
	// Simple Mesh Position VBO
	GLuint simpleMeshPositionVBO = 0;
	glGenBuffers(1, &simpleMeshPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

	// Simple mesh Color VBO
	GLuint simpleMeshColorVBO = 0;
	glGenBuffers(1, &simpleMeshColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshColorVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

	// Normals VBO
	GLuint simpleMeshNormalVBO = 0;
	glGenBuffers(1, &simpleMeshNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);

	// Texture Coords VBO
	GLuint simpleMeshTextureCoordsVBO = 0;

	// Bind VBO into VAO
	GLuint simpleMeshVAO = 0;
	if (VAO)
	{
		glBindVertexArray(*VAO);
	}
	else 
	{
		glGenVertexArrays(1, &simpleMeshVAO);
		glBindVertexArray(simpleMeshVAO);
	}

	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshPositionVBO);
	glVertexAttribPointer(
		0,						// loc 0 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshColorVBO);
	glVertexAttribPointer(
		1,						// loc 1 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshNormalVBO);
	glVertexAttribPointer(
		2,						// loc 2 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, simpleMeshTextureCoordsVBO);
	glVertexAttribPointer(
		3,						// loc 3 in vert shader
		2, GL_FLOAT, GL_FALSE,	// 2 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(3);

	// Reset state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Clean up simple mesh VBOs
	// Note: these are not deleted fully, as the VAO holds a reference to them.
	/*glDeleteBuffers(1, &simpleMeshColorVBO);
	glDeleteBuffers(1, &simpleMeshPositionVBO);*/

	return simpleMeshVAO;
}

