#ifdef RASPBERRY

#include "ARMWindow.h"

ARMWindow::ARMWindow()
{
	// don't really want to do anything here but could move init here
}

ARMWindow::~ARMWindow()
{
}

int ARMWindow::Init(const unsigned int width, const unsigned int height, const bool fullscreen)
{
	m_isfullscreen = fullscreen;

	m_width = width;
	m_height = height;

	esInitContext(p_state);
	init_ogl(p_state, m_width, m_height, m_width, m_height);

	ObjectData object_data; // create a single instance of object_data to initially draw all objects with the same data.

	p_state->object_data = &object_data;
}

void ARMWindow::SwapBuffer()
{
	eglSwapBuffers(p_state->display, p_state->surface);
}

BaseInput* ARMWindow::PrepareInput()
{
	return new ARMInput();
}

void ARMWindow::SetWindowShouldClose(const bool val)
{
	m_should_window_close = val;
}

bool ARMWindow::ShouldWindowClose()
{
	return m_should_window_close;
}

void ARMWindow::esInitContext(Target_State* p_state)
{
	if (p_state != NULL)
	{
		memset(p_state, 0, sizeof(Target_State));
	}
}

/*
Set up the EGL and in the case of Raspberry, DispmanX
Set up OpenGL states needed
Also set up the X11 window for rendering on non Taspberry (also used for Gui cursor use on Raspberry)
*/
#ifdef RASPBERRY // do the raspberry set up

