#include "animation_object.hpp"

float AnimationObj::interpolate(float in)
{
	if (in < 0) in = 0;
	else if (in > 1) in = 1;

	float out = 0;

	switch (this->interpolationStyle)
	{
	case LINEAR:
		{
			out = in;
			break;
		}
	case S_CURVE:
		{
			const float kEuler = 2.718281828;
			float denom = 1 + powf(kEuler, -10.f * (in - 0.5f));
			out = 1 / denom;
			break;
		}
	case CUBIC:
		{
			out = in;
			break;
		}
	case SINUSOIDAL:
		{
			const float u = (3 * in) - 1.5f;
			out = u - (0.166666f * powf(u, 3.f)) + (0.008333f * powf(u, 5.f));
			out = (0.5f * out) + 0.5f;
			break;
		}
	case INVERSE:
		{
			float denom = 1.f - (0.5f * in);
			out = (1.f / denom) - 1.f;
			break;
		}
	case RECIPROCAL:
		{
			float denom = -(2.f * in + 0.74f);
			out = (1.f / denom) + 1.36f;
			break;
		}
	}

	if (out > 1) out = 1;
	else if (out < 0) out = 0;
	return out;
}

void AnimationObj::setRotationAnchors(std::optional<Vec3f> aRotStart, std::optional<Vec3f> aRotEnd)
{
	if (aRotStart) rotationStart = *aRotStart;
	if (aRotEnd) rotationEnd = *aRotEnd;
	rotationSet = true;
}

void AnimationObj::setPositionAnchors(std::optional<Vec3f> aPosStart, std::optional<Vec3f> aPosEnd)
{
	if (aPosStart) positionStart = *aPosStart;
	if (aPosEnd) positionEnd = *aPosEnd;
	positionSet = true;
}

void AnimationObj::setScaleAnchors(std::optional<Vec3f> aScaleStart, std::optional<Vec3f> aScaleEnd)
{
	if (aScaleStart) scaleStart = *aScaleStart;
	if (aScaleEnd) scaleEnd = *aScaleEnd;
	scaleSet = true;
}


void AnimationObj::setupAnimation(size_t aSteps, INTERPOLATION_STYLE aIntStyle, ANIMATION_STYLE aAnimStyle)
{
	currentStep = 0;
	totalSteps = aSteps;

	interpolationStyle = aIntStyle;
	animationStyle = aAnimStyle;
}

void AnimationObj::updateAnimation(int speed)
{
	currentStep += speed;

	if (currentStep >= totalSteps)
	{
		switch (animationStyle)
		{
			case STOP: return;
			case BOUNCE:
				{
					if (rotationSet) std::swap(rotationStart, rotationEnd);
					if (positionSet) std::swap(positionStart, positionEnd);
					if (scaleSet) std::swap(scaleStart, scaleEnd);
					// falls thru on purpose
					[[clang::fallthrough]];
				}
			case REPEAT:
				{
					currentStep = 0;
				}
		}
	}
	

	float input = static_cast<float>(currentStep) / static_cast<float>(totalSteps);
	float t = interpolate(input);

	if (rotationSet)
	{
		this->transform.setRotation(
			(1-t) * rotationStart + (t) * rotationEnd
		);
	}
	if (positionSet)
	{
		this->transform.setPosition(
			(1-t) * positionStart + (t) * positionEnd
		);
	}
	if (scaleSet)
	{
		this->transform.setScale(
			(1-t) * scaleStart + (t) * scaleEnd
		);
	}
}
