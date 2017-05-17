/*	FIT2096 - Example Code
*	Texture.h
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	This class stores the required information for a single texture.
*	It uses the functions from the DirectX Tool Kit to achieve this.
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include "Direct3D.h"

class Texture
{
private:
	int m_referenceCount;
	const char* m_filename;

	ID3D11Resource* m_texture;					//The texture resource, can be used for procedurally modifying the texture
	ID3D11ShaderResourceView* m_textureView;	//The texture view, this is passed into our Shaders and the Sprite Batch

	Texture();
	~Texture();
	bool Load(Direct3D* renderer, const char* filename);

	void AddRef() { m_referenceCount++; }
	void RemoveRef() { m_referenceCount--; }
	int GetRefCount() { return m_referenceCount; }

	wchar_t* ConvertString(const char* str);
	
public:
	//Accessors
	ID3D11Resource* GetTexture() { return m_texture; }
	ID3D11ShaderResourceView* GetShaderResourceView() { return m_textureView; }
	const char* GetFilename() { return m_filename; }
	
	friend class TextureManager;
};

#endif