#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "defaults.hpp"
#include "complex_object.hpp"
#include "camera.hpp"

// ex 4
#include "cone.hpp"
#include "cylinder.hpp"
#include "loadobj.hpp"
#include "point_light.hpp"
#include "scene_object.hpp"
#include "animation_object.hpp"
#include "path_object.hpp"

// include STB_IMAGE for texture mapping, provided in the "third_party" directory
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

#include "screenshot.hpp"

namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - Coursework 2";
	constexpr float const kMouseSensitivity = 0.01f;
	constexpr float const kMovementSensitivity = 2.f;
	constexpr float const kPi = 3.1415962f;
	constexpr size_t kLightCount = 3;
	float kFlightSpeed = 3.f;
	float kNormFlightSpeed = 3.f;
	float kSlowFlightSpeed = 1.f;
	float kFastFlightSpeed = 8.f;

	struct State_
	{
		cameraControl camControl;
		GLenum polygonMode;
		pointLight sceneLights[kLightCount];
		int currentLight = 0;
		int animationFactor = 1;
		bool animationPause = false;
		bool screenshotQueued = false;
		bool fastFlight = false;
		bool slowFlight = false;
	};

	void glfw_callback_error_( int, char const* );

	void glfw_callback_key_( GLFWwindow*, int, int, int, int );
	void glfw_callback_motion_( GLFWwindow*, double, double );

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

