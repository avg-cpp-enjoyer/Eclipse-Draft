#pragma once

#include "utils/Log.hpp"
#include "utils/Direct3D11Utils.hpp"

#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <unordered_map>
#include <mutex>
#include <format>
#include <cstdint>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

__declspec(align(16)) struct TransformBuffer {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

__declspec(align(16)) struct LightBuffer {
	DirectX::XMFLOAT3 lightPos;
	float pad1;
	DirectX::XMFLOAT3 lightColor;
	float pad2;
};

__declspec(align(16)) struct GridParams {
	float cellSize;
	float lineWidth;
	float majorStep;
	float majorWidth;
	float baseAlpha;
	DirectX::XMFLOAT3 gridColor;
	DirectX::XMFLOAT3 majorColor;
	DirectX::XMFLOAT3 axisXColor;
	DirectX::XMFLOAT3 axisZColor;
	float axisWidth;
	float fadeDistance;
	float _pad[2];
};

__declspec(align(16)) struct CameraParams {
	DirectX::XMFLOAT3 cameraPos;
	float pad;
};

struct RenderJob {
	RenderJob() = default;
	~RenderJob();

	RenderJob(const RenderJob&) = delete;
	RenderJob& operator=(const RenderJob&) = delete;
	RenderJob(RenderJob&& other) noexcept;
	RenderJob& operator=(RenderJob&& other) noexcept;

	ID3D11CommandList* commandList = nullptr;
	IDXGISwapChain* swapChain = nullptr;
};

class GraphicsDevice {
public:
	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;

	static void Initialize();
	static void PushRenderJob(HWND window, RenderJob renderJob);
	static void ExecuteRenderJobs();
	static void RemoveRenderJob(HWND window);
	static std::unordered_map<HWND, RenderJob>& PendingJobs();
	static std::mutex& Mutex();
	static std::condition_variable& CV();
	static IDXGIFactory6* DXGIFactory();
	static ID3D11DeviceContext* ImmediateContext();
	static ID3D11Device* D3D11Device();
	static void DumpD3D11LiveObjects();
	static void SetViewMatrix(const DirectX::XMMATRIX& view);
	static void SetProjectionMatrix(float fov, float aspectRatio, float zNear, float zFar);
	static const DirectX::XMMATRIX& ViewMatrix();
	static const DirectX::XMMATRIX& ProjectionMatrix();
	static void SetLightBuffer(const LightBuffer& buffer);
	static void UpdateLightBuffer(ID3D11DeviceContext* context);
	static ID3D11Buffer** LightBufferAddr();
	static ID3D11Buffer** TransformBufferAddr();
	static ID3D11Buffer* TransformBufferPtr();
	static ID3D11BlendState* GridBlendState();
	static ID3D11RasterizerState* GridRasterizerState();
	static ID3D11DepthStencilState* GridDepthStencilState();
	static void SetGridParams(const GridParams& params);
	static ID3D11Buffer* GridParamsBuffer();
	static GridParams& GridParamsData();
	static ID3D11Buffer* CameraParamsBuffer();
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
	void InitBuffers();
	void EnableGridAlphaBlending();
private:
	Microsoft::WRL::ComPtr<ID3D11Device>             m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>      m_immediateContext;
	Microsoft::WRL::ComPtr<IDXGIFactory6>            m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             m_transformBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             m_lightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             m_gridParamsBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             m_cameraParamsBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState>         m_bsGrid;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_rsGrid;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_dsGrid;
#ifdef _DEBUG									     
	Microsoft::WRL::ComPtr<IDXGIDebug1>              m_dxgiDebug;
	Microsoft::WRL::ComPtr<ID3D11Debug>              m_d3d11Debug;
#endif											     
	std::unordered_map<HWND, RenderJob>              m_pendingJobs;
	std::unordered_map<HWND, RenderJob>              m_activeJobs;
	std::condition_variable                          m_renderCV;
	std::mutex                                       m_renderMutex;
	DirectX::XMMATRIX                                m_viewMatrix{};
	DirectX::XMMATRIX                                m_projMatrix{};
	LightBuffer                                      m_lightBufferData{};
	GridParams                                       m_gridParams{};
};