#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <d3d11sdklayers.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

class GraphicsDevice {
public:
	static void Initialize();
	static IDXGIFactory6* DXGIFactory();
	static ID3D11DeviceContext* ImmediateContext();
	static ID3D11Device* D3D11Device();
	static void DumpD3D11LiveObjects();
private:
	static void EnableDXGIDebugLayer();
	static void EnableD3D11DebugLayer();
	static Microsoft::WRL::ComPtr<IDXGIAdapter4> PickBestAdapter();
	static void InitDXGIFactory();
	static void InitD3D11Device();
	static void InitShaders();
private:
	static inline Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	static inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
	static inline Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
#ifdef _DEBUG									     
	static inline Microsoft::WRL::ComPtr<IDXGIDebug1> m_dxgiDebug;
	static inline Microsoft::WRL::ComPtr<ID3D11Debug> m_d3d11Debug;
#endif											     
};