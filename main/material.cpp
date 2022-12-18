#include "material.hpp"

#include <filesystem>
#include <stb_image.h>

#include "../vmlib/mat44.hpp"

void Material::setAmbient(Vec3f aAmbient)
{
	kA = aAmbient;
}

void Material::setDiffuse(Vec3f aDiffuse)
{
	kD = aDiffuse;
}

void Material::setSpecular(Vec3f aSpecular)
{
	kS = aSpecular;
}

void Material::setEmissive(Vec3f aEmissive)
{
	kE = aEmissive;
}

void Material::setTexture(std::string aPath)
{
	textureFilepath = aPath;
}

int Material::loadTexture()
{
	if (std::filesystem::exists(this->textureFilepath) == false)
	{
		return -1;
	}

	glGenTextures(1, &this->textureId);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load wooden container texture
	int textureWidth, textureHeight, woodTextureChannels;
	unsigned char* textureData = stbi_load(this->textureFilepath.c_str(), &textureWidth, &textureHeight, &woodTextureChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(textureData);

	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}

void Material::useMaterial()
{
	Mat44f materialPacked = {
			kA.x, kA.y, kA.z, 0.f,
			kD.x, kD.y, kD.z, 0.f,  
			kS.x, kS.y, kS.z, 0.f,
			kE.x, kE.y, kE.z, 0.f
	};

	glBindTexture(GL_TEXTURE_2D, textureId);

	glUniformMatrix4fv(
		3, 1,
		GL_FALSE, materialPacked.v
	);
}






