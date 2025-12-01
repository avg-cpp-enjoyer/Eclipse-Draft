#include "Mesh.hpp"

#include <engine/core/GraphicsDevice.hpp>
#include <utils/Direct3D11Utils.hpp>
#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/VertexShader.hpp>
#include <engine/graphics/PixelShader.hpp>
#include <engine/graphics/PipelineStateManager.hpp>
#include <utils/Log.hpp>

#include <d3d11.h>
#include <cstdint>
#include <dxgiformat.h>
#include <d3dcommon.h>

void Mesh::CreateBuffers(ID3D11Device* device) {
	D3D11_BUFFER_DESC vbd{};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = static_cast<uint32_t>(m_vertices.size() * sizeof(Vertex));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vinit{};
	vinit.pSysMem = m_vertices.data();

	D3D11_BUFFER_DESC ibd{};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = static_cast<uint32_t>(m_indices.size() * sizeof(uint32_t));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA iinit{};
	iinit.pSysMem = m_indices.data();

	HR_LOG(device->CreateBuffer(&vbd, &vinit, &m_vertexBuffer));
	HR_LOG(device->CreateBuffer(&ibd, &iinit, &m_indexBuffer));
}

void Mesh::Draw(ID3D11DeviceContext* context) const {
	TransformBuffer transformBuffer{};
	transformBuffer.world       = Direct3D11Utils::SetWorldMatrix(m_position, m_rotation, m_scale);
	transformBuffer.view        = GraphicsDevice::ViewMatrix();
	transformBuffer.projection  = GraphicsDevice::ProjectionMatrix();

	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;

	ShaderProgram::GetShaderByName<VertexShader>(L"VertexShader.hlsl")->Bind(context);
	ShaderProgram::GetShaderByName<PixelShader>(L"PixelShader.hlsl")->Bind(context);

	//TODO: const PipelineState& state = PipelineStateManager::Get(m_material->Mode());
	const PipelineState& state = PipelineStateManager::Get(PipelineMode::OpaqueSolid);
	context->RSSetState(state.rasterizer);
	context->OMSetBlendState(state.blend, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(state.depthStencil, 0);

	context->UpdateSubresource(GraphicsDevice::TransformBufferPtr(), 0, nullptr, &transformBuffer, 0, 0);
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(static_cast<uint32_t>(m_indices.size()), 0, 0);
}

void Mesh::SetPosition(const DirectX::XMFLOAT3& pos) {
	m_position = pos;
}

void Mesh::SetRotation(const DirectX::XMFLOAT3& rotation) {
	m_rotation = rotation;
}

void Mesh::SetScale(const DirectX::XMFLOAT3& scale) {
	m_scale = scale;
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices) {
	m_vertices = vertices;
}

void Mesh::SetIndices(const std::vector<uint32_t>& indices) {
	m_indices = indices;
}

const std::vector<Vertex>& Mesh::Vertices() {
	return m_vertices;
}

const std::vector<uint32_t>& Mesh::Indices() {
	return m_indices;
}
