/*	FIT2096 - Example Code
*	Texture.cpp
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	Implementation of Texture.h
*/

#include "Texture.h"
#include "DirectXTK/WICTextureLoader.h"
#include <sstream>

using namespace DirectX;

Texture::Texture()
{
	m_referenceCount = 0;
	m_filename = "";
	m_texture = NULL;
	m_textureView = NULL;
}

Texture::~Texture()
{
	if (m_texture)
	{
		m_texture->Release();
		m_texture = NULL;
	}

	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = NULL;
	}
}

bool Texture::Load(Direct3D* direct3D, const char* filename)
{
	//This method uses the CreateWICTextureFromFile function. This function comes from the DirectXToolKit library

	wchar_t* wideFilename = ConvertString(filename);

	HRESULT result = CreateWICTextureFromFile(direct3D->GetDevice(), wideFilename, &m_texture, &m_textureView);

	if (FAILED(result))
	{
		return false;
	}

	m_filename = filename;

	return true;
}

wchar_t* Texture::ConvertString(const char * str)
{
	// Convert C string (which we like using) into a wchar_t* (which the texture loader likes) 
	// https://msdn.microsoft.com/en-us/library/ms235631.aspx
	size_t newsize = strlen(str) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, str, _TRUNCATE);
	return wcstring; 
}

