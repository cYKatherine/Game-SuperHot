/*	FIT2096 - Example Code
*	FirstPersonCamera.h
*	Created by Mike Yeates - 2016 - Monash University
*	A third person camera follows along behind a gameobject
*/

#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H

#include "Camera.h"
#include "GameObject.h"
#include "Player.h"
#include "InputController.h"

class FirstPersonCamera : public Camera
{
private:
	InputController* m_input;
	Player* m_objectToFollow;
	Vector3 m_offset;
	float m_rotationSpeed;

	float m_heading;
	float m_pitch;

	Vector3 TransformOffset();

public:
	FirstPersonCamera();
	FirstPersonCamera(InputController* input, Player* target);

	void Update(float timestep);
};

#endif