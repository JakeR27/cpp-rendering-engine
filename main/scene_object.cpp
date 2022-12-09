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
		printf("Error initialising object! Failed to load mesh from file");
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
