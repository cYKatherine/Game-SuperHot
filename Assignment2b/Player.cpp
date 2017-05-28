#include "Player.h"
#include "Game.h"
#include "Enemy.h"
#include "Ammunition.h"


Player::Player(Camera* cam, InputController* input, AudioSystem* audio) {
	m_camera = cam;
	m_input = input;
	m_audio = audio;

	m_moveSpeed = 8.0f;
	m_rotationSpeed = 1.0f;
	m_cameraHeight = 1.8f;
	m_lookAtXRotation = 0;

	m_minBounds = Vector3(-0.2f, 0.0f, -0.2f);
	m_maxBounds = Vector3(0.2f, m_cameraHeight, 0.2f);
	m_boundingBox = CBoundingBox(m_position + m_minBounds, m_position + m_maxBounds);

	m_fireCoolDown = 50;
	m_lastTimeShoot = 0;

	m_scaleX = m_scaleY = m_scaleZ = 1;

	// Try lowering the friction - it's good fun :)
	m_frictionAmount = 0.08f;

	m_health = 500;
}

void Player::Update(float timestep)
{
	// Accumulate change in mouse position 
	m_rotY += m_input->GetMouseDeltaX() * m_rotationSpeed * timestep;
	m_lookAtXRotation += m_input->GetMouseDeltaY() * m_rotationSpeed * timestep;
	m_lookAtXRotation = MathsHelper::Clamp(m_lookAtXRotation, ToRadians(-80.0f), ToRadians(80.0f));

	// Next we'll wrap up our Y rotation in a matrix (remember matrices transform vectors)
	m_heading = Matrix::CreateRotationY(m_rotY);
	m_pitch = Matrix::CreateRotationX(m_lookAtXRotation);

	// Finally, we'll transform our world forward vector by the heading matrix which 
	// will essentially spin it from a world forward into a local forward which takes
	// the object's rotation into account.
	Vector3 localForward = Vector3::TransformNormal(Vector3(0, 0, 1), m_heading);

	// Transform a world right vector from world space into local space
	Vector3 localRight = Vector3::TransformNormal(Vector3(1, 0, 0), m_heading);

	if (m_input->GetKeyHold('W'))
	{
		m_position += localForward * m_moveSpeed * timestep;
		//ApplyForce(localForward * m_moveSpeed);
		Game::GetInstance()->setMove(true);
	}
	else if (m_input->GetKeyHold('S'))
	{
		m_position -= localForward * m_moveSpeed * timestep;
		//ApplyForce(-localForward * m_moveSpeed);
		Game::GetInstance()->setMove(true);
	}
	else if (m_input->GetKeyHold('A'))
	{
		m_position -= localRight * m_moveSpeed * timestep;
		//ApplyForce(-localRight * m_moveSpeed);
		Game::GetInstance()->setMove(true);
	}
	else if (m_input->GetKeyHold('D'))
	{
		m_position += localRight * m_moveSpeed * timestep;
		//ApplyForce(localRight * m_moveSpeed);
		Game::GetInstance()->setMove(true);
	}
	else {
		Game::GetInstance()->setMove(false);
	}

	// Add the camera offset to our position to determine final camera position
	Vector3 cameraPos = m_position + Vector3(0, m_cameraHeight, 0);

	// Combine pitch and heading into one matrix for convenience
	Matrix lookAtRotation = m_pitch * m_heading;

	// Transform a world forward vector into local space (take pitch and heading into account)
	Vector3 lookAt = Vector3::TransformNormal(Vector3(0, 0, 1), lookAtRotation);

	if (m_input->GetMouseDown(0)) {
		m_audio->Play("Assets/Sounds/Shoot.wav", false);
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
			Game::GetInstance()->RefreshAmmunicationUI();
		}
	}

	m_fireCoolDown += 1;

	// Our look-at vector is still a tiny vector
	// Add our position to it so it describes a point in front of the camera
	// Remember the look-at vector needs to describe a point in the world relative to the origin
	lookAt += cameraPos;

	m_camera->SetPosition(cameraPos);
	m_camera->SetLookAt(lookAt);

	// Keep collider up to date with our position
	m_boundingBox.SetMin(m_position + m_minBounds);
	m_boundingBox.SetMax(m_position + m_maxBounds);

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

void Player::OnItemBoxCollisionEnter(ItemBox* itemBox)
{
	m_audio->Play("Assets/Sounds/beep.mp3", false);
	// Bounce off box - how could you take our velocity into account?
	//ApplyForce((m_position - other->GetPosition()) * 0.5f);
	m_health += 100;
	Game::GetInstance()->RefreshHealthUI();

}

void Player::OnItemBoxCollisionStay()
{
}

void Player::OnItemBoxCollisionExit()
{
}

void Player::OnAmmunitionCollisionEnter(Ammunition* ammunition)
{
	m_audio->Play("Assets/Sounds/beep.mp3", false);
	// Bounce off box - how could you take our velocity into account?
	//ApplyForce((m_position - other->GetPosition()) * 0.5f);
	if (m_ammunitionNo < 2) {
		m_ammunitionNo += 1;
		Game::GetInstance()->RefreshAmmunicationUI();
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
	m_audio->Play("Assets/Sounds/beep.mp3", false);
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
		m_audio->Play("Assets/Sounds/Torture.wav", false);
		Vector3 updatePosition = Vector3(bullet->GetPosition().x, 0, bullet->GetPosition().z);
		ApplyForce((m_position - updatePosition) * 0.5f);
		m_health -= 10;
		Game::GetInstance()->m_hurtOverlayColor->A(1);
		Game::GetInstance()->RefreshHealthUI();
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
