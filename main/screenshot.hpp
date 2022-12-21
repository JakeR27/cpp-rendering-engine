#ifndef SCREENSHOT_HEADER_FILE
#define SCREENSHOT_HEADER_FILE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

void saveScreenshot(GLFWwindow* aWindow, std::string aFilepath)
{
	//get size of window
	int width, height;
	glfwGetFramebufferSize(aWindow, &width, &height);

	// calculate info
	GLsizei channels = 3;
	GLsizei stride = channels * width;
	stride += (stride % 4) ? (4 - stride % 4) : 0;

	// create buffer
	GLsizei bufferSize = stride * height;
	std::vector<char> buffer(bufferSize);

	// read buffer 
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

	//save image
	stbi_flip_vertically_on_write(true);
	//stbi_write_png("test2.png", width, height, channels, buffer.data(), 0);
	stbi_write_png(aFilepath.c_str(), width, height, channels, buffer.data(), stride);

	printf(stbi_failure_reason());

}

#endif//SCREENSHOT_HEADER_FILE
