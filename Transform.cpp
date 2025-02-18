#include "Transform.h"

using namespace DirectX;

// Setting default values in constructor
Transform::Transform()
	: position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), dirty(true),
	rightward(1.0f, 0.0f, 0.0f), upward(0.0f, 1.0f, 0.0f), forward(0.0f, 0.0f, 1.0f)
{
	// Storing the identity matrix as the world matrix and inverse matrix
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());

}

/// <summary>
/// Updates the world martix based on any tranformations made to it
/// </summary>
void Transform::UpdateWorldMatrix()
{
	if (dirty)
	{
		// Using SRT to apply transformations
		XMMATRIX world = XMMatrixScaling(scale.x, scale.y, scale.z) * 
			XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * 
			XMMatrixTranslation(position.x, position.y, position.z);

		// Storing the updated world matrix and its inverse transpose
		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTranspose, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));

		// Updating directional vectors
		UpdateDirectionalVectors();

		dirty = false;
	}
}

/// <summary>
/// Updating directional vectors instead of updating them in the getters
/// </summary>
void Transform::UpdateDirectionalVectors()
{
	// Making a rotation quaternion
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	// Updating directional vectors
	XMVECTOR rightVector = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat);
	XMVECTOR upVector = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat);
	XMVECTOR forwardVector = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat);

	// Storing vectors inside fields
	XMStoreFloat3(&rightward, rightVector);
	XMStoreFloat3(&upward, upVector);
	XMStoreFloat3(&forward, forwardVector);
}

//--------
// Setters
// -------

// Position setters
void Transform::SetPosition(float x, float y, float z) { position = { x, y, z }; dirty = true; }
void Transform::SetPosition(XMFLOAT3 pos) { position = pos; dirty = true; }

// Rotation setters
void Transform::SetRotation(float pitch, float yaw, float roll) { rotation = { pitch, yaw, roll }; dirty = true; }
void Transform::SetRotation(XMFLOAT3 rot) { rotation = rot; dirty = true;}

// Scale setters
void Transform::SetScale(float x, float y, float z) { scale = { x, y, z }; dirty = true; }
void Transform::SetScale(XMFLOAT3 scl) { scale = scl; dirty = true; }


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

// Directional vectors getters
XMFLOAT3 Transform::GetRight() { return rightward; }
XMFLOAT3 Transform::GetUp() { return upward; }
XMFLOAT3 Transform::GetForward() { return forward; }


//-------------
// Transformers
//-------------

// Position transformers
void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	dirty = true;
}

void Transform::MoveAbsolute(XMFLOAT3 pos)
{
	position.x += pos.x;
	position.y += pos.y;
	position.z += pos.z;
	dirty = true;
}

// Rotation transformers
void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
	dirty = true;
}

void Transform::Rotate(XMFLOAT3 rot)
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;
	dirty = true;
}

// Scale transformers
void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	dirty = true;
}

void Transform::Scale(XMFLOAT3 scl)
{
	scale.x *= scl.x;
	scale.y *= scl.y;
	scale.z *= scl.z;
	dirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
}

void Transform::MoveRelative(XMFLOAT3 offset)
{
}




