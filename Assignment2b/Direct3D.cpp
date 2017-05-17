#include "Direct3D.h"

Direct3D::Direct3D()
{
	m_vsync = false;
	m_swapChain = NULL;
	m_device = NULL;
	m_deviceContext = NULL;
	m_renderTargetView = NULL;
	m_depthStencilBuffer = NULL;
	m_depthStencilState = NULL;
	m_depthStencilView = NULL;
	m_rasterState = NULL;
}

Direct3D::~Direct3D()
{

}

bool Direct3D::Initialise(int width, int height, HWND windowHandle, bool fullscreen, bool vsync)
{
	//This is a big method! There is a lot to get through but if you don't understand something, don't worry we will be covering most of it again

	m_vsync = vsync;							//Remember our vsync setting
	IDXGIFactory* factory;						//This factory object will tell us about the state of the graphics hardware
	IDXGIAdapter* adapter;						//This will represent a single graphics adapter (card)
	IDXGIOutput* adapterOutput;					//This will represent a single graphic output (screen)

	unsigned int numValidDisplayModes;			//We need to know what display modes are supported by our display, this will store the number of display modes
	DXGI_MODE_DESC* displayModes;				//And this will be an array of display mode descriptions

	unsigned int refreshNumerator;				//Using the display mode we can find the refresh rate.
	unsigned int refreshDenominator;			//The refresh rate is expressed as a numberator and denominator

	DXGI_ADAPTER_DESC adapterDescription;		//The adapter can give us some information about itself, like amount of memory and name of the device
	DXGI_SWAP_CHAIN_DESC swapChainDescription;	//We use this struct to set up options and settings for the swap chain we are going to create

	ID3D11Texture2D* backBuffer;				//This texture points to our back buffer, this is where we are going to make our rendertargetview point to

	//NOTE: Most of the Direct3D functions return a HRESULT, this is basically just a number that tells us the if the method was sucessful.
	//The FAILED macro checks this HRESULT and returns false if it is less then 0 (which means the method failed)

	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))	//This method create the DXGI factory, the __uuidof operator is used by microsoft to 
	{																		//to get a unique id for the COM interface we are trying to create
		return false;
	}

	if (FAILED(factory->EnumAdapters(0, &adapter)))		//The factory is able to get us information about the graphics adapters that are available on the system
	{
		return false;
	}

	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))	//The adapter object is able to get us information about the Outputs (screens) that are available on the system
	{
		return false;
	}

	//The output can tell us about the display modes (resolutions) that it supports, however first need need to know the number of display modes
	//Note the final two parameters, the fist is a reference to an unsigned int which stores the number of display modes
	//The second is a pointer to an array to fill out, we don't know how many display mdoes there are so we need to find that number before we allocate an array
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numValidDisplayModes, NULL)))
	{
		return false;
	}

	//Once we have the number of display mdoes we create an array of Mode Descriptions...
	displayModes = new DXGI_MODE_DESC[numValidDisplayModes];
	//... and call the Get Display mode list function again, but this time we pass in the array we just created.
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numValidDisplayModes, displayModes)))
	{
		delete[] displayModes;	//if it fails we need to delete the array we created
		return false;
	}

	//Once we have an array of display modes we need to go through them...
	for (unsigned int i = 0; i < numValidDisplayModes; i++)
	{
		//...and look for one with the same size as the window we created
		if ((displayModes[i].Width == (unsigned int)width) && (displayModes[i].Height == (unsigned int)height))
		{
			//When we find a valid display mode, we save the refresh rate values!
			refreshNumerator = displayModes[i].RefreshRate.Numerator;
			refreshDenominator = displayModes[i].RefreshRate.Denominator;
		}
	}

	if (FAILED(adapter->GetDesc(&adapterDescription)))	//This method fills out a struct with information about our graphics adapter
	{
		delete[] displayModes;
		return false;
	}

	m_videoCardMemory = (int)(adapterDescription.DedicatedVideoMemory / 1024 / 1024);	//One piece of information is the amount of video memory we have available.

	//Now that we have found the information we need we can get rid of the objects we created
	delete[] displayModes;
	displayModes = NULL;

	adapterOutput->Release();
	adapterOutput = NULL;

	adapter->Release();
	adapter = NULL;

	factory->Release();
	factory = NULL;

	memset(&swapChainDescription, 0, sizeof(swapChainDescription));			//We now need to set up our swap chain, we start by clearing out the struct
	swapChainDescription.BufferCount = 1;									//We then set the number of buffers we want on our swap chain, 1 is enough for us
	swapChainDescription.BufferDesc.Width = width;							//Set the width...
	swapChainDescription.BufferDesc.Height = height;						//and height
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//We then set the pixel format, we're not going to worry too much about the different pixel formats.

	if (m_vsync)
	{
		//If we want to use vsync then we need to tell the swap chain what refresh rate to use.
		swapChainDescription.BufferDesc.RefreshRate.Numerator = refreshNumerator;
		swapChainDescription.BufferDesc.RefreshRate.Denominator = refreshDenominator;
	}
	else
	{
		//Otherwise use no refresh rate
		swapChainDescription.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//Here we set what the swap chain buffers are used for, we're just going to render into them
	swapChainDescription.OutputWindow = windowHandle;					//Here we set the output window for the swap chain
	swapChainDescription.SampleDesc.Count = 1;							//Here we set up multisampling/anti aliasing values
	swapChainDescription.SampleDesc.Quality = 0;						//Right now we are setting up no AA

	if (fullscreen)
	{
		swapChainDescription.Windowed = false;	//If the game is fullscreen, set windowed to false
	}
	else
	{
		swapChainDescription.Windowed = true;
	}

	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	//Setting up the scanline ordering, we don't need to worry about it
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					//Setting up the buffer scaling, again we don't need to worry about it
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;									//This is the swap effect, we will cover this in a bit more detail later

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };	//We are going to target the DirectX 11 feature level, and fallback to 10 if needed

	//This method creates the major direct3D objects we need, the device, the device context and the swap chain. It has quite a few parameters...
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL,						//What adapter should we use? NULL means default
		D3D_DRIVER_TYPE_HARDWARE,	//What kind of driver to use? A Hardware driver for most games
		NULL,						//We will not be using a software rasterizer
		0,							//Option flags can go here, we don't need any
		&featureLevel[0], 2,		//An array of feature levels to support, the second value is the number of values in that array
		D3D11_SDK_VERSION,			//The SDK version, you should pretty much always use D3D11
		&swapChainDescription,		//We need to pass in the swap chain description/options that we created
		&m_swapChain,				//This is an output parameter, if everything works our Swap Chain should end up in this pointer
		&m_device,					//This is the output parameter for the device object
		NULL,						//This will tell us the actual feature level that was choosen, but we only provided one so we don't need it
		&m_deviceContext)))			//This is the output parameter for the device context
	{
		return false;
	}

	if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))	//After we have created the swap chain we want to get a pointer to the back buffer in the chain
	{
		return false;
	}

	if (FAILED(m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView)))		//We then use the back buffer create a Render Target View that we can use to render into that buffer
	{
		backBuffer->Release();
		backBuffer = NULL;
		return false;
	}

	backBuffer->Release();	//After we have the Render Target View, we don't need the direct pointer to the back buffer
	backBuffer = NULL;

	if (!InitDepthBuffer(width, height))
		return false;

	if (!InitDepthStencil())
		return false;

	if (!InitRasteriser())
		return false;

	InitViewport(width, height);

	return true;
}

