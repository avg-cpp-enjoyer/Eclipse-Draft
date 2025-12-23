#include "BufferManager.hpp"
#include "ConstantBuffers.hpp"

#include <utils/Log.hpp>
#include <d3d11.h>
#include <cstdint>

void BufferManager::Initialize(ID3D11Device* device) {
	auto align16 = [](uint32_t x) -> uint32_t { return (x + 15u) & ~15u; };

	D3D11_BUFFER_DESC transformDesc{};
	transformDesc.Usage = D3D11_USAGE_DEFAULT;
	transformDesc.ByteWidth = sizeof(TransformBuffer);
	transformDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR_LOG(device->CreateBuffer(&transformDesc, nullptr, &m_transformBuffer));

	D3D11_BUFFER_DESC lightDesc{};
	lightDesc.Usage = D3D11_USAGE_DEFAULT;
	lightDesc.ByteWidth = sizeof(LightBuffer);
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR_LOG(device->CreateBuffer(&lightDesc, nullptr, &m_lightBuffer));

	D3D11_BUFFER_DESC gridDesc{};
	gridDesc.Usage = D3D11_USAGE_DEFAULT;
	gridDesc.ByteWidth = align16(sizeof(GridParams));
	gridDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR_LOG(device->CreateBuffer(&gridDesc, nullptr, &m_gridParamsBuffer));

	D3D11_BUFFER_DESC camDesc{};
	camDesc.Usage = D3D11_USAGE_DEFAULT;
	camDesc.ByteWidth = sizeof(CameraParams);
	camDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HR_LOG(device->CreateBuffer(&camDesc, nullptr, &m_cameraParamsBuffer));
}

ID3D11Buffer* BufferManager::GetTransformBuffer() { 
	return m_transformBuffer.Get(); 
}

ID3D11Buffer* BufferManager::GetLightBuffer() { 
	return m_lightBuffer.Get(); 
}

ID3D11Buffer* BufferManager::GetGridParamsBuffer() { 
	return m_gridParamsBuffer.Get(); 
}

ID3D11Buffer* BufferManager::GetCameraParamsBuffer() { 
	return m_cameraParamsBuffer.Get(); 
}

void BufferManager::UpdateTransformBuffer(ID3D11DeviceContext* context, const TransformBuffer& data) {
	context->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &data, 0, 0);
}

void BufferManager::UpdateLightBuffer(ID3D11DeviceContext* context, const LightBuffer& data) {
	context->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &data, 0, 0);
}

void BufferManager::UpdateGridParams(ID3D11DeviceContext* context, const GridParams& data) {
	context->UpdateSubresource(m_gridParamsBuffer.Get(), 0, nullptr, &data, 0, 0);
}

void BufferManager::UpdateCameraParams(ID3D11DeviceContext* context, const CameraParams& data) {
	context->UpdateSubresource(m_cameraParamsBuffer.Get(), 0, nullptr, &data, 0, 0);
}
