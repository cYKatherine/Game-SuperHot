#ifndef PLAYER_H
#define PLAYER_H

#include "PhysicsObject.h"
#include "InputController.h"
#include "Ammunition.h"
#include "MathsHelper.h"
#include "Bullet.h"
#include "Enemy.h"

#include <iostream>

class Ruby;
class Enemy;
class Bullet;
class Ammunition;


class Player : public PhysicsObject
{
protected:
	InputController* m_input;

	CBoundingBox m_boundingBox;

	float m_moveSpeed;
	float m_rotationSpeed;

	float m_fireCoolDown;
	float m_lastTimeShoot;

	int m_health;
	int m_ammunitionNo = 2;
	int m_bulletNo = 0;

	Matrix m_heading;

public:
	Player(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, InputController* input);

	virtual void Update(float timestep);

	void OnPlayerCollisionEnter(Player* other);
	void OnPlayerCollisionStay(Player* other);
	void OnPlayerCollisionExit(Player* other);

	void OnAmmunitionCollisionEnter(Ammunition* ammunition);
	void OnAmmunitionCollisionStay();
	void OnAmmunitionCollisionExit();

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
