#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

class GraphicsDevice {
public:
	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;

	static void Initialize();
	static IDXGIFactory6* DXGIFactory();
	static ID3D11DeviceContext* ImmediateContext();
	static ID3D11Device* D3D11Device();
	static void DumpD3D11LiveObjects();
	static void SetViewMatrix(const DirectX::XMMATRIX& view);
	static void SetProjectionMatrix(float fov, float aspectRatio, float zNear, float zFar);
	static const DirectX::XMMATRIX& ViewMatrix();
	static const DirectX::XMMATRIX& ProjectionMatrix();
private:
	GraphicsDevice() = default;
	~GraphicsDevice() = default;

	static GraphicsDevice& Instance();
	void EnableDXGIDebugLayer();
	void EnableD3D11DebugLayer();
	Microsoft::WRL::ComPtr<IDXGIAdapter4> PickBestAdapter();
	void InitDXGIFactory();
	void InitD3D11Device();
	void InitShaders();
private:
	Microsoft::WRL::ComPtr<ID3D11Device>             m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>      m_immediateContext;
	Microsoft::WRL::ComPtr<IDXGIFactory6>            m_dxgiFactory;
#ifdef _DEBUG									     
	Microsoft::WRL::ComPtr<IDXGIDebug1>              m_dxgiDebug;
	Microsoft::WRL::ComPtr<ID3D11Debug>              m_d3d11Debug;
#endif											     
	DirectX::XMMATRIX                                m_viewMatrix{};
	DirectX::XMMATRIX                                m_projMatrix{};
};