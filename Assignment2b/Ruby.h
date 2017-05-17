#ifndef RUBY_H
#define RUBY_H

#include "GameObject.h"

class Ruby : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	bool m_picked = false;
public:
	Ruby(Mesh* mesh, Shader* shader, Vector3 position);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }
	void OnPlayerCollisionEnter();
	void OnPlayerCollisionStay();
	void OnPlayerCollisionExit();

	bool getPicked();

};

#endif