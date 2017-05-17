#include "MeshManager.h"

using namespace std;

MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{
	Release();
}

bool MeshManager::Load(Direct3D* renderer, const char* filename)
{
	if (filename == NULL)
		return false;

	if (GetMesh(filename) != NULL)
		return true;

	Mesh* tempMesh = new Mesh();

	if (tempMesh->Load(renderer, filename))
	{
		string filenameStr = filename;
		m_meshMap[filenameStr] = tempMesh;
		return true;
	}
	else
	{
		delete tempMesh;
		return false;
	}
}

bool MeshManager::LoadTriangle(Direct3D* renderer, const char* identifier)
{
	if (identifier == NULL)
		return false;

	if (GetMesh(identifier) != NULL)
		return true;

	Mesh* tempMesh = new Mesh();

	if (tempMesh->CreateTriangle(renderer, identifier))
	{
		string identifierStr = identifier;
		m_meshMap[identifierStr] = tempMesh;
		return true;
	}
	else
	{
		delete tempMesh;
		return false;
	}
}

bool MeshManager::LoadSquare(Direct3D* renderer, const char* identifier)
{
	if (identifier == NULL)
		return false;

	if (GetMesh(identifier) != NULL)
		return true;

	Mesh* tempMesh = new Mesh();

	if (tempMesh->CreateSquare(renderer, identifier))
	{
		string identifierStr = identifier;
		m_meshMap[identifierStr] = tempMesh;
		return true;
	}
	else
	{
		delete tempMesh;
		return false;
	}
}

Mesh* MeshManager::GetMesh(const char* filename)
{
	string filenameStr = filename;

	MeshMap::const_iterator searchResult = m_meshMap.find(filenameStr);
	if (searchResult != m_meshMap.end())
	{
		searchResult->second->AddRef();
		return searchResult->second;
	}

	return NULL;
}

void MeshManager::ReleaseMesh(Mesh* mesh)
{
	if (mesh)
	{
		mesh->RemoveRef();
		if (mesh->GetRefCount() <= 0)
		{
			string filename = mesh->GetFilename();
			MeshMap::const_iterator searchResult = m_meshMap.find(filename);
			if (searchResult != m_meshMap.end())
			{
				m_meshMap.erase(searchResult);
			}
			delete mesh;
		}
	}
}

void MeshManager::Release()
{
	if (!m_meshMap.empty())
	{
		MeshMap::iterator mapIterator;
		for (mapIterator = m_meshMap.begin(); mapIterator != m_meshMap.end(); mapIterator++)
		{
			delete mapIterator->second;
			mapIterator->second = NULL;
		}
		m_meshMap.clear();
	}
}