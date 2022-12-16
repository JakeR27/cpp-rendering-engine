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

typedef struct _complexSceneObject
{
	SceneObject object;
	std::vector<SimpleMeshData> meshes;
	std::vector<GLuint> VAOs;

	size_t objectCount;
} ComplexSceneObject;

typedef struct _texturedSceneObject
{
	TexturedMeshData mesh;
	const char* filepath;

	Vec3f position;
	Vec3f scaling;
	Vec3f rotation;

	bool _initialised = false;
	GLuint VAO;
} TexturedSceneObject;

typedef struct _complexTexturedSceneObject
{
	SceneObject object;
	std::vector<TexturedMeshData> meshes;
	std::vector<GLuint> VAOs;

	size_t objectCount;
} ComplexTexturedSceneObject;

// load object and create VAO, must be called before sending to GPU
int initObject(SceneObject *aObject, char const* aPath);

// update VAO data with modified mesh data
void updateObject(SceneObject* aObject);

// send object data to GPU, must call initObject() before this can be used, must come after glUseProgram()
void drawObject(const SceneObject* aObject, const Mat44f projCamera);

// load object and create VAO, must be called before sending to GPU
int initComplexObject(ComplexSceneObject *aObject, char const* aPath);

// update VAO data with modified mesh data
void updateComplexObject(ComplexSceneObject* aObject);

// send object data to GPU, must call initObject() before this can be used, must come after glUseProgram()
void drawComplexObject(const ComplexSceneObject* aObject, const Mat44f projCamera);

// load object and create VAO, must be called before sending to GPU
int initTexturedObject(TexturedSceneObject *aObject, char const* aPath);

// update VAO data with modified mesh data
void updateTexturedObject(TexturedSceneObject* aObject);

// send object data to GPU, must call initObject() before this can be used, must come after glUseProgram()
void drawTexturedObject(const TexturedSceneObject* aObject, const Mat44f projCamera);

// load object and create VAO, must be called before sending to GPU
int initComplexTexturedObject(ComplexTexturedSceneObject *aObject, char const* aPath);

// update VAO data with modified mesh data
void updateComplexTexturedObject(ComplexTexturedSceneObject* aObject);

// send object data to GPU, must call initObject() before this can be used, must come after glUseProgram()
void drawComplexTexturedObject(const ComplexTexturedSceneObject* aObject, const Mat44f projCamera);

#endif//SCENE_OBJECT_HEADER_FILE