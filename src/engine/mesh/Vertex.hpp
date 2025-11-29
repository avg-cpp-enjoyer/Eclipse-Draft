#pragma once

#include <DirectXMath.h>

__declspec(align(16)) struct Vertex {
	Vertex();
	Vertex(DirectX::XMFLOAT3A pos, DirectX::XMFLOAT3A normal, DirectX::XMFLOAT2A uv, DirectX::XMFLOAT4A color = { 1.0f, 1.0f, 1.0f, 1.0f});
	Vertex(DirectX::XMVECTOR pos, DirectX::XMVECTOR normal, DirectX::XMVECTOR uv, DirectX::XMVECTOR color);

	Vertex& operator*=(const DirectX::XMMATRIX& matrix);
	friend Vertex operator*(Vertex vertex, const DirectX::XMMATRIX& matrix);

	Vertex& TransformPosition(const DirectX::XMMATRIX& matrix);
	Vertex& TransformNormal(const DirectX::XMMATRIX& matrix);
	DirectX::XMVECTOR PositionVec() const noexcept;
	DirectX::XMVECTOR NormalVec() const noexcept;
	DirectX::XMVECTOR UVVec() const noexcept;
	DirectX::XMVECTOR ColorVec() const noexcept;
	void SetPosition(DirectX::XMVECTOR vec) noexcept;
	void SetNormal(DirectX::XMVECTOR vec) noexcept;
	void SetUV(DirectX::XMVECTOR vec) noexcept;
	void SetColor(DirectX::XMVECTOR vec) noexcept;

	DirectX::XMFLOAT3A pos;
	DirectX::XMFLOAT3A normal;
	DirectX::XMFLOAT2A uv;
	DirectX::XMFLOAT4A color;
};