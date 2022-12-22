#ifndef SCREENSHOT_HEADER_FILE
#define SCREENSHOT_HEADER_FILE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

struct ScreenshotData
{
	std::vector<char> buffer;
	int width, height;
	GLsizei channels, stride;
};

void saveScreenshot(ScreenshotData aData, std::string aFilepath)
{


	//save image
	stbi_flip_vertically_on_write(true);
	//stbi_write_png("test2.png", width, height, channels, buffer.data(), 0);
	stbi_write_png(aFilepath.c_str(), aData.width, aData.height, aData.channels, aData.buffer.data(), aData.stride);

	//printf(stbi_failure_reason());

}

ScreenshotData getScreenshotData(GLFWwindow* aWindow)
{
	//get size of window
	ScreenshotData data;
	glfwGetFramebufferSize(aWindow, &data.width, &data.height);

	// calculate info
	data.channels = 3;
	data.stride = data.channels * data.width;
	data.stride += (data.stride % 4) ? (4 - data.stride % 4) : 0;

	// create buffer
	GLsizei bufferSize = data.stride * data.height;
	data.buffer = std::vector<char>(bufferSize);

	// read buffer 
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, data.width, data.height, GL_RGB, GL_UNSIGNED_BYTE, data.buffer.data());
	return data;
}

#endif//SCREENSHOT_HEADER_FILE
