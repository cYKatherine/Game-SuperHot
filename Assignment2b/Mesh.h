#ifndef MESH_H
#define MESH_H

#include <d3d11.h>
#include "Direct3D.h"
#include "DirectXTK/SimpleMath.h"
#include "Camera.h"
#include "Texture.h"

class Mesh
{
private:
	struct Vertex
	{
		Vector3 position;
		Color colour;
		Vector3 normal;
		Vector2 texCoord;
	};

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	int m_referenceCount;
	const char* m_filename;

	int m_vertexCount;
	int m_indexCount;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	Vector3 m_minVector;	//For our bounding boxes we need to know the min position of the mesh...
	Vector3 m_maxVector;	//... and the max position
	Vector3 m_centre;		//For the bounding spheres we need to know the centre point...
	float m_radius;			//... and the overall radius.

	Mesh();
	~Mesh();
	bool Load(Direct3D* renderer, const char* filename);

	void AddRef() { m_referenceCount++; }
	void RemoveRef() { m_referenceCount--; }
	int GetRefCount() { return m_referenceCount; }

	//This method takes vertex and index data and creates the Direct3D buffers
	bool InitialiseBuffers(Direct3D* renderer, Vertex* vertexData, unsigned long* indexData);

	bool CreateTriangle(Direct3D* renderer, const char* identifier);
	bool CreateSquare(Direct3D* renderer, const char* identifier);


public:
	void Render(Direct3D* renderer, Shader* shader, Matrix world, Camera* cam, Texture* texture);

	int GetVertexCount() { return m_vertexCount; }
	int GetIndexCount() { return m_indexCount; }
	const char* GetFilename() { return m_filename; }	

	Vector3 GetMin() { return m_minVector; }
	Vector3 GetMax() { return m_maxVector; }
	Vector3 GetCentre() { return m_centre; }
	float GetRadius() { return m_radius; }

	// The MeshManager can access our private members and functions
	friend class MeshManager;
};

#endif