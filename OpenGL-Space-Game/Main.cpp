/* 
code adapted from OpenGL� ES 2.0 Programming Guide
and code snippets from RPI Forum to set up Dispmanx

Project builds as Visual GDB (Raspberry), or Visual GDB (X11)
Also C++ for Linux Builds for Raspberry  or X11
*/

#include "Headers/Game.h"

int main(int argc, char* argv[])
{
	Game the_game;

	the_game.Init();
	the_game.MainLoop();

	return 0;
}