#include "Player.h"
#include "Game.h"
#include "Enemy.h"
#include "Ammunition.h"


Player::Player(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, InputController* input) :
	PhysicsObject(mesh, shader, texture, position)
{
	m_input = input;

	m_moveSpeed = 0.03f;
	m_rotationSpeed = 2.0f;

	m_fireCoolDown = 50;
	m_lastTimeShoot = 0;

	m_scaleX = m_scaleY = m_scaleZ = 1;

	// Try lowering the friction - it's good fun :)
	m_frictionAmount = 0.08f;

	m_health = 500;

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void Player::Update(float timestep)
{
	// This is the first time a moving object in our game can rotate and change where
	// it's looking. Suddenly moving along the world axes is not useful to us anymore.
	// We need to work out what direction is forward for this particular object. 

	// We'll start by declaring a forward vector in world space
	Vector3 worldForward = Vector3(0, 0, 1);

	// Accumulate change in mouse position 
	m_rotY += m_input->GetMouseDeltaX() * m_rotationSpeed * timestep;

	// Next we'll wrap up our Y rotation in a matrix (remember matrices transform vectors)
	m_heading = Matrix::CreateRotationY(m_rotY);

	// Finally, we'll transform our world forward vector by the heading matrix which 
	// will essentially spin it from a world forward into a local forward which takes
	// the object's rotation into account.
	Vector3 localForward = Vector3::TransformNormal(worldForward, m_heading);

	// Transform a world right vector from world space into local space
	Vector3 localRight = Vector3::TransformNormal(Vector3(1, 0, 0), m_heading);

	// Essentially our local forward vector but always parallel with the ground
	// Remember a cross product gives us a vector perpendicular to the two input vectors
	Vector3 localForwardXZ = localRight.Cross(Vector3(0, 1, 0));

	// We're going to need this a lot. Store it locally here to save on our function calls 
	Vector3 currentPos = GetPosition();

	if (m_input->GetKeyHold('W'))
	{
		ApplyForce(localForwardXZ * m_moveSpeed);
		Game::GetInstance()->setMove(true);
		//currentPos += localForwardXZ * m_moveSpeed * timestep;
	}
	else if (m_input->GetKeyHold('S'))
	{
		ApplyForce(-localForwardXZ * m_moveSpeed);
		Game::GetInstance()->setMove(true);
		//currentPos -= localForwardXZ * m_moveSpeed * timestep;
	}
	else if (m_input->GetKeyHold('A'))
	{
		ApplyForce(-localRight * m_moveSpeed);
		Game::GetInstance()->setMove(true);
		//currentPos -= localRight * m_moveSpeed * timestep;
	}
	else if (m_input->GetKeyHold('D'))
	{
		ApplyForce(localRight * m_moveSpeed);
		Game::GetInstance()->setMove(true);
		//currentPos += localRight * m_moveSpeed * timestep;
	}
	else {
		Game::GetInstance()->setMove(false);
	}
	if (m_input->GetMouseDown(0)) {
		if (m_bulletNo <= 0) {
			if (m_ammunitionNo > 0) {
				m_bulletNo += 10;
				m_ammunitionNo -= 1;
			}
		}
		if (m_fireCoolDown - m_lastTimeShoot > 50 && m_bulletNo > 0) {
			Vector3 bulletPosition = Vector3(m_position.x, 1, m_position.z);
			Game::GetInstance()->createBullet(bulletPosition, m_rotY, true);
			m_fireCoolDown = 0;
			m_lastTimeShoot = 0;
			m_bulletNo -= 1;
			Game::GetInstance()->RefreshUI();
		}
	}

	m_position = currentPos;
	m_fireCoolDown += 1;

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());

	//std::cout << currentPos.x << currentPos.y << currentPos.z << "\n";

	PhysicsObject::Update(timestep);
}

int Player::getHealth() {
	return m_health;
}

int Player::getAmmunitionNo() {
	return m_ammunitionNo;
}

int Player::getBulletNo() {
	return m_bulletNo;
}

void Player::OnPlayerCollisionEnter(Player* other)
{
}

void Player::OnPlayerCollisionStay(Player* other)
{
}

void Player::OnPlayerCollisionExit(Player* other)
{
}

void Player::OnAmmunitionCollisionEnter(Ammunition* ammunition)
{
	// Bounce off box - how could you take our velocity into account?
	//ApplyForce((m_position - other->GetPosition()) * 0.5f);
	if (m_ammunitionNo < 2) {
		m_ammunitionNo += 1;
		Game::GetInstance()->RefreshUI();
	}
}

void Player::OnAmmunitionCollisionStay()
{
}

void Player::OnAmmunitionCollisionExit()
{
}

void Player::OnRubyCollisionEnter(Ruby* other)
{
}

void Player::OnRubyCollisionStay(Ruby* other)
{
}

void Player::OnRubyCollisionExit(Ruby* other)
{
}

void Player::OnEnemyCollisionEnter(Enemy* enemy) {
}

void Player::OnEnemyCollisionStay(Enemy* enemy) {
}

void Player::OnEnemyCollisionExit(Enemy* enemy) {

}

void Player::OnBulletCollisionEnter(bool fromPlayer, Bullet* bullet) {
	if (!fromPlayer) {
		Vector3 updatePosition = Vector3(bullet->GetPosition().x, 0, bullet->GetPosition().z);
		ApplyForce((m_position - updatePosition) * 0.5f);
		m_health -= 10;
		Game::GetInstance()->m_hurtOverlayColor->A(1);
		Game::GetInstance()->RefreshUI();
	}
}

void Player::OnBulletCollisionStay(bool fromPlayer, Bullet* bullet) {
	if (fromPlayer) {

	}
}


void Player::OnBulletCollisionExit(bool fromPlayer, Bullet* bullet) {
	if (fromPlayer) {

	}
}

float Player::getMoveSpeed() {
	return m_moveSpeed;
}

float Player::getRotationSpeed() {
	return m_rotationSpeed;
}

Matrix Player::getHeading() {
	return m_heading;
}
