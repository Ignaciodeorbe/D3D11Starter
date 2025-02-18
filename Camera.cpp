#include "Camera.h"

using namespace DirectX;

Camera::Camera(XMFLOAT3 position, float movmentSpeed, float mouseSpeed, float fov, float aspectRatio)
	: movmentSpeed(movmentSpeed), mouseSpeed(mouseSpeed), fov(fov)
{
	transform.SetPosition(position);
}

void Camera::UpdateViewMatrix()
{
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
}

XMFLOAT4X4 Camera::ViewMatrix() { return viewMatrix; }
XMFLOAT4X4 Camera::ProjectionMatrix() { return projectionMatrix; }