bool Direct3D::InitDepthBuffer(int width, int height)
{
	D3D11_TEXTURE2D_DESC depthBufferDescription;
	memset(&depthBufferDescription, 0, sizeof(depthBufferDescription));

	depthBufferDescription.Width = width;
	depthBufferDescription.Height = height;
	depthBufferDescription.MipLevels = 1;
	depthBufferDescription.ArraySize = 1;
	depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescription.SampleDesc.Count = 1;
	depthBufferDescription.SampleDesc.Quality = 0;
	depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDescription.CPUAccessFlags = 0;
	depthBufferDescription.MiscFlags = 0;

	if (FAILED(m_device->CreateTexture2D(&depthBufferDescription, NULL, &m_depthStencilBuffer)))
	{
		return false;
	}

	return true;
}

bool Direct3D::InitDepthStencil()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription;
	memset(&depthStencilDescription, 0, sizeof(depthStencilDescription));

	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDescription.StencilEnable = true;
	depthStencilDescription.StencilReadMask = 0xFF;
	depthStencilDescription.StencilWriteMask = 0xFF;
	depthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDescription, &m_depthStencilState)))
	{
		return false;
	}

	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	memset(&depthStencilViewDescription, 0, sizeof(depthStencilViewDescription));
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;

	if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDescription, &m_depthStencilView)))
	{
		return false;
	}

	//After we have created a render target view and a depth buffer view we set the device context to use them as render targets
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return true;
}

bool Direct3D::InitRasteriser()
{
	//The Raster description allows us to set some options regarding the raster (make 2D) process. These are basically just default settings.
	D3D11_RASTERIZER_DESC rasterDescription;
	rasterDescription.AntialiasedLineEnable = false;
	rasterDescription.CullMode = D3D11_CULL_BACK;
	rasterDescription.DepthBias = 0;
	rasterDescription.DepthBiasClamp = 0.0f;
	rasterDescription.DepthClipEnable = true;
	rasterDescription.FillMode = D3D11_FILL_SOLID;
	rasterDescription.FrontCounterClockwise = false;
	rasterDescription.MultisampleEnable = false;
	rasterDescription.ScissorEnable = false;
	rasterDescription.SlopeScaledDepthBias = 0.0f;

	//After we have the raster setting we use the device to create a raster state
	if (FAILED(m_device->CreateRasterizerState(&rasterDescription, &m_rasterState)))
	{
		return false;
	}

	m_deviceContext->RSSetState(m_rasterState);	//We then set that raster state via the device context

	return true;
}

void Direct3D::InitViewport(int width, int height)
{
	//This view port object allows us to define which part of our screen will be rendered into.
	D3D11_VIEWPORT viewport;
	//A viewport is a section of the screen that graphics are rendered into. It is possible to have multiple view ports for a split screen multiplayer game. We only wnat one though.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void Direct3D::Shutdown()
{
	//The shut down method just involves releasing all of the COM interfaces we created during the initialise method
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}
}

void Direct3D::BeginScene(float red, float green, float blue, float alpha)
{
	//The clear method requires an array of 4 floats to represent the clear colour
	float colour[4];
	colour[0] = red;
	colour[1] = green;
	colour[2] = blue;
	colour[3] = alpha;

	//When we use a sprite batch it messes with a lot of values on our device context. We need to set them back before we start a new scene
	m_deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	m_deviceContext->RSSetState(m_rasterState);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colour);		//We clear the buffer out to the colour specified
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);		//We also clear the depth buffer
}

void Direct3D::EndScene()
{
	if (m_vsync)
	{
		m_swapChain->Present(1, 0);	//If we use vsync that we state that we want to wait for the vertical blank (refresh rate)
	}
	else
	{
		m_swapChain->Present(0, 0);	//Otherwise, don't wait at all
	}
}