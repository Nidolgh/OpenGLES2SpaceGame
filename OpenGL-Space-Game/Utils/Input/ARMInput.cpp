#ifdef RASPBERRY

#include "ARMInput.h"
#include <dirent.h>
#include <sstream>

void* ARMInput::ProcessMouseThread(void* arg)
{
	FILE* fmouse;
	fmouse = fopen("/dev/input/mice", "r");

	const int scr_width = (((ARMInput*)arg)->m_screen_width);
	const int scr_height = (((ARMInput*)arg)->m_screen_height);

	if (fmouse != NULL)
	{
		while (((ARMInput*)arg)->mQuit == false) // so as long as mQuit is FALSE, this will endlessly loop
		{
			signed char b[3];
			fread(b, sizeof(char), 3, fmouse);

			// if we do plan to scale, best make these into floats for greater precision before they are cast down to ints.
			float mousex = (float)b[1];
			float mousey = -(float)b[2];

			((ARMInput*)arg)->m_mouse_data.RelX = mousex;
			((ARMInput*)arg)->m_mouse_data.RelY = -mousey;

			((ARMInput*)arg)->m_mouse_data.PositionX += (mousex / 1.0f);
			// 1.0 can be replaced by a scale factor (entierly optional)
			if (((ARMInput*)arg)->m_mouse_data.PositionX > scr_width) ((ARMInput*)arg)->m_mouse_data.PositionX = scr_width;
			if (((ARMInput*)arg)->m_mouse_data.PositionX < 0) ((ARMInput*)arg)->m_mouse_data.PositionX = 0;

			((ARMInput*)arg)->m_mouse_data.PositionY += (mousey / 1.0f);

			if (((ARMInput*)arg)->m_mouse_data.PositionY > scr_height) ((ARMInput*)arg)->m_mouse_data.PositionY = scr_height;
			if (((ARMInput*)arg)->m_mouse_data.PositionY < 0) ((ARMInput*)arg)->m_mouse_data.PositionY = 0;


			((ARMInput*)arg)->m_mouse_data.LeftButton = (b[0] & 1) > 0;
			// using a test( x >0 )  allows it to return and store a bool
			((ARMInput*)arg)->m_mouse_data.MidButton = (b[0] & 4) > 0;
			((ARMInput*)arg)->m_mouse_data.RightButton = (b[0] & 2) > 0;
		}
		fclose(fmouse);
	}
	printf("Mouse Thread closing \n");
	pthread_exit(NULL);
}

void* ARMInput::ProcessKeyboardThread(void* arg)
{
	FILE* fp;
	fp = fopen(((ARMInput*)arg)->kbd.c_str(), "r"); // normal scanned keyboard
	printf("Using keyboard string :%s\n ", ((ARMInput*)arg)->kbd.c_str());

	//fp = fopen("/dev/input/event4", "r"); // hacked forced event0 or event1 or... (up to max events)

	struct input_event ev;

	if (fp != NULL)
	{
		while (((ARMInput*)arg)->kQuit == false) // kQuit is set to false by the init
		{
			fread(&ev, sizeof(struct input_event), 1, fp);

			if (ev.type == (__u16)EV_KEY)
			{
				((ARMInput*)arg)->Keys[ev.code] = ev.value > 0 ? TRUE : FALSE; // never gets here to give me key values
				((ARMInput*)arg)->KeyPressed = true;
			}
			else ((ARMInput*)arg)->KeyPressed = false;
		}
		printf("quit responded too\n");
		fclose(fp);
	}

	printf("Key Thread closing\n");
	printf("  err %d \n", errno); // we closed so let's see the error
	pthread_exit(NULL);
}


ARMInput::ARMInput()
{
}

ARMInput::~ARMInput()
{
}

bool ARMInput::TestKey(unsigned int WhatKey)
{
	return Keys[WhatKey] == TRUE; // simply return binary true or false
}

void ARMInput::PollEvents()
{
	return;
}

bool ARMInput::Init(const unsigned int scr_width, const unsigned int scr_height)
{
	m_screen_width = scr_width;
	m_screen_height = scr_height;

	kQuit = false;
	mQuit = false;
	iterations = 0;

	int result;

	//mice don't usually provide any issues
	result = pthread_create(&threadMouse, NULL, &ProcessMouseThread, this);
	// we send the HandleShipInput class (this) as an argument to allow for easy cast ((HandleShipInput*)arg)-> type access to the classes data. 
	if (result != 0) printf("got an error\n");
	else printf("mouse thread started\n");

	if (AreYouMyKeyboard() == false) printf("Oh Bugger, we can't seen to find the keyboard\n");
	// go find an active keyboard

	result = pthread_create(&threadKeyboard, NULL, &ProcessKeyboardThread, this);
	if (result != 0) printf("got an error\n");
	else printf("Key thread started\n");


	return true;
}

// tests for the keyboard, which can be on different events in Linux
// thanks to my student Petar Dimitrov for this improvement to the keyboard search systems
int ARMInput::AreYouMyKeyboard()
{
	/*
	 Note linux machines may have their key and mouse event files access protected,
	 in which case open a command line terminal, and enter
	 sudo chmod  a+r /dev/input/* (assuming your input event files are there)
	 this is kinda frowned on by linux users, but I don't know a way to overcome this in code yet.
	 it may also be possible to get VisualGDB to execute the build as sudo for root access but I find that flakey
	*/
	// Some bluetooth keyboards are registered as "event-mouse".
	// If this is your case, then just change this variable to event-mouse.
	std::string pattern = "event-kbd";
	//<-change to event-mouse if your BT keyboard is a "mouse" or test for a failure to find a kbd, then try as a mouse.
	std::string file = "";

	DIR* dir;
	struct dirent* ent;

	printf("Checking for active keyboard\n");

	if ((dir = opendir("/dev/input/by-path/")) != nullptr)
	{
		while ((ent = readdir(dir)) != nullptr)
		{
			fprintf(stdout, "%s\n", ent->d_name);
			file = std::string(ent->d_name);

			if (!file.compare(".") || !file.compare("..")) continue;

			if (file.substr(file.length() - pattern.length()) == pattern)
			{
				kbd = "/dev/input/by-path/" + file;
				fprintf(stdout, "%s\n", kbd.c_str());

				return true;
			}
		}
	}

	return false;
}

#endif