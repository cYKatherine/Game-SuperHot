#include "ItemBox.h"
#include "MathsHelper.h"

ItemBox::ItemBox(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position) :
	GameObject(mesh, shader, texture, position)
{
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void ItemBox::Update(float timestep)
{
	// At the moment we don't need an item box to do anything
	
	// If this were a real game, our duties in here might include:
	//	- increase Y rotation so we spin on the spot
	//	- animate our scale so we shrink when dissapearing and grow when respawning
	//	- manage a respawn timer to delay our respawn
	if (respawnCount <= 50) {
		m_scaleX += 0.1;
		m_scaleY += 0.1;
		m_scaleZ += 0.1;
	}
	respawnCount += 1;
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void ItemBox::OnPlayerCollisionEnter()
{

}

void ItemBox::OnPlayerCollisionStay()
{

}

void ItemBox::OnPlayerCollisionExit()
{

}