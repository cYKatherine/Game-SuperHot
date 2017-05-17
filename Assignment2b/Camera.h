/*	FIT2096 - Example Code
*	Camera.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This class represents a Camera in our scene
*	It contains all of the inforamtion needed to create a View and Projection Matrix
*	As well as an Update method which will create the matrices if needed
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "Direct3D.h"
#include "DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class Camera
{
private:
	Vector3 m_position;			//A view matrix is made up of the position of the camera
	Vector3 m_lookAtTarget;		//The point in space it is looking at
	Vector3 m_up;				//And a vector to defines which way is "up" for the camera

	float m_aspectRatio;		//A projection matrix is made up of the aspect ratio of the screen (width/height)
	float m_fieldOfView;		//The field of view of the camera
	float m_nearClip;			//The near clipping plane (anything closer then this to the camera won't be rendered)
	float m_farClip;			//The far clipping plane (anything further then this from the camera won't be rendered)

	Matrix m_view;				//This stores our final view matrix
	Matrix m_projection;		//This stores our fial projection matrix

	bool m_viewDirty;			//These bools are used to determine if we need to recalculate the View and Projection Matrices
	bool m_projectionDirty;		//Whenever a value is changed these bools are set true, the update method only calculates a matrix if its state is "dirty"

public:
	Camera();	//Constructor
	Camera(Vector3 pos, Vector3 lookAt, Vector3 up, float aspect, float fov, float nearClip, float farClip);	//Parameter Constructor
	~Camera();	//Destructor

	//Mutators
	void SetPosition(Vector3 pos);
	void SetLookAt(Vector3 lookAt);
	void SetUp(Vector3 up);

	void SetAspectRatio(float aspect);
	void SetFieldOfView(float fov);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);

	//Accessors
	Matrix GetView() { return m_view; }
	Matrix GetProjection() { return m_projection; }

	Vector3 GetPosition() { return m_position; }

	virtual void Update(float timestep);	//The Update method is used to recalculate the matrices, however later on we could use it to move the camera around
};											//This is why it is virtual and why it receives the timestep as a parameter


#endif