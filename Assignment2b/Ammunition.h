#ifndef AMMUNITION_H
#define AMMUNITION_H

#include "GameObject.h"
#include "Player.h"

class Player;
class Enemy;

class Ammunition : public GameObject
{
private:
	CBoundingBox m_boundingBox;

	void respawn();

	int respawnCount = 0;

public:
	Ammunition(Mesh* mesh, Shader* shader, Vector3 position);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }
	void OnPlayerCollisionEnter();
	void OnPlayerCollisionStay();
	void OnPlayerCollisionExit();

	void OnEnemyCollisionEnter();
	void OnEnemyCollisionStay();
	void OnEnemyCollisionExit();

};

#endif