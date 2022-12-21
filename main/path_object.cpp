#include "path_object.hpp"

void PathObj::updatePath(int speed)
{

	if (this->currentStep == 0 && this->currentPointIndex == 0)
	{
		this->setRotationAnchors(this->points[currentPointIndex].rotation, this->points[currentPointIndex+1].rotation);
		this->setPositionAnchors(this->points[currentPointIndex].position, this->points[currentPointIndex+1].position);
		this->setScaleAnchors(this->points[currentPointIndex].scale, this->points[currentPointIndex+1].scale);
		this->setupAnimation(
			points[currentPointIndex].steps, 
			points[currentPointIndex].interpolationStyle
		);
		currentPointIndex++;
	}

	this->animationStyle = STOP;
	this->updateAnimation(speed);

	// check if at end of current point to point
	if (this->currentStep >= this->totalSteps)
	{
		currentPointIndex++;
		// check if at end of path
		if (currentPointIndex >= this->points.size())
		{
			switch (pathStyle)
			{
				case STOP:
					{
						return;
					}
				case BOUNCE:
					{
						std::reverse(points.begin(), points.end());
						[[clang::fallthrough]];
					}
				case REPEAT:
					{
						currentPointIndex = 0;
					}
			}
		}

		// set next path point
		this->setRotationAnchors(rotationEnd, this->points[currentPointIndex].rotation);
		this->setPositionAnchors(positionEnd, this->points[currentPointIndex].position);
		this->setScaleAnchors(scaleEnd, this->points[currentPointIndex].scale);

		this->setupAnimation(
			points[currentPointIndex].steps, 
			points[currentPointIndex].interpolationStyle
		);
	}
}
