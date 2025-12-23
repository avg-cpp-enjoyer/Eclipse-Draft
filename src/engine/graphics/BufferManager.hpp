#pragma once

#include "ConstantBuffers.hpp"

#include <d3d11.h>
#include <wrl/client.h>

class BufferManager {
public:
	static void Initialize(ID3D11Device* device);
	static ID3D11Buffer* GetTransformBuffer();
	static ID3D11Buffer* GetLightBuffer();
	static ID3D11Buffer* GetGridParamsBuffer();
	static ID3D11Buffer* GetCameraParamsBuffer();
	static void UpdateTransformBuffer(ID3D11DeviceContext* context, const TransformBuffer& data);
	static void UpdateLightBuffer(ID3D11DeviceContext* context, const LightBuffer& data);
	static void UpdateGridParams(ID3D11DeviceContext* context, const GridParams& data);
	static void UpdateCameraParams(ID3D11DeviceContext* context, const CameraParams& data);
private:
	static inline Microsoft::WRL::ComPtr<ID3D11Buffer> m_transformBuffer;
	static inline Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightBuffer;
	static inline Microsoft::WRL::ComPtr<ID3D11Buffer> m_gridParamsBuffer;
	static inline Microsoft::WRL::ComPtr<ID3D11Buffer> m_cameraParamsBuffer;
};