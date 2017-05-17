#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera()
{
	m_objectToFollow = NULL;
	m_offset = Vector3(0.0f, 5.0f, -5.0f);

	m_rotationSpeed = 2.0f;

	m_heading = 0.0f;
	m_pitch = 0.0f;

	SetPosition(m_objectToFollow->GetPosition() + TransformOffset());
}

FirstPersonCamera::FirstPersonCamera(InputController* input, Player* target)
{
	m_input = input;
	m_objectToFollow = target;
	m_offset = Vector3(0, target->GetMesh()->GetMax().y + 1, target->GetMesh()->GetMax().z);

	m_rotationSpeed = target->getRotationSpeed();

	m_heading = 0.0f;
	m_pitch = 0.0f;

	// Set starting position so catchup mode doesn't show camera animating from the origin
	SetPosition(m_objectToFollow->GetPosition() + TransformOffset());
}

void FirstPersonCamera::Update(float timestep)
{
	if (m_objectToFollow != NULL)
	{
		m_pitch += m_input->GetMouseDeltaY() * m_rotationSpeed * timestep;

		// Limit how far the player can look down and up
		m_pitch = MathsHelper::Clamp(m_pitch, ToRadians(-80.0f), ToRadians(80.0f));

		// Wrap heading and pitch up in a matrix so we can transform our look at vector
		// Heading is controlled by MouseX (horizontal movement) but it is a rotation around Y
		// Pitch  controlled by MouseY (vertical movement) but it is a rotation around X
		Matrix heading = m_objectToFollow->getHeading();
		Matrix pitch = Matrix::CreateRotationX(m_pitch);

		// Transform a world right vector from world space into local space
		Vector3 localRight = Vector3::TransformNormal(Vector3(1, 0, 0), heading);

		// Essentially our local forward vector but always parallel with the ground
		// Remember a cross product gives us a vector perpendicular to the two input vectors
		Vector3 localForwardXZ = localRight.Cross(Vector3(0, 1, 0));

		// Not storing this directly in m_offset as it would accumulate over time and spin out of control
		Vector3 rotatedOffset = TransformOffset();
		// We're going to need this a lot. Store it locally here to save on our function calls 
		Vector3 currentPos = m_objectToFollow->GetPosition() + rotatedOffset;

		// Combine pitch and heading into one matrix for convenience
		Matrix lookAtRotation = pitch * heading;

		// Transform a world forward vector into local space (take pitch and heading into account)
		Vector3 lookAt = Vector3::TransformNormal(Vector3(0, 0, 1), lookAtRotation);

		// At this point, our look-at vector is still relative to the origin
		// Add our position to it so it originates from the camera and points slightly in front of it
		// Remember the look-at vector needs to describe a point in the world relative to the origin
		lookAt += currentPos;

		// Use parent's mutators so isDirty flags get flipped
		SetLookAt(lookAt);
		SetPosition(currentPos);
	}

	// Give our parent a chance to regenerate the view matrix
	Camera::Update(timestep);
}

Vector3 FirstPersonCamera::TransformOffset()
{
	// Wrap up the Y rotation of the object we're following in a matrix
	Matrix rotation = Matrix::CreateRotationY(m_objectToFollow->GetYRotation());

	// Transform our offset vector by the rotation so the offset rotates with our follow object
	return Vector3::TransformNormal(m_offset, rotation);
}

