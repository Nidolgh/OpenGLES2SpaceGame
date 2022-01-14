#ifdef RASPBERRY

#pragma once
#include "BaseInput.h"

#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <linux/input.h>

//*******************************************************************************************
//Mega thanks to Grumpy old Git Developer Gareth Lewis for his help in putting this together
//*******************************************************************************************

// Keys[] sets the relevent index to TRUE when pressed and FALSE when released, index names can be found in <linux/input.h> and follow the usual KEY_A...KEY_Z format
// mouse keeps track of the motion of the mouse not the absolute position, which needs to be known beforehand if you plan to use the mouse for direct control. 
// wherever the GUI mouse is at startup is considered 0,0 and all movement is offset from there. If no GUI it assumes 0,0 is start. User must draw their own mouse!

#define TRUE  1
#define FALSE 0

class ARMInput : 
	public BaseInput
{
protected:
	//****************************************************************************************************	
	// Intended to be a thread which processes the mouse events and stores  Mouse info in TheMouse struct.
	//****************************************************************************************************	
	static void* ProcessMouseThread(void* arg);

	//*********************************************************************************	
	// This thread processes the keys, and stores TRUE/FALSE values in the Keys[] array.
	//*********************************************************************************	
	static void* ProcessKeyboardThread(void* arg);

public:
	char Keys[512]; // maximum possible keys is a little less than this, but best to be careful in case of future expansion

	pthread_t threadKeyboard;
	pthread_t threadMouse; // handles for the threads

	bool mQuit; // set true to kill the mouse thread
	bool kQuit; // set true to kill the key thread

	int iterations;
	bool KeyPressed;

	ARMInput();
	virtual ~ARMInput();

	//small accessor function to test a specific key
	virtual bool TestKey(unsigned int WhatKey);

	// not really necessary for linux yet
	virtual void PollEvents();
	virtual void SwitchMouseMode() {}

	/************************
	must call init before use
	************************/
	virtual bool Init(const unsigned int pScr_width, const unsigned int pScr_height);

	//*****************************************************************************
	//this will test for our keyboards
	//*****************************************************************************
	int AreYouMyKeyboard();
	std::string kbd; // this will be the event filename for the keyboard discovered in the init/AreYouMyKeyboard test
};

#endif