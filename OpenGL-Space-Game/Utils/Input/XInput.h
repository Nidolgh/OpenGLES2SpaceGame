#ifndef RASPBERRY
#pragma once

#include "GLFW/glfw3.h"

#include "BaseInput.h"

class XInput :
	public BaseInput
{
protected:
	GLFWwindow* m_pWindow;

	// mouse specific vars
	bool m_first_mouse			= true;
	bool m_mouse_gui_mode		= false;
	float m_mouse_sensitivity	= 0.05;
	float m_last_mouse_pos_x;
	float m_last_mouse_pos_y;

public:
	XInput(GLFWwindow* pWindow);
	virtual ~XInput();

	virtual bool TestKey(unsigned int WhatKey);
	virtual bool Init(const unsigned int scr_width, const unsigned int scr_height);

	virtual void PollEvents();

	virtual void SwitchMouseMode();
	void RawMouseUpdate(); // for space, camera movement
	void GUIMouseUpdate(); // for GUI stuff
};

#endif