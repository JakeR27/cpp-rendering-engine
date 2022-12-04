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

namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - Coursework 2";
	
	void glfw_callback_error_( int, char const* );

	void glfw_callback_key_( GLFWwindow*, int, int, int, int );

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

	// Set up event handling
	glfwSetKeyCallback( window, &glfw_callback_key_ );

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
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
		});

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

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

	// Reset state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Clean up buffers
	// Note: these are not deleted fully, as the VAO holds a reference to them.
	glDeleteBuffers(1, &complexObjectColorVBO);
	glDeleteBuffers(1, &complexObjectPositionVBO);

	OGL_CHECKPOINT_ALWAYS();

	//####################### Main Loop #######################

	// Main loop
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

			glViewport( 0, 0, iwidth, iheight );
		}

		//####################### Update state #######################

		Mat44f projection = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			fbwidth / float(fbheight),
			0.1f, 100.f
		) * make_translation({0.f, 0.f, -10.f}) * make_rotation_y(0.4f);


		OGL_CHECKPOINT_DEBUG();
		//####################### Draw frame #######################

		// Prepare to draw
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(prog.programId());

		glBindVertexArray(complexObjectVAO);
		glUniformMatrix4fv(
			0, 1,
			GL_TRUE, projection.v
		);

		// Draw complex object
		glDrawArrays(GL_TRIANGLES, 0, sizeof(kCubePositions));

		// Reset state
		glBindVertexArray(0);
		glUseProgram(0);

		OGL_CHECKPOINT_DEBUG();
		//####################### Display frame #######################
		glfwSwapBuffers( window );
	}

	//####################### Cleanup (on exit) #######################
	//TODO: additional cleanup
	
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
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
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

