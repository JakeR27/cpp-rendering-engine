#ifndef POINT_LIGHT_HEADER
#define POINT_LIGHT_HEADER

#include "../vmlib/vec3.hpp"

typedef struct _pointLight
{
	Vec3f position;
	Vec3f color;
	float brightness; // affects distance
} pointLight;

#endif //POINT_LIGHT_HEADER