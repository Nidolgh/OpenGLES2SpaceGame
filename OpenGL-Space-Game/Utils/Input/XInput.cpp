#ifndef RASPBERRY
#include "XInput.h"

XInput::XInput(GLFWwindow* pWindow)
	: m_pWindow(pWindow)
{
}

XInput::~XInput()
{
}

bool XInput::TestKey(unsigned int WhatKey)
{
	return (glfwGetKey(m_pWindow, WhatKey) == GLFW_PRESS); //, GLFW__RELEASED
}

bool XInput::Init(const unsigned int scr_width, const unsigned int scr_height)
{
	m_screen_width = scr_width;
	m_screen_height = scr_height;

	m_last_mouse_pos_x = scr_width * 0.5f;
	m_last_mouse_pos_y = scr_height * 0.5f;

	// mouse input mode 
	glfwSetInputMode(m_pWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);// mouse button stays true until queryd

	return false;
}

void XInput::PollEvents()
{
	glfwPollEvents();

	if (m_mouse_gui_mode)
		GUIMouseUpdate();
	else
		RawMouseUpdate();
}

void XInput::SwitchMouseMode()
{
	m_mouse_gui_mode = !m_mouse_gui_mode;
	m_first_mouse = true;
}

void XInput::RawMouseUpdate()
{
	// great way to handle mouse movement using a last and offset position: https://learnopengl.com/Getting-started/Camera
	double xpos, ypos;
	glfwGetCursorPos(m_pWindow, &xpos, &ypos);

	if (m_first_mouse) // initially set to true
	{
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (!glfwRawMouseMotionSupported())
			glfwSetInputMode(m_pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		m_last_mouse_pos_x = xpos;
		m_last_mouse_pos_y = ypos;
		m_first_mouse = false;
	}

	float xoffset = xpos - m_last_mouse_pos_x;
	float yoffset = m_last_mouse_pos_y - ypos; // reversed since y coordinates range from bottom to top
	m_last_mouse_pos_x = xpos;
	m_last_mouse_pos_y = ypos;

	m_mouse_data.RelX = xoffset * m_mouse_sensitivity;
	m_mouse_data.RelY = yoffset * m_mouse_sensitivity;

	m_mouse_data.LeftButton = (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE);
	m_mouse_data.MidButton = (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_RELEASE);
	m_mouse_data.RightButton = (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE);

	// printf("relx:%f, rely:%f\n", m_mouse_data.RelX, m_mouse_data.RelY);
}

void XInput::GUIMouseUpdate()
{
	if (m_first_mouse) // initially set to true
	{
		// set back to normal
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (!glfwRawMouseMotionSupported())
			glfwSetInputMode(m_pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

		m_first_mouse = false;
	}

	// mouse input according to the docs of glfw https://www.glfw.org/docs/3.3/input_guide.html#input_mouse
	double xpos, ypos;
	glfwGetCursorPos(m_pWindow, &xpos, &ypos); // return position inside window, use raw input to return from screen
	m_mouse_data.PositionX = xpos;
	m_mouse_data.PositionY = ypos;
}

#endif