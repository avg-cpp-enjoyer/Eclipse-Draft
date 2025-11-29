#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <algorithm>

class Camera {
public:
	Camera() = default;
	~Camera() = default;

	void OnMouseButtonDown(int x, int y);
	void OnMouseButtonUp();
	void OnMouseMove(int x, int y);
	void OnMouseWheel(int16_t delta);
	DirectX::XMMATRIX ViewMatrix() const;
	DirectX::XMFLOAT3 Position() const;
private:
	float m_radius = 10.0f;
	float m_yaw = 0.0f; 
	float m_pitch = 0.0f;
	POINT m_lastMousePos{};
	bool m_rotating = false;
};