/*	FIT2096 - Example Code
*	Direct3D.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This class represents the Direct3D rendering system that we will
*   be using throughout the semester. We will be modifying throughout
*	the unit.
*/
#ifndef DIRECT3D_H
#define DIRECT3D_H

//These pragma statements tell the compiler to link in the listed binary libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#pragma comment(lib, "DirectXTK_VS2013.lib")
#endif

#if defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma comment(lib, "DirectXTK_VS2015.lib")
#endif

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include "Shader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Direct3D
{
private:
	bool m_vsync;									//Do we want to wait for vsync? (match our timing with the refresh rate of the monitor)
	int m_videoCardMemory;							//This will store the amount of dedicated video memory
	const char* m_videoCardDescription;				//This will store the name of the video card
	unsigned int refreshNumerator;					//These variables will store the refresh rate of the primary screen
	unsigned int refreshDenominator;				//The refresh rate is expressed as a numberator and denominator
	IDXGISwapChain* m_swapChain;					//This is our swap chain, it's important but we will be covering it in more detail in the coming weeks
	ID3D11Device* m_device;							//This is our Direct3D 11 device, you can think of it as virtual graphics adapter
	ID3D11DeviceContext* m_deviceContext;			//This is the device context, it is linked to the device object and we send our rendering commands here
	ID3D11RenderTargetView* m_renderTargetView;		//This is a render target view, it points of a piece of graphics memory that we are able to render to
	ID3D11Texture2D* m_depthStencilBuffer;			//This texture represents our depth (or Z) buffer. We'll be talking about this more in the coming weeks
	ID3D11DepthStencilState* m_depthStencilState;	//This represents the state of the depth buffer, we can use it to set options regarding the depth buffer
	ID3D11DepthStencilView* m_depthStencilView;		//This is the depth buffer view, we use this to render to our depth buffer
	ID3D11RasterizerState* m_rasterState;			//This is the rasterstate, it is used to turn our 3D simulation into a 2D image

	Shader* m_currentShader;

	// Initialisation helpers
	bool InitDepthBuffer(int width, int height);
	bool InitDepthStencil();
	bool InitRasteriser();
	void InitViewport(int width, int height);

public:
	//Constructors
	Direct3D();
	//Destructor
	~Direct3D();

	bool Initialise(int width, int height, HWND windowHandle, bool fullscreen, bool vsync);	//This method starts up Direct3D 11. It needs to know a bit of information
	//such as the width and height of the window, the handle to the window that we
	//want to render into, if we want full screen and if we want to use vsync.
	void Shutdown();	//This method cleanly shuts down Direct3D11

	void BeginScene(float red, float green, float blue, float alpha);	//This method begins a frame of rendering, it clears the screen to a solid colour
	void EndScene();	//This method ends a frame, it uses our swap chain to switch the buffer we are rendering into with the buffer on the screen

	//Both the Device and the DeviceContext are used throughout Direct3D so we expose them here in case we need them later
	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; }

	Shader* GetCurrentShader() { return m_currentShader; }
	void SetCurrentShader(Shader* shader) { m_currentShader = shader; }
};

#endif