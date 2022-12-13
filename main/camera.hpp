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
inline Vec3f cam_right(const cameraControl* aCamera)
{
	Vec4f dir = make_rotation_y(-aCamera->phi) * Vec4f{aCamera->forwards.x, aCamera->forwards.y, aCamera->forwards.z, 1.0f};
	return normalize(cross({dir.x, dir.y, dir.z}, aCamera->up));
	
}

// Vector to move aCamera to the left by 1 unit
inline Vec3f cam_left(const cameraControl* aCamera)
{
	return -cam_right(aCamera);
}

// Vector to move camera up by 1 unit
inline Vec3f cam_up(const cameraControl* aCamera)
{
	return normalize(aCamera->up);
}

// Vector to move camera down by 1 unit
inline Vec3f cam_down(const cameraControl* aCamera)
{
	return -normalize(aCamera->up);
}

// Vector to move camera forwards by 1 unit
inline Vec3f cam_forwards(const cameraControl* aCamera)
{
	Vec4f dir = make_rotation_y(-aCamera->phi) * Vec4f{aCamera->forwards.x, aCamera->forwards.y, aCamera->forwards.z, 1.0f};
	return normalize({dir.x, dir.y, dir.z});
}

// Vector to move camera backwards by 1 unit
inline Vec3f cam_backwards(const cameraControl* aCamera)
{
	return -cam_forwards(aCamera);
}

inline void cam_action_reset(cameraControl* aCamera)
{
	aCamera->actionForwards = false;
	aCamera->actionBackwards = false;
	aCamera->actionLeft = false;
	aCamera->actionRight = false;
	aCamera->actionUp = false;
	aCamera->actionDown = false;
}

inline void cam_toggle(cameraControl* aCamera)
{
	aCamera->cameraActive = !aCamera->cameraActive;
}

void cam_handle_key(cameraControl* aCamera, int aKey, int aAction);

void cam_handle_mouse(cameraControl*, const float, double, double);

#endif // CAMERA_CONTROL
