#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <vector>
#include "Ammunition.h"
#include "Collisions.h"
#include "Player.h"
#include "Ruby.h"
#include "Enemy.h"
#include "Bullet.h"

#define MAX_ALLOWED_COLLISIONS 2048

class CollisionManager
{
private:
	std::vector<Player*>* m_players;
	std::vector<Ammunition*>* m_ammunitions;
	std::vector<Ruby*>* m_rubies;
	std::vector<Enemy*>* m_enemies;
	std::vector<Bullet*>* m_bullets;

	GameObject* m_currentCollisions[MAX_ALLOWED_COLLISIONS];

	// We need to know what objects were colliding last frame so we can determine if a collision has just begun or ended
	GameObject* m_previousCollisions[MAX_ALLOWED_COLLISIONS];

	int m_nextCurrentCollisionSlot;

	// Check if we already know about two objects colliding
	bool ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second);

	// Register that a collision has occurred
	void AddCollision(GameObject* first, GameObject* second);

	// Collision check helpers
	void PlayerToAmmunition();
	void EnemyToEnemy();
	void PlayerToRuby();
	void EnemyToAmmunition();
	void EnemyToPlayer();
	void BulletToPlayer();
	void BulletToEnemy();

public:
	CollisionManager(std::vector<Player*>* players, std::vector<Ammunition*>* ammunitions, std::vector<Ruby*>* rubies, std::vector<Enemy*>* enemies, std::vector<Bullet*>* bullets);
	void CheckCollisions();

};

#endif