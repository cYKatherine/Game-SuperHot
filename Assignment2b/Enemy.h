#ifndef ENEMY_H
#define ENEMY_H

#include "AudioSystem.h"
#include "PhysicsObject.h"
#include "MathsHelper.h"
#include "Ruby.h"
#include "Player.h"

#include <iostream>
#include <vector>

class Player;
class Ammunition;

class Enemy : public PhysicsObject
{
protected:
	CBoundingBox m_boundingBox;
	AudioSystem* m_audio;

	Player* m_playerToFollow;

	std::vector<Ruby*> m_rubies;

	int m_enemyNo;
	int m_health;
	float m_moveSpeed;
	int m_shootTime = 0;
	Vector3 targetPosition;
	Vector3 localForward;

	Vector3 worldForward = Vector3(0, 0, 1);
	Matrix m_heading;

public:
	Enemy(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, int enemyNo, Player* playerToFollow, std::vector<Ruby*> rubies, AudioSystem* audio);

	virtual void Update(float timestep);

	void OnEnemyCollisionEnter(Enemy* other);
	void OnEnemyCollisionStay(Enemy* other);
	void OnEnemyCollisionExit(Enemy* other);

	void OnAmmunitionCollisionEnter(Ammunition* ammunition);
	void OnAmmunitionCollisionStay(Ammunition* ammunition);
	void OnAmmunitionCollisionExit(Ammunition* ammunition);

	void OnPlayerCollisionEnter(Player* player);
	void OnPlayerCollisionStay(Player* player);
	void OnPlayerCollisionExit(Player* player);

	void OnBulletCollisionEnter(bool fromPlayer, Bullet* bullet);
	void OnBulletCollisionStay(bool fromPlayer, Bullet* bullet);
	void OnBulletCollisionExit(bool fromPlayer, Bullet* bullet);

	void updateRubies(std::vector<Ruby*> rubies);

	int getHealth();
	Matrix getHeading();

	CBoundingBox GetBounds() { return m_boundingBox; }
};

#endif
