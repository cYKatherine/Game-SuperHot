#include "TextureManager.h"

using namespace std;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
	Release();
}

bool TextureManager::Load(Direct3D* renderer, const char* filename)
{
	if (filename == NULL)
		return false;

	if (GetTexture(filename) != NULL)
		return true;

	Texture* tempTexture = new Texture();

	if (tempTexture->Load(renderer, filename))
	{
		string filenameStr = filename;
		m_textureMap[filenameStr] = tempTexture;
		return true;
	}
	else
	{
		delete tempTexture;
		return false;
	}
}

Texture* TextureManager::GetTexture(const char* filename)
{
	string filenameStr = filename;

	TextureMap::const_iterator searchResult = m_textureMap.find(filenameStr);
	if (searchResult != m_textureMap.end())
	{
		searchResult->second->AddRef();
		return searchResult->second;
	}

	return NULL;
}

void TextureManager::ReleaseTexture(Texture* texture)
{
	if (texture)
	{
		texture->RemoveRef();
		if (texture->GetRefCount() <= 0)
		{
			string filename = texture->GetFilename();
			TextureMap::const_iterator searchResult = m_textureMap.find(filename);
			if (searchResult != m_textureMap.end())
			{
				m_textureMap.erase(searchResult);
			}
			delete texture;
		}
	}
}

void TextureManager::Release()
{
	if (!m_textureMap.empty())
	{
		TextureMap::iterator mapIterator;
		for (mapIterator = m_textureMap.begin(); mapIterator != m_textureMap.end(); mapIterator++)
		{
			delete mapIterator->second;
			mapIterator->second = NULL;
		}
		m_textureMap.clear();
	}
}