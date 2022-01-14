#ifndef RASPBERRY
#pragma once
#include <cstdio>

#include "BaseWindow.h"
#include "Utils/Input/XInput.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class XWindow :
	public BaseWindow
{
protected:
	GLFWwindow* m_window;

public:
	XWindow();
	virtual ~XWindow();

	virtual int Init(const unsigned int width, const unsigned int height, const bool fullscreen = false);

	virtual void SwapBuffer();

	virtual void SetRes(int index, unsigned int& screenX, unsigned int& screenY);
	virtual void GetRes(unsigned int& ReturnX, unsigned int& ReturnY);
	virtual void RestoreRes();

	virtual void SetWindowShouldClose(const bool val);
	virtual bool ShouldWindowClose();

	virtual BaseInput* PrepareInput();
};

#endif // RASPBERRY