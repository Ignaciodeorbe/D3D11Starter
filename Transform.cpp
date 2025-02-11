#include "Transform.h"

Transform::Transform()
	: position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(0.0f, 0.0f, 0.0f)
{
}

//--------
// Setters
// -------

// Position setters
void Transform::SetPosition(float x, float y, float z) { position = { x, y, z }; }
void Transform::SetPosition(DirectX::XMFLOAT3 pos) { position = pos; }

// Rotation setters
void Transform::SetRotation(float pitch, float yaw, float roll) { rotation = { pitch, yaw, roll }; }
void Transform::SetRotation(DirectX::XMFLOAT3 rot) { rotation = rot; }


void Transform::SetScale(float x, float y, float z) { scale = { x, y, z }; }
void Transform::SetScale(DirectX::XMFLOAT3 scl) { scale = scl; }



//--------
// Getters
// -------

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return DirectX::XMFLOAT4X4();
}


//-------------
// Transformers
//-------------

void Transform::MoveAbsolute(float x, float y, float z)
{
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
}

void Transform::Scale(float x, float y, float z)
{
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
}

