#include "scene_object.hpp"
#include "loadobj.hpp"

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

//int initTexturedObject(TexturedSceneObject *aObject, char const* aPath)
//{
//	if (aObject->_initialised == true)
//	{
//		printf("Error initialising object! Object already initialised");
//		return -1;
//	}
//
//	// load object from path;
//	aObject->filepath = aPath;
//	try
//	{
//		aObject->mesh.meshData = load_wavefront_obj(aObject->filepath);
//		aObject->mesh.texture = load_texture(aObject->filepath);
//	} catch  (const std::exception& ex)
//	{
//		printf("Error: %s\n", ex.what());
//		printf("Error initialising object! Failed to load mesh from file: %s\n", aPath);
//		return -1;
//	}
//
//	aObject->position = {0.f, 0.f, 0.f};
//	aObject->rotation = {0.f, 0.f, 0.f};
//	aObject->scaling = {1.f, 1.f, 1.f};
//	aObject->VAO = create_vao(aObject->mesh.meshData);
//
//	aObject->_initialised = true;
//
//	return 0;
//}

void updateTexturedObject(TexturedSceneObject* aObject)
{
	create_vao(aObject->mesh.meshData, aObject->VAO);
}

void drawTextureObject(const TexturedSceneObject* aObject, const Mat44f projCamera)
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

	glBindTexture(GL_TEXTURE_2D, aObject->mesh.texture.id);

	glBindVertexArray(aObject->VAO);
	glDrawArrays(GL_TRIANGLES, 0, aObject->mesh.meshData.size);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int initComplexTexturedObject(ComplexTexturedSceneObject *aObject, char const* aPath)
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
		aObject->meshes = load_wavefront_multi_obj_textured(aObject->object.filepath);

		std::vector<Texture> textures = load_texture_multi(aObject->object.filepath);
		for (int i = 0; i < aObject->objectCount; i++)
		{
			aObject->meshes[i].texture = textures[i];
		}
		
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
		aObject->VAOs.push_back(create_vao(mesh.meshData));
	}

	aObject->objectCount = aObject->meshes.size();

	aObject->object._initialised = true;

	return 0;
}

void updateComplexTexturedObject(ComplexTexturedSceneObject* aObject)
{
	for(int i = 0; i < aObject->objectCount; i++)
	{
		create_vao(aObject->meshes[i].meshData, aObject->VAOs[i]);
	}
}

void drawComplexTexturedObject(const ComplexTexturedSceneObject* aObject, const Mat44f projCamera)
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
		glBindTexture(GL_TEXTURE_2D, aObject->meshes[i].texture.id);

		glBindVertexArray(aObject->VAOs[i]);
		glDrawArrays(GL_TRIANGLES, 0, aObject->meshes[i].meshData.size);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}