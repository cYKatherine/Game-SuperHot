#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <d3d11.h>
#include <map>
#include <string>

#include "Texture.h"

class TextureManager
{
private:
	typedef std::map<std::string, Texture*> TextureMap;
	TextureMap m_textureMap;

public:
	TextureManager();
	~TextureManager();
	bool Load(Direct3D* renderer, const char* filename);
	Texture* GetTexture(const char* identifier);
	void ReleaseTexture(Texture* texture);
	void Release();
};

#endif