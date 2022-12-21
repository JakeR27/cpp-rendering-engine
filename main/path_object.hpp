#ifndef PATH_OBJECT_HEADER_FILE
#define PATH_OBJECT_HEADER_FILE

#include "animation_object.hpp"

struct PathPoint
{
	Vec3f rotation = {0.f, 0.f, 0.f};
	Vec3f position = {0.f, 0.f, 0.f};
	Vec3f scale = {1.f, 1.f, 1.f};

	size_t steps = 100;
	INTERPOLATION_STYLE interpolationStyle = SINUSOIDAL;
};

class PathObj : public AnimationObj
{
	std::vector<PathPoint> points;

	size_t currentPointIndex = 0;

	ANIMATION_STYLE pathStyle;

public:
	void setupPath(ANIMATION_STYLE aPathStyle = REPEAT) {pathStyle = aPathStyle;}
	void addPathPoint(const PathPoint aPathPoint) {points.push_back(aPathPoint);}
	void updatePath();
};

#endif//PATH_OBJECT_HEADER_FILE
