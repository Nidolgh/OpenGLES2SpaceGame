#pragma once

#include "Utils/Input/BaseInput.h"

// these defines are used often (and Pi is a joke :D) 
#define PI	3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f
#define DEG2RAD(x) (x*PI)/180
#define RAD2DEG(x) x*(180/PI)

//most standard OGL demos use int versions of TRUE and FALSE (careful not to mix up with bool true and false)
#define TRUE 1
#define FALSE 0

class BaseWindow
{
protected:
	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_num_resolutions = 0;
	
	bool m_should_window_close = false;

public:
	BaseWindow();
	virtual ~BaseWindow();

	virtual int Init(const unsigned int width, const unsigned int height, const bool fullscreen = false) = 0;

	virtual void SwapBuffer() = 0;

	virtual void SetRes(int index, unsigned int& screenX, unsigned int& screenY) = 0;
	virtual void GetRes(unsigned int& ReturnX, unsigned int& ReturnY) = 0;
	virtual void RestoreRes() = 0;

	virtual void SetWindowShouldClose(const bool val) = 0;
	virtual bool ShouldWindowClose() = 0;

	virtual BaseInput* PrepareInput() = 0;

	unsigned int GetWidth() const { return m_width; } 
	unsigned int GetHeight() const { return m_height; } 
};