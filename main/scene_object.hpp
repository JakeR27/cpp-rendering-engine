#ifndef SCENE_OBJECT_HEADER_FILE
#define SCENE_OBJECT_HEADER_FILE

#include "simple_mesh.hpp"
#include "mesh_data.hpp"
#include "transform.hpp"
#include "../vmlib/mat44.hpp"
#include "rapidobj/rapidobj.hpp"

class SceneObj
{
	std::string filepath;
	Transform transform;

	std::vector<MeshData>	meshes;
	std::vector<Material>	materials;
	std::vector<GLuint>		VAOs;
	size_t					meshCount;

	bool initialised = false;

	int loadMaterials(rapidobj::Materials);
	int loadWavefrontObj();
	GLuint createVAO(MeshData aMeshData, std::optional<GLuint> aVAO = std::nullopt);
	int generateVAOs();

public:
	int initialise(std::string aPath);
	int updateVAO();
	void move(Vec3f aVec) {transform.setPosition(aVec);}
	int draw(Mat44f aProjCamera);
	void forceFakeTexCoords();
	void forceTexture(std::string aPath);
};

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

#endif//SCENE_OBJECT_HEADER_FILE