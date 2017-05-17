#include "Ruby.h"
#include "MathsHelper.h"

Ruby::Ruby(Mesh* mesh, Shader* shader, Vector3 position) :
	GameObject(mesh, shader, position)
{
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void Ruby::Update(float timestep)
{
	// If this were a real game, our duties in here might include:
	//	- increase Y rotation so we spin on the spot
	//	- animate our scale so we shrink when dissapearing and grow when respawning
	//	- manage a respawn timer to delay our respawn

}

bool Ruby::getPicked() {
	return m_picked;
}

void Ruby::OnPlayerCollisionEnter()
{
	m_picked = true;
}

void Ruby::OnPlayerCollisionStay()
{
	m_picked = true;
}

void Ruby::OnPlayerCollisionExit()
{
	m_picked = true;
}