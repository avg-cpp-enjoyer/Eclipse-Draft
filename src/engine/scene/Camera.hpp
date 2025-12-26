#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>

class Camera {
public:
	void OnMouseButtonDown(int x, int y);
	void OnMouseButtonUp();
	void OnMouseMove(int x, int y);
	void OnMouseWheel(int16_t delta);
	void SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
	DirectX::XMMATRIX ViewMatrix() const;
	DirectX::XMMATRIX ProjectionMatrix() const;
	DirectX::XMFLOAT3 Position() const;
private:
	DirectX::XMMATRIX m_projectionMatrix{};
	float m_radius = 10.0f;
	float m_yaw = 0.0f; 
	float m_pitch = 0.0f;
	POINT m_lastMousePos{};
	bool m_rotating = false;
};