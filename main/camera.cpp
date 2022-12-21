#include "camera.hpp"

#include <GLFW/glfw3.h>

inline void _cam_handle_keyaction(bool* aCameraAction, int aAction)
{
	*aCameraAction = aAction == GLFW_PRESS ? true : (aAction == GLFW_RELEASE ? false : *aCameraAction);
}

void cam_handle_key(cameraControl* aCamera, int aKey, int aAction)
{
	if( !aCamera->cameraActive ) return;

	switch (aKey)
	{
		case GLFW_KEY_W: _cam_handle_keyaction(&aCamera->actionForwards, aAction); break;
		case GLFW_KEY_S: _cam_handle_keyaction(&aCamera->actionBackwards, aAction); break;
		case GLFW_KEY_A: _cam_handle_keyaction(&aCamera->actionLeft, aAction); break;
		case GLFW_KEY_D: _cam_handle_keyaction(&aCamera->actionRight, aAction); break;
		case GLFW_KEY_E: _cam_handle_keyaction(&aCamera->actionUp, aAction); break;
		case GLFW_KEY_Q: _cam_handle_keyaction(&aCamera->actionDown, aAction); break;
	}
}

constexpr float kPi = 3.1415926;

void cam_handle_mouse(cameraControl* aCamera, const float aMouseSensitivity, double aX, double aY)
{
	if( aCamera->cameraActive )
	{
		auto const dx = float(aX-aCamera->lastX);
		auto const dy = float(aY-aCamera->lastY);

		aCamera->phi += dx*aMouseSensitivity;
		aCamera->theta += dy*aMouseSensitivity;

		if( aCamera->theta > kPi/2.f ) 
		{
			aCamera->theta = kPi/2.f;
		}
		else if( aCamera->theta < -kPi/2.f )
		{
			aCamera->theta = -kPi/2.f;
		}
	}


	aCamera->lastX = float(aX);
	aCamera->lastY = float(aY);
	
}