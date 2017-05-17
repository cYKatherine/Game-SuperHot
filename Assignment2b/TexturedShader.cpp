/*	FIT2096 - Example Code
*	TexturedShader.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Implementation of TexturedShader.h
*/

#include "TexturedShader.h"

TexturedShader::TexturedShader() : Shader()
{
	m_textureSampler = NULL;
}

TexturedShader::~TexturedShader()
{

}

bool TexturedShader::Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename)
{
	D3D11_SAMPLER_DESC textureSamplerDescription;	//When we create a sampler we need a Description struct to describe how we want to create the sampler

	if (!Shader::Initialise(device, vertexFilename, pixelFilename))		//We'll use the parent method to create most of the shader
	{
		return false;
	}

	//Now all we need to do is fill out our sampler description
	textureSamplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;			//This is the Filtering method used for the texture... 
																			//...different values will give you different quality texture output
	textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//This defines what happens when we sample outside of the range [0...1]
	textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//In our case we just want it to wrap around so that we always are sampling something
	textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.MipLODBias = 0.0f;							//The rest of these values are really just "Defaults"
	textureSamplerDescription.MaxAnisotropy = 8;							//If you want more info look up D3D11_SAMPLER_DESC on MSDN
	textureSamplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDescription.BorderColor[0] = 1;
	textureSamplerDescription.BorderColor[1] = 1;
	textureSamplerDescription.BorderColor[2] = 1;
	textureSamplerDescription.BorderColor[3] = 1;
	textureSamplerDescription.MinLOD = 0;
	textureSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	//and create the sampler!
	if (FAILED(device->CreateSamplerState(&textureSamplerDescription, &m_textureSampler)))
	{
		return false;
	}

	return true;
}

void TexturedShader::Begin(ID3D11DeviceContext* context)
{
	Shader::Begin(context);		//Let the parent class do most of the work!
	context->PSSetSamplers(0, 1, &m_textureSampler);
}

bool TexturedShader::SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView)
{
	//This shader does have a texture so it does something in this method!
	context->PSSetShaderResources(0, 1, &textureView);
	return true;
}

void TexturedShader::Release()
{
	Shader::Release();
	if (m_textureSampler)
	{
		m_textureSampler->Release();
		m_textureSampler = NULL;
	}
}