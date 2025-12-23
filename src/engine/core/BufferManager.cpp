#include "BufferManager.hpp"

#include <d3d11.h>
#include <DirectXMath.h>

void BufferManager::Initialize(ID3D11Device* device) {
	m_transformBuffer.Initialize(device);
	m_lightBuffer.Initialize(device);
	m_gridBuffer.Initialize(device);
	m_cameraBuffer.Initialize(device);

	m_lightBufferData = {
		.lightPos = DirectX::XMFLOAT3(0.0f, 5.0f, -5.0f),
		.lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)
	};

	m_gridParamsData = { 
		.cellSize = 0.5f, 
		.lineWidth = 0.02f, 
		.majorStep = 5.0f, 
		.majorWidth = 0.05f, 
		.baseAlpha = 0.5f, 
		.gridColor = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f), 
		.majorColor = DirectX::XMFLOAT3(0.6f, 0.6f, 0.6f), 
		.axisXColor = DirectX::XMFLOAT3(0.0f, 0.5f, 1.0f), 
		.axisZColor = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 
		.axisWidth = 0.3f, 
		.fadeDistance = 50.0f 
	};
}

void BufferManager::UpdateTransformBuffer(ID3D11DeviceContext* context, const TransformBuffer& data) {
	m_transformBuffer.Update(context, data);
}

void BufferManager::UpdateLightBuffer(ID3D11DeviceContext* context) {
	m_lightBuffer.Update(context, m_lightBufferData);
}

void BufferManager::UpdateGridBuffer(ID3D11DeviceContext* context) {
	m_gridBuffer.Update(context, m_gridParamsData);
}

void BufferManager::UpdateCameraBuffer(ID3D11DeviceContext* context, const CameraParams& data) {
	m_cameraBuffer.Update(context, data);
}

ID3D11Buffer* BufferManager::GetTransformBuffer() {
	return m_transformBuffer.Get();
}

ID3D11Buffer* BufferManager::GetLightBuffer() {
	return m_lightBuffer.Get();
}

ID3D11Buffer* BufferManager::GetGridBuffer() {
	return m_gridBuffer.Get();
}

ID3D11Buffer* BufferManager::GetCameraBuffer() {
	return m_cameraBuffer.Get();
}
