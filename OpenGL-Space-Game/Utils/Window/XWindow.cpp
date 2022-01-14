#ifndef RASPBERRY

#include "XWindow.h"

XWindow::XWindow()
{
}

XWindow::~XWindow()
{
	if (m_window != NULL)
		glfwTerminate();
}

// LearnOpenGL window implementation https://learnopengl.com/Getting-started/Hello-Window
int XWindow::Init(const unsigned int width, const unsigned int height, const bool fullscreen)
{
	m_width = width;
	m_height = height;

	// must execute before we can use most glfw functions
	// set correct GL version as GLFW does support GLES 2.0 https://stackoverflow.com/questions/38936425/glfw-fails-to-load-all-gl-es-2-0-functions
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	//glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// https://www.glfw.org/docs/latest/window.html#window_hints more glfw hints
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	if (fullscreen)
	{
		// no scaling
		//GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		//const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		m_window = glfwCreateWindow(width, height, "Nido Window", glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		m_window = glfwCreateWindow(width, height, "Nido Window", NULL, NULL);
	}

	if (m_window == NULL)
	{
		printf("Failed to create GLFW window!\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(m_window);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // for window resizing, project will probably now handle this

	// glad: load all OpenGL function pointers // GLES2 for ES // gladLoadGLLoader for non ES
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	// Some OpenGLES2.0 states that we might need
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(TRUE);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);

	//these are the options you can have for the depth, play with them?
	//#define GL_NEVER                          0x0200
	//#define GL_LESS                           0x0201
	//#define GL_EQUAL                          0x0202
	//#define GL_LEQUAL                         0x0203
	//#define GL_GREATER                        0x0204
	//#define GL_NOTEQUAL                       0x0205
	//#define GL_GEQUAL                         0x0206

	// Setup the viewport
	glViewport(0, 0, (uint32_t)width, (uint32_t)height);
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	int error = 0;
	if ((error = glGetError()) != GL_NO_ERROR) // todo: why is this slower, remove later on if sure no errors occur as its wastefull
	{
		printf("Oh bugger CubeModel Draw error: %i\n", error);
		return false;
	}

	return 1;
}

void XWindow::SwapBuffer()
{
	glfwSwapBuffers(m_window);
}

void XWindow::SetRes(int index, unsigned int& screenX, unsigned int& screenY)
{
	m_width = screenX;
	m_height = screenY;
	glViewport(0, 0, screenX, screenY);
}

void XWindow::GetRes(unsigned int& ReturnX, unsigned int& ReturnY)
{
	ReturnX = m_width;
	ReturnY = m_height;
}

void XWindow::RestoreRes()
{
	// hmm
}

void XWindow::SetWindowShouldClose(const bool val)
{
	glfwSetWindowShouldClose(m_window, val);
}

bool XWindow::ShouldWindowClose()
{
	return glfwWindowShouldClose(m_window);
}

BaseInput* XWindow::PrepareInput()
{
	return new XInput(m_window);
}

#endif // DEBUG