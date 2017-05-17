#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<Player*>* players, std::vector<Ammunition*>* ammunitions, std::vector<Ruby*>* rubies, std::vector<Enemy*>* enemies, std::vector<Bullet*>* bullets)
{
	m_players = players;
	m_ammunitions = ammunitions;
	m_rubies = rubies;
	m_enemies = enemies;
	m_bullets = bullets;

	// Clear our arrays to 0 (NULL)
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	memset(m_previousCollisions, 0, sizeof(m_previousCollisions));

	m_nextCurrentCollisionSlot = 0;
}

void CollisionManager::CheckCollisions()
{
	// Check Player to item box collisions
	PlayerToAmmunition();
	EnemyToEnemy();
	PlayerToRuby();
	EnemyToAmmunition();
	EnemyToPlayer();
	BulletToPlayer();
	BulletToEnemy();

	// Move all current collisions into previous
	memcpy(m_previousCollisions, m_currentCollisions, sizeof(m_currentCollisions));

	// Clear out current collisions
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	
	// Now current collisions is empty, we'll start adding from the start again
	m_nextCurrentCollisionSlot = 0;

}

bool CollisionManager::ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second)
{
	// See if these two GameObjects appear one after the other in specified collisions array
	// Stop one before length so we don't overrun as we'll be checking two elements per iteration
	for (int i = 0; i < MAX_ALLOWED_COLLISIONS - 1; i += 2)
	{
		if ((arrayToSearch[i] == first && arrayToSearch[i + 1] == second) ||
			arrayToSearch[i] == second && arrayToSearch[i + 1] == first)
		{
			// Found them!
			return true;
		}
	}

	// These objects were not colliding last frame
	return false;
}

void CollisionManager::AddCollision(GameObject* first, GameObject* second)
{	
	// Add the two colliding objects to the current collisions array
	// We keep track of the next free slot so no searching is required
	m_currentCollisions[m_nextCurrentCollisionSlot] = first;
	m_currentCollisions[m_nextCurrentCollisionSlot + 1] = second;
	
	m_nextCurrentCollisionSlot += 2;
}

void CollisionManager::PlayerToAmmunition()
{
	// We'll check each Player against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_players->size(); i++)
	{
		for (unsigned int j = 0; j < m_ammunitions->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Player* player = (*m_players)[i];
			Ammunition* ammunition = (*m_ammunitions)[j];

			CBoundingBox playerBounds = player->GetBounds();
			CBoundingBox ammunitionBounds = ammunition->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(playerBounds, ammunitionBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, player, ammunition);

			if (isColliding)
			{
				// Register the collision
				AddCollision(player, ammunition);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a Player has collided with it (we could pass it the actual Player too if we like)
					ammunition->OnPlayerCollisionStay();
					player->OnAmmunitionCollisionStay();
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					ammunition->OnPlayerCollisionEnter();
					player->OnAmmunitionCollisionEnter(ammunition);
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					ammunition->OnPlayerCollisionExit();
					player->OnAmmunitionCollisionExit();
				}
			}
		}
	}
}

void CollisionManager::EnemyToEnemy()
{
	// We'll check each Player against every other Player
	// Note this is not overly efficient, both in readability and runtime performance

	for (int i = 0; i < m_enemies->size(); i++)
	{
		for (int j = 0; j < m_enemies->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			Enemy* firstEnemy = (*m_enemies)[i];
			Enemy* secondEnemy = (*m_enemies)[j];

			// Don't compare a Enemy to itself
			// Comparing memory addresses - beauty of pointers
			if (firstEnemy != secondEnemy)
			{
				CBoundingBox firstBounds = firstEnemy->GetBounds();
				CBoundingBox secondBounds = secondEnemy->GetBounds();

				// Do we already know about a collision between these two Enemys
				bool alreadyHandled = ArrayContainsCollision(m_currentCollisions, firstEnemy, secondEnemy);

				if (!alreadyHandled)
				{
					// Are they colliding this frame?
					bool isColliding = CheckCollision(firstBounds, secondBounds);

					// Were they colliding last frame?
					bool wasColliding = ArrayContainsCollision(m_previousCollisions, firstEnemy, secondEnemy);

					if (isColliding)
					{
						// Register the collision
						AddCollision(firstEnemy, secondEnemy);

						if (wasColliding)
						{
							// We are colliding this frame and we were also colliding last frame - that's a collision stay
							// Tell both Enemys so they can respond. Also give each one the other Enemy.
							firstEnemy->OnEnemyCollisionStay(secondEnemy);
							secondEnemy->OnEnemyCollisionStay(firstEnemy);
						}
						else
						{
							// We are colliding this frame and we weren't last frame - that's a collision enter
							firstEnemy->OnEnemyCollisionEnter(secondEnemy);
							secondEnemy->OnEnemyCollisionEnter(firstEnemy);
						}
					}
					else
					{
						if (wasColliding)
						{
							// We aren't colliding this frame but we were last frame - that's a collision exit
							firstEnemy->OnEnemyCollisionExit(secondEnemy);
							secondEnemy->OnEnemyCollisionExit(firstEnemy);
						}
					}
				}
			}
		}
	}
}

