/*	FIT2096 - Week 1 Example Code
 *	Main.cpp
 *	Created by Elliott Wilson - 2015 - Monash University
 *	Entry point and main driver for the application
 */

#include "Window.h"
#include <time.h>

void CreateConsole()
{
	AllocConsole();
	FILE* out;
	freopen_s(&out, "CONOUT$", "w", stdout);
}


//Windows API programs have a special Main method, WinMain!
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	srand((unsigned int)time(0));

	CoInitialize(0);

	CreateConsole();

	Window* win = new Window("FIT2096: Games Programming 1 - Week 7", 1280, 720, false);	//We'll create our window object, set a size and if we want it fullscreen

	if(win->Initialise())	//If the window initialises...
	{
		win->Start();		//...then we start the message pump running!
	}
	else
	{
		MessageBox(NULL, "Could not create Window", "Error", MB_OK); //otherwise, show an error message
	}

	win->Shutdown();	//If we have an error, or the message pump stops then we Shutdown our window...
	delete win;			//...delete it...
	win = NULL;

	return 0;			//...and quit!



	
}