int main() try
{
	//####################### SETUP #######################
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Store state in window data
	State_ state{};
	glfwSetWindowUserPointer( window, &state );

	// Initialise camera info
	state.camControl.position = {0.f, -1.f, -5.f};
	state.camControl.direction = {0.f , 0.f, 1.f};
	state.camControl.forwards = {0.f , 0.f, 1.f};
	state.camControl.up = {0.f , 1.0f, 0.f};

	// Setup lights
	//constexpr size_t kLightCount = 2;
	//pointLight sceneLights[kLightCount];
	/*state.sceneLights[0] = {
		{0.f, 4.5f, 0.5f},
		{1.f, 1.f, 0.f}
	};

	state.sceneLights[1] = {
		{0.f, 1.5f, 0.5f},
		{0.f, 1.f, 1.f}
	};*/

	// SE
	state.sceneLights[0] = {
		{-4.45f, 3.55f, -4.45f},
		{1.f, 1.f, 0.f},
		1
	};

	// SW
	state.sceneLights[1] = {
		{4.45f, 3.55f, -4.45f},
		{0.f, 1.f, 1.f},
		1
	};

	// N
	state.sceneLights[2] = {
		{0.f, 3.55f, 4.2f},
		{1.f, 0.1f, 0.1f},
		1
	};

	// Set up event handling
	glfwSetKeyCallback( window, &glfw_callback_key_ );
	glfwSetCursorPosCallback( window, &glfw_callback_motion_ );

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// DONE: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	// face culling
	glEnable(GL_CULL_FACE);
	// Clear colour is defined as a grey here
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	// enable alpha blending
	glEnable(GL_BLEND);
	// define alpha blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	// Load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/correct_blinn-phong.frag" }
		});

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

	//####################### Texture Loading ############################
	// Guide for texture mapping: https://learnopengl.com/Getting-started/Textures
	// As a rule of thumb, we want to load textures once only so we do it out of the main loop
	// Reserve an ID to our cobblestoneFloor and bind
	unsigned int cobblestoneFloor;
	glGenTextures(1, &cobblestoneFloor);
	glBindTexture(GL_TEXTURE_2D, cobblestoneFloor);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load wooden container texture
	int cobblestoneFloorWidth, cobblestoneFloorHeight, cobblestoneFloorNRChannels;
	unsigned char* cobblestoneFloorData = stbi_load("assets/textures/cobblestonefloor.jpeg", &cobblestoneFloorWidth, &cobblestoneFloorHeight, &cobblestoneFloorNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cobblestoneFloorWidth, cobblestoneFloorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, cobblestoneFloorData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(cobblestoneFloorData);

	// Reserve an ID for our markusTexture and bind
	unsigned int markusTexture;
	glGenTextures(1, &markusTexture);
	glBindTexture(GL_TEXTURE_2D, markusTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load markus container texture
	int markusTextureWidth, markusTextureHeight, markusTextureNRChannels;
	unsigned char* markusTextureData = stbi_load("assets/textures/markus.png", &markusTextureWidth, &markusTextureHeight, &markusTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, markusTextureWidth, markusTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, markusTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(markusTextureData);

	// window texture
	unsigned int windowTexture;
	glGenTextures(1, &windowTexture);
	glBindTexture(GL_TEXTURE_2D, windowTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load window texture
	int windowTextureWidth, windowTextureHeight, windowTextureNRChannels;
	unsigned char* windowTextureData = stbi_load("assets/textures/window.png", &windowTextureWidth, &windowTextureHeight, &windowTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowTextureWidth, windowTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, windowTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(windowTextureData);

	// nightsky texture
	unsigned int nightSkyTexture;
	glGenTextures(1, &nightSkyTexture);
	glBindTexture(GL_TEXTURE_2D, nightSkyTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load nightSky texture
	int nightSkyTextureWidth, nightSkyTextureHeight, nightSkyTextureNRChannels;
	unsigned char* nightSkyTextureData = stbi_load("assets/textures/nightsky.jpeg", &nightSkyTextureWidth, &nightSkyTextureHeight, &nightSkyTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nightSkyTextureWidth, nightSkyTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nightSkyTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(nightSkyTextureData);

	// northcity texture
	unsigned int northCityTexture;
	glGenTextures(1, &northCityTexture);
	glBindTexture(GL_TEXTURE_2D, northCityTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load northCity texture
	int northCityTextureWidth, northCityTextureHeight, northCityTextureNRChannels;
	unsigned char* northCityTextureData = stbi_load("assets/textures/northcity.jpg", &northCityTextureWidth, &northCityTextureHeight, &northCityTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, northCityTextureWidth, northCityTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, northCityTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(northCityTextureData);

	// southcity texture
	unsigned int southCityTexture;
	glGenTextures(1, &southCityTexture);
	glBindTexture(GL_TEXTURE_2D, southCityTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load southCity texture
	int southCityTextureWidth, southCityTextureHeight, southCityTextureNRChannels;
	unsigned char* southCityTextureData = stbi_load("assets/textures/southcity.jpg", &southCityTextureWidth, &southCityTextureHeight, &southCityTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, southCityTextureWidth, southCityTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, southCityTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(southCityTextureData);

	// eastcity texture
	unsigned int eastCityTexture;
	glGenTextures(1, &eastCityTexture);
	glBindTexture(GL_TEXTURE_2D, eastCityTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load eastCity texture
	int eastCityTextureWidth, eastCityTextureHeight, eastCityTextureNRChannels;
	unsigned char* eastCityTextureData = stbi_load("assets/textures/eastcity.jpg", &eastCityTextureWidth, &eastCityTextureHeight, &eastCityTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, eastCityTextureWidth, eastCityTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, eastCityTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(eastCityTextureData);

	// westcity texture
	unsigned int westCityTexture;
	glGenTextures(1, &westCityTexture);
	glBindTexture(GL_TEXTURE_2D, westCityTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load westCity texture
	int westCityTextureWidth, westCityTextureHeight, westCityTextureNRChannels;
	unsigned char* westCityTextureData = stbi_load("assets/textures/westcity.jpg", &westCityTextureWidth, &westCityTextureHeight, &westCityTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, westCityTextureWidth, westCityTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, westCityTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(westCityTextureData);

	// iron texture
	unsigned int ironTexture;
	glGenTextures(1, &ironTexture);
	glBindTexture(GL_TEXTURE_2D, ironTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load iron texture
	int ironTextureWidth, ironTextureHeight, ironTextureNRChannels;
	unsigned char* ironTextureData = stbi_load("assets/textures/iron.jpg", &ironTextureWidth, &ironTextureHeight, &ironTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ironTextureWidth, ironTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, ironTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(ironTextureData);
	

	//####################### VBO and VAO creation #######################

	// Complex Object Position VBO
	GLuint complexObjectPositionVBO = 0;
	glGenBuffers(1, &complexObjectPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, complexObjectPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kCubePositions), kCubePositions, GL_STATIC_DRAW);

	// Complex Object Color VBO
	GLuint complexObjectColorVBO = 0;
	glGenBuffers(1, &complexObjectColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, complexObjectColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeColors), kCubeColors, GL_STATIC_DRAW);

	// Complex Object TextureCoord VBO
	GLuint complexObjectTextureCoordVBO = 0;
	glGenBuffers(1, &complexObjectTextureCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, complexObjectTextureCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeTextureCoord), kCubeTextureCoord, GL_STATIC_DRAW);

	// predefine the array for normals
	float kCubeNormals[] = { 
		+1.f, +1.f, -1.f,
		-1.f, +1.f, -1.f,
		-1.f, +1.f, +1.f,
		+1.f, +1.f, -1.f,
		-1.f, +1.f, +1.f,
		+1.f, +1.f, +1.f,

		+1.f, -1.f, +1.f,
		+1.f, +1.f, +1.f,
		-1.f, +1.f, +1.f,
		+1.f, -1.f, +1.f,
		-1.f, +1.f, +1.f,
		-1.f, -1.f, +1.f,

		-1.f, -1.f, +1.f,
		-1.f, +1.f, +1.f,
		-1.f, +1.f, -1.f,
		-1.f, -1.f, +1.f,
		-1.f, +1.f, -1.f,
		-1.f, -1.f, -1.f,

		-1.f, -1.f, -1.f,
		+1.f, -1.f, -1.f,
		+1.f, -1.f, +1.f,
		-1.f, -1.f, -1.f,
		+1.f, -1.f, +1.f,
		-1.f, -1.f, +1.f,

		+1.f, -1.f, -1.f,
		+1.f, +1.f, -1.f,
		+1.f, +1.f, +1.f,
		+1.f, -1.f, -1.f,
		+1.f, +1.f, +1.f,
		+1.f, -1.f, +1.f,

		-1.f, -1.f, -1.f,
		-1.f, +1.f, -1.f,
		+1.f, +1.f, -1.f,
		-1.f, -1.f, -1.f,
		+1.f, +1.f, -1.f,
		+1.f, -1.f, -1.f
	};

	// generate normal vectors for the cube
	// kCubePositions should have indices 0 to 107
	for (int i = 0; i < 107; i += 3) {
		float x, y, z;
		if (i >= 0 && i < 18) {	// Top
			x = 0.f;
			y = 1.f;
			z = 0.f;
		}
		else if (i >= 18 && i < 36) { // North
			x = 0.f;
			y = 0.f;
			z = 1.f;
		}
		else if (i >= 36 && i < 54) { // East
			x = -1.f;
			y = 0.f;
			z = 0.f;
		}
		else if (i >= 54 && i < 72) { // Bottom
			x = 0.f;
			y = -1.f;
			z = 0.f;
		}
		else if (i >= 72 && i < 90) { // West
			x = 1.f;
			y = 0.f;
			z = 0.f;
		}
		else if (i >= 90 && i < 108) { // South
			x = 0.f;
			y = 0.f;
			z = -1.f;
		}
		kCubeNormals[i] = x;
		kCubeNormals[i + 1] = y;
		kCubeNormals[i + 2] = z;
	}
	
	// Complex Object Normal VBO
	GLuint complexObjectNormalVBO = 0;
	glGenBuffers(1, &complexObjectNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, complexObjectNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeNormals), kCubeNormals, GL_STATIC_DRAW);


	// Bind VBO into VAO
	GLuint complexObjectVAO = 0;
	glGenVertexArrays(1, &complexObjectVAO);
	glBindVertexArray(complexObjectVAO);

	glBindBuffer(GL_ARRAY_BUFFER, complexObjectPositionVBO);
	glVertexAttribPointer(
		0,						// loc 0 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, complexObjectColorVBO);
	glVertexAttribPointer(
		1,						// loc 1 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, complexObjectNormalVBO);
	glVertexAttribPointer(
		2,						// loc 2 in vert shader
		3, GL_FLOAT, GL_FALSE,	// 3 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, complexObjectTextureCoordVBO);
	glVertexAttribPointer(
		3,						// loc 3 in vert shader
		2, GL_FLOAT, GL_FALSE,	// 2 floats, not normalized
		0,						// no padding
		0						// no offset
	);
	glEnableVertexAttribArray(3);


	// Reset state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	OGL_CHECKPOINT_ALWAYS();
	
	// define scene objects

	SceneObj streetlampObj;
	streetlampObj.initialise("assets/streetlamp.obj");
	streetlampObj.forceTexture("textures/iron.jpg");

	SceneObject armadilloObj;
	initObject(&armadilloObj, "assets/Armadillo.obj");

	ComplexSceneObject f1carObj;
	initComplexObject(&f1carObj, "assets/f1_modified/f1.obj");
	for (int j = 0; j < f1carObj.objectCount; j++)
	{
		for (int i = 0; i < f1carObj.meshes[j].size; i++)
		{
			f1carObj.meshes[j].colors[i] = {1.f, 1.f, 1.f};
		}
	}

	PathObj f1Obj;
	f1Obj.initialise("assets/f1_modified/f1.obj");
	//f1Obj.initialise("assets/f1/ferrari-f1-race-car.obj");
	/*f1Obj.rotate({0.f, 0.5f * kPi, 0.f});
	f1Obj.setPositionAnchors(Vec3f{-10.f, 0.f, 6.f}, Vec3f{10.f, 0.f, 6.f});
	f1Obj.setupAnimation(300, RECIPROCAL, REPEAT);*/

	float r = 0.5f * kPi;

	f1Obj.addPathPoint({
		{0.f, r, 0.f},
		{-10.f, 0.f, 6.f},
		{1.f, 1.f, 1.f},
		200, S_CURVE
	});

	f1Obj.addPathPoint({
		{0.f, r, 0.f},
		{10.f, 0.f, 6.f},
		{1.f, 1.f, 1.f},
		100, SINUSOIDAL
	});

	f1Obj.addPathPoint({
		{0.f, r * 1.5f, 0.f},
		{12.f, 0.f, 4.f},
		{1.f, 1.f, 1.f},
		100, SINUSOIDAL
	});

	f1Obj.addPathPoint({
		{0.f, r * 2.f, 0.f},
		{10.f, 0.f, 6.f},
		{1.f, 1.f, 1.f},
		100, SINUSOIDAL
	});

	f1Obj.addPathPoint({
		{0.f, r * 2.5f, 0.f},
		{12.f, 0.f, 8.f},
		{1.f, 1.f, 1.f},
		100, SINUSOIDAL
	});

	f1Obj.addPathPoint({
		{0.f, r * 3.f, 0.f},
		{10.f, 0.f, 6.f},
		{1.f, 1.f, 1.f},
		100, SINUSOIDAL
	});

	f1Obj.addPathPoint({
		{0.f, r * 3.f, 0.f},
		{-10.f, 0.f, 6.f},
		{1.f, 1.f, 1.f},
		200, S_CURVE
	});

	f1Obj.setupPath();

	AnimationObj arm2Obj;
	arm2Obj.initialise("assets/Armadillo.obj");
	arm2Obj.move({0.f, 0.f, -4.f});
	arm2Obj.forceFakeTexCoords();
	arm2Obj.forceTexture("squiggle.png");
	arm2Obj.setRotationAnchors(Vec3f{0.f, 0.5f*kPi, 0.f}, Vec3f{0.f, 1.5*kPi, 0.f});
	arm2Obj.setupAnimation(200, LINEAR, BOUNCE);
	//arm2Obj.forceTexture("assets/squiggle.png");

	AnimationObj muscleCarObj;
	muscleCarObj.initialise("assets/msc_car/1967-shelby-ford-mustang.obj");
	muscleCarObj.scale({0.4f, 0.4f, 0.4f});
	muscleCarObj.rotate({0.f, 1.5f * kPi, 0.f});
	muscleCarObj.setPositionAnchors(Vec3f{-10.f, 0.f, 4.f}, Vec3f{10.f, 0.f, 4.f});
	muscleCarObj.setupAnimation(300, SINUSOIDAL, REPEAT);

	
	updateComplexObject(&f1carObj);

	SceneObject bulbObj;
	initObject(&bulbObj, "assets/globe-sphere.obj");

	bulbObj.scaling = {0.1f, 0.1f, 0.1f};
	for (int i = 0; i < bulbObj.mesh.size; i++)
	{
		bulbObj.mesh.colors[i] = {1.f, 1.f, 1.f};
	}
	updateObject(&bulbObj);

	auto lastTime = Clock::now();

	//####################### Main Loop #######################
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
		}

		//####################### Update state #######################
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now-lastTime).count();
		lastTime = now;

		//flying
		kFlightSpeed = kNormFlightSpeed;
		if (state.fastFlight) {
			kFlightSpeed = kFastFlightSpeed;
		}
		if (state.slowFlight) {
			kFlightSpeed = kSlowFlightSpeed;
		}

		if (state.camControl.actionForwards)	state.camControl.position += dt * kFlightSpeed * cam_forwards(&state.camControl);
		if (state.camControl.actionBackwards)	state.camControl.position += dt * kFlightSpeed * cam_backwards(&state.camControl);
		if (state.camControl.actionLeft)		state.camControl.position += dt * kFlightSpeed * cam_left(&state.camControl);
		if (state.camControl.actionRight)		state.camControl.position += dt * kFlightSpeed * cam_right(&state.camControl);
		if (state.camControl.actionUp)			state.camControl.position += dt * kFlightSpeed * cam_up(&state.camControl);
		if (state.camControl.actionDown)		state.camControl.position += dt * kFlightSpeed * cam_down(&state.camControl);


		Mat44f projection = make_perspective_projection(
			60.f * kPi / 180.f,
			fbwidth / float(fbheight),
			0.1f, 100.f
		);

		Mat44f worldRotationX = make_rotation_x(state.camControl.theta);
		Mat44f worldRotationY = make_rotation_y(state.camControl.phi);
		Mat44f worldTranslation = make_translation(state.camControl.position);
		Mat44f world2camera = worldRotationX * worldRotationY *  worldTranslation;

		// define model to world transformations
		// make sure to also save the transformation matrix to pass to the vertex shader
		Mat44f projCameraWorld = projection * world2camera;
		// boundary box
		// floor
		Mat44f projCameraWorldFloor = projection * world2camera * make_translation({ 0.f, 0.f, 0.f }) * make_scaling(20.f, 0.01f, 20.f);
		Mat44f transformFloor = make_translation({ 0.f, 0.f, 0.f }) * make_scaling(20.f, 0.01f, 20.f);
		// ceiling
		Mat44f projCameraWorldCeiling = projection * world2camera * make_translation({ 0.f, 15.f, 0.f }) * make_scaling(20.f, 0.01f, 20.f);
		Mat44f transformCeiling = make_translation({ 0.f, 15.f, 0.f }) * make_scaling(20.f, 0.01f, 20.f);
		// north wall
		Mat44f projCameraWorldNorth = projection * world2camera * make_translation({ 0.f, 7.5f, 20.f }) * make_scaling(20.f, 7.5f, 0.01f);
		Mat44f transformNorth = make_translation({ 0.f, 7.5f, 20.f }) * make_scaling(20.f, 7.5f, 0.01f);
		// south wall
		Mat44f projCameraWorldSouth = projection * world2camera * make_translation({0.f, 7.5f, -20.f}) * make_scaling(20.f, 7.5f, 0.01f);
		Mat44f transformSouth = make_translation({ 0.f, 7.5f, -20.f }) * make_scaling(20.f, 7.5f, 0.01f);
		// west wall
		Mat44f projCameraWorldWest = projection * world2camera * make_translation({20.f, 7.5f, 0.f}) * make_scaling(0.01f, 7.5f, 20.f);
		Mat44f transformWest = make_translation({ 20.f, 7.5f, 0.f }) * make_scaling(0.01f, 7.5f, 20.f);
		// east wall
		Mat44f projCameraWorldEast = projection * world2camera * make_translation({-20.f, 7.5f, 0.f }) * make_scaling(0.01f, 7.5f, 20.f);
		Mat44f transformEast = make_translation({ -20.f, 7.5f, 0.f }) * make_scaling(0.01f, 7.5f, 20.f);
		// markus monument pieces
		// markus
		Mat44f projCameraWorldMarkus = projection * world2camera * make_translation({ -5.f, 0.21f, 0.f }) * make_scaling(0.4f, 0.025f, 0.4f);
		Mat44f transformMarkus = make_translation({ -5.f, 0.21f, 0.f }) * make_scaling(0.4f, 0.025f, 0.4f);
		// monument
		Mat44f projCameraWorldMonument = projection * world2camera * make_translation({ -5.f, 0.1f, 0.f }) * make_scaling(0.5f, 0.1f, 0.5f);
		Mat44f transformMonument = make_translation({ -5.f, 0.1f, 0.f }) * make_scaling(0.5f, 0.1f, 0.5f);
		// glass box top
		Mat44f projCameraWorldGlassTop = projection * world2camera * make_translation({ -5.f, 0.5f, 0.f }) * make_scaling(1.f, 0.01f, 1.f);
		Mat44f transformGlassTop = make_translation({ -5.f, 0.5f, 0.f }) * make_scaling(1.f, 0.01f, 1.f);
		// glass box north
		Mat44f projCameraWorldGlassNorth = projection * world2camera * make_translation({ -5.f, 0.25f, 1.f }) * make_scaling(1.f, 0.25f, 0.01f);
		Mat44f transformGlassNorth = make_translation({ -5.f, 0.25f, 1.f }) * make_scaling(1.f, 0.25f, 0.01f);
		// glass box south
		Mat44f projCameraWorldGlassSouth = projection * world2camera * make_translation({ -5.f, 0.25f, -1.f }) * make_scaling(1.f, 0.25f, 0.01f);
		Mat44f transformGlassSouth = make_translation({ -5.f, 0.25f, -1.f }) * make_scaling(1.f, 0.25f, 0.01f);
		// glass box west
		Mat44f projCameraWorldGlassWest = projection * world2camera * make_translation({ -4.f, 0.25f, 0.f }) * make_scaling(0.01f, 0.25f, 1.f);
		Mat44f transformGlassWest = projection * world2camera * make_translation({ -4.f, 0.25f, 0.f }) * make_scaling(0.01f, 0.25f, 1.f);
		// glass box east
		Mat44f projCameraWorldGlassEast = projection * world2camera * make_translation({ -6.f, 0.25f, 0.f }) * make_scaling(0.01f, 0.25f, 1.f);
		Mat44f transformGlassEast = make_translation({ -6.f, 0.25f, 0.f }) * make_scaling(0.01f, 0.25f, 1.f);

		

		OGL_CHECKPOINT_DEBUG();
		//####################### Draw frame #######################

		// General draw frame settings
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

		// Prepare to draw using simple meshes (armadillo)
		glUseProgram(prog.programId());
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorld.v
		);

		glUniform3fv(
			4, 1,
			&state.sceneLights[0].position.x
		);
		glUniform3fv(
			5, 1,
			&state.sceneLights[0].color.x
		);
		glUniform1fv(
			6, 1, &state.sceneLights[0].brightness
		);
		glUniform3fv(
			7, 1,
			&state.sceneLights[1].position.x
		);
		glUniform3fv(
			8, 1,
			&state.sceneLights[1].color.x
		);
		glUniform1f(
			9, state.sceneLights[1].brightness
		);
		glUniform3fv(
			10, 1,
			&state.sceneLights[2].position.x
		);
		glUniform3fv(
			11, 1,
			&state.sceneLights[2].color.x
		);
		glUniform1f(
			12, state.sceneLights[2].brightness
		);
		
		// move camera forwards slightly when passed to renderer to enhance specular lighting
		Vec3f camPos = state.camControl.position + cam_forwards(&state.camControl) * 3.0f;

		glUniform3f(
			2,
			camPos.x,
			camPos.y,
			camPos.z
		);

		Mat44f standardMaterialProps = {
			0.8f, 0.8f, 0.8f, 0.f, // kA
			0.8f, 0.8f, 0.8f, 0.f, // kD
			0.8f, 0.8f, 0.8f, 0.f,// kS
			0.f, 0.f, 0.f, 0.f  // kE
		};

		Mat44f lightMaterialProps = {
			0.f, 0.f, 0.f, 0.f,   // kA
			0.f, 0.f, 0.f, 0.f,   // kD
			0.f, 0.f, 0.f, 0.f,  // kS
			1.f, 1.f, 1.f, 0.f // kE
		};

		glUniformMatrix4fv(
			3, 1,
			GL_FALSE, standardMaterialProps.v
		);

		// draw f1 car
		drawComplexObject(&f1carObj, projCameraWorld);

		// draw a SceneObj f1 car
		if (!state.animationPause) {
			f1Obj.updatePath(state.animationFactor);
		}
		f1Obj.draw(projCameraWorld);

		if (!state.animationPause) {
			arm2Obj.updateAnimation(state.animationFactor);
		}
		arm2Obj.draw(projCameraWorld);


		if (!state.animationPause) {
			muscleCarObj.updateAnimation(state.animationFactor);
		}
		muscleCarObj.draw(projCameraWorld);

		glUniformMatrix4fv(
			3, 1,
			GL_FALSE, standardMaterialProps.v
		);

		// define terms for the armadillo
		Vec3f pos1 = { 0.f, 0.f, 0.f };
		Vec3f pos2 = { 4.f, 0.f, 0.f };

		// adjust the armadillo's rotation, then draw it
		armadilloObj.position = pos2;
		if (!state.animationPause) {
			armadilloObj.rotation.y += dt * state.animationFactor;
			armadilloObj.rotation.y = armadilloObj.rotation.y > 2 * kPi ? 0 : armadilloObj.rotation.y;
		}
		drawObject(&armadilloObj, projCameraWorld);

		// setting material properties for cubes

		// define positions for the streetlamps
		Vec3f streetlampPos1 = { -5.f, 0.f, -5.f };	// SE
		Vec3f streetlampPos2 = { 5.f, 0.f, -5.f };	// SW
		Vec3f streetlampPos3 = { 0.f, 0.f, 5.f };	// N

		// draw streetlamps
		// bind iron
		glBindTexture(GL_TEXTURE_2D, ironTexture);
		streetlampObj.move(streetlampPos1);
		streetlampObj.scale({ 0.25f, 0.25f, 0.25f });
		streetlampObj.rotate({ 0.f, kPi * 3 / 4, 0.f });
		streetlampObj.draw(projCameraWorld);

		streetlampObj.move(streetlampPos2);
		streetlampObj.rotate({ 0.f, kPi / 4, 0.f });
		streetlampObj.draw(projCameraWorld);

		streetlampObj.move(streetlampPos3);
		streetlampObj.rotate({ 0.f, -kPi / 2, 0.f });
		streetlampObj.draw(projCameraWorld);

		// draw the box around the scene
		// draw floor
		// bind cobblestonefloor
		glBindTexture(GL_TEXTURE_2D, cobblestoneFloor);

		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldFloor.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformFloor.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw ceiling
		// bind nightSky
		glBindTexture(GL_TEXTURE_2D, nightSkyTexture);

		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldCeiling.v
		); 
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformCeiling.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw north wall
		// bind northCity
		glBindTexture(GL_TEXTURE_2D, northCityTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldNorth.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformNorth.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw south wall
		// bind southCity
		glBindTexture(GL_TEXTURE_2D, southCityTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldSouth.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformSouth.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw west wall
		// bind westCity
		glBindTexture(GL_TEXTURE_2D, westCityTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldWest.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformWest.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// draw east wall
		// bind eastCity
		glBindTexture(GL_TEXTURE_2D, eastCityTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldEast.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformEast.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw the monument to Markus
		// iron monument base
		glBindTexture(GL_TEXTURE_2D, ironTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldMonument.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformMonument.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// markus plaque
		glBindTexture(GL_TEXTURE_2D, markusTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldMarkus.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformMarkus.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// last things to be drawn should be our transparent objects
		// draw glass top
		// bind glass
		glBindTexture(GL_TEXTURE_2D, windowTexture);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldGlassTop.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformGlassTop.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw glass north
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldGlassNorth.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformGlassNorth.v
		);
		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw glass south
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldGlassSouth.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformGlassSouth.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw glass east
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldGlassEast.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformGlassSouth.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw glass west
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorldGlassWest.v
		);
		glUniformMatrix4fv(
			1, 1,
			GL_TRUE, transformGlassWest.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// reset texture state (using iron texture as a reset)
		glBindTexture(GL_TEXTURE_2D, ironTexture);

		lightMaterialProps.v[12] = state.sceneLights[0].color.x;
		lightMaterialProps.v[13] = state.sceneLights[0].color.y;
		lightMaterialProps.v[14] = state.sceneLights[0].color.z;

		// seetting material properties
		glUniformMatrix4fv(
			3, 1,
			GL_FALSE, lightMaterialProps.v
		);

		bulbObj.position = state.sceneLights[0].position;
		drawObject(&bulbObj, projCameraWorld);

		lightMaterialProps.v[12] = state.sceneLights[1].color.x;
		lightMaterialProps.v[13] = state.sceneLights[1].color.y;
		lightMaterialProps.v[14] = state.sceneLights[1].color.z;

		// seetting material properties
		glUniformMatrix4fv(
			3, 1,
			GL_FALSE, lightMaterialProps.v
		);

		bulbObj.position = state.sceneLights[1].position;
		drawObject(&bulbObj, projCameraWorld);

		lightMaterialProps.v[12] = state.sceneLights[2].color.x;
		lightMaterialProps.v[13] = state.sceneLights[2].color.y;
		lightMaterialProps.v[14] = state.sceneLights[2].color.z;

		// seetting material properties
		glUniformMatrix4fv(
			3, 1,
			GL_FALSE, lightMaterialProps.v
		);

		bulbObj.position = state.sceneLights[2].position;
		drawObject(&bulbObj, projCameraWorld);

		// Reset state
		glBindVertexArray(0);
		glUseProgram(0);
		// End of drawing using simple meshes

		OGL_CHECKPOINT_DEBUG();
		//####################### Display frame #######################
		glfwSwapBuffers( window );

		if (state.screenshotQueued)
		{
			auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			std::string filepath = "screenshots/" + std::to_string(epoch.count()) + ".png";
			ScreenshotData ssData = getScreenshotData(window);
			std::thread t([ssData, filepath]{saveScreenshot(ssData, filepath);});
			t.detach();
			state.screenshotQueued = false;
		}
	}

	//####################### Cleanup (on exit) #######################
	//TODO: additional cleanup

	glDeleteBuffers(1, &complexObjectPositionVBO);
	glDeleteBuffers(1, &complexObjectColorVBO);

	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}


namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_( GLFWwindow* aWindow, int aKey, int, int aAction, int )
	{
		// Check if window should close
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
		}

		// get state info
		if( auto* state = static_cast<State_*>(glfwGetWindowUserPointer( aWindow )) ) {

			// change polygon mode
			if ( GLFW_KEY_P == aKey && GLFW_PRESS == aAction)
			{
				switch (state->polygonMode)
				{
				case GL_POINT:
					{
						state->polygonMode = GL_LINE; break;
					}
				case GL_LINE:
					{
						state->polygonMode = GL_FILL; break;
					}
				case GL_FILL:
					{
						state->polygonMode = GL_POINT; break;
					}
				default:
					{
						state->polygonMode = GL_LINE; break;
					}
				}
				printf("polygon mode: %0xf\n", state->polygonMode);
				glPolygonMode(GL_FRONT_AND_BACK, state->polygonMode);
			}

			if ( GLFW_KEY_L == aKey && GLFW_PRESS == aAction)
			{
				switch (state->currentLight)
				{
					case kLightCount-1:
						{
							state->currentLight = 0; break;
						}
					default:
						{
							state->currentLight++; break;
						}
				}
			}

			if ( GLFW_KEY_U == aKey && GLFW_PRESS == aAction)
			{
				state->sceneLights[state->currentLight].position.z += 0.25f;
			}

			if ( GLFW_KEY_J == aKey && GLFW_PRESS == aAction)
			{
				state->sceneLights[state->currentLight].position.z -= 0.25f;
			}

			if ( GLFW_KEY_H == aKey && GLFW_PRESS == aAction)
			{
				state->sceneLights[state->currentLight].position.x += 0.25f;
			}

			if ( GLFW_KEY_K == aKey && GLFW_PRESS == aAction)
			{
				state->sceneLights[state->currentLight].position.x -= 0.25f;
			}

			if ( GLFW_KEY_Y == aKey && GLFW_PRESS == aAction)
			{
				state->sceneLights[state->currentLight].position.y += 0.25f;
			}

			if ( GLFW_KEY_I == aKey && GLFW_PRESS == aAction)
			{
				state->sceneLights[state->currentLight].position.y -= 0.25f;
			}

			if ( GLFW_KEY_O == aKey && GLFW_PRESS == aAction)
			{
				printf("Light %d, pos: {%f, %f, %f}\n", state->currentLight, state->sceneLights[state->currentLight].position.x, state->sceneLights[state->currentLight].position.y, state->sceneLights[state->currentLight].position.z);
			}

			// control the animation speed
			// left arrow slows, right arrow speeds, down arrow pauses
			if (GLFW_KEY_LEFT == aKey && GLFW_PRESS == aAction) {
				state->animationFactor--;
				if (state->animationFactor <= 0) {
					state->animationFactor = 1;
				}
			}
			if (GLFW_KEY_RIGHT == aKey && GLFW_PRESS == aAction) {
				state->animationFactor++;
				if (state->animationFactor > 5) {
					state->animationFactor = 5;
				}
			}
			if (GLFW_KEY_DOWN == aKey && GLFW_PRESS == aAction) {
				state->animationPause = !state->animationPause;
			}

			if (GLFW_KEY_PRINT_SCREEN == aKey && GLFW_PRESS == aAction)
			{
				state->screenshotQueued = true;
			}

			if ((GLFW_KEY_LEFT_SHIFT == aKey || GLFW_KEY_RIGHT_SHIFT == aKey) && GLFW_PRESS == aAction)
			{
				if (!state->fastFlight)
				{
					state->slowFlight = true;
				}
			}

			if ((GLFW_KEY_LEFT_CONTROL == aKey || GLFW_KEY_RIGHT_CONTROL == aKey) && GLFW_PRESS == aAction)
			{
				if (!state->slowFlight)
				{
					state->fastFlight = true;
				}
			}

			if ((GLFW_KEY_LEFT_SHIFT == aKey || GLFW_KEY_RIGHT_SHIFT == aKey) && GLFW_RELEASE == aAction)
			{
				if (state->slowFlight)
				{
					state->slowFlight = false;
				}
			}

			if ((GLFW_KEY_LEFT_CONTROL == aKey || GLFW_KEY_RIGHT_CONTROL == aKey) && GLFW_RELEASE == aAction)
			{
				if (state->fastFlight)
				{
					state->fastFlight = false;
				}
			}

			// make camera active if SPACE pressed
			if( GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction )
			{
				cam_toggle(&state->camControl);

				if( state->camControl.cameraActive ) 
				{
					glfwSetInputMode( aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
				}
				else 
				{
					cam_action_reset(&state->camControl);
					glfwSetInputMode( aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
				}
			}

			// Camera controls if camera is active
			cam_handle_key(&state->camControl, aKey, aAction);
		}
		
	}

	void glfw_callback_motion_( GLFWwindow* aWindow, double aX, double aY )
	{
		if( auto* state = static_cast<State_*>(glfwGetWindowUserPointer( aWindow )) )
		{
			//if( state->camControl.cameraActive )
			//{
			//	auto const dx = float(aX-state->camControl.lastX);
			//	auto const dy = float(aY-state->camControl.lastY);

			//	state->camControl.phi += dx*kMouseSensitivity;
			//	
			//	state->camControl.theta += dy*kMouseSensitivity;
			//	if( state->camControl.theta > kPi/2.f )
			//		state->camControl.theta = kPi/2.f;
			//	else if( state->camControl.theta < -kPi/2.f )
			//		state->camControl.theta = -kPi/2.f;
			//}

			////printf("camera: theta: %f, phi: %f\n", state->camControl.theta, state->camControl.phi);


			//state->camControl.lastX = float(aX);
			//state->camControl.lastY = float(aY);
			cam_handle_mouse(&state->camControl, kMouseSensitivity, aX, aY);
		}
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}

