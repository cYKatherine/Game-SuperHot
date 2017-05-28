/*	FIT2096 - Week 1 Example Code
 *	Window.cpp
 *	Created by Elliott Wilson - 2015 - Monash University
 *	Implementation of Window.h
 */

#include "Window.h"
#include <iostream>

Window::Window(const char* windowName, int width, int height, bool fullscreen)
{
	//Setting up the basic information of the Window, nothing exciting here!
	m_windowName = windowName;
	m_width = width;
	m_height = height;
	m_fullscreen = true;
	m_renderer = NULL;
	m_input = NULL;

	QueryPerformanceFrequency(&m_counterFrequency);
	QueryPerformanceCounter(&m_lastCount);

}
Window::~Window()
{
	//When the window is destroyed we need to delete the Direct3D renderer
	if(m_renderer)
	{
		m_renderer->Shutdown();
		delete m_renderer;
		m_renderer = NULL;
	}
}

bool Window::Initialise()
{
	WNDCLASSEX windowClass;	//The WNDCLASSEX struct stores information about how the window functions
	DEVMODE screenSettings;	//The DEVMODE struct allows us to control the display mode of the screen (we can change the res, go fullscreen, etc.)
	int posX;	//Used to store the horizontal position of the window
	int posY;	//Used to store the vertical position of the window

	g_window = this;	//We need to set the global window pointer to this object. The WindowProc function will use this pointer so that it can
						//interact with our window object

	m_hInstance = GetModuleHandle(NULL);	//The GetModuleHandle function returns the HINSTANCE of the running program, this is required by the
											//WNDCLASS.

	memset(&windowClass, 0, sizeof(windowClass));	//We set the memory of the Window Class to 0, this clears it out so that we can fill it in cleanly

	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	//The style of a window refers to it's bsic behaviour, we don't need to worry too much about it.
	windowClass.lpfnWndProc = WindowProc;					//Here we specify the Window Procedure. This is the function that is run whenever the window receives a message from the OS.
	windowClass.hInstance = m_hInstance;					//The Window Class needs to know which program it belongs to so we set the HINSTANCE
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//Here we load a simple Application icon which is displayed in the taskbar
	windowClass.hIconSm = windowClass.hIcon;				//We also set it as the small icon which is displayed in the title bar of the window
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);		//We also load a default arrow to use a cursor
	windowClass.lpszClassName = m_windowName;				//A Window Class needs a unique name so that it can be referenced later, we use the one that was set in the constructor
	windowClass.cbSize = sizeof(WNDCLASSEX);				//Finally the Window Class needs to know how big it is, this information is used deep in the Windows API

	RegisterClassEx(&windowClass);	//Once we have created our Window Class we register it with the OS. We can now create windows basic off this class!

	if(m_fullscreen)
	{
		//If we want a full screen window we have to create a new DEVMODE struct to change the screen display mode.
		m_width = GetSystemMetrics(SM_CXSCREEN);	//First we need the Max width of the screen
		m_height = GetSystemMetrics(SM_CYSCREEN);	//Then the max height

		memset(&screenSettings, 0, sizeof(screenSettings));		//We then clean out our screen settings struct...
		screenSettings.dmPelsWidth = (unsigned long)m_width;	//...set the new width...
		screenSettings.dmPelsHeight = (unsigned long)m_height;	//...set the new height...
		screenSettings.dmBitsPerPel = 32;						//.. set the number of bits per pixel (colour depth), 32 is the highest for most machines
		screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;	//These fields basic describe with the DEVMODE struct is setting up
		screenSettings.dmSize = sizeof(screenSettings);			//Again like the Window Class this struct needs to know how big it is.

		ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);	//Once we have created our fullscreen settings we tell the OS to change over it's Display Settings

		posX = 0;	//Set the position of the top left corner of the window to the top left of the screen.
		posY = 0;
	}
	else
	{
		//if it's not a fullscreen window then we'll just place the window in the middle of the window.
		posX = (GetSystemMetrics(SM_CXSCREEN) - m_width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - m_height) / 2;
	}

	//After we have set the correct display settings and the window position we can then create the actual window!
	//The CreateWindowEx method returns a HWND which is a Handle that we use to refer to our window.
	m_windowHandle = CreateWindowEx(WS_EX_APPWINDOW,	//This is the extended window style, it allows us to define special functionality for the window, but we just want a simple app window
									m_windowName,		//This is the name of the Window Class that we will use for this window, this must match the name of the class we register earlier
									m_windowName,		//This is the title of the window, it appears in the title bar.
									WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,	//This is the window style, you can create interesting windows by ORing styles together
																					//The WS_POPUP gives us a nice boarderless window but try changing it to WS_OVERLAPPEDWINDOW
									posX, posY,			//Here we set the screen position of the Window
									m_width, m_height,	//Here we set the width and height
									NULL,				//Which window is the parent of this window? Noone!
									NULL,				//Does this window have a menu bar? Nope!
									m_hInstance,		//Which program does this Window belong to?
									NULL);				//Extra data for things that beyond the scope of this unit

	ShowWindow(m_windowHandle, SW_SHOW);	//Display our newly create window!
	SetForegroundWindow(m_windowHandle);	//Make sure it's in the foreground
	SetFocus(m_windowHandle);				//Also make sure its the active window!

	//ShowCursor(false);		//Do we want to see the cursor when we mouse over the window?

	//We create our 3D renderer and initialise it 
	m_renderer = new Direct3D();
	if(!m_renderer->Initialise(m_width, m_height, m_windowHandle, m_fullscreen, false))
	{
		ShowCursor(true);
		return false;
	}

	//Create our AudioSystem which will be passed into Game
	m_audio = new AudioSystem();
	if (!m_audio->Initialise())
	{
		ShowCursor(true);
		return false;
	}

	//Create an Input Controller
	m_input = new InputController(m_windowHandle);

	//We create our Game object and initialise it
	m_game = Game::GetInstance();
	if (!m_game->Initialise(m_renderer, m_audio, m_input))
	{
		ShowCursor(true);
		return false;
	}

	return true;
}

