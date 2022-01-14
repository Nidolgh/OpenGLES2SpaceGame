#pragma once
// Image GUI UI is a practical UI interface we can use to provide an interacive UI for game related values
// its not especially useful for in game use but can be adapeted. Its main advantage is the ability to alter and display variables
// and system info during game set up or mode changes

#include "Utils/ImGUI/imgui.h"
#include "Utils/ImGUI/imgui_impl_opengl3.h"

#include <istream>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Utils/Managers/UniverseManager.h"
#include "Utils/Window/BaseWindow.h" // included as it has the resolution data we can use for display
#include "Utils/Input/BaseInput.h" // included as it has the resolution data we can use for display

class ImgGuiUI
{
public:
	ImgGuiUI();
	~ImgGuiUI();

	// maybe we can keep a list of different windows and display them in turn or in order	
	//	std::vector<> List?;

	bool Init(const BaseWindow*, const BaseInput*, const UniverseManager*);

	bool Remove()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
		return true;
	}; // if we are done with UI (after an initial set up), lets remove it

	bool Update(bool DoINeedTo, float DT);
	bool Render(bool DoINeedTo);

	const BaseWindow* m_pWindow				= nullptr;
	const BaseInput* m_pInput				= nullptr;
	const UniverseManager* m_pUniverse_man	= nullptr;

	// fps counter
	float totaltime		= 0.0f;
	unsigned int frames = 0;

	float cur_fps		= 0.0f;
	float cur_frames	= 0.0f;
	float cur_time		= 0.0f;
	float cur_dt		= 0.0f;
};