void ARMWindow::init_ogl(Target_State* state, int width, int height, int fbx, int fby)
{
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	//RPI setup is a little different to normal EGL
	DISPMANX_ELEMENT_HANDLE_T DispmanElementH;
	DISPMANX_DISPLAY_HANDLE_T DispmanDisplayH;
	DISPMANX_UPDATE_HANDLE_T DispmanUpdateH;
	VC_RECT_T dest_rect;
	VC_RECT_T src_rect;
	EGLConfig config;

	// we also need an xwindow
	Window root;
	XSetWindowAttributes swa;
	XSetWindowAttributes xattr;
	Atom wm_state;
	XWMHints hints;
	XEvent xev;

	swa.event_mask = LockMask | ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;
	swa.background_pixmap = None;
	swa.background_pixel = 0;
	swa.border_pixel = 0;
	swa.override_redirect = true;
	swa.cursor = false;

	/*
	 * X11 native display initialization
	 */
	x_display = XOpenDisplay(NULL);
	if (x_display == NULL)
	{
		printf("Sorry to say we can't create an Xwindow and this will fail");
		exit(0); // we need to trap this;
	}
	else
		printf("we got an xdisplay\n");


	root = DefaultRootWindow(x_display);

	win = XCreateWindow(
		x_display,
		root,
		0, // puts it at the top left of the screen
		0,
		width, //set size  
		height,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&swa);

	if (win == 0)
	{
		printf("Sorry to say we can't create a window and this will fail\n");
		exit(0); // we need to trap this;
	}
	else
		printf("we got an (Native) XWindow\n");

	XFlush(x_display);

	bcm_host_init(); //RPI needs this

	// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(state->display, NULL, NULL);

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
	assert(state->context != EGL_NO_CONTEXT);

	// create an EGL window surface

	if (m_isfullscreen)
	{
		state->width = fbx;
		state->height = fby;

		src_rect.width = (fbx) << 16;
		src_rect.height = (fby) << 16;
	}
	else
	{
		state->width = width;
		state->height = height;
	}

	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.width = state->width; // it needs to know our window size
	dest_rect.height = state->height;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = 0;
	src_rect.height = 0;

	DispmanDisplayH = vc_dispmanx_display_open(0);
	DispmanUpdateH = vc_dispmanx_update_start(0);

	DispmanElementH = vc_dispmanx_element_add(
		DispmanUpdateH,
		DispmanDisplayH,
		m_isfullscreen/*layer*/,
		&dest_rect,
		0/*source*/,
		&src_rect,
		DISPMANX_PROTECTION_NONE,
		0 /*alpha value*/,
		0/*clamp*/,
		(DISPMANX_TRANSFORM_T)0/*transform*/);

	state->nativewindow.element = DispmanElementH;

	if (m_isfullscreen)
	{
		state->nativewindow.width = fbx;
		state->nativewindow.height = fby;
	}
	else
	{
		state->nativewindow.width = state->width;
		state->nativewindow.height = state->height;
	}
	
	vc_dispmanx_update_submit_sync(DispmanUpdateH);

	state->surface = eglCreateWindowSurface(state->display, config, &(state->nativewindow), NULL);
	assert(state->surface != EGL_NO_SURFACE);

	// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);

	EGLBoolean test = eglSwapInterval(state->display, 1);
	// 1 to lock speed to 60fps (assuming we are able to maintain it), 0 for immediate swap (may cause tearing) which will indicate actual frame rate

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

	glViewport(0, 0, state->width, state->height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	if (glGetError() == GL_NO_ERROR) return;
	else
		printf("Oh bugger, Some part of the EGL/OGL graphic init failed\n");
}
#else // the standard X11 set up for non raspberry systems (or Raspberry 4B with X11)

void the_ARMWindow::init_ogl(Target_State* state, int width, int height, int FBResX, int FBResY)

{

#define ES_WINDOW_RGB           0
	state->width = width;
	state->height = height;

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;

	EGLDisplay egldisplay;
	EGLContext context;
	EGLSurface surface;
	EGLConfig config;
	Window root;
	XSetWindowAttributes swa;
	XSetWindowAttributes  xattr;
	Atom wm_state;
	XWMHints hints;
	XEvent xev;
	EGLint num_config;


	//eglBindAPI(EGL_OPENGL_ES_API);
		/*
		 * X11 native display initialization
		 */

	x_display = XOpenDisplay(NULL);
	if (x_display == NULL)
	{
		printf("Sorry to say we can't open an XDisplay and this will fail\n");
		exit(0);      // we need to trap this;
	}
	else
		printf("we got an XDisplay\n");


	root = DefaultRootWindow(x_display);

	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
	swa.background_pixmap = None;
	swa.background_pixel = 0;
	swa.border_pixel = 0;
	swa.override_redirect = true;

	win = XCreateWindow(
		x_display,
		root,
		0,		// puts it at the top left of the screen
		0,
		state->width,	//set size  
		state->height,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&swa);

	if (win == 0)
	{
		printf("Sorry to say we can't create a window and this will fail\n");
		exit(0);       // we need to trap this;
	}
	else
		printf("we got an (Native) XWindow\n");

	state->nativewindow = (EGLNativeWindowType)win;

	XSelectInput(x_display, win, KeyPressMask | KeyReleaseMask);
	xattr.override_redirect = TRUE;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	hints.input = TRUE;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);


	char* title = (char*)"x11 Demo";
	// make the window visible on the screen
	XMapWindow(x_display, win);
	XStoreName(x_display, win, title);

	// get identifiers for the provided atom name strings
	wm_state = XInternAtom(x_display, "_NET_WM_STATE", FALSE);

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = FALSE;
	XSendEvent(
		x_display,
		DefaultRootWindow(x_display),
		FALSE,
		SubstructureNotifyMask,
		&xev);


	// Get Display	
	egldisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);       //eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (egldisplay == EGL_NO_DISPLAY)
	{
		printf("Sorry to say we have an GetDisplay error and this will fail");
		exit(0);          // we need to trap this;
	}
	else  	printf("we got an EGLDisplay\n");

	// Initialize EGL
	if (!eglInitialize(egldisplay, &majorVersion, &minorVersion))
	{
		printf("Sorry to say we have an EGLinit error and this will fail");
		EGLint err = eglGetError();    // should be getting error values that make sense now
		exit(err);            // we need to trap this;
	}
	else 	printf("we initialised EGL\n");


	// Get configs
	if (!eglGetConfigs(egldisplay, NULL, 0, &numConfigs))
	{
		printf("Sorry to say we have EGL config errors and this will fail");
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}
	else 	printf("we got %i Configs\n", numConfigs);


	// Choose config
	if (!eglChooseConfig(egldisplay, attribute_list, &config, 1, &numConfigs))
	{
		printf("Sorry to say we have config choice issues, and this will fail");
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}
	else	printf("we chose our config\n");




	// Create a GL context
	context = eglCreateContext(egldisplay, config, EGL_NO_CONTEXT, context_attributes);
	if (context == EGL_NO_CONTEXT)
	{
		EGLint err = eglGetError();
		exit(err);             // we need to trap this;
	}
	else	printf("Created a context ok\n");

	// Create a surface
	surface = eglCreateWindowSurface(egldisplay, config, state->nativewindow, NULL);     // this fails with a segmentation error?
