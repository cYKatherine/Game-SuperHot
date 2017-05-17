/*	FIT2096 - Example Code
*	Shader.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Implementation of Shader.h
*/

#include "Shader.h"

Shader::Shader()
{
	m_vertexShader = NULL;
	m_pixelShader = NULL;
	m_layout = NULL;
	m_matrixBuffer = NULL;
}

Shader::~Shader()
{

}

bool Shader::Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename)
{
	//This method loads and compiles the Vertex and Pixel shader code, sets up the vertex layout and creates a CPU accessable buffer to hold the matrices
	
	ID3DBlob* vertexShaderBlob = NULL;		//Compiled shader code is stored as a binary blob of data, this is for the vertex shader
	ID3DBlob* pixelShaderBlob = NULL;		//and this one is for the pixel shader
	ID3DBlob* errorBlob = NULL;				//Any compiler errors are stored in this blob, they will be a string which we can output if needed

	const unsigned int numberOfVertexElements = 4;					//The input layout needs to know how many elements make up our vertices
	D3D11_INPUT_ELEMENT_DESC vertexLayout[numberOfVertexElements];	//Each element will have a Description struct which tells us how they should be layed out
	D3D11_BUFFER_DESC matrixBufferDescription;						//We will also need to create a Description struct for the buffer we are creating for the matrices

	//We use D3DCompileFromFile to compile the HLSL code for our shaders
	if (FAILED(D3DCompileFromFile(vertexFilename,	//The filename of the source code file
		NULL,										//Any marco defines we want to include
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			//Here we can specify include files
		"main",										//This is the name of the entry point method
		"vs_4_0",									//What Shader Model (version) do we want to target, shader model 4 is old but works almost everywhere
		D3DCOMPILE_ENABLE_STRICTNESS, 0,			//Misc flags, more info can be found at MSDN
		&vertexShaderBlob, &errorBlob)))			//Pointers to the shader blob and error blob, these are output parameters, if everything went well then
													//vertexSahderBlob should be something and errorBlob should be NULL
	{
		//If the compilation failed...
		if (errorBlob)	//If there is an error blob then it was most likely a syntax error in the shader
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());	//We'll output it to the Output window in Visual Studio
			errorBlob->Release();										//And release everything and return false
		}
		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		return false;
	}

	//Compiling the pixel shader is the same process but we use "ps_4_0" as the shader model
	if (FAILED(D3DCompileFromFile(pixelFilename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&pixelShaderBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		return false;
	}

	//If we have compiled everything correctly then we can create the actual shader objects, if they fail we release everything and return false
	if (FAILED(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &m_vertexShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		return false;
	}

	if (FAILED(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &m_pixelShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		return false;
	}

	//Here we start filling out the Descriptions of each Vertex Element
	//The Input Layout uses a concept known as "Semantics". Each element has a semantic name, these names match semantics that are defined in the shader code
	//The order of these descriptions must match the order of the elements on our Vertex struct, see Mesh.h

	vertexLayout[0].SemanticName = "POSITION";				//The first element is the position vector so we use the "POSITION" semantic
	vertexLayout[0].SemanticIndex = 0;						//Semantics can be indexed (like an array) so that you can pass more then one in. We only have one position though
	vertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	//This specifies how the element is interpreted, our vector is made up of 3 32-bit floats so the format needs to 
															//support that much data
	vertexLayout[0].InputSlot = 0;							//Shader input can come from lots of different places, we only use one input slot so 0 is fine for us			
	vertexLayout[0].AlignedByteOffset = 0;					//This specifies the offset from the start of the Vertex where this element begins, 
															//this is the first element so there is no offset. 
	vertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	//This specifies that this element is present for each vertex
	vertexLayout[0].InstanceDataStepRate = 0;				//This is for a technique called instancing, we won't be covering it this semester

	vertexLayout[1].SemanticName = "COLOR";					//The next input element is our colour
	vertexLayout[1].SemanticIndex = 0;
	vertexLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//It is 4 32-bit floats so our format matches that
	vertexLayout[1].InputSlot = 0;
	vertexLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	//Here we would calculate the byte offset by factoring in the size of the last element
																		//however this value here tells Direct3D to calculate it for us, which is nice!
	vertexLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[1].InstanceDataStepRate = 0;

	vertexLayout[2].SemanticName = "NORMAL";				//The next element is the normal
	vertexLayout[2].SemanticIndex = 0;
	vertexLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;	//A normal is 3 32-bit floats
	vertexLayout[2].InputSlot = 0;
	vertexLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[2].InstanceDataStepRate = 0;

	vertexLayout[3].SemanticName = "TEXCOORD";				//The final element is the texture coordinate
	vertexLayout[3].SemanticIndex = 0;
	vertexLayout[3].Format = DXGI_FORMAT_R32G32_FLOAT;		//A texture coord is 2 32-bit floats
	vertexLayout[3].InputSlot = 0;
	vertexLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[3].InstanceDataStepRate = 0;

	//After we have described our input elements we can create our input layout, this method needs the descriptions we created 
	//and the vertex shader with the semantics that match the ones in the descriptions
	if (FAILED(device->CreateInputLayout(vertexLayout, numberOfVertexElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_layout)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		return false;
	}

	//After the shaders and the input layout are created then we can release the shader blobs
	vertexShaderBlob->Release();
	vertexShaderBlob = NULL;

	pixelShaderBlob->Release();
	pixelShaderBlob = NULL;

	//The final step is to configure the buffer that will hold the matrices for the vertex shader
	matrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;				//The buffer is used for dynamic data that changes often
	matrixBufferDescription.ByteWidth = sizeof(MatrixBuffer);			//It's big enough to hold one "MatrixBuffer" sized object
	matrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		//It's a buffer containing shader constants
	matrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//It is directly writable to by the CPU
	matrixBufferDescription.MiscFlags = 0;								//Misc flags
	matrixBufferDescription.StructureByteStride = 0;					//Data offsets

	//Create the buffer based on that description!
	if (FAILED(device->CreateBuffer(&matrixBufferDescription, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	return true;
}

void Shader::Release()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = NULL;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = NULL;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = NULL;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = NULL;
	}
}

void Shader::Begin(ID3D11DeviceContext* context)
{
	//To be begin a shader we set the context up to use the objects we have created

	context->IASetInputLayout(m_layout);					//We set the input layout to tell the shader how to understand the vertices

	context->VSSetShader(m_vertexShader, NULL, 0);			//We set the vertex shader to tell direct3D how to transform the vertex data
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);	//We set the matrix buffer as a constant that goes into the vertex shader.

	context->PSSetShader(m_pixelShader, NULL, 0);			//We set the pixel shader so direct3D knows how to determine the final colour of a transformed vertex
}

bool Shader::SetMatrices(ID3D11DeviceContext* context, Matrix world, Matrix view, Matrix projection)
{
	//This method copies the provided matrices over to the constant buffer

	D3D11_MAPPED_SUBRESOURCE mappedResource;	//To do this we us the Map method, this method gives us a "Mapped Subresource", that will end up here
	MatrixBuffer* inputData;

	//We call the Map method and pass in our Mapped Subresource struct, the method will fill it out for us
	if (FAILED(context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	
	inputData = (MatrixBuffer*)mappedResource.pData;	//The pData pointer in the Mapped Subresource points to the memory within the buffer, so we cast it to a MatrixBuffer

	inputData->world = world.Transpose();				//and fill it out!
	inputData->view = view.Transpose();					//The matrices we use a in row-major format, HLSL assumes column-major format so we transpose them
	inputData->projection = projection.Transpose();		//as we pass them into the matrix buffer

	context->Unmap(m_matrixBuffer, 0);					//Unmapping the subresource frees it and finishes the process

	return true;
}

bool Shader::SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureResource)
{
	//There is no texture for this shader, don't set one up!
	return false;
}