/*	FIT2096 - Example Code
*	Shader.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This class represents a single Shader effect.
*	It is made up of a Vertex shader and Pixel Shader
*	It compiles shader code and sets up the Direct3D device for rendering the shader
*	This includes passing constant information like Matrices into the shaders
*/

#ifndef SHADER_H
#define SHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include "DirectXTK/SimpleMath.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX::SimpleMath;

class Shader
{
protected:
	struct MatrixBuffer		//Most vertex shaders need to know the matrices to use to render the scene
	{						//This struct is used to pass them simply into the shader
		Matrix world;		//The world matrix is the first transformation (local space to world space)
		Matrix view;		//The next transformation is the view matrix (world space to camera/view space)
		Matrix projection;	//The final transformation is the projection matrix (camera/view space to screen space (i.e 2D space))
	};

	ID3D11VertexShader* m_vertexShader;		//This is a pointer to the compiled and initialised Vertex Shader
	ID3D11PixelShader* m_pixelShader;		//This is a pointer to the compiled and initialised Pixel Shader
	ID3D11InputLayout* m_layout;			//This is the Vertex layout, it defines the mapping between the vertex data and the input variables in the shader code
	ID3D11Buffer* m_matrixBuffer;			//This buffer stores that matrix data so it can be easly passed into the Vertex shader

public:
	Shader();			//Constructor
	virtual ~Shader();	//Destructor

	virtual bool Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename);	//Initialises the shader, here we specify the file name of the 
																									//vertex and pixel shader source code files
	virtual void Release();		//Cleanup
	
	virtual void Begin(ID3D11DeviceContext* context);	//The begin method tells the device context to use the Shaders as the current rendering shaders

	virtual bool SetMatrices(ID3D11DeviceContext* context, Matrix world, Matrix view, Matrix projection);	//This method copies the world, view, proj matrices 
																											//over to the shader
	virtual bool SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView);	//This shader doesn't use a texture, but most do. We declare the method
																									//here in the base shader class so that all shaders will have it.

};

#endif