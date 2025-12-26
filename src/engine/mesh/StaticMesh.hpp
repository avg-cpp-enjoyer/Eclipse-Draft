#pragma once

#include "Vertex.hpp"
#include "IMesh.hpp"

#include <vector>
#include <d3d11.h>
#include <wrl/client.h>
#include <engine/scene/Camera.hpp>
#include <DirectXMath.h>
#include <cstdint>

class StaticMesh : public IMesh {
public:
	StaticMesh() = default;
	~StaticMesh() = default;

	void CreateBuffers(ID3D11Device* device) override;
	virtual void Draw(ID3D11DeviceContext* context, const Camera& camera) const override;
	void Update(float deltaTime) override {}
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotation(const DirectX::XMFLOAT3& rotation);
	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetVertices(const std::vector<Vertex>& vertices);
	void SetIndices(const std::vector<uint32_t>& indices);
	const std::vector<Vertex>& Vertices();
	const std::vector<uint32_t>& Indices();
protected:
	DirectX::XMFLOAT3 m_position{ 0, 0, 0 };
	DirectX::XMFLOAT3 m_rotation{ 0, 0, 0 };
	DirectX::XMFLOAT3 m_scale{ 1, 1, 1 };
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};