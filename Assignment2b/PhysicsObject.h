#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include "GameObject.h"

class PhysicsObject : public GameObject
{
protected:
	// Implementing basic physics
	// http://natureofcode.com/book/chapter-2-forces/

	// Parent manages position vector
	Vector3 m_velocity;
	Vector3 m_acceleration;

	float m_frictionAmount;

	void ApplyForce(Vector3 force);
	void ApplyFriction(float strength);

public:
	PhysicsObject();
	PhysicsObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position);
	virtual void Update(float timestep);

	Vector3 GetVelocity() { return m_velocity; }

};

#endif