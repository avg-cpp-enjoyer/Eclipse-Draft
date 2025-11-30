#include "Grid.hpp"

#include <d3d11.h>
#include <cstdint>
#include <d3dcommon.h>
#include <dxgiformat.h>

#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/VertexShader.hpp>
#include <engine/graphics/PixelShader.hpp>
#include <engine/graphics/PipelineStateManager.hpp>
#include <engine/core/GraphicsDevice.hpp>
#include <engine/mesh/Vertex.hpp>
#include <utils/Direct3D11Utils.hpp>

void Grid::Draw(ID3D11DeviceContext* context) const {
	TransformBuffer transformBuffer{};
	transformBuffer.world = Direct3D11Utils::SetWorldMatrix(m_position, m_rotation, m_scale);
	transformBuffer.view = GraphicsDevice::ViewMatrix();
	transformBuffer.projection = GraphicsDevice::ProjectionMatrix();

	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;

	ID3D11Buffer* gridParamsBuf = GraphicsDevice::GridParamsBuffer();
	ID3D11Buffer* camBuf = GraphicsDevice::CameraParamsBuffer();
	__m128 blendFactor = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);

	ShaderProgram::GetShaderByName<VertexShader>(L"VertexShader.hlsl")->Bind(context);
	ShaderProgram::GetShaderByName<PixelShader>(L"Grid.hlsl")->Bind(context);

	const PipelineState& state = PipelineStateManager::Get(PipelineMode::TransparentAlpha);
	context->RSSetState(state.rasterizer);
	context->OMSetBlendState(state.blend, reinterpret_cast<const float*>(&blendFactor), 0xFFFFFFFF);
	context->OMSetDepthStencilState(state.depthStencil, 0);

	context->UpdateSubresource(GraphicsDevice::TransformBufferPtr(), 0, nullptr, &transformBuffer, 0, 0);
	context->VSSetConstantBuffers(0, 1, GraphicsDevice::TransformBufferAddr());
	context->UpdateSubresource(GraphicsDevice::GridParamsBuffer(), 0, nullptr, &GraphicsDevice::GridParamsData(), 0, 0);
	context->PSSetConstantBuffers(2, 1, &gridParamsBuf);
	context->PSSetConstantBuffers(3, 1, &camBuf);

	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(static_cast<uint32_t>(m_indices.size()), 0, 0);
}

