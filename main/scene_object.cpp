#include "scene_object.hpp"
#include "loadobj.hpp"
#include "../support/error.hpp"

int initObject(SceneObject *aObject, char const* aPath)
{
	if (aObject->_initialised == true)
	{
		printf("Error initialising object! Object already initialised");
		return -1;
	}

	// load object from path;
	aObject->filepath = aPath;
	try
	{
		aObject->mesh = load_wavefront_obj(aObject->filepath);
	} catch  (const std::exception& ex)
	{
		printf("Error: %s\n", ex.what());
		printf("Error initialising object! Failed to load mesh from file: %s\n", aPath);
		return -1;
	}

	aObject->position = {0.f, 0.f, 0.f};
	aObject->rotation = {0.f, 0.f, 0.f};
	aObject->scaling = {1.f, 1.f, 1.f};
	aObject->VAO = create_vao(aObject->mesh);

	aObject->_initialised = true;

	return 0;
}

void updateObject(SceneObject* aObject)
{
	create_vao(aObject->mesh, aObject->VAO);
}

void drawObject(const SceneObject* aObject, const Mat44f projCamera)
{
	if (aObject->_initialised == false) return;

	Mat44f rotationTransform = make_rotation_z(aObject->rotation.z) * make_rotation_y(aObject->rotation.y) * make_rotation_x(aObject->rotation.x);
	Mat44f modelTransform = make_translation(aObject->position) * make_scaling(aObject->scaling.x, aObject->scaling.y, aObject->scaling.z) * rotationTransform;
	Mat44f finalTransform = projCamera * modelTransform;

	glUniformMatrix4fv(
		0, 1,
		GL_TRUE, finalTransform.v
	);

	glUniformMatrix4fv(
		1, 1,
		GL_TRUE, modelTransform.v
	);

	glBindVertexArray(aObject->VAO);
	glDrawArrays(GL_TRIANGLES, 0, aObject->mesh.size);

	glBindVertexArray(0);
}

int SceneObj::loadMaterials(rapidobj::Materials aMaterials)
{
	//create materials
	for (auto const& material: aMaterials)
	{
		Material loadedMaterial = Material();
		loadedMaterial.setAmbient(material.ambient);
		loadedMaterial.setDiffuse(material.diffuse);
		loadedMaterial.setSpecular(material.specular);
		loadedMaterial.setEmissive(material.emission);

		loadedMaterial.setTexture(material.diffuse_texname);
		loadedMaterial.loadTexture();

		this->materials.push_back(loadedMaterial);
	}
	return 0;
}


int SceneObj::loadWavefrontObj()
{
	auto result = rapidobj::ParseFile(this->filepath);

	if (result.error)
	{
		throw Error("Unable to load OBJ file '%s': %s", this->filepath, result.error.code.message().c_str());
	}

	// OBJs may contain non triangle faces, rapidobj will handle this for us
	rapidobj::Triangulate(result);

	//create materials
	this->loadMaterials(result.materials);
	
	for (auto const& shape : result.shapes)
	{
		MeshData loadedMesh = MeshData();
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			loadedMesh.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			loadedMesh.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			if (idx.texcoord_index != -1)
			{
				loadedMesh.texcoords.emplace_back(Vec2f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1]
				});
			}

			


			// Always triangles, so we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			// Just replicate the material ambient color for each vertex... 
			loadedMesh.colors.emplace_back(Vec3f{
				/*mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]*/
				1.f, 1.f, 1.f
				});
			loadedMesh.material = this->materials[shape.mesh.material_ids[i/3]];

		}
		loadedMesh.size = loadedMesh.positions.size();
		//loadedMesh.material = this->materials[shape.mesh.material_ids[0]];
		this->meshes.push_back(loadedMesh);
	}
	this->meshCount = this->meshes.size();
}