void Window::Start()
{
	//This method runs the message pump which looks out for new message sent by the OS

	//This struct stores the received message
	MSG msg;

	// We start off with 0 in our bank. See below for an explanation.
	float timeBank = 0.0f;

	// Our desired timestep - 60 frames a second.
	const float timeStep = 1.0f / 60.0f;

	bool running = true;

	while (running)
	{
		//If is a message, store it in msg and remove it from the message queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		//Translate the message...
			DispatchMessage(&msg);		//...and pass it off to the Window Procedure we registered earlier
		}

		//If we received a Quit message then terminate the message pump and quit the application
		if (msg.message == WM_QUIT)
		{
			running = false;
		}
		else
		{
			//Here we get the current count of the Performance Counter
			LARGE_INTEGER currentCount;
			QueryPerformanceCounter(&currentCount);

			//The calculation of the timestep is the difference between the count on this frame and the count on last frame divided by the frequency of the counter
			//This value is now the number of seconds that have passed since the last frame to this frame, hopefully it is a fractional number, otherwise we are running very slow!
			float timeElapsed = (currentCount.QuadPart - m_lastCount.QuadPart) / (float)m_counterFrequency.QuadPart;

			// Notice that, up until this point, we've been doing everything exactly the same as we did
			// in the variable time step version.

			// Here, we need to do something different. It basically boils down to:
			// 1) Decoupling Update and Render,
			// 2) Treating time more as a 'currency'. i.e. "Do I have enough time banked up to Update my game?"

			timeBank += timeElapsed;

			int framesSimulated = 0;

			// If we have time banked up, simulate a frame (call Update). 
			// Do this by looping fixed time increments (1/60) and passing this for update.
			// Depending on the timeBank, it's possible we will perform multiple updates
			// before a render pass takes place.
			while (timeBank > timeStep)
			{
				framesSimulated++;
				m_game->Update(timeStep);

				timeBank -= timeStep;
			}

			if (framesSimulated > 0)
			{
				m_game->Render();
			}

			m_lastCount = currentCount;

		}
	}
}

void Window::Shutdown()
{
	ShowCursor(true);	//When we shutdown we want to make sure the cursor comes back

	if(m_fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);	//If we were fullscreen we need to go back to the default display settings
	}

	if (m_game)
	{
		m_game->Shutdown();
		delete m_game;
		m_game = NULL;
	}

	if (m_renderer)
	{
		m_renderer->Shutdown();
		delete m_renderer;
		m_renderer = NULL;
	}

	if (m_input)
	{
		delete m_input;
		m_input = NULL;
	}

	DestroyWindow(m_windowHandle);		
	m_windowHandle = NULL;

	UnregisterClass(m_windowName, m_hInstance);	
	m_hInstance = NULL;

	g_window = NULL;
}

LRESULT CALLBACK Window::MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//This is the message procedure within the window class. The Global Window Procedure passes most of the messages it recieved into here
	switch(message)
	{
	case WM_KEYDOWN:
		m_input->SetKeyDown(wParam);
		return 0;

	case WM_KEYUP:
		m_input->SetKeyUp(wParam);
		return 0;

	case WM_LBUTTONDOWN:
		m_input->SetMouseDown(LEFT_MOUSE);
		return 0;

	case WM_LBUTTONUP:
		m_input->SetMouseUp(LEFT_MOUSE);
		return 0;

	case WM_RBUTTONDOWN:
		m_input->SetMouseDown(RIGHT_MOUSE);
		return 0;

	case WM_RBUTTONUP:
		m_input->SetMouseUp(RIGHT_MOUSE);
		return 0;

	case WM_MBUTTONDOWN:
		m_input->SetMouseDown(MIDDLE_MOUSE);
		return 0;

	case WM_MBUTTONUP:
		m_input->SetMouseUp(MIDDLE_MOUSE);
		return 0;

	case WM_INPUT:
	{
		UINT dwSize = 40;
		static BYTE lpb[40];
		memset(&lpb, 0, sizeof(lpb));
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* input = (RAWINPUT*)lpb;

		if (input->header.dwType == RIM_TYPEMOUSE)
		{
			m_input->SetMouseDeltaX(input->data.mouse.lLastX);
			m_input->SetMouseDeltaY(input->data.mouse.lLastY);
		}
		return 0;
	}

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);	//If it's any other kind of message then we'll just return the default message behaviour
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return g_window->MessageProc(hWnd, message, wParam, lParam);	//Here we use the global window pointer to pass all messages into the window object
}