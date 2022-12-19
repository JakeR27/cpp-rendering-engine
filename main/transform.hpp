#ifndef TRANSFORM_HEADER_FILE
#define TRANSFORM_HEADER_FILE

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

class Transform
{
	Vec3f position = {0.f, 0.f, 0.f};
	Vec3f rotation = {0.f, 0.f, 0.f};
	Vec3f scale =	 {1.f, 1.f, 1.f};

public:
	Mat44f matrix();
	void setPosition(Vec3f aPosition);
	void setRotation(Vec3f aRotation);
	void setScale(Vec3f aScale);
};


#endif//TRANSFORM_HEADER_FILE
