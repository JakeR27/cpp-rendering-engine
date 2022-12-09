#ifndef SCENE_OBJECT_HEADER_FILE
#define SCENE_OBJECT_HEADER_FILE

#include "simple_mesh.hpp"
#include "../vmlib/mat44.hpp"


typedef struct _sceneObject
{
	SimpleMeshData mesh;
	const char* filepath;

	Vec3f position;
	Vec3f scaling;
	Vec3f rotation;

	bool _initialised = false;
	GLuint VAO;
} SceneObject;

// load object and create VAO, must be called before sending to GPU
int initObject(SceneObject *aObject, char const* aPath);

// update VAO data with modified mesh data
void updateObject(SceneObject* aObject);

// send object data to GPU, must call initObject() before this can be used, must come after glUseProgram()
void drawObject(const SceneObject* aObject, const Mat44f projCamera);

#endif//SCENE_OBJECT_HEADER_FILE