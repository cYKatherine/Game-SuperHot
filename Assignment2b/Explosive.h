#ifndef EXPLOSIVE_H
#define EXPLOSIVE_H

#include "PhysicsObject.h"
#include "MathsHelper.h"
#include "AudioSystem.h"

class Explosive : public PhysicsObject
{
protected:
	CBoundingBox m_boundingBox;
	AudioSystem* m_audio;

	bool m_readyToExplode = false;
	bool m_picked = false;
	bool m_exploded = false;

	int explodeCount = 0;
public:
	Explosive(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, AudioSystem* audio);

	virtual void Update(float timestep);
	CBoundingBox GetBounds() { return m_boundingBox; }

	void setReadyToExplode(bool ready);
	void setPicked(bool picked);
	void setExploded(bool exploded);
	bool getReadyToExplode();
	bool getPicked();
	bool getExploded();
};

#endif