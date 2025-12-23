#pragma once

#include "ConstantBuffers.hpp"
#include "ConstantBuffer.hpp"

#include <d3d11.h>

class BufferManager { 
public: 
	static void Initialize(ID3D11Device* device);
	static void UpdateTransformBuffer(ID3D11DeviceContext* context, const TransformBuffer& data);
	static void UpdateLightBuffer(ID3D11DeviceContext* context);
	static void UpdateGridBuffer(ID3D11DeviceContext* context);
	static void UpdateCameraBuffer(ID3D11DeviceContext* context, const CameraParams& data);
	static ID3D11Buffer* GetTransformBuffer();
	static ID3D11Buffer* GetLightBuffer();
	static ID3D11Buffer* GetGridBuffer();
	static ID3D11Buffer* GetCameraBuffer();
private: 
	inline static ConstantBuffer<TransformBuffer> m_transformBuffer; 
	inline static ConstantBuffer<LightBuffer> m_lightBuffer; 
	inline static ConstantBuffer<GridParams> m_gridBuffer; 
	inline static ConstantBuffer<CameraParams> m_cameraBuffer; 
	inline static LightBuffer m_lightBufferData; //TODO: Bind light direction to camera orientation. LightBuffer needs to be stored inside RenderTarget
	inline static GridParams m_gridParamsData;
};