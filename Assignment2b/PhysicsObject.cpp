#include "PhysicsObject.h"

PhysicsObject::PhysicsObject() :
	GameObject()
{
	m_velocity = Vector3::Zero;
	m_acceleration = Vector3::Zero;
}

PhysicsObject::PhysicsObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position) : 
	GameObject(mesh, shader, texture, position)
{
	m_velocity = Vector3::Zero;
	m_acceleration = Vector3::Zero;
}

void PhysicsObject::Update(float timestep)
{
	// Apply friction every frame
	ApplyFriction(m_frictionAmount);

	// Acceleration trickles down into velocity
	m_velocity += m_acceleration;

	// Velocity trickles down into position
	m_position += m_velocity;

	// Reset acceleration each frame so it doesn't accumulate
	m_acceleration = Vector3::Zero;

}

void PhysicsObject::ApplyForce(Vector3 force)
{
	// You could take mass into account here if you want to get fancy
	m_acceleration += force;
}

void PhysicsObject::ApplyFriction(float strength)
{
	// Force in opposite direction to velocity
	ApplyForce(-m_velocity * strength);
}
