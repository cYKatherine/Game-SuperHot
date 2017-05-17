#include "Bullet.h"
#include "Game.h"

Bullet::Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float rotationY) : PhysicsObject(mesh, shader, texture, position) {
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
	m_moveSpeed = 5.0f;
	m_rotY = rotationY;
}

void Bullet::Update(float timestep)
{
	Matrix m_heading = Matrix::CreateRotationY(m_rotY);
	Vector3 worldForward = Vector3(1, 0, 0);

	Vector3 localForward = Vector3::TransformNormal(worldForward, m_heading);
	Vector3 localRight = Vector3::TransformNormal(Vector3(1, 0, 0), m_heading);
	Vector3 localForwardXZ = localRight.Cross(Vector3(0, 5, 0));

	if (Game::GetInstance()->getMove()) {
		m_position += localForwardXZ * m_moveSpeed * timestep;
	}
	else {
		m_position += localForwardXZ * m_moveSpeed * timestep * 0.05;
	}

	if (m_flyingTime > 1000) {
		m_disappear = true;
	}

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());

	m_flyingTime += 1;

	PhysicsObject::Update(timestep);
}

bool Bullet::getFromPlyer() {
	return m_fromPlayer;
}

bool Bullet::getDisappear() {
	return m_disappear;
}

void Bullet::setFromPlyer(bool from_player) {
	m_fromPlayer = from_player;
}

void Bullet::setHeading(float heading){}

void Bullet::OnPlayerCollisionEnter(){
	if (!m_fromPlayer) m_disappear = true;
}
void Bullet::OnPlayerCollisionStay(){}
void Bullet::OnPlayerCollisionExit(){}

void Bullet::OnEnemyCollisionEnter(){
	if (m_fromPlayer) m_disappear = true;
}
void Bullet::OnEnemyCollisionStay(){}
void Bullet::OnEnemyCollisionExit(){}