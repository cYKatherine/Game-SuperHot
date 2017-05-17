#include "Enemy.h"
#include "Bullet.h"
#include "Game.h"
#include "Ammunition.h"


Enemy::Enemy(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, int enemyNo, Player* playerToFollow, std::vector<Ruby*> rubies) :
	PhysicsObject(mesh, shader, texture, position)
{
	m_playerToFollow = playerToFollow;
	m_moveSpeed = MathsHelper::RandomRange(0.05f, 0.1f);
	m_enemyNo = enemyNo;
	m_rubies = rubies;
	m_health = 100;

	m_scaleX = m_scaleY = m_scaleZ = -3;

	switch (m_enemyNo) {
	case 1:
		targetPosition = m_playerToFollow->GetPosition();
		break;
	case 2:
		targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
		break;
	case 3:
		targetPosition = m_rubies[MathsHelper::RandomRange(0, m_rubies.size() - 1)]->GetPosition();
		break;
	case 4:
		targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
		m_moveSpeed = 10.0f;
		break;
	case 5:
		targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
		m_moveSpeed = 10.0f;
		break;
	}

	// Try lowering the friction - it's good fun :)
	m_frictionAmount = 0.08f;

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void Enemy::Update(float timestep)
{
	if (m_scaleX < 1 && m_scaleY < 1 && m_scaleZ < 1) {
		m_scaleX += 0.05;
		m_scaleY += 0.05;
		m_scaleZ += 0.05;
	}

	// Find the heading for the enemies
	Vector3 heading = m_playerToFollow->GetPosition() - m_position;
	heading.Normalize();
	m_rotY = acosf(heading.Dot(worldForward));
	m_rotY *= heading.x / abs(heading.x);

	// Calculate the distance to target and player
	float distanceToTarget = Vector3::Distance(m_position, targetPosition);
	float distanceToPlayer = Vector3::Distance(m_position, m_playerToFollow->GetPosition());

	switch (m_enemyNo) {
	case 1:
		if (distanceToTarget <= 1.0f) {
			m_position = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
		}
		targetPosition = m_playerToFollow->GetPosition();
		break;
	case 2:
		if (distanceToTarget <= 1.0f) {
			targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
		}
		break;
	case 3:
		if (distanceToTarget <= 1.0f) {
			targetPosition = m_rubies[MathsHelper::RandomRange(0, m_rubies.size() - 1)]->GetPosition();
		}
		break;
	case 4:
		if (distanceToPlayer <= 5.0f) {
			targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
			Vector3 usToTarget = targetPosition - m_position;
			usToTarget.Normalize();

			m_position += usToTarget * (m_moveSpeed);
		}
		break;
	case 5:
		if (distanceToPlayer <= 10.0f) {
			targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
			Vector3 usToTarget = targetPosition - m_position;
			usToTarget.Normalize();

			m_position += usToTarget * (m_moveSpeed);
		}
		break;
	}

	if (m_health <= 0) {
		switch (m_enemyNo) {
		case 1:
			targetPosition = m_playerToFollow->GetPosition();
			break;
		case 2:
			targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
			break;
		case 3:
			targetPosition = m_rubies[MathsHelper::RandomRange(0, m_rubies.size() - 1)]->GetPosition();
			break;
		case 4:
			targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
			m_moveSpeed = 10.0f;
			break;
		case 5:
			targetPosition = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
			m_moveSpeed = 10.0f;
			break;
		}
		m_position = Vector3(MathsHelper::RandomRange(-100.0f, 100.0f), 0.0f, MathsHelper::RandomRange(-100.0f, 100.0f));
		m_health = 100;
		m_scaleX = m_scaleY = m_scaleZ = -3;
	}

	
	if (m_enemyNo == 1 || m_enemyNo == 2 || m_enemyNo == 3) {
		// Slide towards point
		Vector3 usToTarget = targetPosition - m_position;
		usToTarget.Normalize();

		if (Game::GetInstance()->getMove()) {
			m_position += usToTarget * m_moveSpeed;
		}
		else {
			m_position += usToTarget * m_moveSpeed * 0.1;
		}
	}

	switch (m_enemyNo) {
	case 1:
		if (m_shootTime % 1000 == 0) {
			Vector3 bulletPosition = Vector3(m_position.x, 1.2, m_position.z);
			Game::GetInstance()->createBullet(bulletPosition, m_rotY, false);
		}
	case 2:
		if (m_shootTime % 500 == 0) {
			Vector3 bulletPosition = Vector3(m_position.x, 1.2, m_position.z);
			Game::GetInstance()->createBullet(bulletPosition, m_rotY, false);
		}
	case 3:
		if (m_shootTime % 1000 == 0) {
			Vector3 bulletPosition = Vector3(m_position.x, 1.2, m_position.z);
			Game::GetInstance()->createBullet(bulletPosition, m_rotY, false);
		}
	case 4:
		if (m_shootTime % 100 == 0) {
			Vector3 bulletPosition = Vector3(m_position.x, 1.2, m_position.z);
			Game::GetInstance()->createBullet(bulletPosition, m_rotY, false);
		}
	case 5:
		if (m_shootTime % 100 == 0) {
			Vector3 bulletPosition = Vector3(m_position.x, 1.2, m_position.z);
			Game::GetInstance()->createBullet(bulletPosition, m_rotY, false);
		}
	}

	m_shootTime += 1;


	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());

	PhysicsObject::Update(timestep);
}

int Enemy::getHealth() {
	return m_health;
}

void Enemy::updateRubies(std::vector<Ruby*> rubies) {
	m_rubies = rubies;
}

void Enemy::OnEnemyCollisionEnter(Enemy* other)
{
	ApplyForce((m_position - other->GetPosition()) * 0.5f);
}

void Enemy::OnEnemyCollisionStay(Enemy* other)
{
}

void Enemy::OnEnemyCollisionExit(Enemy* other)
{
}

void Enemy::OnAmmunitionCollisionEnter(Ammunition* ammunition)
{
	ApplyForce((m_position - ammunition->GetPosition()) * 0.5f);

}

void Enemy::OnAmmunitionCollisionStay(Ammunition* ammunition)
{
}

void Enemy::OnAmmunitionCollisionExit(Ammunition* ammunition)
{
}

void Enemy::OnPlayerCollisionEnter(Player* player) {
	ApplyForce((m_position - player->GetPosition()) * 0.5f);
}

void Enemy::OnPlayerCollisionStay(Player* player) {

}

void Enemy::OnPlayerCollisionExit(Player* player) {

}

void Enemy::OnBulletCollisionEnter(bool fromPlayer, Bullet* bullet) {
	if (fromPlayer) {
		Vector3 updatePosition = Vector3(bullet->GetPosition().x, 0, bullet->GetPosition().z);
		ApplyForce((m_position - updatePosition) * 0.5f);
		m_health -= 10;
	}

}

void Enemy::OnBulletCollisionStay(bool fromPlayer, Bullet* bullet) {
	if (fromPlayer) {
	}
}

void Enemy::OnBulletCollisionExit(bool fromPlayer, Bullet* bullet) {
	if (fromPlayer) {
	}
}

Matrix Enemy::getHeading() {
	return m_heading;
}
