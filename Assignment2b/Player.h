#ifndef PLAYER_H
#define PLAYER_H

#include "AudioSystem.h"
#include "PhysicsObject.h"
#include "InputController.h"
#include "Ammunition.h"
#include "MathsHelper.h"
#include "Bullet.h"
#include "Enemy.h"
#include "ItemBox.h"

#include <iostream>

class Ruby;
class Enemy;
class Bullet;
class Ammunition;


class Player : public PhysicsObject
{
protected:
	Camera* m_camera;

	InputController* m_input;
	AudioSystem* m_audio;

	CBoundingBox m_boundingBox;
	Vector3 m_minBounds;
	Vector3 m_maxBounds;

	float m_moveSpeed;
	float m_rotationSpeed;
	float m_cameraHeight;
	float m_lookAtXRotation;

	float m_fireCoolDown;
	float m_lastTimeShoot;

	int m_health;
	int m_ammunitionNo = 2;
	int m_bulletNo = 0;

	Matrix m_heading;
	Matrix m_pitch;

public:
	Player(Camera* cam, InputController* input, AudioSystem* audio);

	virtual void Update(float timestep);

	void OnPlayerCollisionEnter(Player* other);
	void OnPlayerCollisionStay(Player* other);
	void OnPlayerCollisionExit(Player* other);

	void OnAmmunitionCollisionEnter(Ammunition* ammunition);
	void OnAmmunitionCollisionStay();
	void OnAmmunitionCollisionExit();

	void OnItemBoxCollisionEnter(ItemBox* itemBox);
	void OnItemBoxCollisionStay();
	void OnItemBoxCollisionExit();

	void OnRubyCollisionEnter(Ruby* other);
	void OnRubyCollisionStay(Ruby* other);
	void OnRubyCollisionExit(Ruby* other);

	void OnEnemyCollisionEnter(Enemy* enemy);
	void OnEnemyCollisionStay(Enemy* enemy);
	void OnEnemyCollisionExit(Enemy* enemy);

	void OnBulletCollisionEnter(bool fromPlayer, Bullet* bullet);
	void OnBulletCollisionStay(bool fromPlayer, Bullet* bullet);
	void OnBulletCollisionExit(bool fromPlayer, Bullet* bullet);

	float getMoveSpeed();
	float getRotationSpeed();
	int getHealth();
	int getAmmunitionNo();
	int getBulletNo();
	Matrix getHeading();

	CBoundingBox GetBounds() { return m_boundingBox; }
};

#endif
