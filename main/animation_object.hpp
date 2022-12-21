#ifndef ANIMATION_OBJECT_HEADER_FILE
#define ANIMATION_OBJECT_HEADER_FILE

#include "scene_object.hpp"

enum INTERPOLATION_STYLE
{
	LINEAR,
	S_CURVE,
	CUBIC,
	SINUSOIDAL,
	INVERSE,
	RECIPROCAL
};

enum ANIMATION_STYLE
{
	STOP,
	REPEAT,
	BOUNCE
};

class AnimationObj : public SceneObj
{
	

	bool rotationSet = false, positionSet = false, scaleSet = false;
	INTERPOLATION_STYLE interpolationStyle;
	
	float interpolate(float in);

protected:
	Vec3f rotationStart, rotationEnd;
	Vec3f positionStart, positionEnd;
	Vec3f scaleStart, scaleEnd;
	size_t currentStep = 0;
	size_t totalSteps = 1;
	ANIMATION_STYLE animationStyle;
public:
	void setRotationAnchors(std::optional<Vec3f> aRotStart = std::nullopt, std::optional<Vec3f> aRotEnd = std::nullopt);
	void setPositionAnchors(std::optional<Vec3f> aPosStart = std::nullopt, std::optional<Vec3f> aPosEnd = std::nullopt);
	void setScaleAnchors(std::optional<Vec3f> aScaleStart = std::nullopt, std::optional<Vec3f> aScaleEnd = std::nullopt);
	void setupAnimation(size_t aSteps, INTERPOLATION_STYLE aIntStyle, ANIMATION_STYLE aAnimStyle = STOP);
	void updateAnimation();

};

#endif//ANIMATION_OBJECT_HEADER_FILE