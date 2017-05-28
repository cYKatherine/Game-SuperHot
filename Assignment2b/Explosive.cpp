#include "Explosive.h"
#include "Game.h"

Explosive::Explosive(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, AudioSystem* audio) : PhysicsObject(mesh, shader, texture, position) {
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
	m_audio = audio;
}

void Explosive::Update(float timestep)
{
	Matrix m_heading = Matrix::CreateRotationY(m_rotY);
	Vector3 worldForward = Vector3(1, 0, 0);

	Vector3 localForward = Vector3::TransformNormal(worldForward, m_heading);
	Vector3 localRight = Vector3::TransformNormal(Vector3(1, 0, 0), m_heading);
	Vector3 localForwardXZ = localRight.Cross(Vector3(0, 5, 0));

	if (m_readyToExplode)
	{
		explodeCount += 1;
		if (explodeCount >= 1000)
		{
			m_audio->Play("Assets/Sounds/Explosion.wav", false);
			Game::GetInstance()->explode(m_position);
		}
	}

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());

	PhysicsObject::Update(timestep);
}

void Explosive::setReadyToExplode(bool ready)
{
	m_readyToExplode = ready;
}

void Explosive::setPicked(bool picked)
{
	m_picked = picked;
}

void Explosive::setExploded(bool exploded)
{
	m_exploded = exploded;
}

bool Explosive::getReadyToExplode()
{
	return m_readyToExplode;
}

bool Explosive::getPicked()
{
	return m_picked;
}

bool Explosive::getExploded()
{
	return m_exploded;
}