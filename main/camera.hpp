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
			Vec3f forwards;
			Vec3f up;

			float theta, phi;
			float lastX, lastY;
			
		} cameraControl;

// Vector to move camera to the right by 1 unit
inline Vec3f cam_right(const cameraControl* camera)
{
	Vec4f dir = make_rotation_y(-camera->phi) * Vec4f{camera->forwards.x, camera->forwards.y, camera->forwards.z, 1.0f};
	return normalize(cross({dir.x, dir.y, dir.z}, camera->up));
	
}

// Vector to move camera to the left by 1 unit
inline Vec3f cam_left(const cameraControl* camera)
{
	return -cam_right(camera);
}

// Vector to move camera up by 1 unit
inline Vec3f cam_up(const cameraControl* camera)
{
	return normalize(camera->up);
}

// Vector to move camera down by 1 unit
inline Vec3f cam_down(const cameraControl* camera)
{
	return -normalize(camera->up);
}

// Vector to move camera forwards by 1 unit
inline Vec3f cam_forwards(const cameraControl* camera)
{
	Vec4f dir = make_rotation_y(-camera->phi) * Vec4f{camera->forwards.x, camera->forwards.y, camera->forwards.z, 1.0f};
	return normalize({dir.x, dir.y, dir.z});
}

// Vector to move camera backwards by 1 unit
inline Vec3f cam_backwards(const cameraControl* camera)
{
	return -cam_forwards(camera);
}
#endif // CAMERA_CONTROL
