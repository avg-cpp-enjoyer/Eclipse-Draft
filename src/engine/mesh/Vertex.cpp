#include "Vertex.hpp"

#include <DirectXMathConvert.inl>
#include <DirectXMathVector.inl>

Vertex::Vertex() : pos(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f) {}

Vertex::Vertex(DirectX::XMFLOAT3A pos, DirectX::XMFLOAT3A normal, DirectX::XMFLOAT2A uv, DirectX::XMFLOAT4A color)
	: pos(pos), normal(normal), uv(uv), color(color)
{}

Vertex::Vertex(DirectX::XMVECTOR pos, DirectX::XMVECTOR normal, DirectX::XMVECTOR uv, DirectX::XMVECTOR color) {
	DirectX::XMStoreFloat3A(&this->pos, pos);
	DirectX::XMStoreFloat3A(&this->normal, normal);
	DirectX::XMStoreFloat2A(&this->uv, uv);
	DirectX::XMStoreFloat4A(&this->color, color);
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
	return XMLoadFloat3A(&pos);
}

DirectX::XMVECTOR Vertex::NormalVec() const noexcept {
	return XMLoadFloat3A(&normal);
}

DirectX::XMVECTOR Vertex::UVVec() const noexcept {
	return XMLoadFloat2A(&uv);
}

DirectX::XMVECTOR Vertex::ColorVec() const noexcept {
	return XMLoadFloat4A(&color);
}

void Vertex::SetPosition(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat3A(&pos, vec);
}

void Vertex::SetNormal(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat3A(&normal, vec);
}

void Vertex::SetUV(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat2A(&uv, vec);
}

void Vertex::SetColor(DirectX::XMVECTOR vec) noexcept {
	XMStoreFloat4A(&color, vec);
}
