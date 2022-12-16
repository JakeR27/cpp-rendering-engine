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

#include "defaults.hpp"
#include "complex_object.hpp"
#include "camera.hpp"

// ex 4
#include "cone.hpp"
#include "cylinder.hpp"
#include "loadobj.hpp"
#include "point_light.hpp"
#include "scene_object.hpp"

// include STB_IMAGE for texture mapping, provided in the "third_party" directory
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - Coursework 2";
	constexpr float const kMouseSensitivity = 0.01f;
	constexpr float const kMovementSensitivity = 2.f;
	constexpr float const kPi = 3.1415962f;
	constexpr size_t kLightCount = 3;

	struct State_
	{
		cameraControl camControl;
		GLenum polygonMode;
		pointLight sceneLights[kLightCount];
		int currentLight = 0;
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
	state.camControl.position = {0.f, 0.f, -10.f};
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

	state.sceneLights[0] = {
		{0.f, 4.5f, 0.5f},
		{1.f, 1.f, 0.f},
		1
	};

	state.sceneLights[1] = {
		{0.75f, 3.25f, 0.5f},
		{0.f, 1.f, 1.f},
		1
	};

	state.sceneLights[2] = {
		{0.25f, 1.5f, 1.f},
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
	// Reserve an ID to our woodTexture and bind
	unsigned int woodTexture;
	glGenTextures(1, &woodTexture);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load wooden container texture
	int woodTextureWidth, woodTextureHeight, woodTextureNRChannels;
	unsigned char* woodTextureData = stbi_load("assets/textures/container.jpg", &woodTextureWidth, &woodTextureHeight, &woodTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, woodTextureWidth, woodTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, woodTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(woodTextureData);

	// Reserve an ID for our markusTexture and bind
	unsigned int markusTexture;
	glGenTextures(1, &markusTexture);
	glBindTexture(GL_TEXTURE_2D, markusTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load markusen container texture
	int markusTextureWidth, markusTextureHeight, markusTextureNRChannels;
	unsigned char* markusTextureData = stbi_load("assets/textures/markus.png", &markusTextureWidth, &markusTextureHeight, &markusTextureNRChannels, 0);
	// Generate a texture using the image data
	// Take note if there's an Alpha value or not, you'll either use GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, markusTextureWidth, markusTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, markusTextureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free the memory used for the image data
	stbi_image_free(markusTextureData);

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

		if (state.camControl.actionForwards)	state.camControl.position += dt * kMovementSensitivity * cam_forwards(&state.camControl);
		if (state.camControl.actionBackwards)	state.camControl.position += dt * kMovementSensitivity * cam_backwards(&state.camControl);
		if (state.camControl.actionLeft)		state.camControl.position += dt * kMovementSensitivity * cam_left(&state.camControl);
		if (state.camControl.actionRight)		state.camControl.position += dt * kMovementSensitivity * cam_right(&state.camControl);
		if (state.camControl.actionUp)			state.camControl.position += dt * kMovementSensitivity * cam_up(&state.camControl);
		if (state.camControl.actionDown)		state.camControl.position += dt * kMovementSensitivity * cam_down(&state.camControl);


		Mat44f projection = make_perspective_projection(
			60.f * kPi / 180.f,
			fbwidth / float(fbheight),
			0.1f, 100.f
		);

		Mat44f worldRotationX = make_rotation_x(state.camControl.theta);
		Mat44f worldRotationY = make_rotation_y(state.camControl.phi);
		Mat44f worldTranslation = make_translation(state.camControl.position);
		Mat44f world2camera = worldRotationX * worldRotationY *  worldTranslation;

		Mat44f projCameraWorld = projection * world2camera;
		Mat44f projCameraWorld2 = projection * world2camera * make_translation({10.f, -1.f, 0.f}) * make_scaling(10.f, 1.f, 1.f) ;
		Mat44f projCameraWorld3 = projection * world2camera * make_translation({0.f, -1.f, 10.f}) * make_scaling(1.f, 1.f, 10.f);

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

		Vec3f camPos = state.camControl.position + cam_forwards(&state.camControl) *3;
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

		// draw armadillo2
		Vec3f pos1 = {0.f, 0.f, 0.f};
		Vec3f pos2 = {4.f, 0.f, 0.f};

		drawComplexObject(&f1carObj, projCameraWorld);

		armadilloObj.position = pos2;
		armadilloObj.rotation.y += dt;
		armadilloObj.rotation.y = armadilloObj.rotation.y > 2 * kPi ? 0 : armadilloObj.rotation.y;
		drawObject(&armadilloObj, projCameraWorld);

		// draw cube 1
		// bind markusTexture
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorld2.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw cube 2
		//bind woodenTexture
		glBindTexture(GL_TEXTURE_2D, markusTexture);

		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projCameraWorld3.v
		);

		glBindVertexArray(complexObjectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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