void CollisionManager::PlayerToRuby()
{
	// We'll check each Player against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_players->size(); i++)
	{
		for (unsigned int j = 0; j < m_rubies->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Player* player = (*m_players)[i];
			Ruby* ruby = (*m_rubies)[j];

			CBoundingBox playerBounds = player->GetBounds();
			CBoundingBox rubyBounds = ruby->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(playerBounds, rubyBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, player, ruby);

			if (isColliding)
			{
				// Register the collision
				AddCollision(player, ruby);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a Player has collided with it (we could pass it the actual Player too if we like)
					ruby->OnPlayerCollisionStay();
					player->OnRubyCollisionStay(ruby);
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					ruby->OnPlayerCollisionEnter();
					player->OnRubyCollisionEnter(ruby);
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					ruby->OnPlayerCollisionExit();
					player->OnRubyCollisionExit(ruby);
				}
			}
		}
	}
}

void CollisionManager::EnemyToAmmunition()
{
	// We'll check each Enemy against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_enemies->size(); i++)
	{
		for (unsigned int j = 0; j < m_ammunitions->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Enemy* enemy = (*m_enemies)[i];
			Ammunition* ammunition = (*m_ammunitions)[j];

			CBoundingBox enemyBounds = enemy->GetBounds();
			CBoundingBox ammunitionBounds = ammunition->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(enemyBounds, ammunitionBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, enemy, ammunition);

			if (isColliding)
			{
				// Register the collision
				AddCollision(enemy, ammunition);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a Enemy has collided with it (we could pass it the actual Enemy too if we like)
					ammunition->OnEnemyCollisionStay();
					enemy->OnAmmunitionCollisionStay(ammunition);
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					ammunition->OnEnemyCollisionEnter();
					enemy->OnAmmunitionCollisionEnter(ammunition);
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					ammunition->OnEnemyCollisionExit();
					enemy->OnAmmunitionCollisionExit(ammunition);
				}
			}
		}
	}
}

void CollisionManager::EnemyToPlayer()
{
	// We'll check each Enemy against every other Player
	// Note this is not overly efficient, both in readability and runtime performance

	for (int i = 0; i < m_enemies->size(); i++)
	{
		for (int j = 0; j < m_players->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			Enemy* enemy = (*m_enemies)[i];
			Player* player = (*m_players)[j];

			// Don't compare a Player to itself
			// Comparing memory addresses - beauty of pointers

			CBoundingBox firstBounds = enemy->GetBounds();
			CBoundingBox secondBounds = player->GetBounds();

			// Do we already know about a collision between these two Players
			bool alreadyHandled = ArrayContainsCollision(m_currentCollisions, enemy, player);

			if (!alreadyHandled)
			{
				// Are they colliding this frame?
				bool isColliding = CheckCollision(firstBounds, secondBounds);

				// Were they colliding last frame?
				bool wasColliding = ArrayContainsCollision(m_previousCollisions, enemy, player);

				if (isColliding)
				{
					// Register the collision
					AddCollision(enemy, player);

					if (wasColliding)
					{
						// We are colliding this frame and we were also colliding last frame - that's a collision stay
						// Tell both Players so they can respond. Also give each one the other Player.
						enemy->OnPlayerCollisionStay(player);
						player->OnEnemyCollisionStay(enemy);
					}
					else
					{
						// We are colliding this frame and we weren't last frame - that's a collision enter
						enemy->OnPlayerCollisionEnter(player);
						player->OnEnemyCollisionEnter(enemy);
					}
				}
				else
				{
					if (wasColliding)
					{
						// We aren't colliding this frame but we were last frame - that's a collision exit
						enemy->OnPlayerCollisionExit(player);
						player->OnEnemyCollisionExit(enemy);
					}
				}
			}
		}
	}
}

