#ifndef AMMUNITION_H
#define AMMUNITION_H

#include "GameObject.h"
#include "Player.h"
#include "AudioSystem.h"

class Player;
class Enemy;

class Ammunition : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	AudioSystem* m_audio;

	bool m_played = false;

	void respawn();

	int respawnCount = 0;

public:
	Ammunition(Mesh* mesh, Shader* shader, Vector3 position, AudioSystem* audio);

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