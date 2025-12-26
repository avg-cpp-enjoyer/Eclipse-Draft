#include "BufferManager.hpp"
#include "ConstantBuffers.hpp"
#include "ConstantBuffer.hpp"

#include <d3d11.h>
#include <DirectXMath.h>
#include <utility>
#include <memory>

void BufferManager::Initialize(ID3D11Device* device) {
	auto transformBuf = std::make_unique<ConstantBuffer<TransformBuffer>>();
	transformBuf->Initialize(device);
	m_buffers[typeid(TransformBuffer)] = std::move(transformBuf);

	auto lightBuf = std::make_unique<ConstantBuffer<LightBuffer>>();
	lightBuf->Initialize(device);
	m_buffers[typeid(LightBuffer)] = std::move(lightBuf);

	auto gridParams = std::make_unique<ConstantBuffer<GridParams>>();
	gridParams->Initialize(device);
	m_buffers[typeid(GridParams)] = std::move(gridParams);

	auto cameraParams = std::make_unique<ConstantBuffer<CameraParams>>();
	cameraParams->Initialize(device);
	m_buffers[typeid(CameraParams)] = std::move(cameraParams);

	m_gridParamsData = { 
		.cellSize = 0.5f, 
		.lineWidth = 0.02f, 
		.baseAlpha = 0.5f, 
		.gridColor = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f), 
		.axisXColor = DirectX::XMFLOAT3(0.0f, 0.5f, 1.0f), 
		.axisZColor = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 
		.axisWidth = 0.3f, 
		.fadeDistance = 50.0f 
	};
}

void BufferManager::UpdateGridBuffer(ID3D11DeviceContext* context) {
	UpdateBuffer(context, m_gridParamsData);
}
