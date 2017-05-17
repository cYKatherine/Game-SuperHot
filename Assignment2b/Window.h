/*	FIT2096 - Week 1 Example Code
 *	Window.h
 *	Created by Elliott Wilson - 2015 - Monash University
 *	This class represents a single Win32 window with a message pump
 *	and Window Procedure. It is also linked to the Direct 3D renderer
 *	which it uses to set the fill colour of the window.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include "Direct3D.h"
#include "Game.h"

class Window
{
private:
	int m_width;			//The width of the Window in pixels
	int m_height;			//The height of the Window in pixels
	bool m_fullscreen;		//Should the window be displayed fullscreen?
	LPCSTR m_windowName;	//The name of the Window (also used as the name of the window class) NOTE: LPCSTR is Windows syntax for a const char*
	HINSTANCE m_hInstance;	//The Handle to the program, this is a unique number that represents the running program
	HWND m_windowHandle;	//The Handle to this window, whenever to ask the Windows API to do something to the window you will pass it this handle

	Direct3D* m_renderer;	//The direct3D renderer class that we wrote, this will be used to set the background colour for our window.
	InputController* m_input;//The Input Controller is declared here. It will use the Window Proc to update it's state
	Game* m_game;			//This is our Game object. Here we create, update and render all of our game objects/models

	//These values are used to calculate the timestep, or the amount of time that has passed since the last frame
	//The timestep means that we calculate movement/animation in real time and not frame by frame
	//To work this value out we use the Performance Counter. 
	LARGE_INTEGER m_counterFrequency;	//The performance counter has a frequency which is basically the number counts per second
	LARGE_INTEGER m_lastCount;			//This value will store where the counter was upto on the last frame

public:
	Window(const char* windowName, int width, int height, bool fullscreen);	//A simple constructor used to set some initial values
	~Window();	//Destructor

	bool Initialise();	//The Initialise method is used to set up the window, it must be called before Start.
	void Start();		//The Start method starts the message pump running, while there are messages to process, the program will remain running.
	void Shutdown();	//The Shutdown method cleans up the window when it is about to be deleted.

	LRESULT CALLBACK MessageProc(HWND, UINT, WPARAM, LPARAM);	//This method is an internal message procedure.
																//It is declared within the scope of the Window class which means it is able to
																//modify variables inside the Window (for example an Input class which we will be
																//creating in a few weeks.) The Globally defined Window Procedure will call this
																//method via a static pointer which is set up when the Window is created.
};

//Global Functions
static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);	//Here is our real Window Procedure function, however it is declared in global
																//space. We don't like having important functions in global space so we will
																//use the pointer below to call the MessageProc function from our actual Window
																//object.

//Global Variables
static Window* g_window = 0;

#endif