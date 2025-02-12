#pragma once
#include <DirectXMath.h>


class Transform
{

private:

	// Variables
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTranspose;

	bool dirty;

	//--------
	// Methods
	//--------

	void UpdateWorldMatrix();

public:
	Transform();

	//--------
	// Setters
	// -------
	
	// Position setters
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);

	// Rotation setters
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation); 

	// Scale setters
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	//--------
	// Getters
	// -------
	
	// Getters for transforms
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	
	// World matrix getters
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	//-------------
	// Transformers
	//-------------

	// Transformers for modifying values
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);

	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);

	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);


};