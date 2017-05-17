/*	FIT2096 - Example Code
*	TexturedShader.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This subclass of Shader extends the existing functionality so that we can texture our meshes
*	When we create a mesh we can now pass either a basic Shader or a TexturedShader to it
*	The methods here extend the Shader methods by calling them and then doing extra tasks afterwards.
*/

#ifndef TEXTUREDSHADER_H
#define TEXTUREDSHADER_H

#include "Shader.h"

class TexturedShader : public Shader
{
private:
	ID3D11SamplerState* m_textureSampler;	//A sampler is an object that can be "sampled" across a range of values, we use them to represent textures in our shaders
											//A Texture is usually a 2D sampler where we can plug values in for the horizontal (u) position and the vertical (v) position
											//and it will give us a colour back.
public:
	TexturedShader();	//Constructor
	~TexturedShader();	//Destructor

	void Begin(ID3D11DeviceContext* context);
	bool Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename);
	bool SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView);
	void Release();
};

#endif