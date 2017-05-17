#ifndef BULLET_H
#define BULLET_H

#include "PhysicsObject.h"
#include "MathsHelper.h"

class Bullet : public PhysicsObject 
{
protected:
	CBoundingBox m_boundingBox;
	float m_moveSpeed;
	float m_flyingTime = 0;

	bool m_fromPlayer;
	bool m_disappear = false;
public:
	Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float rotationY);

	virtual void Update(float timestep);
	CBoundingBox GetBounds() { return m_boundingBox; }

	void setHeading(float heading);

	void OnPlayerCollisionEnter();
	void OnPlayerCollisionStay();
	void OnPlayerCollisionExit();

	void OnEnemyCollisionEnter();
	void OnEnemyCollisionStay();
	void OnEnemyCollisionExit();

	bool getFromPlyer();
	bool getDisappear();
	void setFromPlyer(bool from_player);
};

#endif