// don't get this far, forced error on eglCreateWindowSurface	
	if (surface == EGL_NO_SURFACE)
	{
		EGLint err = eglGetError();
		exit(err);               // we need to trap this;
	}
	else 	printf("we got a Surface\n");


	// Make the context current
	if (!eglMakeCurrent(egldisplay, surface, surface, context))
	{
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}

	state->display = egldisplay;
	state->surface = surface;
	state->context = context;


	EGLBoolean test = eglSwapInterval(egldisplay, 1);       // 1 to lock speed to 60fps (assuming we are able to maintain it), 0 for immediate swap (may cause tearing) which will indicate actual frame rate
// on xu4 this seems to have no effect

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



	printf("This SBC supports version %i.%i of EGL\n", majorVersion, minorVersion);
	printf("This GPU supplied by  :%s\n", glGetString(GL_VENDOR));
	printf("This GPU supports     :%s\n", glGetString(GL_VERSION));
	printf("This GPU Renders with :%s\n", glGetString(GL_RENDERER));
	printf("This GPU supports     :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("This GPU supports these extensions	:%s\n", glGetString(GL_EXTENSIONS));

	glViewport(0, 0, state->width, state->height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glCullFace(GL_BACK);
	if (glGetError() == GL_NO_ERROR)	return;
	else
		printf("Oh bugger, Some part of the EGL/OGL graphic init failed\n");
}

#endif // RASPBERRY

void ARMWindow::GetRes(unsigned int& ReturnX, unsigned int& ReturnY)
{
	//#ifdef RASPBERRY // raspberry dosn't really work well with xrandr.. so we simply return our current user set res
	//	uint32_t width;
	//	uint32_t height;
	//	ARMWindow_get_display_size(0, &width, &height);
	//		
	//	ReturnX =  width;
	//	ReturnY  = height;
	//	
	//#endif

	// you will need to make sure you have  installed libXrandr to use this..
	//xhisudo apt-get install libxrandr-dev

	//
	//     CONNECT TO X-SERVER, GET ROOT WINDOW ID
	//
	//
	//     GET POSSIBLE SCREEN RESOLUTIONS
	//
	xrrs = XRRSizes(x_display, 0, &num_sizes);
	//
	//     LOOP THROUGH ALL POSSIBLE RESOLUTIONS,
	//     GETTING THE SELECTABLE DISPLAY FREQUENCIES
	//


	for (int i = 0; i < num_sizes; i++)
	{
		short* rates;


		printf("\n\t%2i : %4i x %4i   (%4imm x%4imm ) ", i, xrrs[i].width, xrrs[i].height, xrrs[i].mwidth,
			xrrs[i].mheight);

		rates = XRRRates(x_display, 0, i, &num_rates);

		for (int j = 0; j < num_rates; j++)
		{
			possible_frequencies[i][j] = rates[j];
			printf("%4i ", rates[j]);
		}
	}

	printf("\n");
}

void ARMWindow::SetRes(int index, unsigned int& screenX, unsigned int& screenY)
{
	//
	//     Set RESOLUTION AND FREQUENCY
	//
	GetRes(screenX, screenY); // check what we have
	// store the original info for and end
	conf = XRRGetScreenInfo(x_display, DefaultRootWindow(x_display));
	original_rate = XRRConfigCurrentRate(conf);
	original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);

	printf("\n\tCURRENT SIZE ID  : %i\n", original_size_id);
	printf("\tCURRENT ROTATION : %i \n", original_rotation);
	printf("\tCURRENT RATE     : %i Hz\n\n", original_rate);
	// find 1280x720		

	//
	int SelectedRes = index; // pretty much any system can handle 1024x768 
	printf("\tCHANGED TO %i x %i PIXELS, %i Hz\n\n", xrrs[SelectedRes].width, xrrs[SelectedRes].height,
		possible_frequencies[SelectedRes][0]);
	XRRSetScreenConfigAndRate(x_display, conf, DefaultRootWindow(x_display), SelectedRes, RR_Rotate_0,
		possible_frequencies[SelectedRes][0], CurrentTime);
	screenX = xrrs[SelectedRes].width;
	screenY = xrrs[SelectedRes].height;
}

void ARMWindow::RestoreRes()
{
	printf("\tRESTORING %i x %i PIXELS, %i Hz\n\n", xrrs[original_size_id].width, xrrs[original_size_id].height,
		original_rate);
	XRRSetScreenConfigAndRate(x_display, conf, DefaultRootWindow(x_display), original_size_id, original_rotation,
		original_rate, CurrentTime);
}

#endif // RASPBERRY