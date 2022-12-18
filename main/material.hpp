#ifndef MATERIAL_HEADER_FILE
#define MATERIAL_HEADER_FILE

#include <string>
#include <vector>
#include <glad.h>
#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

class Material
{
	Vec3f kA = {0.1f, 0.1f, 0.1f};
	Vec3f kD = {0.8f, 0.8f, 0.8f};
	Vec3f kS = {0.8f, 0.8f, 0.8f};
	Vec3f kE = {0.0f, 0.0f, 0.0f};

	std::string textureFilepath;
	GLuint textureId = 0;
	bool textureLoaded = false;

public:
	void setAmbient(Vec3f aAmbient);
	void setDiffuse(Vec3f aDiffuse);
	void setSpecular(Vec3f aSpecular);
	void setEmissive(Vec3f aEmissive);
	void setTexture(std::string aPath);

	int loadTexture();
	void useMaterial();
};


#endif//MATERIAL_HEADER_FILE
