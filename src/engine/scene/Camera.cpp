#include "Camera.hpp"

#include <algorithm>
#include <cmath>
#include <DirectXMathMatrix.inl>
#include <DirectXMathVector.inl>
#include <DirectXMath.h>
#include <cstdint>

void Camera::OnMouseButtonDown(int x, int y) {
	m_rotating = true;
	m_lastMousePos = { x, y };
}

void Camera::OnMouseButtonUp() {
	m_rotating = false;
}

void Camera::OnMouseMove(int x, int y) {
	if (m_rotating) {
		static constexpr float limit = DirectX::XM_PIDIV2 - 0.01f;
		static constexpr float rotationSpeed = 0.1f;
		m_yaw   += DirectX::XMConvertToRadians(static_cast<float>(x - m_lastMousePos.x)) * rotationSpeed;
		m_pitch += DirectX::XMConvertToRadians(static_cast<float>(y - m_lastMousePos.y)) * rotationSpeed;
		m_pitch = std::clamp(m_pitch, -limit, limit);
		m_lastMousePos = { x, y };
	}
}

void Camera::OnMouseWheel(int16_t delta) {
	m_radius -= delta * 0.01f;
	m_radius = std::clamp(m_radius, 1.0f, 100.0f);
}

DirectX::XMMATRIX Camera::ViewMatrix() const {
	float camX = m_radius * cosf(m_pitch) * sinf(m_yaw);
	float camY = m_radius * sinf(m_pitch);
	float camZ = m_radius * cosf(m_pitch) * cosf(m_yaw);

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(camX, camY, camZ, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return DirectX::XMMatrixLookAtLH(eye, target, up);
}

void Camera::SetProjectionMatrix(const DirectX::XMMATRIX& matrix) {
	m_projectionMatrix = matrix;
}

DirectX::XMMATRIX Camera::ProjectionMatrix() const {
	return m_projectionMatrix;
}

DirectX::XMFLOAT3 Camera::Position() const {
	float camX = m_radius * cosf(m_pitch) * sinf(m_yaw);
	float camY = m_radius * sinf(m_pitch);
	float camZ = m_radius * cosf(m_pitch) * cosf(m_yaw);

	return DirectX::XMFLOAT3(camX, camY, camZ);
}
