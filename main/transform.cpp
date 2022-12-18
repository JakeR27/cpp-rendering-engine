#include "transform.hpp"

Mat44f Transform::matrix()
{
	Mat44f rotationTransform = make_rotation_z(this->rotation.z)
							 * make_rotation_y(this->rotation.y)
							 * make_rotation_x(this->rotation.x);

	Mat44f finalTransform =  make_translation(this->position)
						  *  make_scaling(this->scale.x, this->scale.y, this->scale.z)
						  *  rotationTransform;

	return finalTransform;
}

void Transform::setPosition(Vec3f aPosition)
{
	position = aPosition;
}

void Transform::setRotation(Vec3f aRotation)
{
	rotation = aRotation;
}

void Transform::setScale(Vec3f aScale)
{
	scale = aScale;
}

