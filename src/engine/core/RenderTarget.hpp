#pragma once

#include "GraphicsDevice.hpp"
#include "ConstantBuffers.hpp"

#include <engine/scene/Camera.hpp>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdint>

class RenderTarget {
public:
	explicit RenderTarget(HWND window);
	~RenderTarget() = default;

	void ToggleFullscreen();
	void HandleResize();
	void BeginRender();
	void EndRender();
	ID3D11CommandList* CommandList();
	IDXGISwapChain4* SwapChain();
	ID3D11DeviceContext* Context();
	Camera& GetCamera();
private:
	void Initialize();
	DEVMODEW GetCurrentMonitorMode() const;
	void InitDXGISwapChain();
	void InitDeferredContext();
	void InitDepthStencilView();
private:
	HWND m_window;
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	Camera m_camera;
	LightBuffer m_lightBufferData;

	Microsoft::WRL::ComPtr<IDXGISwapChain4>         m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_rtv;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_deferredContext;
	Microsoft::WRL::ComPtr<ID3D11CommandList>       m_cmdList;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
};