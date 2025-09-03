#include "Vertex.hpp"

Vertex::Vertex() : m_pos(0.0f, 0.0f, 0.0f), m_normal(0.0f, 0.0f, 0.0f), m_uv(0.0f, 0.0f) {}

Vertex::Vertex(DirectX::XMFLOAT3A pos, DirectX::XMFLOAT3A normal, DirectX::XMFLOAT2A uv, DirectX::XMFLOAT4A color)
	: m_pos(pos), m_normal(normal), m_uv(uv), m_color(color)
{}

Vertex::Vertex(DirectX::XMVECTOR pos, DirectX::XMVECTOR normal, DirectX::XMVECTOR uv, DirectX::XMVECTOR color) {
	XMStoreFloat3A(&m_pos, pos);
	XMStoreFloat3A(&m_normal, normal);
	XMStoreFloat2A(&m_uv, uv);
	XMStoreFloat4A(&m_color, color);
}

Vertex& Vertex::operator*=(const DirectX::XMMATRIX& matrix) {
	SetPosition(DirectX::XMVector3Transform(PositionVec(), matrix));
	SetNormal(DirectX::XMVector3TransformNormal(NormalVec(), matrix));
	return *this;
}

Vertex operator*(Vertex vertex, const DirectX::XMMATRIX& matrix) {
	vertex *= matrix;
	return vertex;
}

Vertex& Vertex::TransformPosition(const DirectX::XMMATRIX& matrix) {
	SetPosition(DirectX::XMVector3Transform(PositionVec(), matrix));
	return *this;
}

Vertex& Vertex::TransformNormal(const DirectX::XMMATRIX& matrix) {
	SetNormal(DirectX::XMVector3TransformNormal(NormalVec(), matrix));
	return *this;
}

DirectX::XMVECTOR Vertex::PositionVec() const noexcept {
	return XMLoadFloat3A(&m_pos);
}

DirectX::XMVECTOR Vertex::NormalVec() const noexcept {
	return XMLoadFloat3A(&m_normal);
}

DirectX::XMVECTOR Vertex::UVVec() const noexcept {
	return XMLoadFloat2A(&m_uv);
}

DirectX::XMVECTOR Vertex::ColorVec() const noexcept {
	return XMLoadFloat4A(&m_color);
}

const DirectX::XMFLOAT2A& Vertex::UV() const noexcept {
	return m_uv;
}

const DirectX::XMFLOAT4A& Vertex::Color() const noexcept {
	return m_color;
}

void Vertex::SetPosition(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat3A(&m_pos, vec);
}

void Vertex::SetPosition(DirectX::XMFLOAT3A pos) noexcept {
	m_pos = pos;
}

void Vertex::SetNormal(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat3A(&m_normal, vec);
}

void Vertex::SetNormal(DirectX::XMFLOAT3A normal) noexcept {
	m_normal = normal;
}

void Vertex::SetUV(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat2A(&m_uv, vec);
}

void Vertex::SetUV(const DirectX::XMFLOAT2A& uv) noexcept {
	m_uv = uv;
}

void Vertex::SetColor(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat4A(&m_color, vec);
}

void Vertex::SetColor(const DirectX::XMFLOAT4A& color) noexcept {
	m_color = color;
}
