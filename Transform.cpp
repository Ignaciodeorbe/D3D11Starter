#include "Transform.h"

using namespace DirectX;

// Setting default values in constructor
Transform::Transform()
	: position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f)
{
	// Storing the identity matrix as the world matrix
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());


}

void UpdateWorldMatrix()
{
	if (true)
	{

	}
}

//--------
// Setters
// -------

// Position setters
void Transform::SetPosition(float x, float y, float z) { position = { x, y, z };  }
void Transform::SetPosition(XMFLOAT3 pos) { position = pos; }

// Rotation setters
void Transform::SetRotation(float pitch, float yaw, float roll) { rotation = { pitch, yaw, roll }; }
void Transform::SetRotation(XMFLOAT3 rot) { rotation = rot; }

// Scale setters
void Transform::SetScale(float x, float y, float z) { scale = { x, y, z }; }
void Transform::SetScale(XMFLOAT3 scl) { scale = scl; }


//--------
// Getters
// -------

// Transform getters
XMFLOAT3 Transform::GetPosition() { return position; }
XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
XMFLOAT3 Transform::GetScale() { return scale; }

// Matrix getters
XMFLOAT4X4 Transform::GetWorldMatrix() 
{ 
	UpdateWorldMatrix();
	return worldMatrix; 
}
//XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() { return  }


//-------------
// Transformers
//-------------

// Position transformers
void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::MoveAbsolute(XMFLOAT3 pos)
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
}

// Rotation transformers
void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
}

void Transform::Rotate(XMFLOAT3 rot)
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;
}

// Scale transformers
void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void Transform::Scale(XMFLOAT3 scl)
{
	scale.x *= scl.x;
	scale.y *= scl.y;
	scale.z *= scl.z;
}

