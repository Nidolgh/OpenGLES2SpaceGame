#ifdef RASPBERRY

#pragma once

#include "BaseWindow.h"
#include "Utils/Input/ARMInput.h"

// the_graphics class primary responsibility is to set up OpenGLES for render and to hold info on X11 window
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>

#include "bcm_host.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <vector>

// if we use imgui we are going to need these
#include <X11/Xlib.h> // we now include X11 for all systems including early Rpi's so we can get access to an Xwindow cursor, but only Rpi4>> uses X11 for render
#include <X11/extensions/Xrandr.h> //this is used for our resolution systems, which are handled a little different on early Rpi's,we still need some of the info, but Rpi res data will be invalid

// these are more focused on X11 for display so we don't need to include them on a Raspbery, the Xlib and XrandR gives us all the Raspberry needs
#ifndef RASPBERRY
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

#endif

//ObjectDate is a small container struct that provides info for render of an object
typedef struct
{
	// save a Handle to a program object
	GLuint programObject;
	// Attribute locations in that shader
	GLint positionLoc; // these are common to almost all shaders
	GLint texCoordLoc;
	GLint samplerLoc; // Sampler location
	// Texture handle
	GLuint textureId; // assuming the texture is in gpu ram here's its ID
} ObjectData;

// general info on the state of the EGL/DispmanX/X11 screen
typedef struct Target_State
{
	uint32_t width;
	uint32_t height;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	// slight difference in the kind of window needed for X11 and Rpi	
#ifdef RASPBERRY
	EGL_DISPMANX_WINDOW_T nativewindow;
#else
	EGLNativeWindowType  nativewindow;
#endif

	ObjectData* object_data;
} Target_State;

// define what kind of EGL config we want, we can add more options but the more we add the less likely it might be available across systems
static const EGLint attribute_list[] =
{
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_DEPTH_SIZE,
	8,
	EGL_SURFACE_TYPE,
	EGL_WINDOW_BIT,
	EGL_CONFORMANT,
	EGL_OPENGL_ES2_BIT,

	//be aware, some emulated Mesa OpenGLES2.0 drivers, can choke on these two lines	
	EGL_SAMPLE_BUFFERS,
	1,
	// if you want anti alias at a slight fps cost
	EGL_SAMPLES,
	4,
	//keep these 2lines, especially useful for lower resolution

	EGL_NONE
};

static const EGLint context_attributes[] =
{
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};


class ARMWindow : 
	public BaseWindow
{
public:
	ARMWindow();
	~ARMWindow();

	virtual int Init(const unsigned int width, const unsigned int height, const bool fullscreen = false);
	virtual void SwapBuffer();
	virtual BaseInput* PrepareInput();

	virtual void SetWindowShouldClose(const bool val);
	virtual bool ShouldWindowClose();

	void init_ogl(Target_State*, int width, int height, int, int);
	void esInitContext(Target_State*);

	void SetRes(int index, unsigned int& screenX, unsigned int& screenY);
	void GetRes(unsigned int& ReturnX, unsigned int& ReturnY);
	void RestoreRes();

	// info needed for X11 screen info, not all used initally.
	Display* x_display;
	Window win;

	short original_rate;
	Rotation original_rotation;
	SizeID original_size_id;

	XRRScreenSize* xrrs;
	XRRScreenConfiguration* conf;

	// when we start up we could use the current mouse cursor point to start our own cursor, though X11 can always interrogate it
	int CursorStartPointX;
	int CursorStartPointY;

	// we can store all the possible resolutions here		
	short possible_frequencies[64][64]; // a big list of details
	int num_rates; // how many possible rates
	int num_sizes; // how many possible sizes

protected:
	Target_State state;
	Target_State* p_state = &state;

	bool m_isfullscreen;
};

#endif // RASPBERRY