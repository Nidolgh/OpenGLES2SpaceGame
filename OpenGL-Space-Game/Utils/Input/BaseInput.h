#pragma once
class BaseInput
{
protected:

	unsigned int m_screen_width		= 0;
	unsigned int m_screen_height	= 0;

public:
	typedef struct // A very simple structure to hold the mouse info
	{
		int PositionX;
		// contains the relative position from the start point (take care to not confuse it with the GUI mouse position) 
		int PositionY;
		float RelY;
		float RelX;
		unsigned char LeftButton; // TRUE when pressed FALSE otherwise
		unsigned char MidButton;
		unsigned char RightButton;
	} MouseData;

	MouseData m_mouse_data; // filled by inherited classes and handled differently for every system

	BaseInput() {}
	virtual ~BaseInput() {}

	virtual bool Init(const unsigned int pScr_width, const unsigned int pScr_height) = 0;
	virtual bool TestKey(unsigned int what_key) = 0; // tests for keyboard keys
	virtual void PollEvents() = 0;
	virtual void SwitchMouseMode() = 0;
};