void CollisionManager::BulletToPlayer() {
	// We'll check each Bullet against every other Player
	// Note this is not overly efficient, both in readability and runtime performance

	for (int i = 0; i < m_bullets->size(); i++)
	{
		for (int j = 0; j < m_players->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			Bullet* bullet = (*m_bullets)[i];
			Player* player = (*m_players)[j];

			// Don't compare a Player to itself
			// Comparing memory addresses - beauty of pointers

			CBoundingBox firstBounds = bullet->GetBounds();
			CBoundingBox secondBounds = player->GetBounds();

			// Do we already know about a collision between these two Players
			bool alreadyHandled = ArrayContainsCollision(m_currentCollisions, bullet, player);

			if (!alreadyHandled)
			{
				// Are they colliding this frame?
				bool isColliding = CheckCollision(firstBounds, secondBounds);

				// Were they colliding last frame?
				bool wasColliding = ArrayContainsCollision(m_previousCollisions, bullet, player);

				if (isColliding)
				{
					// Register the collision
					AddCollision(bullet, player);

					if (wasColliding)
					{
						// We are colliding this frame and we were also colliding last frame - that's a collision stay
						// Tell both Players so they can respond. Also give each one the other Player.
						bullet->OnPlayerCollisionStay();
						player->OnBulletCollisionStay(bullet->getFromPlyer(), bullet);
					}
					else
					{
						// We are colliding this frame and we weren't last frame - that's a collision enter
						bullet->OnPlayerCollisionEnter();
						player->OnBulletCollisionEnter(bullet->getFromPlyer(), bullet);
					}
				}
				else
				{
					if (wasColliding)
					{
						// We aren't colliding this frame but we were last frame - that's a collision exit
						bullet->OnPlayerCollisionExit();
						player->OnBulletCollisionExit(bullet->getFromPlyer(), bullet);
					}
				}
			}
		}
	}
}

void CollisionManager::BulletToEnemy() {
	// We'll check each Bullet against every other enemy
	// Note this is not overly efficient, both in readability and runtime performance

	for (int i = 0; i < m_bullets->size(); i++)
	{
		for (int j = 0; j < m_enemies->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			Bullet* bullet = (*m_bullets)[i];
			Enemy* enemy = (*m_enemies)[j];

			// Don't compare a enemy to itself
			// Comparing memory addresses - beauty of pointers

			CBoundingBox firstBounds = bullet->GetBounds();
			CBoundingBox secondBounds = enemy->GetBounds();

			// Do we already know about a collision between these two enemys
			bool alreadyHandled = ArrayContainsCollision(m_currentCollisions, bullet, enemy);

			if (!alreadyHandled)
			{
				// Are they colliding this frame?
				bool isColliding = CheckCollision(firstBounds, secondBounds);

				// Were they colliding last frame?
				bool wasColliding = ArrayContainsCollision(m_previousCollisions, bullet, enemy);

				if (isColliding)
				{
					// Register the collision
					AddCollision(bullet, enemy);

					if (wasColliding)
					{
						// We are colliding this frame and we were also colliding last frame - that's a collision stay
						// Tell both enemys so they can respond. Also give each one the other enemy.
						bullet->OnEnemyCollisionStay();
						enemy->OnBulletCollisionStay(bullet->getFromPlyer(), bullet);
					}
					else
					{
						// We are colliding this frame and we weren't last frame - that's a collision enter
						bullet->OnEnemyCollisionEnter();
						enemy->OnBulletCollisionEnter(bullet->getFromPlyer(), bullet);
					}
				}
				else
				{
					if (wasColliding)
					{
						// We aren't colliding this frame but we were last frame - that's a collision exit
						bullet->OnEnemyCollisionExit();
						enemy->OnBulletCollisionExit(bullet->getFromPlyer(), bullet);
					}
				}
			}
		}
	}
}