GLuint SceneObj::createVAO(const MeshData aMeshData, std::optional<GLuint> aVAO)
{
	// Simple Mesh Position VBO
	GLuint meshPositionVBO = 0;
	glGenBuffers(1, &meshPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

	// Simple mesh Color VBO
	GLuint meshColorVBO = 0;
	glGenBuffers(1, &meshColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshColorVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

	// Normals VBO
	GLuint meshNormalVBO = 0;
	glGenBuffers(1, &meshNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);
	
	// Texture Coords VBO
	GLuint meshTextureCoordVBO = 0;
	glGenBuffers(1, &meshTextureCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshTextureCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f), aMeshData.texcoords.data(), GL_STATIC_DRAW);

	// Bind VBO into VAO
	GLuint MeshDataVAO = 0;
	if (aVAO)
	{
		glBindVertexArray(*aVAO);
	}
	else 
	{
		glGenVertexArrays(1, &MeshDataVAO);
		glBindVertexArray(MeshDataVAO);
	}

	glBindBuffer(GL_ARRAY_BUFFER, meshPositionVBO);
	glVertexAttribPointer(
		0,						// loc 0 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, meshColorVBO);
	glVertexAttribPointer(
		1,						// loc 1 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, meshNormalVBO);
	glVertexAttribPointer(
		2,						// loc 2 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, meshTextureCoordVBO);
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


	return MeshDataVAO;
} 

int SceneObj::generateVAOs()
{
	for (auto const& mesh : this->meshes)
	{
		this->VAOs.push_back(this->createVAO(mesh));
	}
	return 0;
}



int SceneObj::initialise(std::string aPath)
{
	// check object not already initialised
	if (this->initialised) return -1;

	this->filepath = aPath;
	this->loadWavefrontObj();
	this->generateVAOs();

	this->transform = Transform();
	this->initialised = true;
	return 0;
}

int SceneObj::updateVAO()
{
	for(int i = 0; i < this->meshCount; i++)
	{
		this->createVAO(this->meshes[i], this->VAOs[i]);
	}
	return 0;
}

int SceneObj::draw(const Mat44f aProjCamera)
{
	if (this->initialised == false) return -1;

	Mat44f modelTransform = this->transform.matrix();
	Mat44f finalTransform = aProjCamera * modelTransform;

	glUniformMatrix4fv(
		0, 1,
		GL_TRUE, finalTransform.v
	);

	glUniformMatrix4fv(
		1, 1,
		GL_TRUE, modelTransform.v
	);

	for(int i = 0; i < this->meshCount; i++)
	{
		this->meshes[i].material.useMaterial();

		glBindVertexArray(this->VAOs[i]);
		glDrawArrays(GL_TRIANGLES, 0, this->meshes[i].size);
	}
	

	glBindVertexArray(0);

	return 0;
}

void SceneObj::forceTexture(std::string aPath)
{
	for(auto & [positions, colors, normals, texcoords, material, size] : this->meshes)
	{
		texcoords.clear();
		for (int j = 0; j < size; j++)
		{
			texcoords.push_back({
				positions[j].x,
				positions[j].y,
			});
		}
	}

	for (int i = 0; i < this->meshCount; i++)
	{
		this->meshes[i].material.setTexture(aPath);
		this->meshes[i].material.loadTexture();
	}

	this->updateVAO();
}


int initComplexObject(ComplexSceneObject *aObject, char const* aPath)
{
	if (aObject->object._initialised == true)
	{
		printf("Error initialising object! Object already initialised");
		return -1;
	}

	// load object from path;
	aObject->object.filepath = aPath;
	try
	{
		aObject->meshes = load_wavefront_multi_obj(aObject->object.filepath);
	} catch  (const std::exception& ex)
	{
		printf("Error: %s\n", ex.what());
		printf("Error initialising object! Failed to load mesh from file: %s\n", aPath);
		return -1;
	}

	aObject->object.position = {0.f, 0.f, 0.f};
	aObject->object.rotation = {0.f, 0.f, 0.f};
	aObject->object.scaling = {1.f, 1.f, 1.f};

	for (auto const& mesh : aObject->meshes)
	{
		aObject->VAOs.push_back(create_vao(mesh));
	}

	aObject->objectCount = aObject->meshes.size();

	aObject->object._initialised = true;

	return 0;
}

void updateComplexObject(ComplexSceneObject* aObject)
{
	for(int i = 0; i < aObject->objectCount; i++)
	{
		create_vao(aObject->meshes[i], aObject->VAOs[i]);
	}
}

void drawComplexObject(const ComplexSceneObject* aObject, const Mat44f projCamera)
{
	if (aObject->object._initialised == false) return;

	Mat44f rotationTransform = make_rotation_z(aObject->object.rotation.z) * make_rotation_y(aObject->object.rotation.y) * make_rotation_x(aObject->object.rotation.x);
	Mat44f modelTransform = make_translation(aObject->object.position) * make_scaling(aObject->object.scaling.x, aObject->object.scaling.y, aObject->object.scaling.z) * rotationTransform;
	Mat44f finalTransform = projCamera * modelTransform;
	Mat44f secondFinalTransform = projCamera * modelTransform * make_translation({1.f, 1.f, 1.f});

	glUniformMatrix4fv(
		0, 1,
		GL_TRUE, finalTransform.v
	);

	glUniformMatrix4fv(
		1, 1,
		GL_TRUE, modelTransform.v
	);

	for(int i = 0; i < aObject->objectCount; i++)
	{
		glBindVertexArray(aObject->VAOs[i]);
		glDrawArrays(GL_TRIANGLES, 0, aObject->meshes[i].size);
	}
	

	glBindVertexArray(0);
}