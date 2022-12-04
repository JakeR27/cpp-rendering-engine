#ifndef CAMERA_CONTROL
#define CAMERA_CONTROL

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

typedef struct _cameraControl
		{
			bool cameraActive;
			bool actionForwards, actionBackwards,
				 actionLeft, actionRight,
				 actionUp, actionDown;

			Vec3f position;
			Vec3f direction;
			Vec3f up;
			
		} cameraControl;

// Vector to move camera to the right by 1 unit
Vec3f cam_right(const cameraControl* camera)
{
	return normalize(cross(camera->direction, camera->up));
	
}

// Vector to move camera to the left by 1 unit
Vec3f cam_left(const cameraControl* camera)
{
	return -normalize(cross(camera->direction, camera->up));
	
}

// Vector to move camera up by 1 unit
Vec3f cam_up(const cameraControl* camera)
{
	return normalize(camera->up);
}

// Vector to move camera down by 1 unit
Vec3f cam_down(const cameraControl* camera)
{
	return -normalize(camera->up);
}

// Vector to move camera forwards by 1 unit
Vec3f cam_forwards(const cameraControl* camera)
{
	return normalize(camera->direction);
}

// Vector to move camera backwards by 1 unit
Vec3f cam_backwards(const cameraControl* camera)
{
	return -normalize(camera->direction);
}
#endif // CAMERA_CONTROL
