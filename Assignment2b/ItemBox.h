#ifndef ITEM_BOX_H
#define ITEM_BOX_H

#include "GameObject.h"

class ItemBox : public GameObject
{
private:
	int respawnCount = 0;

	CBoundingBox m_boundingBox;

public:
	ItemBox(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }
	void OnPlayerCollisionEnter();
	void OnPlayerCollisionStay();
	void OnPlayerCollisionExit();

};

#endif