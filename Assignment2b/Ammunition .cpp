#include "Ammunition.h"
#include "MathsHelper.h"

Ammunition::Ammunition(Mesh* mesh, Shader* shader, Vector3 position, AudioSystem* audio) :
	GameObject(mesh, shader, position)
{
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
	m_audio = audio;
}

void Ammunition::Update(float timestep)
{
	// If this were a real game, our duties in here might include:
	//	- increase Y rotation so we spin on the spot
	//	- animate our scale so we shrink when dissapearing and grow when respawning
	//	- manage a respawn timer to delay our respawn
	if (respawnCount <= 50) {
		m_scaleX += 0.1;
		m_scaleY += 0.1;
		m_scaleZ += 0.1;
	}
	if (respawnCount >= 1000) {
		if(!m_played)
		{
			m_audio->Play("Assets/Sounds/beep.mp3", false);
			m_audio->Play("Assets/Sounds/beep.mp3", false);
			m_played = true;
		}
		respawn();
	}
	respawnCount += 1;

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void Ammunition::respawn() {
	if (m_scaleX != 0 && m_scaleY != 0 && m_scaleZ != 0) {
		m_scaleX -= 0.1;
		m_scaleY -= 0.1;
		m_scaleZ -= 0.1;
	}
	else {
		m_position = Vector3(MathsHelper::RandomRange(-50.0f, 50.0f), 0.0f, MathsHelper::RandomRange(-50.0f, 50.0f));
		respawnCount = 0;
		m_played = false;
	}
}

void Ammunition::OnPlayerCollisionEnter()
{
	m_position = Vector3(MathsHelper::RandomRange(-50.0f, 50.0f), 0.0f, MathsHelper::RandomRange(-50.0f, 50.0f));
	m_scaleX = 0;
	m_scaleY = 0;
	m_scaleZ = 0;
	respawnCount = 0;
}

void Ammunition::OnPlayerCollisionStay()
{

}

void Ammunition::OnPlayerCollisionExit()
{

}

void Ammunition::OnEnemyCollisionEnter() {

}

void Ammunition::OnEnemyCollisionStay() {

}

void Ammunition::OnEnemyCollisionExit() {

}